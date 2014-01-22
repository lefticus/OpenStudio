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

#ifndef TREEVIEWWIDGET_H
#define TREEVIEWWIDGET_H

#include <QModelIndex>

#include <model/Building.hpp>
#include <model_editor/ModelEditorAPI.hpp>
#include <model_editor/ViewWidget.hpp>

namespace modeleditor
{
  
class TreeModel;
class TreeView;

class TreeViewWidget : public ViewWidget
{
  Q_OBJECT

public:
  TreeViewWidget(openstudio::model::Model& model, QWidget *parent = 0);
  TreeViewWidget(QWidget *parent = 0);
  virtual ~TreeViewWidget();
  virtual void addObjects(openstudio::IddObjectType type = openstudio::IddObjectType::UserCustom);
  virtual void loadModel();
  virtual void removeObjects();
  virtual void copyObjects();
  virtual void pasteObjects();
  virtual bool hasSelectedRows();
  virtual bool hasRowsToPaste();
  std::vector<openstudio::IddObjectType> getAllowableChildTypes();
  void expandAllNodes();
  void collapseAllNodes();
  TreeView* getTreeView();
  TreeModel* getTreeModel();
  virtual void toggleGUIDs();

public slots:
  virtual void viewSelection();
  virtual void viewSelection(const QModelIndex& modelIndex);
  virtual void on_nameChanged(QString);

signals:
  void expandAll();
  void collapseAll();

protected:
  TreeView * mTreeView;
  TreeModel * mTreeModel;

private:
  virtual void connectSignalsAndSlots();
  virtual void createLayout();
  virtual void createWidgets();
  virtual void loadData();
  virtual void loadModel(openstudio::model::Model& model);
  void expandAppropriateTreeNodes();
  void setExpandedIndexHandles();

  ///! only expanded nodes are here
  std::vector< std::pair<QModelIndex, openstudio::Handle> > mExpandedIndexHandles;

  std::vector<openstudio::model::ModelObject> mModelObjectsToPaste;
};

} // namespace modeleditor

#endif // TREEVIEWWIDGET_H
