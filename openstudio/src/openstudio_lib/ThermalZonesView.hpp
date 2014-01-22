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

#ifndef OPENSTUDIO_THERMALZONESVIEW_H
#define OPENSTUDIO_THERMALZONESVIEW_H

#include <openstudio_lib/OSVectorController.hpp>
#include <openstudio_lib/ModelObjectVectorController.hpp>
#include <openstudio_lib/ModelObjectInspectorView.hpp>
#include <openstudio_lib/ModelSubTabView.hpp>
#include <model/Model.hpp>
#include <model/ThermalZone.hpp>
#include <model/ZoneHVACComponent.hpp>
#include <model/Schedule.hpp>
#include <boost/smart_ptr.hpp>
#include <QWidget>
#include <QHash>
#include <QList>
#include <QUuid>
#include <QMouseEvent>
#include <QLabel>

class QVBoxLayout;

class QHBoxLayout;

class QToolButton;

class QPushButton;

class QStackedWidget;

class QCheckBox;

class QDropEvent;

class QDragEnterEvent;

class QDragLeaveEvent;

namespace openstudio {

class OSObjectListCBDS;

class WorkspaceObject;

class OSComboBox;

class SpacesOSVectorController;

class OSIntegerEdit;

class OSQuantityEdit;

class OSLineEdit;

class OSSwitch;

class OSDropZone;

namespace model {

class Schedule;

}

class ZoneEquipmentVectorController;

class ModelObjectPropertyView;

class ThermalZonesView : public ModelSubTabView
{
  Q_OBJECT

public:

  ThermalZonesView(bool isIP, const model::Model & model, QWidget * parent = 0);

  virtual ~ThermalZonesView() {}

};

class ThermalZoneView : public ModelObjectInspectorView
{
  class CoolingScheduleVectorController;
  class HeatingScheduleVectorController;

  Q_OBJECT

public:

  ThermalZoneView(bool isIP, const model::Model & model, QWidget * parent = 0 );

  virtual ~ThermalZoneView() {}

  boost::optional<model::ThermalZone> thermalZone();

  void paintEvent ( QPaintEvent * event );

public slots:

  void refresh();

  void refreshNow();

signals:

  void enableThermostatClicked(model::ThermalZone &, bool);

  void modelObjectSelected(model::OptionalModelObject & modelObject, bool readOnly);

  void toggleUnitsClicked(bool);

protected:

  void onClearSelection();

  void onSelectModelObject(const openstudio::model::ModelObject& modelObject);

  void onUpdate();

private slots:

  void onThermostatButtonClicked(bool);

  void onZoneHVACEquipmentItemClicked(OSItem* item);

  void onToggleUnitsClicked(bool);

private:

  bool m_isIP;

  OSLineEdit * m_nameEdit;

  OSSwitch * m_idealLoadsBox;

  QLabel * m_airLoopNameLabel;

  OSSwitch * m_thermostatButton;

  QWidget * m_zoneNameWidget;

  boost::shared_ptr<OSObjectListCBDS> m_scheduleDataSource;

  bool m_dirty;

  boost::shared_ptr<ZoneEquipmentVectorController> m_equipmentVectorController;

  boost::shared_ptr<ZoneEquipmentVectorController> m_spacesVectorController;

  CoolingScheduleVectorController * m_coolingThermostatVectorController;

  HeatingScheduleVectorController * m_heatingThermostatVectorController;

  OSDropZone * m_coolingThermostatDropZone;

  OSDropZone * m_heatingThermostatDropZone;

  OSQuantityEdit * m_coolingDesignSupplyAirTemperatureEdit;

  OSQuantityEdit * m_zoneHeatingDesignSupplyAirTemperatureEdit;

  OSQuantityEdit * m_zoneCoolingDesignSupplyAirHumidityRatioEdit;

  OSQuantityEdit * m_zoneHeatingDesignSupplyAirHumidityRatioEdit;

  OSQuantityEdit * m_zoneHeatingSizingFactorEdit;

  OSQuantityEdit * m_zoneCoolingSizingFactorEdit;

  OSComboBox * m_coolingDesignAirFlowMethodCB;

  OSQuantityEdit * m_coolingDesignAirFlowRateEdit;

  OSQuantityEdit * m_heatingMaximumAirFlowperZoneFloorAreaEdit;

  OSQuantityEdit * m_heatingMaximumAirFlowEdit;

  OSQuantityEdit * m_heatingMaximumAirFlowFractionEdit;

  OSQuantityEdit * m_designZoneAirDistributionEffectivenessinCoolingModeEdit;

  OSQuantityEdit * m_designZoneAirDistributionEffectivenessinHeatingModeEdit;

  ModelObjectPropertyView * m_sizingParametersView;

  class HeatingScheduleVectorController : public ModelObjectVectorController
  {
    public:

    boost::optional<model::ThermostatSetpointDualSetpoint> thermostatSetpointDualSetpoint();

    protected:

    std::vector<OSItemId> makeVector();

    void onChangeRelationship(
           const openstudio::model::ModelObject& modelObject, 
           int index, 
           Handle newHandle, 
           Handle oldHandle);

    void onObjectRemoved(const openstudio::model::ModelObject& modelObject, 
                         const openstudio::IddObjectType& iddObjectType, 
                         const openstudio::UUID& handle);

    void onRemoveItem(OSItem* item);

    void onReplaceItem(OSItem * currentItem, const OSItemId& replacementItemId);

    void onDrop(const OSItemId& itemId);
  };

  class CoolingScheduleVectorController : public ModelObjectVectorController
  {
    public:

    boost::optional<model::ThermostatSetpointDualSetpoint> thermostatSetpointDualSetpoint();

    protected:

    std::vector<OSItemId> makeVector();

    void onChangeRelationship(
           const openstudio::model::ModelObject& modelObject, 
           int index, 
           Handle newHandle, 
           Handle oldHandle);

    void onObjectRemoved(const openstudio::model::ModelObject& modelObject, 
                         const openstudio::IddObjectType& iddObjectType, 
                         const openstudio::UUID& handle);

    void onRemoveItem(OSItem* item);

    void onReplaceItem(OSItem * currentItem, const OSItemId& replacementItemId);

    void onDrop(const OSItemId& itemId);
  };
};

class ZoneEquipmentVectorController : public ModelObjectVectorController
{
  Q_OBJECT

public:

  virtual ~ZoneEquipmentVectorController() {}

  void attach(model::ThermalZone & thermalZone);

  void detach();

protected:

  std::vector<OSItemId> makeVector();

  void onChange(const openstudio::model::ModelObject& modelObject);

  void onRemoveItem(OSItem* item);
  
  void onReplaceItem(OSItem * currentItem, const OSItemId& replacementItemId);
  
  void onDrop(const OSItemId& itemId);

private slots:

  void reportItemsOnce();

private:

  // Some hacks to reports items only once per event loop.
  // Maybe this should be moved down to base class?
  // It does help peformance in this case.
  bool m_dirty;

  void reportItemsLater();

  boost::optional<model::ThermalZone> m_thermalZone;
};

class ModelObjectPropertyView : public QWidget
{
  Q_OBJECT;

  public:

  ModelObjectPropertyView(bool isIP);

  virtual ~ModelObjectPropertyView() {}

  void addQuantityEdit(const QString & label, const char* propertyName);

  void addComboBox(const QString & label, const char* propertyName);

  void bind(model::ModelObject& modelObject);

  void unbind();

  signals:

  void toggleUnitsClicked(bool);

  private:

  std::vector<QString> m_labels;

  std::vector<const char*> m_propertyNames;

  std::vector<QWidget *> m_views;

  bool m_isIP;

  QVBoxLayout * m_mainLayout;
};

} // openstudio

#endif // OPENSTUDIO_THERMALZONESVIEW_H

