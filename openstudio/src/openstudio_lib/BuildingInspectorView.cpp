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

#include <openstudio_lib/BuildingInspectorView.hpp>
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include <openstudio_lib/OSVectorController.hpp>
#include "../shared_gui_components/OSComboBox.hpp"
#include <openstudio_lib/OSDropZone.hpp>
#include <openstudio_lib/ModelObjectItem.hpp>

#include <model/Building.hpp>
#include <model/Building_Impl.hpp>
#include <model/SpaceType.hpp>
#include <model/SpaceType_Impl.hpp>
#include <model/DefaultConstructionSet.hpp>
#include <model/DefaultConstructionSet_Impl.hpp>
#include <model/DefaultScheduleSet.hpp>
#include <model/DefaultScheduleSet_Impl.hpp>
#include <model/Component.hpp>
#include <model/Component_Impl.hpp>
#include <model/ComponentData.hpp>
#include <model/ComponentData_Impl.hpp>

#include <utilities/idd/OS_Building_FieldEnums.hxx>
#include <utilities/core/Assert.hpp>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QColor>
#include <QScrollArea>
#include <QStackedWidget>

namespace openstudio {

// BuildingSpaceTypeVectorController

void BuildingSpaceTypeVectorController::onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle)
{
  if (index == OS_BuildingFields::SpaceTypeName){
    emit itemIds(makeVector());
  }
}

std::vector<OSItemId> BuildingSpaceTypeVectorController::makeVector()
{
  std::vector<OSItemId> result;
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    boost::optional<model::SpaceType> spaceType = building.spaceType();
    if (spaceType){
      result.push_back(modelObjectToItemId(*spaceType, false));
    }
  }
  return result;
}

void BuildingSpaceTypeVectorController::onRemoveItem(OSItem* item)
{
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    building.resetSpaceType();
  }
}

void BuildingSpaceTypeVectorController::onReplaceItem(OSItem * currentItem, const OSItemId& replacementItemId)
{
  onDrop(replacementItemId);
}

void BuildingSpaceTypeVectorController::onDrop(const OSItemId& itemId)
{
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
    if (modelObject){
      if (modelObject->optionalCast<model::SpaceType>()){
        if (this->fromComponentLibrary(itemId)){
          modelObject = modelObject->clone(m_modelObject->model());
        }
        building.setSpaceType(modelObject->cast<model::SpaceType>());
      }
    }else{
      boost::optional<model::Component> component = this->getComponent(itemId);
      if (component){
        if (component->primaryObject().optionalCast<model::SpaceType>()){
          boost::optional<model::ComponentData> componentData = m_modelObject->model().insertComponent(*component);
          if (componentData){
            if (componentData->primaryComponentObject().optionalCast<model::SpaceType>()){
              building.setSpaceType(componentData->primaryComponentObject().cast<model::SpaceType>());
            }
          }
        }
      }
    }
  }
}

// BuildingDefaultConstructionSetVectorController

void BuildingDefaultConstructionSetVectorController::onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle)
{
  if (index == OS_BuildingFields::DefaultConstructionSetName){
    emit itemIds(makeVector());
  }
}

std::vector<OSItemId> BuildingDefaultConstructionSetVectorController::makeVector()
{
  std::vector<OSItemId> result;
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    boost::optional<model::DefaultConstructionSet> defaultConstructionSet = building.defaultConstructionSet();
    if (defaultConstructionSet){
      result.push_back(modelObjectToItemId(*defaultConstructionSet, false));
    }
  }
  return result;
}

void BuildingDefaultConstructionSetVectorController::onRemoveItem(OSItem* item)
{
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    building.resetDefaultConstructionSet();
  }
}

void BuildingDefaultConstructionSetVectorController::onReplaceItem(OSItem * currentItem, const OSItemId& replacementItemId)
{
  onDrop(replacementItemId);
}

void BuildingDefaultConstructionSetVectorController::onDrop(const OSItemId& itemId)
{
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
    if (modelObject){
      if (modelObject->optionalCast<model::DefaultConstructionSet>()){
        if (this->fromComponentLibrary(itemId)){
          modelObject = modelObject->clone(m_modelObject->model());
        }
        building.setDefaultConstructionSet(modelObject->cast<model::DefaultConstructionSet>());
      }
    }
  }
}

// BuildingDefaultScheduleSetVectorController

void BuildingDefaultScheduleSetVectorController::onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle)
{
  if (index == OS_BuildingFields::DefaultScheduleSetName){
    emit itemIds(makeVector());
  }
}

std::vector<OSItemId> BuildingDefaultScheduleSetVectorController::makeVector()
{
  std::vector<OSItemId> result;
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    boost::optional<model::DefaultScheduleSet> defaultScheduleSet = building.defaultScheduleSet();
    if (defaultScheduleSet){
      result.push_back(modelObjectToItemId(*defaultScheduleSet, false));
    }
  }
  return result;
}

void BuildingDefaultScheduleSetVectorController::onRemoveItem(OSItem* item)
{
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    building.resetDefaultScheduleSet();
  }
}

void BuildingDefaultScheduleSetVectorController::onReplaceItem(OSItem * currentItem, const OSItemId& replacementItemId)
{
  onDrop(replacementItemId);
}

void BuildingDefaultScheduleSetVectorController::onDrop(const OSItemId& itemId)
{
  if (m_modelObject){
    model::Building building = m_modelObject->cast<model::Building>();
    boost::optional<model::ModelObject> modelObject = this->getModelObject(itemId);
    if (modelObject){
      if (modelObject->optionalCast<model::DefaultScheduleSet>()){
        if (this->fromComponentLibrary(itemId)){
          modelObject = modelObject->clone(m_modelObject->model());
        }
        building.setDefaultScheduleSet(modelObject->cast<model::DefaultScheduleSet>());
      }
    }
  }
}

// BuildingInspectorView

BuildingInspectorView::BuildingInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent )
  : ModelObjectInspectorView(model, true, parent)
{
  m_isIP = isIP;

  QWidget* hiddenWidget = new QWidget();
  this->stackedWidget()->insertWidget(0, hiddenWidget);

  QWidget* visibleWidget = new QWidget();
  this->stackedWidget()->insertWidget(1, visibleWidget);

  this->stackedWidget()->setCurrentIndex(0);

  QGridLayout* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7,7,7,7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  // name
  QVBoxLayout* vLayout = new QVBoxLayout();

  QLabel* label = new QLabel();
  label->setText("Name: ");
  label->setStyleSheet("QLabel { font: bold; }");
  vLayout->addWidget(label);

  m_nameEdit = new OSLineEdit();
  vLayout->addWidget(m_nameEdit);

  mainGridLayout->addLayout(vLayout,0,0,1,2);

  // building type and space type
  vLayout = new QVBoxLayout();

  label = new QLabel();
  label->setText("Building Type: ");
  label->setStyleSheet("QLabel { font: bold; }");
  vLayout->addWidget(label);

  m_buildingTypeComboBox = new OSComboBox();
  vLayout->addWidget(m_buildingTypeComboBox);

  vLayout->addStretch();

  mainGridLayout->addLayout(vLayout,1,0);

  vLayout = new QVBoxLayout();

  label = new QLabel();
  label->setText("Space Type: ");
  label->setStyleSheet("QLabel { font: bold; }");
  vLayout->addWidget(label);

  m_spaceTypeVectorController = new BuildingSpaceTypeVectorController();
  m_spaceTypeDropZone = new OSDropZone(m_spaceTypeVectorController);
  m_spaceTypeDropZone->setMinItems(0);
  m_spaceTypeDropZone->setMaxItems(1);
  m_spaceTypeDropZone->setItemsAcceptDrops(true);
  vLayout->addWidget(m_spaceTypeDropZone);

  vLayout->addStretch();

  mainGridLayout->addLayout(vLayout,1,1);

  // default construction set and default schedule set
  vLayout = new QVBoxLayout();

  label = new QLabel();
  label->setText("Default Construction Set: ");
  label->setStyleSheet("QLabel { font: bold; }");
  vLayout->addWidget(label);

  m_defaultConstructionSetVectorController = new BuildingDefaultConstructionSetVectorController();
  m_defaultConstructionSetDropZone = new OSDropZone(m_defaultConstructionSetVectorController);
  m_defaultConstructionSetDropZone->setMinItems(0);
  m_defaultConstructionSetDropZone->setMaxItems(1);
  m_defaultConstructionSetDropZone->setItemsAcceptDrops(true);
  vLayout->addWidget(m_defaultConstructionSetDropZone);

  vLayout->addStretch();

  mainGridLayout->addLayout(vLayout,2,0);

  vLayout = new QVBoxLayout();

  label = new QLabel();
  label->setText("Default Schedule Set: ");
  label->setStyleSheet("QLabel { font: bold; }");
  vLayout->addWidget(label);

  m_defaultScheduleSetVectorController = new BuildingDefaultScheduleSetVectorController();
  m_defaultScheduleSetDropZone = new OSDropZone(m_defaultScheduleSetVectorController);
  m_defaultScheduleSetDropZone->setMinItems(0);
  m_defaultScheduleSetDropZone->setMaxItems(1);
  m_defaultScheduleSetDropZone->setItemsAcceptDrops(true);
  vLayout->addWidget(m_defaultScheduleSetDropZone);

  vLayout->addStretch();

  mainGridLayout->addLayout(vLayout,2,1);

  // north axis and floor to floor height
  vLayout = new QVBoxLayout();

  label = new QLabel();
  label->setText("North Axis: ");
  label->setStyleSheet("QLabel { font: bold; }");
  vLayout->addWidget(label);

  bool isConnected = false;

  m_northAxisEdit = new OSQuantityEdit(m_isIP);
  isConnected = connect(this, SIGNAL(toggleUnitsClicked(bool)), m_northAxisEdit, SLOT(onUnitSystemChange(bool)));
  OS_ASSERT(isConnected);

  vLayout->addWidget(m_northAxisEdit);

  vLayout->addStretch();

  mainGridLayout->addLayout(vLayout,3,0);

  //vLayout = new QVBoxLayout();

  //label = new QLabel();
  //label->setText("Floor To Floor Height: ");
  //label->setStyleSheet("QLabel { font: bold; }");
  //vLayout->addWidget(label);

  //m_floorToFloorHeightEdit = new OSQuantityEdit(m_isIP);
  //isConnected = connect(this, SIGNAL(toggleUnitsClicked(bool)), , SLOT(onUnitSystemChange(bool)));
  //OS_ASSERT(isConnected);
  //vLayout->addWidget(m_floorToFloorHeightEdit);

  //vLayout->addStretch();

  //mainGridLayout->addLayout(vLayout,3,1);

  mainGridLayout->setColumnMinimumWidth(0, 80);
  mainGridLayout->setColumnMinimumWidth(1, 80);
  mainGridLayout->setColumnStretch(2,1);
  mainGridLayout->setRowMinimumHeight(0, 30);
  mainGridLayout->setRowMinimumHeight(1, 30);
  mainGridLayout->setRowMinimumHeight(2, 30);
  mainGridLayout->setRowMinimumHeight(3, 30);
  mainGridLayout->setRowStretch(4,1);
}

void BuildingInspectorView::onClearSelection()
{
  ModelObjectInspectorView::onClearSelection(); // call parent implementation
  detach();
}

void BuildingInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject)
{
  detach();
  model::Building building = modelObject.cast<model::Building>();
  attach(building);
  refresh();
}

void BuildingInspectorView::onUpdate()
{
  refresh();
}

void BuildingInspectorView::attach(openstudio::model::Building& building)
{
  m_nameEdit->bind(building, "name");

  m_buildingTypeComboBox->bind(building, "buildingType");

  m_spaceTypeVectorController->attach(building);
  m_spaceTypeVectorController->reportItems();

  m_defaultConstructionSetVectorController->attach(building);
  m_defaultConstructionSetVectorController->reportItems();

  m_defaultScheduleSetVectorController->attach(building);
  m_defaultScheduleSetVectorController->reportItems();

  m_northAxisEdit->bind(building, "northAxis", m_isIP, std::string("isNorthAxisDefaulted"));

  //m_floorToFloorHeightEdit->bind(building, "nominalFloortoFloorHeight", m_isIP, std::string("isNominalFloortoFloorHeightDefaulted"));

  this->stackedWidget()->setCurrentIndex(1);
}

void BuildingInspectorView::detach()
{
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_buildingTypeComboBox->unbind();
  m_spaceTypeVectorController->detach();
  m_defaultConstructionSetVectorController->detach();
  m_defaultScheduleSetVectorController->detach();
  m_northAxisEdit->unbind();
  //m_floorToFloorHeightEdit->unbind();
}

void BuildingInspectorView::refresh()
{
}

void BuildingInspectorView::toggleUnits(bool displayIP)
{
  m_isIP = displayIP;
}

} // openstudio

