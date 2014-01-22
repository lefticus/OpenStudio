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

#ifndef OPENSTUDIO_CONSTRUCTIONINSPECTORVIEW_H
#define OPENSTUDIO_CONSTRUCTIONINSPECTORVIEW_H

#include <openstudio_lib/ModelObjectInspectorView.hpp>

namespace openstudio {

namespace model {

  class MasslessOpaqueMaterial;

}

class OSComboBox;

class OSLineEdit;

class OSQuantityEdit;

class MaterialNoMassInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

  public:

    MaterialNoMassInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent = 0);

    virtual ~MaterialNoMassInspectorView() {}

  protected:

    virtual void onClearSelection();

    virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject);

    virtual void onUpdate();

  private:

    void createLayout();

    void attach(openstudio::model::MasslessOpaqueMaterial & masslessOpaqueMaterial);

    void detach();

    void refresh();

    OSComboBox * m_roughness;

    OSLineEdit * m_nameEdit;

    OSQuantityEdit * m_thermalResistance;

    OSQuantityEdit * m_thermalAbsorptance;

    OSQuantityEdit * m_solarAbsorptance;

    OSQuantityEdit * m_visibleAbsorptance;

    bool m_isIP;

  public slots:

    void toggleUnits(bool displayIP);
};

} // openstudio

#endif // OPENSTUDIO_CONSTRUCTIONINSPECTORVIEW_H

