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

#ifndef OPENSTUDIO_OSDROPZONE_H
#define OPENSTUDIO_OSDROPZONE_H

#include <openstudio_lib/OSItem.hpp>

#include <QWidget>
#include <QMouseEvent>

class QDropEvent;
class QDragEnterEvent;
class QDragLeaveEvent;
class QBoxLayout;
class QScrollArea;
class QPushButton;

namespace openstudio {

class OSVectorController;

class OSDropZone : public QWidget
{
  Q_OBJECT

public:

  OSDropZone(OSVectorController* vectorController,
             bool m_growsHorizontally = true,
             QWidget * parent = 0 );

  virtual ~OSDropZone() {}

  int maxItems() const;
  bool setMaxItems(int max);

  int minItems() const;
  bool setMinItems(int min);

  bool itemsDraggable() const;
  void setItemsDraggable(bool itemsDraggable);

  bool itemsAcceptDrops() const;
  void setItemsAcceptDrops(bool itemsAcceptDrops);

  bool itemsRemoveable() const;
  void setItemsRemoveable(bool itemsRemoveable);

  void showAddButton();
  void hideAddButton();

  bool useLargeIcon();
  void setUseLargeIcon(bool userLargeIcon);

signals:

  // request items from vector controller
  void itemsRequested();

  // emitted in onDrop if item is created
  void itemDropped(const OSItemId& itemId);

  // emitted if item is clicked
  void itemClicked(OSItem* item);

  // emitted if item remove is clicked
  void itemRemoveClicked(OSItem* item);

  // emitted if item is dropped on existing item
  void itemReplacementDropped(OSItem * currentItem, const OSItemId& replacementItemId);

  void addButtonClicked();

protected:

  // called when drop occurs, emit onDrop here if needed
  virtual void onDrop(const OSItemId& itemId);

  void paintEvent ( QPaintEvent * event );

  //void resizeEvent(QResizeEvent * event);

private slots:

  // set this objects item ids
  void setItemIds(const std::vector<OSItemId>& itemIds);

  // called on drop
  void handleDrop( QDropEvent * event );

private:

  OSVectorController* m_vectorController;
  int m_minItems;
  int m_maxItems;
  bool m_itemsDraggable;
  bool m_itemsAcceptDrops;
  bool m_itemsRemoveable;
  bool m_allowAdd;
  QBoxLayout * m_mainBoxLayout;
  QScrollArea* m_scrollArea;
  QPushButton * m_addButton;
  bool m_growsHorizontally;
  bool m_useLargeIcon;
};

class OSItemDropZone : public QWidget
{
  Q_OBJECT

public:
  OSItemDropZone(bool m_growsHorizontally,
                 QWidget * parent = 0);

  virtual ~OSItemDropZone() {}

  void setExtensible(bool extensible);

signals:
  void dropped( QDropEvent * event );

protected:
  void paintEvent ( QPaintEvent * event );
  void dragEnterEvent( QDragEnterEvent * event );
  void dropEvent( QDropEvent * event );

public:
  bool m_growsHorizontally;

};


} // openstudio

#endif // OPENSTUDIO_OSDROPZONE_H
