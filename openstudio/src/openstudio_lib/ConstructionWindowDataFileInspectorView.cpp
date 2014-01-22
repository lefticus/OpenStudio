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

#include <openstudio_lib/ConstructionWindowDataFileInspectorView.hpp>

#include "../shared_gui_components/OSLineEdit.hpp"

#include <model/WindowDataFile.hpp>
#include <model/WindowDataFile_Impl.hpp>

#include <utilities/core/Assert.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// ConstructionVC

ConstructionWindowDataFileInspectorView::ConstructionWindowDataFileInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent)
  : ModelObjectInspectorView(model, true, parent),
    m_nameEdit(NULL),
    m_urlEdit(NULL),
    m_isIP(isIP)
{
  createLayout();
}

void ConstructionWindowDataFileInspectorView::createLayout()
{
  QWidget* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  QGridLayout* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7,7,7,7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  // Name

  QLabel* label = new QLabel("Name: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,0,0);

  m_nameEdit = new OSLineEdit();
  mainGridLayout->addWidget(m_nameEdit,1,0,1,3);

  // URL

  label = new QLabel("URL: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,2,0);

  m_urlEdit = new OSLineEdit();
  mainGridLayout->addWidget(m_urlEdit,3,0,1,3);

  // Stretch

  mainGridLayout->setRowStretch(100,100);

  mainGridLayout->setColumnStretch(100,100);
}

void ConstructionWindowDataFileInspectorView::onClearSelection()
{
  ModelObjectInspectorView::onClearSelection(); // call parent implementation
  detach();
}

void ConstructionWindowDataFileInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject)
{
  detach();
  model::WindowDataFile windowDataFile = modelObject.cast<model::WindowDataFile>();
  attach(windowDataFile);
  refresh();
}

void ConstructionWindowDataFileInspectorView::onUpdate()
{
  refresh();
}

void ConstructionWindowDataFileInspectorView::attach(openstudio::model::WindowDataFile & windowDataFile)
{
  m_nameEdit->bind(windowDataFile,"name");
  m_urlEdit->bind(windowDataFile,"url");

  this->stackedWidget()->setCurrentIndex(1);
}

void ConstructionWindowDataFileInspectorView::detach()
{
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_urlEdit->unbind();
}

void ConstructionWindowDataFileInspectorView::refresh()
{
}

void ConstructionWindowDataFileInspectorView::toggleUnits(bool displayIP)
{
  m_isIP = displayIP;
}

} // openstudio

