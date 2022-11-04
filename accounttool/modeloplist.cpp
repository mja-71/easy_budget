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

#include "modeloplist.h"

#include "accounttooldata/dataaccount.h"
#include "accounttooldata/accountmgr.h"
#include "accounttooldata/accountsynthesis.h"
#include <QCoreApplication>
#include <QLocale>


ModelOpList::ModelOpList(QObject* parent)
: QAbstractListModel(parent), m_pData(0), m_Transfer(0), m_Synthesis(0)
{
    m_Titles << tr("Status") << tr("Date") << tr("Label") << tr("Value")
             << tr("Item") << tr("Comment");
}

/// Define current state.
void ModelOpList::SetState(DataAccount* pData, Period* year, Statement* state)
{
    beginResetModel();
    if (pData != 0)
    {
        m_pData = pData;
        m_CurrStat = state;
        m_Year = year;
        if (state->Date().month() == 1)
        {
            m_Transfer = m_Year->Transfer();
        }
        else
        {
            m_Transfer = 0;
        }
        m_Synthesis = AccountMgr::Mgr()->Analyser(m_Year);
    }
    else
    {
        m_pData = 0;
        m_CurrStat = 0;
        m_Year = 0;
        m_Transfer = 0;
    }
    endResetModel();
}

void ModelOpList::AddOperation(Operation* op)
{
    beginResetModel();
    QList<Operation*> list_op;
    if (op->IsTransfer())
    {
        m_pData->SetTransfer(m_Year, op);
        m_Transfer = m_Year->Transfer();
    }
    else
    {
        list_op.append(op);
        m_pData->AddOps(list_op, m_CurrStat->Id(), false);
    }
    endResetModel();
}

int ModelOpList::rowCount(const QModelIndex& parent /*=QModelIndex()*/) const
{
    if (m_pData == 0)
        return 0;
    if (m_Transfer != 0)
    {
        return (m_pData->Operations(m_CurrStat->Id()).size() + 1);
    }
    return m_pData->Operations(m_CurrStat->Id()).size();
}

int ModelOpList::columnCount(const QModelIndex& /*parent*/) const
{
    return m_Titles.size();
}

/// insert new operations
/// \note bug with model proxy ?
bool ModelOpList::insertRows(int row,
                             int count,
                             const QModelIndex& parent /*= QModelIndex()*/
)
{
    beginInsertRows(parent, row, row + count);
    QList<Operation*> list_op;
    for (int i = 0; i < count; i++)
    {
        Operation* op = new Operation();
        list_op.append(op);
    }
    m_pData->AddOps(list_op, m_CurrStat->Id(), false);
    endInsertRows();
    return true;
}

bool ModelOpList::removeRows(int row,
                             int count,
                             const QModelIndex& parent /*= QModelIndex()*/
)
{
    beginRemoveRows(parent, row, row + count);
    QList<Operation*>& list_op = m_pData->Operations(m_CurrStat->Id());
    Operation* op;
    for (int i = 0; i < count; i++)
    {
        op = GetOp(row);
        if (op == m_Transfer)
        {
            m_pData->SetTransfer(m_Year, (Operation*)0);
            m_Transfer = m_Year->Transfer();
        }
        else
        {
            list_op.removeOne(op); // update db ...
            delete op;
        }
    }
    endRemoveRows();
    return true;
}

QVariant ModelOpList::data(const QModelIndex& index,
                           int role /*=Qt::DisplayRole*/
) const
{
    if (!index.isValid() || m_pData == 0)
        return QVariant();
    Operation* op = GetOp(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case E_DATE:
            if (role == Qt::EditRole)
                return op->Date();
            else
                return op->StringDate();
            break;
        case E_LABEL:
            return op->Label();
        case E_VAL:
            return op->Value(); // return float to allow sorting
        case E_ITEM:
            return ((op->Item() != 0) ? op->Item()->Name() : "");
        case E_DESC:
            return op->Comment();
        case E_VALIDITY:
            if (op->IsValid())
            {
                return tr("checked");
            }
            else
            {
                return (op->IsUser() ? tr("recorded") : tr("imported"));
            }
        default:
            return "";
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
        case E_VALIDITY:
            return static_cast<int>(op->IsValid() ? Qt::Checked :
                                                    Qt::Unchecked);
        default:
            return QVariant();
        }
    }
    // return operation object
    else if (role == Qt::UserRole)
    {
        QVariant data;
        data.setValue(op);
        return data;
    }
    else
    {
        return QVariant();
    }
}

bool ModelOpList::setData(const QModelIndex& idx,
                          const QVariant& value,
                          int role /* = Qt::EditRole */
)
{
    Operation* op = GetOp(idx.row());
    bool ok;
    QDate d;
    if (role == Qt::EditRole)
    {
        switch (idx.column())
        {
        case E_ITEM:
            op->SetItem(value.value<BudgetItem*>());
            break;
        case E_DESC:
            op->SetComment(value.toString());
            break;
        case E_LABEL:
            op->SetLabel(value.toString());
            break;
        case E_VAL:
            op->SetValue(value.toFloat(&ok));
            if (!ok)
            {
                return false;
            }
            else
            {
                m_Synthesis->Update(m_CurrStat->Date().month() - 1);
                emit dataChanged(index(idx.row(), 0), index(idx.row(), E_MAX));
            }
            break;
        case E_DATE:
            d = value
                    .toDate(); // loc.toDate(value.toString(),QLocale::ShortFormat);
            if (!d.isValid())
            {
                return false;
            }
            // todo : move op if period changed (month or year)
            op->SetDate(d);
            if (op->StatId() != m_CurrStat->Id())
            {
                beginRemoveRows(idx.parent(), idx.row(), idx.row() + 1);
                MoveOp(op);
                endRemoveRows();
            }
            break;
        default:
            return false;
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        bool ok;
        int state = value.toInt(&ok);
        switch (idx.column())
        {
        case E_VALIDITY:
            op->SetStatus(state == Qt::Checked);
            emit dataChanged(index(idx.row(), 0), index(idx.row(), E_MAX));
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

QVariant ModelOpList::headerData(int section,
                                 Qt::Orientation orientation,
                                 int role /*=Qt::DisplayRole*/
) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal && section >= 0)
        return m_Titles.at(section);
    else if (orientation == Qt::Vertical)
        return QString("Row %1").arg(section);
    else
        return QVariant();
}

Qt::ItemFlags ModelOpList::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);
    bool is_transfer = (m_Transfer && index.row() == 0);

    if (!index.isValid())
    {
        return Qt::ItemIsEnabled;
    }

    if (index.column() == E_DESC)
    {
        flag |= Qt::ItemIsEditable;
    }
    else if (!m_Year->IsClose() && !is_transfer)
    {
        if (index.column() == E_ITEM)
        {
            flag |= Qt::ItemIsEditable;
        }
        else if (index.column() == E_VALIDITY)
        {
            flag |= Qt::ItemIsUserCheckable;
        }
        else if (index.column() == E_LABEL || index.column() == E_VAL ||
                 index.column() == E_DATE)
        {
            QVariant data = index.model()->data(index, Qt::UserRole);
            if (data.canConvert<Operation*>())
            {
                Operation* op = data.value<Operation*>();
                if (!op->IsValid())
                {
                    if (op->IsUser())
                    {
                        flag |= Qt::ItemIsEditable;
                    }
                }
            }
        }
    }
    // value of the transfer is editable
    else if (!m_Year->IsClose() && is_transfer)
    {
        if (index.column() == E_VAL)
        {
            flag |= Qt::ItemIsEditable;
        }
    }
    return flag;
}


//---------- PROTECTED ---------------------------------------------------------

void ModelOpList::MoveOp(Operation* op)
{
    QString target_id = op->StatId();
    if (target_id != m_CurrStat->Id())
    {
        // beginRemoveRows(parent,row,row+count);
        QList<Operation*>& list_op = m_pData->Operations(m_CurrStat->Id());
        QList<Operation*> op_to_add;
        list_op.removeOne(op); // update db ...
        op_to_add.push_back(op);
        m_pData->AddOps(op_to_add, target_id);
        // endRemoveRows();
    }
}

inline Operation* ModelOpList::GetOp(int row) const
{
    QList<Operation*>& list_op = m_pData->Operations(m_CurrStat->Id());
    // transfer operation, if any, is the first operation
    if (m_Transfer != 0)
    {
        if (row != 0)
        {
            return list_op.at(row - 1);
        }
        else
        {
            return m_Transfer;
        }
    }
    // default
    return list_op.at(row);
}
