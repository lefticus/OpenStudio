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

#include "AddTool.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <runmanager/lib/JobFactory.hpp>

namespace openstudio {
namespace runmanager {

  AddTool::AddTool(QWidget *parent, Qt::WFlags flags)
          : QDialog(parent, flags)
  {
    ui.setupUi(this);

    std::set<int> values = ToolType::getValues();

    for (std::set<int>::const_iterator itr = values.begin();
         itr != values.end();
         ++itr)
    {
      ui.cbToolType->addItem(toQString(ToolType::valueDescription(*itr)));
    }

    connect(ui.btnRemoteToolLocationBrowse, SIGNAL(clicked()), this, SLOT(remoteBrowse()));
    connect(ui.btnToolLocationBrowse, SIGNAL(clicked()), this, SLOT(localBrowse()));
  }

  AddTool::~AddTool()
  {
  }

  void AddTool::localBrowse()
  {
    QString str = QFileDialog::getExistingDirectory(this, "Choose Folder Containing Local Tools", ui.txtToolLocation->text());

    if (!str.isEmpty() )
    {
      ui.txtToolLocation->setText(toQString(toPath(str).external_file_string()));
    }
  }

  void AddTool::remoteBrowse()
  {
    QString str = QFileDialog::getOpenFileName(this, "Choose Archive Containing Remote Tools", ui.txtRemoteToolLocation->text());

    if (!str.isEmpty() )
    {
      ui.txtRemoteToolLocation->setText(toQString(toPath(str).external_file_string()));
    }
  }

  boost::optional<std::pair<ToolVersion, ToolLocationInfo> > AddTool::getTool()
  {
    openstudio::path localpath = toPath(ui.txtToolLocation->text());
    openstudio::path remotepath = toPath(ui.txtRemoteToolLocation->text());

    int major = ui.sbMajorVersionNumber->value();
    int minor = ui.sbMinorVersionNumber->value();
    int build = ui.sbBuildVersionNumber->value();

    if (major > -1)
    {
      if (minor > -1)
      {
        if (build > -1)
        {
          return std::make_pair(ToolVersion(major, minor, build), 
              ToolLocationInfo(ToolType(toString(ui.cbToolType->currentText())), localpath, remotepath));
        } else {
          return std::make_pair(ToolVersion(major, minor), 
              ToolLocationInfo(ToolType(toString(ui.cbToolType->currentText())), localpath, remotepath));
        }
      } else {
        return std::make_pair(ToolVersion(major), 
            ToolLocationInfo(ToolType(toString(ui.cbToolType->currentText())), localpath, remotepath));
      }
    } else {
      return std::make_pair(ToolVersion(), 
          ToolLocationInfo(ToolType(toString(ui.cbToolType->currentText())), localpath, remotepath));
    }

  }

} // runmanager
} // openstudio


