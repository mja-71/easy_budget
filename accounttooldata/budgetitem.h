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

#ifndef BUDGETITEM_H
#define BUDGETITEM_H

#include "AccountToolData_global.h"
#include <QString>
#include <QStringList>
#include <qmetatype.h>

/**
 * BudgetItem is composed by a list of operation names.
 */
class ACCOUNTTOOLDATASHARED_EXPORT BudgetItem
{
public:
    /// Item type
    enum eOpType
    {
        OP_NONE,   /// could be credit or debit
        OP_CREDIT, /// credit : salary ...
        OP_DEBIT   /// debit : cost
    };
    /// Constructor
    BudgetItem(unsigned long id);
    BudgetItem(const QString& name, eOpType type = OP_NONE);
    BudgetItem(const BudgetItem& i);
    BudgetItem& operator=(const BudgetItem& i);

    QString Name() const { return m_Label; }
    bool IsCredit() const { return m_OpType == OP_CREDIT; }
    bool IsDebit() const { return m_OpType == OP_DEBIT; }
    bool IsUndefined() const { return m_OpType == OP_NONE; }
    bool Include(const QString& op_name) const;
    bool IsDeprecated() const { return m_Deprecated; }
    // setter
    void SetName(const QString& name) { m_Label = name; }
    void SetType(eOpType t) { m_OpType = t; }
    void SetData(const QString& name, const QString& type);
    // operations
    void AddOp(const QString& op_label);
    void RemoveOp(const QString& op_label);
    const QStringList& Ops() const { return m_ListOp; }
    void SetOps(const QStringList& list) { m_ListOp = list; }
    void SetDeprecated(bool flag = true) { m_Deprecated = flag; }
    // id
    static unsigned long LastId() { return m_LastId; }
    static void SetLastId(unsigned long last_id) { m_LastId = last_id; }
    unsigned long Id() const { return m_Id; }


private:
    /// Id counter
    static unsigned long m_LastId;
    /// Unique identification
    unsigned long m_Id;
    /// BudgetItem name
    QString m_Label;
    /// Type of item
    eOpType m_OpType;
    /// Operation list included in a Budget Item
    QStringList m_ListOp;
    /// true if item must to be removed
    bool m_Deprecated;
};

// to use BudgetItem* as QVariant
Q_DECLARE_METATYPE(BudgetItem*)

#endif // BUDGETITEM_H
