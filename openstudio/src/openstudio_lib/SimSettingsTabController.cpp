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

#include <openstudio_lib/SimSettingsTabController.hpp>

#include <openstudio_lib/SimSettingsTabView.hpp>

#include <model/Model.hpp>
#include <model/Model_Impl.hpp>

#include <utilities/core/Assert.hpp>

namespace openstudio {

SimSettingsTabController::SimSettingsTabController(bool isIP, const model::Model & model)
  : MainTabController(new SimSettingsTabView(isIP,model,"Simulation Settings",false))
{
  bool isConnected = connect(this,SIGNAL(toggleUnitsClicked(bool)),
                             mainContentWidget(),SIGNAL(toggleUnitsClicked(bool)));
  OS_ASSERT(isConnected);
}

void SimSettingsTabController::toggleUnits(bool displayIP)
{
}

} // openstudio
