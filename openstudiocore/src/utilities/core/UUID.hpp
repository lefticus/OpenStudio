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

#ifndef UTILITIES_CORE_UUID_HPP
#define UTILITIES_CORE_UUID_HPP

#include <utilities/CoreUtilitiesAPI.hpp>

#include <QUuid>
#include <QMetaType>
#include <boost/optional.hpp>
#include <vector>
#include <ostream>
#include <string>

namespace openstudio {

  /// Universally Unique Identifier
  typedef QUuid UUID;

  /// optional UUID
  typedef boost::optional<UUID> OptionalUUID;

  /// vector of UUID
  typedef std::vector<UUID> UUIDVector;

  /// create a UUID
  COREUTILITIES_API UUID createUUID();

  /// create a UUID from a std::string, does not throw, may return a null UUID
  COREUTILITIES_API UUID toUUID(const std::string& str);

  /// create a std::string from a UUID
  COREUTILITIES_API std::string toString(const UUID& uuid);

  /// create a unique name, prefix << " " << UUID.
  COREUTILITIES_API std::string createUniqueName(const std::string& prefix);

  /// create a std::string without curly brackets from a UUID
  COREUTILITIES_API std::string removeBraces(const UUID& uuid);

  COREUTILITIES_API std::ostream& operator<<(std::ostream& os,const UUID& uuid);

} // openstudio

Q_DECLARE_METATYPE(openstudio::UUID);


#endif // UTILITIES_CORE_UUID_HPP
