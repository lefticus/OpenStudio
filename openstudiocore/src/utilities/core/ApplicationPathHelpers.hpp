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

#ifndef UTILITIES_CORE_APPLICATIONPATHHELPERS_HPP
#define UTILITIES_CORE_APPLICATIONPATHHELPERS_HPP

#include <utilities/CoreUtilitiesAPI.hpp>
#include <utilities/core/Path.hpp>
#include <vector>

namespace openstudio {

/** If possible, creates an openstudio::path to a system directory from outdir, otherwise throws 
 *  a std::runtime_error. For use by auto-generation applications that are writing files for 
 *  OpenStudio. Requires that the path represented by outdir exist up to the parent level of the
 *  final directory. Outputs informative messages to std::cout. */
COREUTILITIES_API path getApplicationOutputDirectory(const std::string& outdir);


  /// \returns The source directory the application was built from
  COREUTILITIES_API openstudio::path getApplicationSourceDirectory();

  /// \returns The directory the application was built in
  COREUTILITIES_API openstudio::path getApplicationBuildDirectory();

  /// \returns The directory the application was configured to be installed in (install prefix)
  COREUTILITIES_API openstudio::path getApplicationInstallDirectory();

  /// \returns The directory the application binary lives in
  COREUTILITIES_API openstudio::path getApplicationRunDirectory();

  /// \returns True if the application is running from the build directory
  COREUTILITIES_API bool applicationIsRunningFromBuildDirectory();

  /// \returns The bare paths of the openstudio ruby libraries, with no root specified
  COREUTILITIES_API std::vector<openstudio::path> getOpenStudioBareRubyPaths();
  
  /// \returns The locations of the openstudio ruby libraries appropriate for this application instance can be found
  COREUTILITIES_API std::vector<openstudio::path> getOpenStudioRubyPaths();

  /// \returns The directory the openstudio ruby library appropriate for this application instance can be found
  COREUTILITIES_API openstudio::path getOpenStudioRubyPath();

  /** \returns The directory that should be included when making calls to ruby scripts from a ruby
   *  job (to make require 'openstudio' work). */
  COREUTILITIES_API openstudio::path getOpenStudioRubyIncludePath();

  /// \returns The openstudio ruby scripts library appropriate for this application instance.
  COREUTILITIES_API openstudio::path getOpenStudioRubyScriptsPath();

  /// \returns The location to load resources from
  COREUTILITIES_API openstudio::path getSharedResourcesPath();

  /// \returns The location of the ruby installation intended for aws-sdk use with aws-sdk
  ///          gem already installed
  COREUTILITIES_API openstudio::path getOpenStudioAWSRubyPath();

  /// \returns The packaged ruby installation from the embedded ruby version.
  ///          Returns nothing on non-windows platforms
  COREUTILITIES_API openstudio::path getOpenStudioEmbeddedRubyPath();
} // openstudio

#endif //UTILITIES_CORE_PATHHELPERS_HPP
