/*
 Copyright 2022 by Michel JANTON (michel.janton@gmail.com)

 This file is part of "Easy Budget" software
 "Easy Budget" is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version. "Easy Budget" is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details. You should have received a copy of the GNU General Public License
 along with "Easy Budget". If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DELEGATEMERGEITEM_H
#define DELEGATEMERGEITEM_H

#include <QObject>

#include <string>
#include <vector>

#include <QStyledItemDelegate>

class QModelIndex;
class QWidget;
class QVariant;

class DelegateMergeItem : public QStyledItemDelegate
{
	Q_OBJECT
public:
	DelegateMergeItem(QObject *parent = 0);
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
						  const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
					  const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
							  const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option,
			   const QModelIndex &index) const;

private:
	QStringList m_ListItemLabels;

};
#endif // DELEGATEMERGEITEM_H
