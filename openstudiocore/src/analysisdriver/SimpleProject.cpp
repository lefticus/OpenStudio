/**********************************************************************
 *  Copyright (c) 2008-2013, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#include <analysisdriver/SimpleProject.hpp>
#include <analysisdriver/CurrentAnalysis.hpp>
#include <analysisdriver/AnalysisRunOptions.hpp>

#include <project/ProjectDatabase.hpp>
#include <project/AnalysisRecord.hpp>
#include <project/DataPointRecord.hpp>
#include <project/DakotaAlgorithmRecord.hpp>
#include <project/DakotaAlgorithmRecord_Impl.hpp>

#include <analysis/Algorithm.hpp>
#include <analysis/Analysis_Impl.hpp>
#include <analysis/DataPoint.hpp>
#include <analysis/DiscreteVariable.hpp>
#include <analysis/DiscreteVariable_Impl.hpp>
#include <analysis/NullPerturbation.hpp>
#include <analysis/NullPerturbation_Impl.hpp>
#include <analysis/OptimizationDataPoint.hpp>
#include <analysis/OptimizationDataPoint_Impl.hpp>
#include <analysis/OptimizationProblem.hpp>
#include <analysis/Problem.hpp>
#include <analysis/RubyContinuousVariable.hpp>
#include <analysis/RubyContinuousVariable_Impl.hpp>
#include <analysis/RubyPerturbation.hpp>
#include <analysis/RubyPerturbation_Impl.hpp>
#include <analysis/Variable.hpp>

#include <runmanager/lib/RunManager.hpp>
#include <runmanager/lib/Workflow.hpp>
#include <runmanager/lib/WorkItem.hpp>

#include <energyplus/ForwardTranslator.hpp>

#include <osversion/VersionTranslator.hpp>

#include <model/Model.hpp>
#include <model/WeatherFile.hpp>
#include <model/WeatherFile_Impl.hpp>

#include <utilities/core/Assert.hpp>
#include <utilities/core/PathHelpers.hpp>
#include <utilities/core/FileReference.hpp>
#include <utilities/core/Optional.hpp>
#include <utilities/core/FileLogSink.hpp>
#include <utilities/core/Containers.hpp>
#include <utilities/core/Compare.hpp>
#include <utilities/core/ApplicationPathHelpers.hpp>

#include <OpenStudio.hxx>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

using namespace openstudio::model;
using namespace openstudio::osversion;
using namespace openstudio::ruleset;
using namespace openstudio::runmanager;
using namespace openstudio::analysis;
using namespace openstudio::project;

namespace openstudio {
namespace analysisdriver {

SimpleProjectOptions::SimpleProjectOptions()
  : m_logLevel(Warn),
    m_initializeRunManagerUI(false),
    m_pauseRunManagerQueue(false)
{}

LogLevel SimpleProjectOptions::logLevel() const {
  return m_logLevel;
}

bool SimpleProjectOptions::initializeRunManagerUI() const {
  return m_initializeRunManagerUI;
}

bool SimpleProjectOptions::pauseRunManagerQueue() const {
  return m_pauseRunManagerQueue;
}

void SimpleProjectOptions::setLogLevel(LogLevel logLevel) {
  m_logLevel = logLevel;
}

void SimpleProjectOptions::setInitializeRunManagerUI(bool value) {
  m_initializeRunManagerUI = value;
}

void SimpleProjectOptions::setPauseRunManagerQueue(bool value) {
  m_pauseRunManagerQueue = value;
}

SimpleProject::SimpleProject(const SimpleProject& other)
  : m_projectDir(other.m_projectDir),
    m_analysisDriver(other.m_analysisDriver),
    m_analysis(other.m_analysis),
    m_logFile(other.m_logFile),
    m_seedModel(other.m_seedModel),
    m_seedIdf(other.m_seedIdf),
    m_measures(other.m_measures),
    m_measureArguments(other.m_measureArguments)
{
  if (m_analysis) {
    m_analysis->connect(SIGNAL(seedChanged()),this,SLOT(onSeedChanged()));
  }
}

SimpleProject& SimpleProject::operator=(const SimpleProject& other) {
  if (this == &other) {
    return *this;
  }

  m_projectDir = other.m_projectDir;
  m_analysisDriver = other.m_analysisDriver;
  m_analysis = other.m_analysis;
  m_logFile = other.m_logFile;
  m_seedModel = other.m_seedModel;
  m_seedIdf = other.m_seedIdf;
  m_measures = other.m_measures;
  m_measureArguments = other.m_measureArguments;
  if (m_analysis) {
    m_analysis->connect(SIGNAL(seedChanged()),this,SLOT(onSeedChanged()));
  }
  return *this;
}

SimpleProject::~SimpleProject()
{
  m_logFile.disable();
}

boost::optional<SimpleProject> SimpleProject::open(const openstudio::path& projectDir,
                                                   const SimpleProjectOptions& options)
{
  OptionalSimpleProject result;

  if (!boost::filesystem::exists(projectDir) || !boost::filesystem::is_directory(projectDir)) {
    LOG(Error,"Cannot open a SimpleProject at " << toString(projectDir) << ", because it either "
        << "does not exist or is not a directory.");
    return result;
  }

  openstudio::path projectDatabasePath;
  for (openstudio::directory_iterator it(projectDir),
       itEnd = openstudio::directory_iterator(); it != itEnd; ++it)
  {
    if (boost::filesystem::is_regular_file(it->status()))
    {
      // check for osp extenstion
      openstudio::path p = it->path();
      std::string ext = getFileExtension(p);
      if (ext != "osp") {
        continue;
      }

      // if projectDatabasePath already defined, log error and return
      if (!projectDatabasePath.empty()) {
        LOG(Error,"Cannot open a SimpleProject at " << toString(projectDir) << ", because it "
            << "contains multiple ProjectDatabases.");
        return result;
      }

      // otherwise keep
      projectDatabasePath = p;
    }
  }

  OptionalProjectDatabase database = ProjectDatabase::open(
        projectDatabasePath,
        true,
        options.initializeRunManagerUI());
  if (!database) {
    LOG(Error,"Cannot open a SimpleProject at " << toString(projectDir) << ", because the open "
        << "operation on the ProjectDatabase at " << toString(projectDatabasePath) << " failed.");
    return result;
  }

  AnalysisDriver analysisDriver(*database);

  AnalysisRecordVector analysisRecords = AnalysisRecord::getAnalysisRecords(*database);
  if (analysisRecords.size() != 1u) {
    LOG(Error,"Cannot open a SimpleProject at " << toString(projectDir) << ", because the "
        << "ProjectDatabase contains " << analysisRecords.size() << " analyses, but "
        << "SimpleProject requires there to be exactly one.");
    return result;
  }

  result = SimpleProject(projectDir,analysisDriver,OptionalAnalysis(),options);

  return result;
}


boost::optional<SimpleProject> SimpleProject::create(const openstudio::path& projectDir,
                                                     const SimpleProjectOptions& options,
                                                     bool ignoreExistingFiles)
{
  OptionalSimpleProject result;

  if (!boost::filesystem::exists(projectDir)) {
    bool ok = boost::filesystem::create_directories(projectDir);
    if (!ok) {
      LOG(Error,"Cannot create a SimpleProject at " << toString(projectDir) << ", because "
          << "the directory cannot be created.");
      return result;
    }
  }

  if (!boost::filesystem::is_directory(projectDir)) {
    LOG(Error,"Cannot create a SimpleProject at " << toString(projectDir) << ", because "
        << "the path exists but is not a directory.");
    return result;
  }

  if (!ignoreExistingFiles)
  {
    for (openstudio::directory_iterator it(projectDir),
        itEnd = openstudio::directory_iterator(); it != itEnd; ++it)
    {
      if (boost::filesystem::is_directory(it->status()) ||
          boost::filesystem::is_regular_file(it->status()))
      {
        LOG(Error,"Cannot create a SimpleProject at " << toString(projectDir) << ", because "
            << "the directory is not empty.");
        return result;
      }
    }
  }

  openstudio::path projectDatabasePath = projectDir / toPath("project.osp");
  openstudio::path runManagerPath = projectDir / toPath("run.db");
  runmanager::RunManager rm(runManagerPath,true,true,options.initializeRunManagerUI());
  ProjectDatabase database(projectDatabasePath,rm,true);
  AnalysisDriver analysisDriver(database);

  // TODO: Use simplest Problem constructor after VariableOrJob feature added.
  Problem problem("Simple Project Problem",
                  VariableVector(),
                  runmanager::Workflow());

  // TODO: Use simplest Analysis constructor (specify seed type, not actual seed)
  // after Analysis setters feature added.
  FileReference seed(toPath("*.osm"));
  Analysis analysis("Simple Project Analysis",problem,seed);

  saveAnalysis(analysis,analysisDriver);
  result = SimpleProject(projectDir,analysisDriver,analysis,options);

  return result;
}

openstudio::path SimpleProject::projectDir() const {
  return m_projectDir;
}

analysisdriver::AnalysisDriver SimpleProject::analysisDriver() const {
  return m_analysisDriver;
}

project::ProjectDatabase SimpleProject::projectDatabase() const {
  return m_analysisDriver.database();
}

runmanager::RunManager SimpleProject::runManager() const {
  return m_analysisDriver.database().runManager();
}

project::AnalysisRecord SimpleProject::analysisRecord() const {
  ProjectDatabase database = projectDatabase();
  AnalysisRecordVector analysisRecords = AnalysisRecord::getAnalysisRecords(database);
  BOOST_ASSERT(analysisRecords.size() == 1u);
  AnalysisRecord result = analysisRecords[0];
  if (analysisIsLoaded()) {
    BOOST_ASSERT(result.handle() == analysis().uuid());
  }
  return result;
}

analysis::Analysis SimpleProject::analysis() const {
  if (!m_analysis) {
    // load from database
    m_analysis = analysisRecord().analysis();
    m_analysis->connect(SIGNAL(seedChanged()),this,SLOT(onSeedChanged()));
  }
  return m_analysis.get();
}

FileReference SimpleProject::seed() const
{
  return analysis().seed();
}

boost::optional<model::Model> SimpleProject::seedModel(ProgressBar* progressBar) const {
  if (!m_seedModel) {
    FileReference seed = analysis().seed();
    if (seed.fileType() == FileReferenceType::OSM) {
      osversion::VersionTranslator translator;
      m_seedModel = translator.loadModel(seed.path(), progressBar);
    }
  }
  return m_seedModel;
}

boost::optional<Workspace> SimpleProject::seedIdf(ProgressBar* progressBar) const {
  if (!m_seedIdf) {
    FileReference seed = analysis().seed();
    if (seed.fileType() == FileReferenceType::IDF) {
      m_seedIdf = Workspace::load(seed.path(),IddFileType(IddFileType::EnergyPlus));
    }
  }
  if (!m_seedIdf) {
    model::OptionalModel seedModel = this->seedModel(progressBar);
    if (seedModel) {
      energyplus::ForwardTranslator translator;
      m_seedIdf = translator.translateModel(*seedModel, progressBar);
    }
  }
  return m_seedIdf;
}

std::vector<BCLMeasure> SimpleProject::measures() const {
  // first go through cache, separating into result and remove
  BCLMeasureVector result, toRemove;
  StringVector resultDirs;
  for (std::map<UUID,BCLMeasure>::const_iterator it = m_measures.begin(),
       itEnd = m_measures.end(); it != itEnd; ++it)
  {
    // make sure measure is still there
    if (boost::filesystem::exists(it->second.directory())) {
      // good enough--pass it back
      result.push_back(it->second);
      resultDirs.push_back(toString(it->second.directory().stem()));
    }
    else {
      // directory no longer there. clear cache for this measure.
      toRemove.push_back(it->second);
    }
  }
  BOOST_FOREACH(BCLMeasure& gone,toRemove) {
    std::map<UUID,BCLMeasure>::iterator it1 = m_measures.find(gone.uuid());
    BOOST_ASSERT(it1 != m_measures.end());
    m_measures.erase(it1);
    std::map<UUID,std::vector<ruleset::OSArgument> >::iterator it2 = m_measureArguments.find(gone.uuid());
    if (it2 != m_measureArguments.end()) {
      m_measureArguments.erase(it2);
    }
  }
  // now check disk
  openstudio::path dir = scriptsDir();
  if (!boost::filesystem::exists(dir) || !boost::filesystem::is_directory(dir)) {
    return result;
  }
  StringVector candidateDirs;
  for (directory_iterator dit(dir), ditEnd; dit != ditEnd; ++dit) {
    if (boost::filesystem::is_directory(dit->status())) {
      candidateDirs.push_back(toString(dit->path().stem()));
    }
  }
  if (candidateDirs.size() > result.size()) {
    StringVector::iterator it = candidateDirs.begin();
    while (it != candidateDirs.end()) {
      if (std::find(resultDirs.begin(),resultDirs.end(),*it) != resultDirs.end()) {
        it = candidateDirs.erase(it);
      }
      else {
        ++it;
      }
    }
    BOOST_FOREACH(const std::string& newDir,candidateDirs) {
      if (OptionalBCLMeasure newMeasure = BCLMeasure::load(dir / toPath(newDir))) {
        UUID newUUID = newMeasure->uuid();
        std::map<UUID,BCLMeasure>::iterator it = m_measures.find(newUUID);
        if (it != m_measures.end()) {
          m_measures.erase(it);
          LOG(Warn,"There are measures with duplicate UUIDs in this project.");
        }
        std::pair<std::map<UUID,BCLMeasure>::const_iterator,bool> insertResult =
            m_measures.insert(std::map<UUID,BCLMeasure>::value_type(newUUID,*newMeasure));
        BOOST_ASSERT(insertResult.second);
        result.push_back(*newMeasure);
      }
    }
  }
  return result;
}

boost::optional<BCLMeasure> SimpleProject::getMeasureByUUID(const UUID& uuid) const {
  OptionalBCLMeasure result;
  // check map
  std::map<UUID,BCLMeasure>::iterator it = m_measures.find(uuid);
  if (it != m_measures.end()) {
    // make sure measure is still there
    if ((boost::filesystem::exists(it->second.directory()) &&
         it->second.primaryRubyScriptPath()))
    {
      // good enough--pass it back
      result = it->second;
    }
    else {
      // directory or primary file no longer there. clear cache for this measure.
      m_measures.erase(it);
    }
  }
  if (!result) {
    // look on file system
    openstudio::path dir = scriptsDir();
    if (!boost::filesystem::exists(dir) || !boost::filesystem::is_directory(dir)) {
      return result;
    }
    for (directory_iterator dit(dir), ditEnd; dit != ditEnd; ++dit) {
      if (boost::filesystem::is_directory(dit->status())) {
        OptionalBCLMeasure candidate = BCLMeasure::load(dit->path());
        if (candidate) {
          UUID candidateUUID = candidate->uuid();
          std::map<UUID,BCLMeasure>::iterator it = m_measures.find(candidateUUID);
          if (it != m_measures.end()) {
            m_measures.erase(it);
          }
          std::pair<std::map<UUID,BCLMeasure>::const_iterator,bool> insertResult =
              m_measures.insert(std::map<UUID,BCLMeasure>::value_type(candidateUUID,*candidate));
          BOOST_ASSERT(insertResult.second);
          if (candidateUUID == uuid) {
            result = *candidate;
            return result;
          }
        }
      }
    }
  }
  return result;
}

bool SimpleProject::hasStoredArguments(const BCLMeasure& measure) const {
  return (m_measureArguments.find(measure.uuid()) != m_measureArguments.end());
}

std::vector<ruleset::OSArgument> SimpleProject::getStoredArguments(const BCLMeasure& measure) const {
  OSArgumentVector result;
  std::map<UUID,std::vector<OSArgument> >::const_iterator it = m_measureArguments.find(measure.uuid());
  if (it != m_measureArguments.end()) {
    result = it->second;
  }
  return result;
}

bool SimpleProject::analysisIsLoaded() const {
  return m_analysis;
}

bool SimpleProject::isRunning() const {
  return analysisDriver().isRunning();
}

AnalysisRunOptions SimpleProject::standardRunOptions() const {
  openstudio::path workingDirectory = projectDir();

  // tools
  runmanager::ConfigOptions configOpts(true);
  runManager().setConfigOptions(configOpts);
  runmanager::Tools tools = configOpts.getTools();
  openstudio::path rubyIncludeDirectory = getOpenStudioRubyIncludePath();
  openstudio::path dakotaExePath;
  try {
    runmanager::ToolInfo dakotaTool = tools.getTool("dakota");
    dakotaExePath = dakotaTool.localBinPath;
    LOG(Debug,"Set dakota.exe path to " << toString(dakotaExePath) << ".");
  }
  catch (...) {}

  analysisdriver::AnalysisRunOptions runOptions(workingDirectory,
                                                rubyIncludeDirectory,
                                                dakotaExePath);
  runOptions.setRunManagerTools(tools);

  // weather file path
  openstudio::path seedPath = analysis().seed().path();
  if (boost::filesystem::exists(seedPath)) {
    openstudio::path searchPath = seedPath.parent_path() / toPath(seedPath.stem()) / toPath("files");
    LOG(Debug,"Appending search path for weather files: " << toString(searchPath));
    runOptions.setUrlSearchPaths(std::vector<openstudio::URLSearchPath>(1u,searchPath));
  }

  // ETH@20130306 - Is this the best option?
  // DLM: for now we will let run manager manage the number of jobs running at a time
  //      even though this does result in long time to queue initially
  // DLM: i did confirm that the data points in the run list update correctly if this is set
  //runOptions.setQueueSize(configOpts.getMaxLocalJobs());

  // DLM: in the future would be good to set JobCleanUpBehavior to standard
  // however there seem to be intermittant failures when this is done (bug 1077)
  // for now keep this setting, should also be a user option for debugging
  runOptions.setJobCleanUpBehavior(analysisdriver::JobCleanUpBehavior::none);

  return runOptions;
}

bool SimpleProject::modelsRequireUpdate() const {
  if ((!analysis().weatherFile()) || (!boost::filesystem::exists(analysis().weatherFile()->path()))) {
    return true;
  }
  if (requiresVersionTranslation(analysis().seed().path())) {
    return true;
  }
  BOOST_FOREACH(const openstudio::path& p, alternateModelPaths()) {
    if (requiresVersionTranslation(p)) {
      return true;
    }
  }
  return false;
}

bool SimpleProject::isPATProject() const {
  Problem problem = analysis().problem();

  // has to have placeholder variable for swapping entire model
  if (!getAlternativeModelVariable()) {
    return false;
  }

  BOOST_FOREACH(const InputVariable& variable,problem.variables()) {
    if (OptionalDiscreteVariable dv = variable.optionalCast<DiscreteVariable>()) {
      BOOST_FOREACH(const DiscretePerturbation& perturbation,dv->perturbations(false)) {
        if (!perturbation.optionalCast<NullPerturbation>() &&
            !perturbation.optionalCast<RubyPerturbation>())
        {
          return false; // no ruleset perturbations
        }
        if (OptionalRubyPerturbation rpert = perturbation.optionalCast<RubyPerturbation>()) {
          // must be BCLMeasure RubyPerturbation
          if (!rpert->usesBCLMeasure()) {
            return false;
          }
        }
      }
    }
    else {
      return false; // no continuous variables
    }
  }

  // check workflow
  bool inputVariableOk = true;
  boost::optional<JobType> nextWorkItemType = JobType(JobType::ModelToIdf);
  BOOST_FOREACH(const WorkflowStep& step,problem.workflow()) {
    if (!inputVariableOk && step.isInputVariable()) {
      return false;
    }
    if (step.isWorkItem()) {
      if ((!nextWorkItemType) || (step.workItemType() != nextWorkItemType.get())) {
        return false;
      }
      BOOST_ASSERT(nextWorkItemType);
      BOOST_ASSERT(step.workItemType() == nextWorkItemType.get());
      switch (nextWorkItemType->value()) {
        case JobType::ModelToIdf :
          nextWorkItemType = JobType(JobType::ExpandObjects);
          inputVariableOk = false;
          break;
        case JobType::ExpandObjects :
          nextWorkItemType = JobType(JobType::EnergyPlusPreProcess);
          inputVariableOk = true;
          break;
        case JobType::EnergyPlusPreProcess :
          nextWorkItemType = JobType(JobType::EnergyPlus);
          inputVariableOk = false;
          break;
        case JobType::EnergyPlus :
          nextWorkItemType = JobType(JobType::OpenStudioPostProcess);
          break;
        case JobType::OpenStudioPostProcess :
          nextWorkItemType.reset();
          break;
        default :
          BOOST_ASSERT(false);
      }
    }
  }

  return true;
}

boost::optional<DiscreteVariable> SimpleProject::getAlternativeModelVariable() const {
  OptionalDiscreteVariable result;
  Problem problem = analysis().problem();
  BOOST_FOREACH(const InputVariable& ivar,problem.variables()) {
    if (OptionalDiscreteVariable dvar = ivar.optionalCast<DiscreteVariable>()) {
      // must have correct name
      if (dvar->name() != "Alternative Model") {
        continue;
      }
      // must have correct perturbations
      bool first(true);
      bool found(true);
      BOOST_FOREACH(const DiscretePerturbation& pert,dvar->perturbations(false)) {
        if (first) {
          if (!pert.optionalCast<NullPerturbation>()) {
            found = false;
            break;
          }
          first = false;
        }
        else {
          if (!pert.optionalCast<RubyPerturbation>()) {
            found = false;
            break;
          }
          RubyPerturbation rpert = pert.cast<RubyPerturbation>();
          if (!rpert.usesBCLMeasure()) {
            found = false;
            break;
          }
          if (rpert.measureUUID() != alternativeModelMeasureUUID()) {
            found = false;
            break;
          }
        }
      }
      if (found) {
        result = *dvar;
        break;
      }
    }
  }
  return result;
}

std::pair<bool,std::vector<BCLMeasure> > SimpleProject::setSeed(const FileReference& currentSeedLocation, 
                                                                ProgressBar* progressBar) 
{
  std::pair<bool,BCLMeasureVector> result(false,BCLMeasureVector());

  // exists?
  if (!boost::filesystem::exists(currentSeedLocation.path())) {
    LOG(Info,"Cannot set seed to " << toString(currentSeedLocation.path()) <<
        ", because that location does not exist.");
    return result;
  }

  if (!boost::filesystem::is_regular_file(currentSeedLocation.path())) {
    LOG(Info,"Cannot set seed to " << toString(currentSeedLocation.path()) <<
        ", because it is not a regular file.");
    return result;
  }

  // is OSM or IDF
  FileReferenceType fileType = currentSeedLocation.fileType();
  if (!((fileType == FileReferenceType::OSM) ||
        (fileType == FileReferenceType::IDF)))
  {
    LOG(Info,"Cannot set seed to " << toString(currentSeedLocation.path()) <<
        ", because it has unexpected file type '*." << fileType.valueDescription()
        << "'.");
    return result;
  }

  // compatible with analysis?
  bool ok = analysis().setSeed(currentSeedLocation);
  if (!ok) {
    return result;
  }

  // DLM: analysis().setSeed will result in call to seedChanged()

  // ok--no turning back
  result.first = true;

  // ETH: If you wanted to clean out fixed measures that came along with previous baseline
  // model, this is where you would do it.

  // clean out existing folder
  openstudio::path seedDir = this->seedDir();
  if (boost::filesystem::exists(seedDir)) {
    try {
      boost::filesystem::remove_all(seedDir);
    }
    catch (std::exception& e) {
      LOG(Warn,"Could not remove old seed model from this project, because " << e.what() << ".");
    }
  }
  boost::filesystem::create_directories(seedDir);

  // copy over main file
  openstudio::path newPath = seedDir / toPath(currentSeedLocation.path().filename());
  boost::filesystem::copy_file(currentSeedLocation.path(),newPath);
  FileReference newSeed(newPath);
  BOOST_ASSERT(newSeed.fileType() == fileType);
  ok = analysis().setSeed(newSeed);
  BOOST_ASSERT(ok);

  if (fileType == FileReferenceType::OSM) {
    // do further fix-up
    bool ok = copyModel(currentSeedLocation.path(),seedDir);
    if (requiresVersionTranslation(newPath)) {
      ok = ok && upgradeModel(newPath, progressBar);
    }
    ok = ok && setAnalysisWeatherFile(progressBar);
    result.second = importSeedModelMeasures(progressBar);
    if (!ok) {
      LOG(Warn,"Some aspect of setting the seed model did not go smoothly. "
          << "See previous log messages for details.")
    }
  }

  return result;
}

BCLMeasure SimpleProject::insertMeasure(const BCLMeasure& measure) {
  OptionalBCLMeasure result = getMeasureByUUID(measure.uuid());
  if (!result) {
    result = addMeasure(measure);
  }
  return result.get();
}

bool SimpleProject::registerArguments(const BCLMeasure& measure,
                                      const std::vector<ruleset::OSArgument>& arguments)
{
  if (getMeasureByUUID(measure.uuid())) {
    m_measureArguments[measure.uuid()] = arguments;
    return true;
  }
  return false;
}

bool SimpleProject::updateMeasure(const BCLMeasure& measure,
                                  const std::vector<ruleset::OSArgument>& arguments)
{
  UUID measureUUID = measure.uuid();
  OptionalBCLMeasure existing = getMeasureByUUID(measureUUID);
  if (existing && (existing.get() != measure)) {
    if (existing->inputFileType() != measure.inputFileType()) {
      LOG(Warn,"The input file type of measure " << toString(measureUUID) << ", currently named "
          << measure.name() << " has changed. This update operation may invalidate data points, "
          << "not just results.");
    }
    if (existing->outputFileType() != measure.outputFileType()) {
      LOG(Warn,"The output file type of measure " << toString(measureUUID) << ", currently named "
          << measure.name() << " has changed. This update operation may invalidate data points, "
          << "not just results.");
    }

    // create project-specific copy
    existing = overwriteMeasure(measure);
    // have arguments, so cache them
    bool ok = registerArguments(*existing,arguments);
    BOOST_ASSERT(ok);

    ok = analysis().problem().updateMeasure(*existing,arguments,false);
    if (!ok) {
      LOG(Warn,"Was not able to replace all instantiations of measure " << toString(measureUUID)
          << ", currently called " << measure.name() << ". Problem may be ill-posed.");
    }

    return true;
  }
  return false;
}

bool SimpleProject::updateModels() {
  bool result(true);
  if (requiresVersionTranslation(analysis().seed().path())) {
    result = result && upgradeModel(analysis().seed().path());
  }
  if ((!analysis().weatherFile()) || (!boost::filesystem::exists(analysis().weatherFile()->path()))) {
    // ETH@20130313 - Thought about not registering failure of setAnalysisWeatherFile, because not
    // running EnergyPlus, and therefore not setting a weather file, is a legitamite workflow. However,
    // in PAT there is no way to remove the EnergyPlus job, so not having a weather file really is a
    // problem.
    result = result && setAnalysisWeatherFile();
  }
  BOOST_FOREACH(const openstudio::path& p, alternateModelPaths()) {
    if (requiresVersionTranslation(p)) {
      result = result && upgradeModel(p);
    }
  }
  return result;
}

bool SimpleProject::makeSelfContained() {
  bool result(true);

  // seed
  FileReference currentSeed = analysis().seed();
  if ((toString(currentSeed.path().stem()) != "*") &&
      (currentSeed.path().parent_path() != seedDir()))
  {
    result = setSeed(currentSeed).first;
  }

  // BCL measures
  // find measures in need of updating
  openstudio::path scriptsDir = this->scriptsDir();
  std::map<UUID,openstudio::path> toUpdate;
  InputVariableVector vars = analysis().problem().variables();
  BOOST_FOREACH(InputVariable& var,vars) {
    if (OptionalDiscreteVariable dv = var.optionalCast<DiscreteVariable>()) {
      DiscretePerturbationVector dps = dv->perturbations(false);
      BOOST_FOREACH(DiscretePerturbation& dp,dps) {
        if (OptionalRubyPerturbation rp = dp.optionalCast<RubyPerturbation>()) {
          if (rp->usesBCLMeasure() &&
              (rp->measureDirectory().parent_path() != scriptsDir))
          {
            toUpdate[rp->measureUUID()] = rp->measureDirectory();
          }
        }
      }
    }
    else if (OptionalRubyContinuousVariable rcv = var.optionalCast<RubyContinuousVariable>()) {
      if (rcv->perturbation().usesBCLMeasure() &&
          (rcv->perturbation().measureDirectory().parent_path() != scriptsDir))
      {
        toUpdate[rcv->perturbation().measureUUID()] = rcv->perturbation().measureDirectory();
      }
    }
  }

  // update them
  for (std::map<UUID,openstudio::path>::const_iterator it = toUpdate.begin(),
       itEnd = toUpdate.end(); it != itEnd; ++it)
  {
    OptionalBCLMeasure projectMeasure = getMeasureByUUID(it->first);
    if (!projectMeasure) {
      OptionalBCLMeasure currentMeasure = BCLMeasure::load(it->second);
      if (currentMeasure) {
        projectMeasure = insertMeasure(*currentMeasure);
      }
      else {
        LOG(Error,"Could not locate measure at " << toString(it->second)
            << ". Unable to update analysis objects that use measure "
            << toString(it->first) << ".");
        result = false;
        continue;
      }
    }
    BOOST_ASSERT(projectMeasure);
    if (hasStoredArguments(*projectMeasure)) {
      result = result && analysis().problem().updateMeasure(
            *projectMeasure,
            getStoredArguments(*projectMeasure),
            false);
    }
    else {
      result = result && analysis().problem().updateMeasure(
            *projectMeasure,
            OSArgumentVector(),
            true);
    }
  }

  return result;
}


void SimpleProject::stop() {
  // the following should be enough to kill any running jobs
  std::vector<analysisdriver::CurrentAnalysis> currentAnalyses = analysisDriver().currentAnalyses();
  BOOST_FOREACH(analysisdriver::CurrentAnalysis currentAnalysis, currentAnalyses){
    analysisDriver().stop(currentAnalysis);
  }
}

bool SimpleProject::clearAllResults() {
  bool result(true);
  if (analysisIsLoaded()) {
    Analysis analysis = this->analysis();
    AnalysisDriver analysisDriver = this->analysisDriver();
    result = analysisdriver::clearAllResults(analysis,analysisDriver);
  }
  else {
    AnalysisDriver analysisDriver = this->analysisDriver();
    ProjectDatabase database = this->projectDatabase();
    database.unloadUnusedCleanRecords();
    bool didStartTransaction = database.startTransaction();
    AnalysisRecord analysisRecord = this->analysisRecord();
    DataPointRecordVector dataPointRecords = analysisRecord.dataPointRecords();
    BOOST_FOREACH(DataPointRecord& dataPointRecord,dataPointRecords) {
      boost::optional<UUID> jobUUID = dataPointRecord.topLevelJobUUID();
      if (jobUUID) {
        try {
          runmanager::Job job = database.runManager().getJob(*jobUUID);
          database.runManager().remove(job);
        }
        catch (...) {}
      }
      openstudio::path dpDir = dataPointRecord.directory();
      if (!dpDir.empty() && boost::filesystem::exists(dpDir)) {
        try {
          boost::filesystem::remove_all(dpDir);
        }
        catch (...) {
          result = false;
        }
      }
      dataPointRecord.clearResults();
    }
    if (OptionalAlgorithmRecord algorithmRecord = analysisRecord.algorithmRecord()) {
      if (OptionalDakotaAlgorithmRecord dakotaAlgorithmRecord = algorithmRecord->optionalCast<DakotaAlgorithmRecord>()) {
        if (boost::optional<UUID> dakotaJobUUID = dakotaAlgorithmRecord->jobUUID()) {
          try {
            runmanager::Job job = database.runManager().getJob(*dakotaJobUUID);
            openstudio::path dakotaDir = job.outdir();
            if (boost::filesystem::exists(dakotaDir)) {
              try {
                boost::filesystem::remove_all(dakotaDir);
              }
              catch (...) {
                result = false;
              }
            }
            database.runManager().remove(job);
          }
          catch (...) {}
        }
      }
      algorithmRecord->reset();
    }
    analysisRecord.setResultsAreInvalid(false);
    database.save();
    if (didStartTransaction) {
      bool ok = database.commitTransaction();
      BOOST_ASSERT(ok);
    }
  }
  // ok to clean up anything left over because only one analysis per SimpleProject
  removeOrphanedResultFiles();
  return result;
}

bool SimpleProject::removeAllDataPoints() {
  bool result(true);
  if (analysisIsLoaded()) {
    Analysis analysis = this->analysis();
    AnalysisDriver analysisDriver = this->analysisDriver();
    result = analysisdriver::removeAllDataPoints(analysis,analysisDriver);
  }
  else {
    // analysis not loaded, so also not running
    ProjectDatabase database = this->projectDatabase();
    database.unloadUnusedCleanRecords();
    bool didStartTransaction = database.startTransaction();
    AnalysisRecord analysisRecord = this->analysisRecord();
    DataPointRecordVector dataPointRecords = analysisRecord.dataPointRecords();
    BOOST_FOREACH(DataPointRecord& dataPointRecord,dataPointRecords) {
      boost::optional<UUID> jobUUID = dataPointRecord.topLevelJobUUID();
      if (jobUUID) {
        try {
          runmanager::Job job = database.runManager().getJob(*jobUUID);
          database.runManager().remove(job);
        }
        catch (...) {}
      }
      openstudio::path dpDir = dataPointRecord.directory();
      if (!dpDir.empty() && boost::filesystem::exists(dpDir)) {
        try {
          boost::filesystem::remove_all(dpDir);
        }
        catch (...) {
          result = false;
        }
      }
      database.removeRecord(dataPointRecord);
    }
    if (OptionalAlgorithmRecord algorithmRecord = analysisRecord.algorithmRecord()) {
      if (OptionalDakotaAlgorithmRecord dakotaAlgorithmRecord = algorithmRecord->optionalCast<DakotaAlgorithmRecord>()) {
        if (boost::optional<UUID> dakotaJobUUID = dakotaAlgorithmRecord->jobUUID()) {
          try {
            runmanager::Job job = database.runManager().getJob(*dakotaJobUUID);
            openstudio::path dakotaDir = job.outdir();
            if (boost::filesystem::exists(dakotaDir)) {
              try {
                boost::filesystem::remove_all(dakotaDir);
              }
              catch (...) {
                result = false;
              }
            }
            database.runManager().remove(job);
          }
          catch (...) {}
        }
      }
      algorithmRecord->reset();
    }
    analysisRecord.setResultsAreInvalid(false);
    analysisRecord.setDataPointsAreInvalid(false);
    database.save();
    if (didStartTransaction) {
      bool ok = database.commitTransaction();
      BOOST_ASSERT(ok);
    }
  }
  // ok to clean up anything left over because only one analysis per SimpleProject
  removeOrphanedResultFiles();
  return result;
}

analysis::DataPoint SimpleProject::baselineDataPoint() const {
  analysis::Analysis analysis = this->analysis();
  analysis::Problem problem = analysis.problem();

  std::vector<analysis::DiscretePerturbation> baselinePerturbations;
  BOOST_FOREACH(const analysis::InputVariable& inputVariable, problem.variables()){
    boost::optional<analysis::DiscreteVariable> discreteVariable = inputVariable.optionalCast<analysis::DiscreteVariable>();
    if (discreteVariable){
      bool found(false);
      std::vector<DiscretePerturbation> perturbations = discreteVariable->perturbations(false);

      BOOST_FOREACH(const analysis::DiscretePerturbation& perturbation, perturbations){
        if (perturbation.optionalCast<analysis::NullPerturbation>()){
          baselinePerturbations.push_back(perturbation);
          found = true;
          break;
        }
      }
      if (!found) {
        // we don't want to add a null, we want to add the one that's there if the null wasn't found,
        // it must be the fixed measure
        BOOST_ASSERT(perturbations.size() == 1);
        baselinePerturbations.push_back(perturbations[0]);
      }
    }
  }

  BOOST_ASSERT(int(baselinePerturbations.size()) == problem.numVariables());

  boost::optional<DataPoint> result = analysis.getDataPoint(baselinePerturbations);
  if (!result){
    result = problem.createDataPoint(baselinePerturbations);
    BOOST_ASSERT(result);
    analysis.addDataPoint(*result);
    result->setName("Baseline");
    result->setDisplayName("Baseline");
  }

  return *result;
}

bool SimpleProject::addAlternateModel(const openstudio::path& altModel) {
  if (analysis().dataPointsAreInvalid()) {
    // will not be able to add data point
    return false;
  }

  if (!isPATProject()) {
    return false;
  }

  // exists?
  if (!boost::filesystem::exists(altModel)) {
    return false;
  }

  if (!boost::filesystem::is_regular_file(altModel)) {
    return false;
  }

  // add file to project
  copyModel(altModel,alternateModelsDir());
  openstudio::path newPath = alternateModelsDir() / toPath(altModel.filename());
  BOOST_ASSERT(boost::filesystem::exists(newPath));
  upgradeModel(newPath);

  // create swap measure for altModel
  DiscreteVariable amvar = getAlternativeModelVariable().get();
  std::vector<BCLMeasure> patMeasures = BCLMeasure::patApplicationMeasures();
  std::vector<BCLMeasure>::const_iterator it = std::find_if(
        patMeasures.begin(),
        patMeasures.end(),
        boost::bind(uuidEquals<BCLMeasure,openstudio::UUID>,_1,alternativeModelMeasureUUID()));
  BOOST_ASSERT(it != patMeasures.end());
  BCLMeasure replaceModelMeasure = insertMeasure(*it);
  RubyPerturbation swapModel(replaceModelMeasure,false); // false so not used in algorithms
  swapModel.setName("Alternate Model: " + toString(newPath.filename()));
  // hard-code argument since can't get arguments from library
  OSArgument arg = OSArgument::makePathArgument("alternativeModelPath",true,"osm");
  arg.setDisplayName("Alternative Model Path");
  arg.setValue(newPath);
  swapModel.setArgument(arg);
  amvar.push(swapModel);
  int pIndex = amvar.numPerturbations(false) - 1;

  // add data point
  Analysis analysis = this->analysis();
  Problem problem = analysis.problem();
  std::vector<QVariant> variableValues;
  BOOST_FOREACH(const InputVariable& ivar,problem.variables()) {
    if (ivar == amvar) {
      variableValues.push_back(QVariant(pIndex));
    }
    else {
      int index = -1;
      DiscreteVariable dvar = ivar.cast<DiscreteVariable>();
      for (int i = 0, n = dvar.numPerturbations(false); i < n; ++i) {
        if (dvar.getPerturbation(i).optionalCast<NullPerturbation>()) {
          index = i;
          break;
        }
      }
      if (index == -1) {
        // no null perturbation -- add it to bottom so doesn't invalidate anything
        dvar.push(NullPerturbation());
        index = dvar.numPerturbations(false) - 1;
      }
      variableValues.push_back(QVariant(index));
    }
  }
  OptionalDataPoint dataPoint = problem.createDataPoint(variableValues);
  BOOST_ASSERT(dataPoint);
  bool ok = analysis.addDataPoint(dataPoint.get());
  BOOST_ASSERT(ok);
  dataPoint->setName(swapModel.name());
  dataPoint->setDescription("Replace baseline model with '" + toString(newPath) + "'.");

  return true;
}

bool SimpleProject::insertAlternativeModelVariable() {
  if (getAlternativeModelVariable()) {
    return true; // nothing to do
  }

  // do not try to do this if analysis is in bad state
  if (analysis().resultsAreInvalid()) {
    return false;
  }

  // try to add the variable
  bool ok = analysis().problem().insert(0,DiscreteVariable("Alternative Model",
                                                           DiscretePerturbationVector(1u,NullPerturbation())));
  if (!ok) {
    return false;
  }

  // no turning back
  // use back-door deserialization constructors to put DataPoints back together
  if (analysis().dataPointsAreInvalid()) {
    Analysis temp = analysis();
    DataPointVector tempPoints = temp.dataPoints();
    DataPointVector dataPoints;
    BOOST_FOREACH(const DataPoint& tempPoint,tempPoints) {
      std::vector<QVariant> variableValues = tempPoint.variableValues();
      variableValues.insert(variableValues.begin(),QVariant(0));
      if (tempPoint.optionalCast<OptimizationDataPoint>()) {
        OptimizationDataPoint optTempPoint = tempPoint.cast<OptimizationDataPoint>();
        OptimizationDataPoint optDataPoint(optTempPoint.uuid(),
                                           optTempPoint.versionUUID(),
                                           "fake name so can use name to make dirty",
                                           optTempPoint.displayName(),
                                           optTempPoint.description(),
                                           optTempPoint.isComplete(),
                                           optTempPoint.failed(),
                                           optTempPoint.optimizationProblem(),
                                           optTempPoint.objectiveValues(),
                                           variableValues,
                                           optTempPoint.responseValues(),
                                           optTempPoint.directory(),
                                           optTempPoint.osmInputData(),
                                           optTempPoint.idfInputData(),
                                           optTempPoint.sqlOutputData(),
                                           optTempPoint.xmlOutputData(),
                                           optTempPoint.tags(),
                                           optTempPoint.topLevelJob(),
                                           optTempPoint.dakotaParametersFiles());
        optDataPoint.setName(optTempPoint.name());
        BOOST_ASSERT(optDataPoint.isDirty());
        dataPoints.push_back(optDataPoint);
      }
      else {
        DataPoint dataPoint(tempPoint.uuid(),
                            tempPoint.versionUUID(),
                            "fake name so can use name to make dirty",
                            tempPoint.displayName(),
                            tempPoint.description(),
                            tempPoint.isComplete(),
                            tempPoint.failed(),
                            tempPoint.problem(),
                            variableValues,
                            tempPoint.responseValues(),
                            tempPoint.directory(),
                            tempPoint.osmInputData(),
                            tempPoint.idfInputData(),
                            tempPoint.sqlOutputData(),
                            tempPoint.xmlOutputData(),
                            tempPoint.tags(),
                            tempPoint.topLevelJob(),
                            tempPoint.dakotaParametersFiles());
        dataPoint.setName(tempPoint.name());
        BOOST_ASSERT(dataPoint.isDirty());
        dataPoints.push_back(dataPoint);
      }
    }
    BOOST_ASSERT(temp.problem().isDirty());
    m_analysis = Analysis(temp.uuid(),
                          temp.versionUUID(),
                          "fake name so can use name to make dirty",
                          temp.displayName(),
                          temp.description(),
                          temp.problem(),
                          temp.algorithm(),
                          temp.seed(),
                          temp.weatherFile(),
                          dataPoints,
                          false,
                          false);
    m_analysis->setName(temp.name());
  }
  BOOST_ASSERT(m_analysis->isDirty());

  return true;
}

void SimpleProject::save() const
{
  analysis::Analysis analysis = this->analysis();
  AnalysisDriver analysisDriver = this->analysisDriver();
  saveAnalysis(analysis, analysisDriver);
}

bool SimpleProject::saveAs(const openstudio::path& newProjectDir) const {
  if (!boost::filesystem::exists(newProjectDir)) {
    bool ok = boost::filesystem::create_directories(newProjectDir);
    if (!ok) {
      LOG(Error,"Cannot copy this SimpleProject to " << toString(newProjectDir) << ", because "
          << "the directory cannot be created.");
      return false;
    }
  }

  if (!boost::filesystem::is_directory(newProjectDir)) {
    LOG(Error,"Cannot copy this SimpleProject to " << toString(newProjectDir) << ", because "
        << "the path exists but is not a directory.");
    return false;
  }

  for (openstudio::directory_iterator it(newProjectDir),
       itEnd = openstudio::directory_iterator(); it != itEnd; ++it)
  {
    if (boost::filesystem::is_directory(it->status()) ||
        boost::filesystem::is_regular_file(it->status()))
    {
      LOG(Error,"Cannot copy this SimpleProject to " << toString(newProjectDir) << ", because "
          << "the directory is not empty.");
      return false;
    }
  }

  removeDirectory(newProjectDir);
  return copyDirectory(projectDir(),newProjectDir);
}

void SimpleProject::onSeedChanged() const {
  m_seedModel.reset();
  m_seedIdf.reset();
}

SimpleProject::SimpleProject(const openstudio::path& projectDir,
                             const analysisdriver::AnalysisDriver& analysisDriver,
                             const boost::optional<analysis::Analysis>& analysis,
                             const SimpleProjectOptions& options)
  : m_projectDir(projectDir),
    m_analysisDriver(analysisDriver),
    m_analysis(analysis),
    m_logFile(projectDir / toPath("project.log"))
{
  if (m_analysis) {
    m_analysis->connect(SIGNAL(seedChanged()),this,SLOT(onSeedChanged()));
  }
  m_logFile.setLogLevel(options.logLevel());
  BOOST_ASSERT(runManager().paused());
  if (!options.pauseRunManagerQueue()) {
    runManager().setPaused(false);
  }
}

openstudio::path SimpleProject::alternateModelsDir() const {
  return projectDir() / toPath("alternatives");
}

openstudio::path SimpleProject::scriptsDir() const {
  return projectDir() / toPath("scripts");
}

openstudio::path SimpleProject::seedDir() const {
  return projectDir() / toPath("seed");
}

std::vector<openstudio::path> SimpleProject::alternateModelPaths() const {
  std::vector<openstudio::path> result;
  if (boost::filesystem::exists(alternateModelsDir())) {
    for (openstudio::directory_iterator it(alternateModelsDir()), itend; it != itend; ++it) {
      if (boost::filesystem::is_regular_file(it->path())) {
        FileReference temp(it->path());
        if (temp.fileType() == FileReferenceType::OSM) {
          result.push_back(it->path());
        }
      }
    }
  }
  return result;
}

bool SimpleProject::copyModel(const openstudio::path& modelPath,
                              const openstudio::path& destinationDirectory)
{
  bool result(true);

  if (!boost::filesystem::exists(modelPath)){
    LOG(Error,"Cannot copy model from " << toString(modelPath) << ", that path does not exist.");
    return false;
  }

  if (!boost::filesystem::exists(destinationDirectory)){
    bool ok = boost::filesystem::create_directories(destinationDirectory);
    if (!ok){
      LOG(Error,"Cannot create directory " << toString(destinationDirectory) << ".");
      return false;
    }
  }

  BOOST_ASSERT(boost::filesystem::exists(modelPath));
  BOOST_ASSERT(boost::filesystem::exists(destinationDirectory));

  // copy primary file
  openstudio::path newPath = destinationDirectory / toPath(modelPath.filename());
  boost::filesystem::copy_file(modelPath,newPath,boost::filesystem::copy_option::overwrite_if_exists);

  // pick up auxillary data
  openstudio::path companionFolder = modelPath.parent_path() / toPath(modelPath.stem());
  if (boost::filesystem::exists(companionFolder) &&
      boost::filesystem::is_directory(companionFolder))
  {
    openstudio::path companionDestination = destinationDirectory / toPath(modelPath.stem());
    copyDirectory(companionFolder,companionDestination);
    // clean out data that is irrelevant/conflicting in the SimpleProject context

    // ETH@20130619 - Was deleting run.db and run folder. However, now run.db is required
    // to open model-specific project.osp, so cannot delete that. Will go ahead and leave
    // run folder too.
  }

  return result;
}

bool SimpleProject::requiresVersionTranslation(const openstudio::path& modelPath) const {
  boost::optional<VersionString> modelVersion = IdfFile::loadVersionOnly(modelPath);
  if (!modelVersion) {
    LOG(Warn,"The version of '" << toString(modelPath) << "' could not be determined.");
    return false;
  }

  if (modelVersion.get() < VersionString(openStudioVersion())) {
    return true;
  }

  if (modelVersion.get() > VersionString(openStudioVersion())) {
    LOG(Error,"Version '" << modelVersion.get().str() << "' extracted from file '"
        << toString(modelPath) << "' is not supported by OpenStudio Version "
        << openStudioVersion() << ". Please check http://openstudio.nrel.gov for updates.");
  }

  return false;
}

bool SimpleProject::upgradeModel(const openstudio::path& modelPath, ProgressBar* progressBar) {
  VersionTranslator translator;
  if (requiresVersionTranslation(modelPath)) {
    OptionalModel upgradedModel = translator.loadModel(modelPath, progressBar);
    if (upgradedModel) {
      upgradedModel->save(modelPath,true);
      return true;
    }
  }
  return false;
}

bool SimpleProject::setAnalysisWeatherFile(ProgressBar* progressBar) {
  // get seedModel weather file object
  OptionalModel model = seedModel(progressBar);
  if (!model) {
    LOG(Info,"Could not set analysis weather file because could not load seed model.");
    return false;
  }
  OptionalWeatherFile weatherFile = model->getOptionalUniqueModelObject<WeatherFile>();
  if (!weatherFile) {
    LOG(Info,"Could not set analysis weather file because seed model has no weather file set.");
    return false;
  }
  boost::optional<openstudio::path> wfp = weatherFile->path();
  if (!wfp) {
    LOG(Info,"Could not set analysis weather file because seed model weather file has no path.");
    return false;
  }

  bool weatherFileLocated(false);
  openstudio::path modelPath = analysis().seed().path();
  openstudio::path companionFolder = completeAndNormalize(modelPath.parent_path() / toPath(modelPath.stem()));
  openstudio::path destinationFolder = companionFolder / toPath("files");

  // if absolute path, copy to companionFolder/files, set url relative to companionFolder
  openstudio::path p;
  if (wfp->is_complete() && boost::filesystem::exists(completeAndNormalize(*wfp))) {
    p = completeAndNormalize(*wfp);
    weatherFileLocated = true;
  }

  // if relative path, see if exists relative to companionFolder or destination folder
  if (!weatherFileLocated) {
    openstudio::path filename = toPath(wfp->filename());
    p = completeAndNormalize(companionFolder / *wfp);
    openstudio::path p2 = completeAndNormalize(companionFolder / filename);
    if (!(boost::filesystem::exists(p) || boost::filesystem::exists(p2))) {
      p = completeAndNormalize(destinationFolder / *wfp);
      p2 = completeAndNormalize(destinationFolder / filename);
    }
    if (boost::filesystem::exists(p)) {
      weatherFileLocated = true;
    }
    else if (boost::filesystem::exists(p2)) {
      p = p2;
      weatherFileLocated = true;
    }
  }

  // TODO: use RunManager WeatherFileFinder with default weather data location

  if (!weatherFileLocated) {
    LOG(Info,"Could not set analysis weather file because could not locate weather file referenced by seed model.");
    return false;
  }

  // if one of the above methods successful, compose relative path in model,
  // compose absolute path and set on analysis
  BOOST_ASSERT(weatherFileLocated);

  // copy weather file into standard location if necessary
  if (!(p.parent_path() == destinationFolder)) {
    if (!boost::filesystem::exists(destinationFolder)) {
      boost::filesystem::create_directories(destinationFolder);
    }
    boost::filesystem::copy_file(p,destinationFolder / toPath(p.filename()),
                                 boost::filesystem::copy_option::overwrite_if_exists);
  }

  // set weather file path in OSM in uniform way
  bool ok = weatherFile->makeUrlRelative(); // totally relative
  BOOST_ASSERT(ok);
  ok = weatherFile->makeUrlAbsolute(destinationFolder); // totally absolute
  BOOST_ASSERT(ok);
  wfp = weatherFile->path();
  BOOST_ASSERT(wfp);
  ok = weatherFile->makeUrlRelative(companionFolder); // relative to companionFolder
  BOOST_ASSERT(ok);
  model->save(modelPath,true);

  analysis().setWeatherFile(FileReference(*wfp));

  return true;
}

std::vector<BCLMeasure> SimpleProject::importSeedModelMeasures(ProgressBar* progressBar) {
  BCLMeasureVector result;
  openstudio::path projectPath = seedDir() / toPath(seed().path().stem());
  if (boost::filesystem::exists(projectPath)) {

    // open seed model project
    SimpleProjectOptions options;
    options.setPauseRunManagerQueue(true); // do not start running when opening
    options.setLogLevel(Debug);
    if (OptionalSimpleProject sp = SimpleProject::open(projectPath, options)) {
      // loop through fixed measures and add to this project
      Problem problem = analysis().problem();

      // ETH: Fixed measures would be more distinct if they were WorkItems, not
      // DiscreteVariables.
      
      int fixedMeasureInsertIndex(0);
      if (OptionalDiscreteVariable altModelVar = getAlternativeModelVariable()) {
        if (OptionalInt altModelVarIndex = problem.getVariableIndexByUUID(altModelVar->uuid())) {
          fixedMeasureInsertIndex = *altModelVarIndex + 1;
        }
      }

      bool previousMeasureWasModelMeasure(true);
      BOOST_FOREACH(const WorkflowStep& seedModelStep,sp->analysis().problem().workflow()) {
        if (isPATFixedMeasure(seedModelStep)) {

          RubyPerturbation fixedMeasure = seedModelStep.inputVariable().cast<DiscreteVariable>().perturbations(false)[0].cast<RubyPerturbation>();
          
          // change insert index if this is first EnergyPlus measure
          if ((fixedMeasure.inputFileType() == FileReferenceType(FileReferenceType::IDF)) && 
              (previousMeasureWasModelMeasure)) 
          {
            // first choice placement is after ExpandObjects job
            OptionalInt oIndex = problem.getWorkflowStepIndexByJobType(JobType::ExpandObjects);
            // second is after ModelToIdf job
            if (!oIndex) {
              oIndex = problem.getWorkflowStepIndexByJobType(JobType::ModelToIdf);
            }
            // third is to try to put ModelToIdf job at the end, then EnergyPlus measures
            if (!oIndex) {
              if (problem.push(WorkItem(JobType::ModelToIdf))) {
                oIndex = problem.numWorkflowSteps() - 1;
              }
            }
            if (oIndex) {
              fixedMeasureInsertIndex = *oIndex + 1;
            }
            else {
              LOG(Info,"Could not insert EnergyPlus measures from new seed model, "
                  << "because could not locate appropriate place in workflow to put them.");
              return result;
            }
          }

          // see if measure is already in project
          OptionalBCLMeasure seedMeasure = fixedMeasure.measure();
          if (seedMeasure) {
            OptionalBCLMeasure projectMeasure = getMeasureByUUID(seedMeasure->uuid());
            if (projectMeasure && (*projectMeasure != *seedMeasure)) {
              result.push_back(*seedMeasure);
            }
            if (!projectMeasure) {
              projectMeasure = insertMeasure(*seedMeasure);
            }
            BOOST_ASSERT(projectMeasure);
            // add new variable with that measure, same arguments as seed model
            RubyPerturbation newMeasure(*projectMeasure);
            newMeasure.setName(fixedMeasure.name());
            newMeasure.setDisplayName(fixedMeasure.displayName());
            newMeasure.setDescription(fixedMeasure.description());
            newMeasure.setArguments(fixedMeasure.arguments());
            DiscreteVariable newDVar(projectMeasure->name(),DiscretePerturbationVector(1u,newMeasure));
            problem.insert(fixedMeasureInsertIndex,newDVar);
            ++fixedMeasureInsertIndex;
          }
        }        
      }
    }
  }
  return result;
}

bool SimpleProject::isPATFixedMeasure(const WorkflowStep& step) const {
  if (!step.isInputVariable()) {
    return false;
  }
  InputVariable ivar = step.inputVariable();
  if (!ivar.optionalCast<DiscreteVariable>()) {
    return false;
  }
  DiscreteVariable dvar = ivar.cast<DiscreteVariable>();
  if (dvar.numPerturbations(false) != 1u) {
    return false;
  }
  DiscretePerturbation dpert = dvar.getPerturbation(0);
  if (!dpert.optionalCast<RubyPerturbation>()) {
    return false;
  }
  RubyPerturbation fixedMeasure = dpert.cast<RubyPerturbation>();
  if (!fixedMeasure.usesBCLMeasure()) {
    return false;
  }
  return true;     
}

void SimpleProject::removeOrphanedResultFiles() {
  // iterate through projectDir()
  for(openstudio::directory_iterator it(projectDir()), endit; it != endit; ++it) {
    if (boost::filesystem::is_directory(it->status())) {
      boost::regex dpDir("dataPoint\\d+");
      boost::regex dakotaDir("[dD]akota");
      std::string folderName = toString(it->path().stem());
      // if is dataPoint\d+ or [dD]akota
      if (boost::regex_match(folderName,dpDir) || boost::regex_match(folderName,dakotaDir)) {
        try {
          boost::filesystem::remove_all(it->path());
        }
        catch (...) {}
      }
    }
  }
}

BCLMeasure SimpleProject::addMeasure(const BCLMeasure& measure) {
  openstudio::path scriptsDir = this->scriptsDir();
  openstudio::path dir = scriptsDir / toPath(measure.directory().stem());

  // DLM: boost::filesystem::exists can throw if the dir is in an indeterminate state (e.g. being deleted)
  bool fileExists = true;
  try {
    fileExists = boost::filesystem::exists(dir);
  }catch(std::exception&){
  }

  int suffix = 1;
  while (fileExists) {
    std::stringstream ss;
    ss << toString(measure.directory().stem()) << " " << suffix;
    dir = scriptsDir / toPath(ss.str());
    ++suffix;

    // DLM: boost::filesystem::exists can throw if the dir is in an indeterminate state (e.g. being deleted)
    fileExists = true;
    try {
      fileExists = boost::filesystem::exists(dir);
    }catch(std::exception&){
    }
  }
  OptionalBCLMeasure measureCopy = measure.clone(dir);
  if (!measureCopy) {
    LOG_AND_THROW("Unable to copy measure at " << toString(measure.directory()) << " to "
                  << toString(dir) << ".");
  }
  BCLMeasure result = *measureCopy;
  std::map<UUID,BCLMeasure>::iterator it1 = m_measures.find(result.uuid());
  if (it1 != m_measures.end()) {
    m_measures.erase(it1);
  }
  std::pair<std::map<UUID,BCLMeasure>::const_iterator,bool> insertResult =
      m_measures.insert(std::map<UUID,BCLMeasure>::value_type(result.uuid(),result));
  BOOST_ASSERT(insertResult.second);
  std::map<UUID,std::vector<ruleset::OSArgument> >::iterator it2 = m_measureArguments.find(result.uuid());
  if (it2 != m_measureArguments.end()) {
    m_measureArguments.erase(it2);
  }
  return result;
}

void SimpleProject::removeMeasure(const BCLMeasure& measure) {
  BOOST_ASSERT(completeAndNormalize(measure.directory().parent_path()) == completeAndNormalize(scriptsDir()));
  std::map<UUID,BCLMeasure>::iterator it1 = m_measures.find(measure.uuid());
  BOOST_ASSERT(it1 != m_measures.end());
  try {
    boost::filesystem::remove_all(measure.directory());
  }
  catch (std::exception& e) {
    LOG(Error,"Could not remove measure at " << toString(measure.directory())
        << " from file system, because " << e.what() << ".");
  }
  m_measures.erase(it1);
  std::map<UUID,std::vector<ruleset::OSArgument> >::iterator it2 = m_measureArguments.find(measure.uuid());
  if (it2 != m_measureArguments.end()) {
    m_measureArguments.erase(it2);
  }
}

BCLMeasure SimpleProject::overwriteMeasure(const BCLMeasure& measure) {
  OptionalBCLMeasure existing = getMeasureByUUID(measure.uuid());
  if (existing) {
    removeMeasure(*existing);
  }
  return addMeasure(measure);
}

openstudio::UUID SimpleProject::alternativeModelMeasureUUID() {
  return toUUID("{d234ecee-c118-44e7-a381-db0a8917d751}");
}

boost::optional<SimpleProject> createPATProject(const openstudio::path& projectDir,
                                                const SimpleProjectOptions& options)
{
  OptionalSimpleProject result = SimpleProject::create(projectDir,options);
  if (result) {
    Problem problem = result->analysis().problem();

    // add swap variable
    DiscreteVariable dvar("Alternative Model",DiscretePerturbationVector(1u,NullPerturbation()));
    problem.push(dvar);

    // set up simulation workflow
    problem.push(WorkItem(JobType::ModelToIdf));
    problem.push(WorkItem(JobType::ExpandObjects));
    problem.push(WorkItem(JobType::EnergyPlusPreProcess));
    problem.push(WorkItem(JobType::EnergyPlus));
    problem.push(WorkItem(JobType::OpenStudioPostProcess));
  }
  return result;
}

boost::optional<SimpleProject> openPATProject(const openstudio::path& projectDir,
                                              const SimpleProjectOptions& options)
{
  OptionalSimpleProject result = SimpleProject::open(projectDir,options);
  if (result) {
    bool save(false);

    // check for swap variable, try to add if not present
    if (!result->getAlternativeModelVariable()) {
      bool ok = result->insertAlternativeModelVariable();
      if (!ok) {
        result.reset();
        return result;
      }
      save = true;
    }

    if (!result->isPATProject()) {
      result.reset();
      return result;
    }

    // fix up workflow as needed
    Problem problem = result->analysis().problem();
    OptionalInt index = problem.getWorkflowStepIndexByJobType(JobType::ModelToIdf);
    if (!index) {
      problem.push(WorkItem(JobType(JobType::ModelToIdf)));
    }
    index = problem.getWorkflowStepIndexByJobType(JobType::ExpandObjects);
    if (!index) {
      problem.push(WorkItem(JobType(JobType::ExpandObjects)));
    }
    index = problem.getWorkflowStepIndexByJobType(JobType::EnergyPlusPreProcess);
    if (!index) {
      problem.push(WorkItem(JobType(JobType::EnergyPlusPreProcess)));
    }
    index = problem.getWorkflowStepIndexByJobType(JobType::EnergyPlus);
    if (!index) {
      problem.push(WorkItem(JobType(JobType::EnergyPlus)));
    }
    index = problem.getWorkflowStepIndexByJobType(JobType::OpenStudioPostProcess);
    if (!index) {
      problem.push(WorkItem(JobType(JobType::OpenStudioPostProcess)));
      save = true;
    }

    if (result && save) {
      result->save();
    }
  }
  return result;
}

boost::optional<SimpleProject> saveAs(const SimpleProject& project,
                                      const openstudio::path& newProjectDir)
{
  OptionalSimpleProject result;
  // creates copy of this project, as it currently is on disk, to newProjectDir
  bool ok = project.saveAs(newProjectDir);
  if (ok) {
    // opens the new project. this will fix up whatever paths are already there.
    result = SimpleProject::open(newProjectDir);
    BOOST_ASSERT(result);
    // save paths used for fixing up paths stored in database
    openstudio::path originalLocation, newLocation;
    { 
      ProjectDatabase database = result->projectDatabase();
      // save paths used for fixing up paths stored in database
      originalLocation = database.originalBasePath();
      newLocation = database.newBasePath();
      // delete analysis saved in new location
      AnalysisRecord analysisRecord = result->analysisRecord();
      bool didStartTransaction = database.startTransaction();
      database.removeRecord(analysisRecord);
      database.save();
      if (didStartTransaction) {
        database.commitTransaction();
      }
    }
    {
      // save clone of current analysis in new location
      ProjectDatabase database = result->projectDatabase();
      bool didStartTransaction = database.startTransaction();
      database.unloadUnusedCleanRecords();
      Analysis analysisCopy = project.analysis().clone().cast<Analysis>();
      AnalysisRecord analysisRecord(analysisCopy,database);
      database.save();
      if (didStartTransaction) {
        database.commitTransaction();
      }
      // fix up paths
      didStartTransaction = database.startTransaction();
      database.updatePathData(originalLocation,newLocation);
      database.save();
      if (didStartTransaction) {
        database.commitTransaction();
      }
    }
  }
  return result;
}

} // analysisdriver
} // openstudio
