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

#ifndef OPENSTUDIO_CONSTRUCTIONSTABCONTROLLER_H
#define OPENSTUDIO_CONSTRUCTIONSTABCONTROLLER_H

#include <openstudio_lib/MainTabController.hpp>

#include <boost/shared_ptr.hpp>

namespace openstudio {

class DefaultConstructionSetsController;
class ConstructionsController;
class MaterialsController;

class ConstructionsTabController : public MainTabController
{
  Q_OBJECT

  public:

    ConstructionsTabController(bool isIP, const openstudio::model::Model& model);

    virtual ~ConstructionsTabController();

    enum TabID
    {
      DEFAULT_CONSTRUCTIONS,
      CONSTRUCTIONS,
      MATERIALS
    };

  private:

    boost::shared_ptr<DefaultConstructionSetsController> m_defaultConstructionSetsController;
    boost::shared_ptr<ConstructionsController> m_constructionsController;
    boost::shared_ptr<MaterialsController> m_materialsController;
};

} // openstudio

#endif // OPENSTUDIO_CONSTRUCTIONSTABCONTROLLER_H
