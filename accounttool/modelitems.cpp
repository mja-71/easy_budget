
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

#include "modelitems.h"

#include "accounttooldata/dataaccount.h"
#include "accounttooldata/accountmgr.h"
#include "accounttooldata/budgetitem.h"


ModelItems::ModelItems(QObject* parent) : QAbstractListModel(parent), m_pData(0)
{
    m_Titles << tr("Name") << tr("Type") << tr("Filters");
}

/// Define data
void ModelItems::SetDb(DataAccount* pData)
{
    beginResetModel();
    m_pData = pData;
    // m_ListItem.reserve(pData->Items().size());
    m_ListItem.clear();
    QList<BudgetItem*> list_item = pData->Items((Period*)0);
    foreach (BudgetItem* bi, list_item)
    {
        if (!bi->IsDeprecated())
        {
            m_ListItem.append(bi);
        }
    }
    endResetModel();

    // emit layoutChanged();
}

int ModelItems::rowCount(const QModelIndex& parent /*=QModelIndex()*/) const
{
    return m_ListItem.size();
}

int ModelItems::columnCount(const QModelIndex& /*parent*/) const
{
    return m_Titles.size();
}

QVariant ModelItems::data(const QModelIndex& index,
                          int role /*=Qt::DisplayRole*/
) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_ListItem.size())
        return QVariant();

    BudgetItem* bi = m_ListItem.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0:
            return bi->Name();
        default:
            return "";
        }
    }
    else if (role == Qt::UserRole)
    {
        QVariant val;
        val.setValue(bi);
        return val;
    }
    else
    {
        return QVariant();
    }
}

/// set data from view
bool ModelItems::setData(const QModelIndex& index,
                         const QVariant& value,
                         int role /* = Qt::EditRole */
)
{
    if (!index.isValid() || index.row() >= m_ListItem.size())
    {
        return false;
    }

    BudgetItem* bi = m_ListItem.at(index.row());
    if (role == Qt::EditRole || role == (Qt::UserRole + COL_NAME))
    {
        bi->SetName(value.toString());
    }
    else if (role == (Qt::UserRole + COL_TYPE))
    {
        bi->SetType((BudgetItem::eOpType)(value.toInt()));
    }
    else if (role == (Qt::UserRole + COL_FILTER))
    {
        bi->SetOps(value.toStringList());
    }
    else
    {
        return false;
    }
    emit dataChanged(index, index);
    return true;
}

QVariant ModelItems::headerData(int section,
                                Qt::Orientation orientation,
                                int role /*=Qt::DisplayRole*/
) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return m_Titles.at(section);
    else
        return QString("Row %1").arg(section);
}

Qt::ItemFlags ModelItems::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);

    if (!index.isValid())
    {
        return Qt::ItemIsEnabled;
    }
    /*
    if (index.column()==E_DESC || index.column()==E_ITEM)
    {
     flag |=  Qt::ItemIsEditable;
    }
    */
    return flag;
}

bool ModelItems::insertRows(int row,
                            int count,
                            const QModelIndex& parent /*= QModelIndex()*/
)
{
    beginInsertRows(parent, row, row + count);
    BudgetItem* bi;
    for (int i = 0; i < count; i++)
    {
        // bi = new BudgetItem(tr("new item %1").arg(i));
        bi = m_pData->InsertItem(tr("new item %1").arg(i));
        m_ListItem.insert(row, bi);
    }
    endInsertRows();
    return true;
}

bool ModelItems::removeRows(int row,
                            int count,
                            const QModelIndex& parent /*= QModelIndex()*/
)
{
    beginRemoveRows(parent, row, row + count - 1);
    BudgetItem* bi;
    for (int i = 0; i < count; i++)
    {
        bi = m_ListItem.at(row);
        bi->SetDeprecated();
        m_ListItem.removeAt(row);
    }
    endRemoveRows();
    return true;
}
