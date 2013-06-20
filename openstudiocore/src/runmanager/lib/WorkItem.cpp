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

#include "WorkItem.hpp"
#include "ModelObjectPerturbationJob.hpp"

#include <runmanager/lib/RubyJobUtils.hpp>

#include <ruleset/ModelRuleset.hpp>

#include <utilities/core/FileReference.hpp>
#include <utilities/core/PathHelpers.hpp>

namespace openstudio {
namespace runmanager {

  WorkItem::WorkItem()
  {
  }

  WorkItem::WorkItem(openstudio::runmanager::JobType t_type)
    : type(t_type)
  {
  }

  WorkItem::WorkItem(openstudio::runmanager::JobType t_type, const openstudio::runmanager::Tools &t_tools,
      const openstudio::runmanager::JobParams &t_params, const openstudio::runmanager::Files &t_files,
      const std::string &t_jobkeyname)
    : type(t_type), tools(t_tools), params(t_params), files(t_files), jobkeyname(t_jobkeyname)
  {
  }

  WorkItem::WorkItem(const openstudio::ruleset::ModelRuleset& modelRuleset)
    : type(JobType::ModelObjectPerturbation),
      params(detail::ModelObjectPerturbationJob::formatRuleData(modelRuleset))
  {}

  bool WorkItem::operator==(const WorkItem &t_rhs) const
  {
    return type == t_rhs.type
      && tools == t_rhs.tools
      && params == t_rhs.params
      && files == t_rhs.files
      && jobkeyname == t_rhs.jobkeyname;
  }

  boost::optional<FileReferenceType> WorkItem::inputFileType() const {
    OptionalFileReferenceType result;
    switch (type.value()) {
      // job types where input file type is undefined/doesn't matter
      case JobType::Workflow :
      case JobType::Null :
      case JobType::Dakota :
      case JobType::EnergyPlusPostProcess :
      case JobType::OpenStudioPostProcess :
        break;
      // job types that take IDF
      case JobType::EnergyPlus :
      case JobType::ExpandObjects :
      case JobType::ParallelEnergyPlusSplit :
      case JobType::ParallelEnergyPlusJoin :
      case JobType::IdfToModel :
      case JobType::EnergyPlusPreProcess :
      case JobType::Basement :
      case JobType::Slab :
        result = FileReferenceType(FileReferenceType::IDF);
        break;
      // job types that take OSM
      case JobType::ModelToIdf :
      case JobType::ModelObjectPerturbation :
      case JobType::ModelToRad :
      case JobType::ModelToRadPreProcess :
        result = FileReferenceType(FileReferenceType::OSM);
        break;
      // job types that take XML
      case JobType::XMLPreprocessor :
        result = FileReferenceType(FileReferenceType::XML);
        break;
      case JobType::Ruby :
      case JobType::UserScript :
      {
        // get extension of argument 3 in RubyJobBuilder::addInputFile
        RubyJobBuilder builder(*this);
        std::vector< boost::tuple<FileSelection, FileSource, std::string, std::string> > inputFiles = builder.inputFiles();
        for (std::vector< boost::tuple<FileSelection, FileSource, std::string, std::string> >::const_iterator it = inputFiles.begin(),
             itEnd = inputFiles.end(); it != itEnd; ++it)
        {
          std::string ext = getFileExtension(toPath(it->get<2>()));
          try {
            result = FileReferenceType(ext);
            break;
          }
          catch (...) {}
        }
        break;
      }
      // job types that I am unsure about
      case JobType::CalculateEconomics :
      case JobType::ReadVars :
      case JobType::PreviewIES :
        break;
      default:
        LOG_AND_THROW("This JobType not handled yet.");
    }
    return result;
  }

  boost::optional<FileReferenceType> WorkItem::outputFileType() const {
    OptionalFileReferenceType result;
    switch (type.value()) {
      // job types where output file type is undefined/is not a mainline energy model
      case JobType::Workflow :
      case JobType::Null :
      case JobType::EnergyPlusPostProcess :
      case JobType::OpenStudioPostProcess :
      case JobType::Dakota :
        break;
      // job types that return IDF
      case JobType::ModelToIdf :
      case JobType::ExpandObjects :
      case JobType::XMLPreprocessor :
      case JobType::ParallelEnergyPlusSplit :
      case JobType::ParallelEnergyPlusJoin :
      case JobType::EnergyPlusPreProcess :
      case JobType::Basement :
      case JobType::Slab :
        result = FileReferenceType(FileReferenceType::IDF);
        break;
      // job types that return OSM
      case JobType::ModelObjectPerturbation :
      case JobType::IdfToModel :
      case JobType::ModelToRadPreProcess :
        result = FileReferenceType(FileReferenceType::OSM);
        break;
      // job types that return SQL
      case JobType::EnergyPlus :
        result = FileReferenceType(FileReferenceType::SQL);
        break;
      case JobType::Ruby :
      case JobType::UserScript :
      {
        // get argument 2 in RubyJobBuilder::copyRequiredFiles
        RubyJobBuilder builder(*this);
        std::vector< boost::tuple<std::string, std::string, std::string> > requiredFiles = builder.copyRequiredFiles();
        for (std::vector< boost::tuple<std::string, std::string, std::string> >::const_iterator it = requiredFiles.begin(),
             itEnd = requiredFiles.end(); it != itEnd; ++it)
        {
          std::string ext = it->get<1>();
          try {
            result = FileReferenceType(ext);
            break;
          }
          catch (...) {}
        }
        break;
      }
      // job types that I am unsure about
      case JobType::CalculateEconomics :
      case JobType::ReadVars :
      case JobType::PreviewIES :
        break;
      default:
        LOG_AND_THROW("This JobType not handled yet.");
    }
    return result;
  }

}
}

