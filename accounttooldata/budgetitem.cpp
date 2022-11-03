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

#include "budgetitem.h"
#include <QFile>
#include <QXmlStreamWriter>

unsigned long BudgetItem::m_LastId = 0;

BudgetItem::BudgetItem(unsigned long id)
: m_OpType(OP_NONE), m_Id(id), m_Deprecated(false)
{
}

BudgetItem::BudgetItem(const QString& name, eOpType type /*=OP_NONE*/)
: m_Label(name), m_OpType(type), m_Deprecated(false)
{
    m_Id = (++m_LastId);
}

BudgetItem::BudgetItem(const BudgetItem& i)
{
    m_Id = i.m_Id;
    m_Label = i.m_Label;
    m_ListOp = i.m_ListOp;
    m_OpType = i.m_OpType;
    m_Deprecated = i.m_Deprecated;
}

BudgetItem& BudgetItem::operator=(const BudgetItem& i)
{
    m_Id = i.m_Id;
    m_Label = i.m_Label;
    m_ListOp = i.m_ListOp;
    m_OpType = i.m_OpType;
    m_Deprecated = i.m_Deprecated;
    return *this;
}

bool BudgetItem::Include(const QString& op_name) const
{
    QStringList::const_iterator it;
    for (it = m_ListOp.constBegin(); it != m_ListOp.constEnd(); ++it)
    {
        if (op_name.contains(*it))
        {
            return true;
        }
    }
    return false;
}

void BudgetItem::SetData(const QString& name, const QString& type)
{
    m_Label = name;
    if (type == "+")
    {
        m_OpType = OP_CREDIT;
    }
    else if (type == "-")
    {
        m_OpType = OP_DEBIT;
    }
    else
    {
        m_OpType = OP_NONE;
    }
}

void BudgetItem::AddOp(const QString& op_label)
{
    m_ListOp.append(op_label);
}

void BudgetItem::RemoveOp(const QString& op_label)
{
    m_ListOp.removeAll(op_label);
}
