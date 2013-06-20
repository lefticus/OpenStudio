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

#include <openstudio_lib/BuildingStoriesView.hpp>
#include <openstudio_lib/BuildingStoryInspectorView.hpp>
#include <openstudio_lib/ModelObjectListView.hpp>

#include <model/Model_Impl.hpp>
#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <sstream>

namespace openstudio {


BuildingStoriesView::BuildingStoriesView(const openstudio::model::Model& model, 
                                         QWidget * parent)
  : ModelSubTabView(new ModelObjectListView(IddObjectType::OS_BuildingStory, model, true, parent),
               new BuildingStoryInspectorView(model, parent),
               parent)
{
  ModelObjectListView* modelObjectListView = qobject_cast<ModelObjectListView*>(this->itemSelector());
  BOOST_ASSERT(modelObjectListView);
  modelObjectListView->setItemsDraggable(false);
}

} // openstudio
