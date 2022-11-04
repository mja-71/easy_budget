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

#include "accountmerger.h"
#include "dataaccount.h"
#include <QVector>
#include <QSet>

//-----------------------------------------------------------------------------
//--------- OperationMerged implementation
//-----------------------------------------------------------------------------

OperationMerged::OperationMerged(Operation* op1, Operation* op2)
: m_Op1(op1), m_Op2(op2), m_DoMerge(false)
{
    if (m_Op1 != 0 && m_Op2 != 0)
    {
        m_Level = MatchLevel(op1, op2);
    }
    else
    {
        m_Level = 0;
    }
}

int OperationMerged::MatchLevel(Operation* op1, Operation* op2)
{
    int level = 0;

    // compare value
    float d = (op1->Value() - op2->Value());
    if ((d < 0.01f) && (d > -0.01f))
    {
        level = 50;
    }
    else
    {
        // integer between [0..20]
        if (d > 0)
        {
            level = (20 - int(d)) / 2;
        }
        else
        {
            level = (20 + int(d)) / 2;
        }
        if (level < 0)
        {
            level = 0;
        }
    }

    // compare date : increase level for nears operations
    int days = op1->Date().daysTo(op2->Date());
    if ((days < 20) && (days >= 0))
    {
        level += (20 - days);
    }
    else if ((days > -20) && (days < 0))
    {
        level += (20 + days);
    }
    return level;
}

//-----------------------------------------------------------------------------
//--------- AccountMerger implementation
//-----------------------------------------------------------------------------

AccountMerger::AccountMerger(DataAccount* pDb, const QString& year)
: m_pDatabase(pDb), m_Year(year)
{
}

/**
 * @brief Get the list of operations for merge
 * @param list_ops
 */
void AccountMerger::GetOps(QVector<OperationMerged>& list_ops)
{
    // get list of user ops, not yet validated
    QList<Operation*> list_user;
    GetOps(list_user, true);
    // get list of bank ops, not yet validated
    QList<Operation*> list_bank;
    GetOps(list_bank, false);
    // fill list with possible merge
    foreach (Operation* op, list_user)
    {
        foreach (Operation* b_op, list_bank)
        {
            list_ops.push_back(OperationMerged(op, b_op));
        }
    }
    // sort, beginning with higher level
    std::sort(list_ops.begin(), list_ops.end());
    // keep only merge with the best levels
    QVector<OperationMerged>::iterator it = list_ops.begin(), it2;
    while (it != list_ops.end())
    {
        it2 = (it + 1);
        while (it2 != list_ops.end())
        {
            if ((*it2).Contains(*it))
            {
                it2 = list_ops.erase(it2);
            }
            else
            {
                ++it2;
            }
        }
        list_user.removeOne((*it).Op1());
        list_bank.removeOne((*it).Op2());
        ++it;
    }
    // complete with non merged operations
    foreach (Operation* op, list_user)
    {
        list_ops.push_back(OperationMerged(op, 0));
    }
    foreach (Operation* op, list_bank)
    {
        list_ops.push_back(OperationMerged(0, op));
    }
}

void AccountMerger::Merge(QVector<OperationMerged>& list_ops)
{
    QVector<OperationMerged>::Iterator it = list_ops.begin();
    QSet<QString> set_of_states; // states to update
    while (it != list_ops.end())
    {
        if ((*it).Merge())
        {
            (*it).Op2()->SetComment((*it).MergedComment());
            (*it).Op2()->SetItem((*it).MergedItem());
            // validate bank operation
            (*it).Op2()->SetStatus(true);
            // store stats to update
            set_of_states.insert((*it).Op1()->StatId());
            set_of_states.insert((*it).Op2()->StatId());
            // remove user operation
            RemoveOp((*it).Op1());
            // remove merged operation
            it = list_ops.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // update database
    foreach (QString s, set_of_states)
    {
        m_pDatabase->UpdateStat(s);
    }
}

//------------------- PROTECTED -----------------------------------------------

void AccountMerger::GetOps(QList<Operation*>& list_ops, bool user)
{
    QString state_id;
    int status = user ? DataAccount::E_USER : DataAccount::E_BANK;
    for (int i = 0; i < 12; i++)
    {
        state_id = QString("%1-%2").arg(m_Year).arg(i + 1, 2, 10, QChar('0'));
        m_pDatabase->Operations(state_id, status, list_ops);
    }
}

void AccountMerger::RemoveOp(Operation* op)
{
    QList<Operation*>& list_op = m_pDatabase->Operations(op->StatId());
    list_op.removeOne(op);
    delete op;
}
