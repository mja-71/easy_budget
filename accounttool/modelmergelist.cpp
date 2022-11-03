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

#include "modelmergelist.h"

#include "accounttooldata/dataaccount.h"
#include "accounttooldata/accountmgr.h"
#include "accounttooldata/accountmerger.h"
#include <QMimeData>

QList<Operation*> ModelMergeList::m_EmptyList;

/// MimeData with operation use for D&D
class MimeDataOperation : public QMimeData
{
public:
    MimeDataOperation() : QMimeData(), m_Op(0) {}
    void SetOp(Operation* op) { m_Op = op; }
    Operation* Op() const { return m_Op; }

private:
    Operation* m_Op;
};

ModelMergeList::ModelMergeList(QObject* parent) : QAbstractListModel(parent)
{
    m_Titles << tr("Merge") << tr("Date") << tr("Label") << tr("Value")
             << tr("Item") << tr("Comment");
}

/// Define current state.
void ModelMergeList::SetState(DataAccount* pData, const QString& str_year)
{
    beginResetModel();
    AccountMerger merger(pData, str_year);
    m_List.clear();
    if (pData != 0)
    {
        merger.GetOps(m_List);
    }
    m_pData = pData;
    m_Year = str_year;
    endResetModel();
}

void ModelMergeList::Apply()
{
    beginResetModel();
    AccountMerger merger(m_pData, m_Year);
    merger.Merge(m_List);
    // remove merged op ?
    endResetModel();
}

void ModelMergeList::Restore(const QString& stat)
{
    /*
        if (m_pData!=0)
        {
            beginResetModel();
            m_pData->RestoreStat(stat);
            endResetModel();
        }
    */
}

int ModelMergeList::rowCount(const QModelIndex& parent /*=QModelIndex()*/) const
{
    return m_List.size() * 2;
}

int ModelMergeList::columnCount(const QModelIndex& /*parent*/) const
{
    return m_Titles.size();
}

bool ModelMergeList::insertRows(int row,
                                int count,
                                const QModelIndex& parent /*= QModelIndex()*/
)
{
    beginInsertRows(parent, row, row + count);
    /*
    QList<Operation*> list_op;
    for (int i=0;i<count;i++)
    {
        list_op.append(new Operation());
    }
    // m_pData->AddOps(list_op, m_CurrStatId, false);
    */
    endInsertRows();
    return true;
}

bool ModelMergeList::removeRows(int row,
                                int count,
                                const QModelIndex& parent /*= QModelIndex()*/
)
{
    beginRemoveRows(parent, row, row + count);
    int i = row / 2;
    if ((row % 2) == 0)
    {
        if (m_List[i].Op1() != 0)
        {
            m_List[i] = OperationMerged(m_List[i].Op1(), 0);
        }
        else
        {
            m_List.remove(i);
        }
    }
    else
    {
        if (m_List[i].Op2() != 0)
        {
            m_List[i] = OperationMerged(0, m_List[i].Op2());
        }
        else
        {
            m_List.remove(i);
        }
    }
    endRemoveRows();
    return true;
}

QVariant ModelMergeList::data(const QModelIndex& index,
                              int role /*=Qt::DisplayRole*/
) const
{
    if (!index.isValid())
        return QVariant();

    int i = index.row() / 2;
    Operation* op = 0;
    bool merged = false;
    if ((index.row() % 2) == 0)
    {
        op = m_List[i].Op2();
        merged = m_List[i].Merge();
    }
    else
    {
        op = m_List[i].Op1();
    }

    if (role == Qt::DisplayRole)
    {
        if (op != 0)
        {
            BudgetItem* bi = 0;
            switch (index.column())
            {
            case E_DATE:
                return op->StringDate();
            case E_LABEL:
                return op->Label();
            case E_VAL:
                return op->Val();
            case E_ITEM:
                bi = (merged ? m_List[i].MergedItem() : op->Item());
                return (bi != 0 ? bi->Name() : QString());
            case E_DESC:
                return (merged ? m_List[i].MergedComment() : op->Comment());
            case E_MERGE:
                if (index.row() % 2 == 0)
                    return m_List[i].Level();
                else
                    return QString("");
            default:
                return "";
            }
        }
        else
        {
            return "";
        }
    }
    else if (role == Qt::CheckStateRole &&
             (m_List[i].Op1() != 0 && m_List[i].Op2() != 0) &&
             (index.row() % 2 == 0))
    {
        switch (index.column())
        {
        case E_MERGE:
            return static_cast<int>(m_List[i].Merge() ? Qt::Checked :
                                                        Qt::Unchecked);
        default:
            return QVariant();
        }
    }
    // return operation object
    else if (role == Qt::UserRole)
    {
        QVariant data;
        if (op != 0)
        {
            data.setValue(m_List[i]);
        }
        return data;
    }
    else
    {
        return QVariant();
    }
}

bool ModelMergeList::setData(const QModelIndex& idx,
                             const QVariant& value,
                             int role /* = Qt::EditRole */
)
{
    int i = idx.row() / 2;
    // Operation* op = m_pData->Operations(m_CurrStatId).at(idx.row());
    if (role == Qt::EditRole)
    {
        switch (idx.column())
        {
        case E_ITEM:
            // op->SetItem(value.toString());
            break;
        case E_DESC:
            // op->SetComment(value.toString());
            break;
        default:
            return false;
        }
    }
    else if (role == Qt::CheckStateRole && (idx.row() % 2 == 0))
    {
        bool ok;
        int state = value.toInt(&ok);
        switch (idx.column())
        {
        case E_MERGE:
            m_List[i].SetMerge(state == Qt::Checked);
            emit dataChanged(index(i * 2, 0), index(i * 2 + 1, E_MAX));
            break;
        default:
            return false;
        }
    }
    else if (role == Qt::UserRole)
    {
        int row = idx.row();
        emit dataChanged(index(row, 0), index(row, E_MAX));
    }

    return true;
}

QVariant ModelMergeList::headerData(int section,
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

Qt::ItemFlags ModelMergeList::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);

    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if (index.column() == E_DESC || index.column() == E_ITEM)
    {
        flag |= Qt::ItemIsEditable;
    }
    else if (index.column() == E_MERGE && (index.row() % 2 == 0))
    {
        flag |= Qt::ItemIsUserCheckable;
    }
    if ((index.row() % 2) == 1)
    {
        flag |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }
    return flag;
}

QStringList ModelMergeList::mimeTypes() const
{
    QStringList types;
    types << "application/budget.operation.list";
    return types;
}

Qt::DropActions ModelMergeList::supportedDropActions() const
{
    return /*Qt::CopyAction|*/ Qt::MoveAction;
}

QMimeData* ModelMergeList::mimeData(const QModelIndexList& indexes) const
{
    if (indexes.size() > 0)
    {
        QModelIndex idx = indexes.front();
        if (idx.isValid())
        {
            QByteArray encodedData;
            MimeDataOperation* data = new MimeDataOperation();
            data->setData("application/budget.operation.list", encodedData);
            int i = idx.row() / 2;
            data->SetOp(m_List[i].Op1());
            return data;
        }
    }
    return 0;
}

bool ModelMergeList::dropMimeData(const QMimeData* data,
                                  Qt::DropAction action,
                                  int row,
                                  int column,
                                  const QModelIndex& parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("application/budget.operation.list"))
        return false;

    const MimeDataOperation* data_op =
        dynamic_cast<const MimeDataOperation*>(data);
    if (data_op != 0)
    {
        int i = parent.row() / 2;
        if ((parent.row() % 2) == 1)
        {
            Operation* op_prev = m_List[i].Op1();
            m_List[i] = OperationMerged(data_op->Op(), m_List[i].Op2());
            if (op_prev != 0)
            {
                beginInsertRows(parent, parent.row() + 1, parent.row() + 2);
                m_List.insert(i + 1, OperationMerged(op_prev, 0));
                endInsertRows();
            }
            return true;
        }
    }

    return false;
}
