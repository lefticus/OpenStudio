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

#include <model/test/ModelFixture.hpp>
#include <model/Space.hpp>
#include <model/Surface.hpp>
#include <model/Surface_Impl.hpp>
#include <model/SubSurface.hpp>
#include <model/SubSurface_Impl.hpp>
#include <model/Model_Impl.hpp>

#include <utilities/geometry/Geometry.hpp>
#include <utilities/geometry/Point3d.hpp>
#include <utilities/geometry/Vector3d.hpp>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>

using namespace openstudio;
using namespace openstudio::model;

/* HAS TO WAIT UNTIL WE GET A GOOD OSM EXAMPLE
TEST_F(ModelFixture, SubSurface_In_File)
{
  Workspace workspace(idfFile);

  // translate energyplus simulation to building model
  energyplus::ReverseTranslator reverseTranslator(workspace);
  OptionalModel optionalModel = reverseTranslator.convert();
  ASSERT_TRUE(optionalModel);
  Model model(*optionalModel);

  model.setSqlFile(sqlFile);
  ASSERT_TRUE(model.sqlFile());

  SubSurfaceVector subSurfaces = model.getModelObjects<SubSurface>();
  EXPECT_TRUE(subSurfaces.size() > 0);

  BOOST_FOREACH(const SubSurface& subSurface, subSurfaces){
    OptionalString name = subSurface.name();
    ASSERT_TRUE(name);

    OptionalString type = subSurface.getString(FenestrationSurface_DetailedFields::SurfaceType);
    ASSERT_TRUE(type);

    // compute metrics from input object
    double area = subSurface.grossArea();
    EXPECT_TRUE(area > 0);

    // TODO: check tilt and azimuth too

    // query to get area from sql file
    std::string query = "SELECT Area FROM surfaces WHERE SurfaceName='" + to_upper_copy(*name) + "'";
    OptionalDouble sqlArea = model.sqlFile()->execAndReturnFirstDouble(query);
    ASSERT_TRUE(sqlArea);
    EXPECT_TRUE(*sqlArea > 0);
    EXPECT_NEAR(*sqlArea, area, 0.000001);

    // query to get gross area from sql file
    query = "SELECT GrossArea FROM surfaces WHERE SurfaceName='" + to_upper_copy(*name) + "'";
    OptionalDouble sqlGrossArea = model.sqlFile()->execAndReturnFirstDouble(query);
    ASSERT_TRUE(sqlGrossArea);
    EXPECT_TRUE(*sqlGrossArea > 0);
    EXPECT_NEAR(*sqlGrossArea, area, 0.000001);

    // check visible transmittance
    if (istringEqual("Door", *type)){
      OptionalDouble tvis = subSurface.visibleTransmittance();
      ASSERT_TRUE(tvis);
      EXPECT_EQ(0.0, *tvis);
    }else{
      OptionalDouble tvis = subSurface.visibleTransmittance();
      ASSERT_TRUE(tvis);
      EXPECT_TRUE(*tvis > 0.0);
    }
  }
}
*/

TEST_F(ModelFixture, 0_Vertex_SubSurface)
{
  Model model;

  std::vector<Point3d> vertices;

  EXPECT_THROW(SubSurface(vertices, model), openstudio::Exception);
}

TEST_F(ModelFixture, 1_Vertex_SubSurface)
{
  Model model;

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,0,1));

  EXPECT_THROW(SubSurface(vertices, model), openstudio::Exception);
}

TEST_F(ModelFixture, 2_Vertex_SubSurface)
{
  Model model;

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,0,1));
  vertices.push_back(Point3d(0,0,0));

  EXPECT_THROW(SubSurface(vertices, model), openstudio::Exception);
}

TEST_F(ModelFixture, 3_Vertex_SubSurface)
{
  Model model;

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,0,1));
  vertices.push_back(Point3d(0,0,0));
  vertices.push_back(Point3d(1,0,0));

  EXPECT_NO_THROW(SubSurface(vertices, model));
}

TEST_F(ModelFixture, 4_Vertex_SubSurface)
{
  Model model;

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,0,1));
  vertices.push_back(Point3d(0,0,0));
  vertices.push_back(Point3d(1,0,0));
  vertices.push_back(Point3d(1,0,1));

  EXPECT_NO_THROW(SubSurface(vertices, model));
}

TEST_F(ModelFixture, 5_Vertex_SubSurface)
{
  Model model;

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,0,1));
  vertices.push_back(Point3d(0,0,0));
  vertices.push_back(Point3d(1,0,0));
  vertices.push_back(Point3d(1,0,1));
  vertices.push_back(Point3d(0.5,0,1.5));

  EXPECT_NO_THROW(SubSurface(vertices, model));
}

TEST_F(ModelFixture, AdjacentSubSurface)
{
  Model model;

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,0,3));
  vertices.push_back(Point3d(0,0,0));
  vertices.push_back(Point3d(3,0,0));
  vertices.push_back(Point3d(3,0,3));

  Space space1(model);
  Surface wall1(vertices, model);
  wall1.setSpace(space1);
  EXPECT_FALSE(wall1.adjacentSurface());

  std::reverse(vertices.begin(), vertices.end());

  Space space2(model);
  Surface wall2(vertices, model);
  wall2.setSpace(space2);
  EXPECT_FALSE(wall2.adjacentSurface());

  vertices.clear();
  vertices.push_back(Point3d(1,0,2));
  vertices.push_back(Point3d(1,0,1));
  vertices.push_back(Point3d(2,0,1));
  vertices.push_back(Point3d(2,0,2));

  SubSurface window1(vertices, model);
  EXPECT_FALSE(window1.adjacentSubSurface());

  std::reverse(vertices.begin(), vertices.end());

  SubSurface window2(vertices, model);
  EXPECT_FALSE(window2.adjacentSubSurface());

  EXPECT_FALSE(window1.setAdjacentSubSurface(window2));

  EXPECT_TRUE(wall1.setAdjacentSurface(wall2));
  EXPECT_TRUE(wall1.setAdjacentSurface(wall2));
  EXPECT_TRUE(wall2.setAdjacentSurface(wall1));
  EXPECT_TRUE(wall2.setAdjacentSurface(wall1));

  EXPECT_FALSE(window1.adjacentSubSurface());

  EXPECT_FALSE(window1.setAdjacentSubSurface(window2));

  window1.setSurface(wall1);
  window2.setSurface(wall2);

  EXPECT_TRUE(window1.setAdjacentSubSurface(window2));
  EXPECT_TRUE(window1.setAdjacentSubSurface(window2));
  EXPECT_TRUE(window2.setAdjacentSubSurface(window1));
  EXPECT_TRUE(window2.setAdjacentSubSurface(window1));

  ASSERT_TRUE(window1.adjacentSubSurface());
  EXPECT_EQ(window2.handle(), window1.adjacentSubSurface()->handle());
  ASSERT_TRUE(window2.adjacentSubSurface());
  EXPECT_EQ(window1.handle(), window2.adjacentSubSurface()->handle());

  wall1.resetAdjacentSurface();
  EXPECT_FALSE(wall1.adjacentSurface());
  EXPECT_FALSE(wall2.adjacentSurface());
  EXPECT_FALSE(window1.adjacentSubSurface());
  EXPECT_FALSE(window2.adjacentSubSurface());
}

TEST_F(ModelFixture, AdjacentSubSurface2)
{
  Model model;

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,0,3));
  vertices.push_back(Point3d(0,0,0));
  vertices.push_back(Point3d(3,0,0));
  vertices.push_back(Point3d(3,0,3));

  Space space1(model);
  Surface wall1(vertices, model);
  wall1.setSpace(space1);
  EXPECT_FALSE(wall1.adjacentSurface());

  std::reverse(vertices.begin(), vertices.end());

  Space space2(model);
  Surface wall2(vertices, model);
  wall2.setSpace(space2);
  EXPECT_FALSE(wall2.adjacentSurface());

  vertices.clear();
  vertices.push_back(Point3d(1,0,2));
  vertices.push_back(Point3d(1,0,1));
  vertices.push_back(Point3d(2,0,1));
  vertices.push_back(Point3d(2,0,2));

  SubSurface window1(vertices, model);
  window1.setSurface(wall1);
  EXPECT_EQ("FixedWindow", window1.subSurfaceType());
  EXPECT_FALSE(window1.adjacentSubSurface());

  std::reverse(vertices.begin(), vertices.end());

  SubSurface window2(vertices, model);
  window2.setSurface(wall2);
  EXPECT_EQ("FixedWindow", window2.subSurfaceType());
  EXPECT_TRUE(window2.setSubSurfaceType("OperableWindow"));
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());
  EXPECT_FALSE(window2.adjacentSubSurface());

  EXPECT_TRUE(wall1.setAdjacentSurface(wall2));
  
  EXPECT_TRUE(window1.setAdjacentSubSurface(window2));
  EXPECT_EQ("OperableWindow", window1.subSurfaceType());
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());

  EXPECT_TRUE(window1.setSubSurfaceType("FixedWindow"));
  EXPECT_EQ("FixedWindow", window1.subSurfaceType());
  EXPECT_EQ("FixedWindow", window2.subSurfaceType());

  EXPECT_TRUE(window2.setSubSurfaceType("OperableWindow"));
  EXPECT_EQ("OperableWindow", window1.subSurfaceType());
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());

  window1.resetAdjacentSubSurface();

  EXPECT_TRUE(window1.setSubSurfaceType("FixedWindow"));
  EXPECT_EQ("FixedWindow", window1.subSurfaceType());
  EXPECT_TRUE(window2.setSubSurfaceType("OperableWindow"));
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());

  EXPECT_TRUE(window2.setAdjacentSubSurface(window1));
  EXPECT_EQ("OperableWindow", window1.subSurfaceType());
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());

  window1.resetAdjacentSubSurface();

  EXPECT_TRUE(window1.setSubSurfaceType("OverheadDoor"));
  EXPECT_EQ("OverheadDoor", window1.subSurfaceType());
  EXPECT_TRUE(window2.setSubSurfaceType("OperableWindow"));
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());

  EXPECT_TRUE(window2.setAdjacentSubSurface(window1));
  EXPECT_EQ("OperableWindow", window1.subSurfaceType());
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());

  window1.resetAdjacentSubSurface();

  EXPECT_TRUE(window1.setSubSurfaceType("OverheadDoor"));
  EXPECT_EQ("OverheadDoor", window1.subSurfaceType());
  EXPECT_TRUE(window2.setSubSurfaceType("OperableWindow"));
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());

  EXPECT_TRUE(window1.setAdjacentSubSurface(window2));
  EXPECT_EQ("OverheadDoor", window1.subSurfaceType());
  EXPECT_EQ("OverheadDoor", window2.subSurfaceType());
}

TEST_F(ModelFixture, AdjacentSubSurface3)
{
  Model model;

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,0,3));
  vertices.push_back(Point3d(0,0,0));
  vertices.push_back(Point3d(3,0,0));
  vertices.push_back(Point3d(3,0,3));

  Space space1(model);
  Surface wall1(vertices, model);
  wall1.setSpace(space1);
  EXPECT_FALSE(wall1.adjacentSurface());

  std::reverse(vertices.begin(), vertices.end());

  Space space2(model);
  Surface wall2(vertices, model);
  wall2.setSpace(space2);
  EXPECT_FALSE(wall2.adjacentSurface());

  vertices.clear();
  vertices.push_back(Point3d(1,0,2));
  vertices.push_back(Point3d(1,0,1));
  vertices.push_back(Point3d(2,0,1));
  vertices.push_back(Point3d(2,0,2));

  SubSurface window1(vertices, model);
  window1.setSurface(wall1);
  EXPECT_EQ("FixedWindow", window1.subSurfaceType());
  EXPECT_FALSE(window1.adjacentSubSurface());

  std::reverse(vertices.begin(), vertices.end());

  SubSurface window2(vertices, model);
  window2.setSurface(wall2);
  EXPECT_EQ("FixedWindow", window2.subSurfaceType());
  EXPECT_TRUE(window2.setSubSurfaceType("OperableWindow"));
  EXPECT_EQ("OperableWindow", window2.subSurfaceType());
  EXPECT_FALSE(window2.adjacentSubSurface());

  EXPECT_TRUE(wall1.setAdjacentSurface(wall2));

  EXPECT_TRUE(window1.setMultiplier(4));

  EXPECT_FALSE(window1.setAdjacentSubSurface(window2));
  EXPECT_EQ(4, window1.multiplier());
  EXPECT_EQ(1, window2.multiplier());

  window1.resetMultiplier();
  
  EXPECT_TRUE(window1.setAdjacentSubSurface(window2));
  EXPECT_EQ(1, window1.multiplier());
  EXPECT_EQ(1, window2.multiplier());

  EXPECT_TRUE(window1.setMultiplier(3));
  EXPECT_EQ(3, window1.multiplier());
  EXPECT_EQ(3, window2.multiplier());

  window1.resetMultiplier();
  EXPECT_EQ(1, window1.multiplier());
  EXPECT_EQ(1, window2.multiplier());
}

TEST_F(ModelFixture, ExampleDaylightingControlPlacement)
{
  Model model;

  Space space(model);

  std::vector<Point3d> vertices;
  vertices.push_back(Point3d(0,10,0));
  vertices.push_back(Point3d(10,10,0));
  vertices.push_back(Point3d(10,0,0));
  vertices.push_back(Point3d(0,0,0));
  Surface floor(vertices, model);
  floor.setSpace(space);
  EXPECT_EQ("Floor", floor.surfaceType());

  vertices.clear();
  vertices.push_back(Point3d(0,0,10));
  vertices.push_back(Point3d(0,0,0));
  vertices.push_back(Point3d(10,0,0));
  vertices.push_back(Point3d(10,0,10));
  Surface wall(vertices, model);
  wall.setSpace(space);
  EXPECT_EQ("Wall", wall.surfaceType());

  vertices.clear();
  vertices.push_back(Point3d(2,0,8));
  vertices.push_back(Point3d(2,0,2));
  vertices.push_back(Point3d(8,0,2));
  vertices.push_back(Point3d(8,0,8));
  SubSurface window(vertices, model);
  window.setSurface(wall);

  boost::optional<Point3d> windowCentroid = getCentroid(window.vertices());
  ASSERT_TRUE(windowCentroid);
  EXPECT_DOUBLE_EQ(5, windowCentroid->x());
  EXPECT_DOUBLE_EQ(0, windowCentroid->y());
  EXPECT_DOUBLE_EQ(5, windowCentroid->z());

  // move back 1 m
  Point3d windowCentroidPrime = windowCentroid.get() + (-1.0)*window.outwardNormal();
  EXPECT_DOUBLE_EQ(5, windowCentroidPrime.x());
  EXPECT_DOUBLE_EQ(1, windowCentroidPrime.y());
  EXPECT_DOUBLE_EQ(5, windowCentroidPrime.z());

  // project to plane of floor
  Plane floorPlane = floor.plane();
  Point3d pointOnFloor = floorPlane.project(windowCentroidPrime);
  EXPECT_DOUBLE_EQ(5, pointOnFloor.x());
  EXPECT_DOUBLE_EQ(1, pointOnFloor.y());
  EXPECT_DOUBLE_EQ(0, pointOnFloor.z());

  // move up 1 m
  Point3d point = pointOnFloor + Vector3d(0,0,1);
  EXPECT_DOUBLE_EQ(5, point.x());
  EXPECT_DOUBLE_EQ(1, point.y());
  EXPECT_DOUBLE_EQ(1, point.z());
}