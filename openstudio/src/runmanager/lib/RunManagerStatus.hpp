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

#ifndef OPENSTUDIO_RUNMANAGER_RUNMANAGERSTATUS_HPP__
#define OPENSTUDIO_RUNMANAGER_RUNMANAGERSTATUS_HPP__

#include <string>
#include <QTimer>
#include "RunManager.hpp"
#include <ui_RunManagerStatus.hxx>
#include "JobErrors.hpp"
#include "JobStatusWidget.hpp"


namespace openstudio {
namespace runmanager {
  class RunManager;

  //! QDialog based user interface for displaying RunManager queue job status to the user
  //! \sa http://doc.qt.nokia.com/qdialog.html
  class RunManagerStatus : public QDialog
  {
    Q_OBJECT

  public:
    //! Create a RunManagerStatus dialog for displaying job status
    //! \param[in] errors The errors to display
    RunManagerStatus(QWidget *parent, Qt::WFlags flags, const RunManager &t_runmanager);
    virtual ~RunManagerStatus();

  private:
    //! Used for QDesigner classes.
    Ui::RunManagerStatus ui;

    JobStatusWidget *m_statuswidget;
  };


} // runmanager namespace
} // openstudio namespace


#endif // OPENSTUDIO_RUNMANAGER_RUNMANAGERSTATUS_HPP_
