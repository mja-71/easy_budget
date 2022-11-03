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

#ifndef ModelItems_H
#define ModelItems_H

#include <QAbstractListModel>
#include <QStringList>
#include "accounttooldata/budgetitem.h"

class DataAccount;
class BudgetItem;

class ModelItems : public QAbstractListModel
{
    Q_OBJECT
public:
    enum eFields
    {
        COL_NAME = 0,
        COL_TYPE = 1,
        COL_FILTER = 2
    };

    ModelItems(QObject* parent = 0);

    void SetDb(DataAccount* pData);

    // subclassing
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index,
                         const QVariant& value,
                         int role = Qt::EditRole);
    virtual QVariant headerData(int section,
                                Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    virtual bool insertRows(int row,
                            int count,
                            const QModelIndex& parent = QModelIndex());
    virtual bool removeRows(int row,
                            int count,
                            const QModelIndex& parent = QModelIndex());

private:
    DataAccount* m_pData;
    QVector<BudgetItem*> m_ListItem;
    QStringList m_Titles;
};

#endif // ModelItems_H
