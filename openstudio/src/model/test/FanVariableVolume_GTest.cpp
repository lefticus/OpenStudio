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

#include <gtest/gtest.h>
#include <model/AirLoopHVAC.hpp>
#include <model/Model.hpp>
#include <model/Node.hpp>
#include <model/Node_Impl.hpp>
#include <model/FanVariableVolume.hpp>
#include <model/ScheduleCompact.hpp>

using namespace openstudio;

TEST(FanVariableVolume,FanVariableVolume_FanVariableVolume)
{
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";

  ASSERT_EXIT ( 
  {  
     model::Model m; 

     model::ScheduleCompact s(m);

     model::FanVariableVolume fan(m,s); 

     exit(0); 
  } ,
    ::testing::ExitedWithCode(0), "" );
}

TEST(FanVariableVolume,FanVariableVolume_addToNode)
{
  model::Model m; 

  model::ScheduleCompact s(m);
  
  model::FanVariableVolume fan(m,s); 

  model::AirLoopHVAC airLoop(m);

  model::Node supplyOutletNode = airLoop.supplyOutletNode();

  fan.addToNode(supplyOutletNode);

  ASSERT_EQ( (unsigned)3, airLoop.supplyComponents().size() );
}


