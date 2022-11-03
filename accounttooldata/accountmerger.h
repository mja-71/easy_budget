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

#ifndef ACCOUNTMERGER_H
#define ACCOUNTMERGER_H

#include "operation.h"

class DataAccount;

class ACCOUNTTOOLDATASHARED_EXPORT OperationMerged
{
public:
    OperationMerged() : m_Op1(0), m_Op2(0), m_Level(0), m_DoMerge(false) {}
    OperationMerged(Operation* op1, Operation* op2);
    OperationMerged(const OperationMerged& op)
    {
        m_Op1 = op.m_Op1;
        m_Op2 = op.m_Op2;
        m_Level = op.m_Level;
        m_DoMerge = op.m_DoMerge;
    }
    OperationMerged& operator=(const OperationMerged& op)
    {
        m_Op1 = op.m_Op1;
        m_Op2 = op.m_Op2;
        m_Level = op.m_Level;
        m_DoMerge = op.m_DoMerge;
        return *this;
    }
    Operation* Op1() const { return m_Op1; }
    Operation* Op2() const { return m_Op2; }
    int Level() const { return m_Level; }
    bool Merge() const { return m_DoMerge; }
    void SetMerge(bool ok) { m_DoMerge = ok; }
    bool operator<(const OperationMerged& r) const
    {
        return (m_Level > r.m_Level);
    }

    /**
     * @brief Clear Op if includes in r
     * @param r
     * @return true if op1 and op2 are null.
     */
    bool DoNotAnd(const OperationMerged& r)
    {
        if ((m_Op1 == r.m_Op1) || (m_Op1 == r.m_Op2))
        {
            m_Op1 = 0;
        }
        if ((m_Op2 == r.m_Op1) || (m_Op2 == r.m_Op2))
        {
            m_Op2 = 0;
        }
        return ((m_Op1 == 0) && (m_Op2 == 0));
    }

    bool Contains(const OperationMerged& r) const
    {
        return ((m_Op1 == r.m_Op1) || (m_Op1 == r.m_Op2) ||
                (m_Op2 == r.m_Op1) || (m_Op2 == r.m_Op2));
    }

    QString MergedComment() const
    {
        return QString("%1 - %2").arg(Op2()->Comment()).arg(Op1()->Comment());
    }

    BudgetItem* MergedItem() const
    {
        if (Op2()->Item() == 0)
        {
            return Op1()->Item();
        }
        else
        {
            return Op2()->Item();
        }
    }

protected:
    int MatchLevel(Operation* op1, Operation* op2);

private:
    Operation* m_Op1;
    Operation* m_Op2;
    int m_Level;
    bool m_DoMerge;
};

/**
 * @brief Merge operations
 */
class ACCOUNTTOOLDATASHARED_EXPORT AccountMerger
{
public:
    AccountMerger(DataAccount* pDb, const QString& year);
    void GetOps(QVector<OperationMerged>& list_ops);
    void Merge(QVector<OperationMerged>& list_ops);

protected:
    void GetOps(QList<Operation*>& list_ops, bool user);
    void RemoveOp(Operation* op);

private:
    DataAccount* m_pDatabase;
    QString m_Year;
};

#endif // ACCOUNTMERGER_H
