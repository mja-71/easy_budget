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

#include "statement.h"
#include "operation.h"

Statement::Statement(const QString& id, const QString& label)
: m_Id(id), m_Label(label), m_Position(QDate(), 0.), m_ListOps(0)
{
}

Statement::Statement(const Statement& s)
{
    m_Id = s.m_Id;
    m_Label = s.m_Label;
    m_Position = s.m_Position;
}

Statement::~Statement()
{
    CleanOps();
}

Statement& Statement::operator=(const Statement& s)
{
    m_Id = s.m_Id;
    m_Label = s.m_Label;
    m_Position = s.m_Position;
    return *this;
}

QDate Statement::Date() const
{
    return QDate(m_Id.mid(0, 4).toInt(), m_Id.mid(5, 2).toInt(), 1);
}

/**
 * @brief Set bank position.
 * @param pos
 * Use only the more recent position.
 */
void Statement::SetPosition(const StatePos& pos)
{
    if (m_Position.Date() <= pos.Date())
    {
        m_Position = pos;
    }
}

/**
 * @brief Statement::SetOps
 * @param list_ops : pointer on operation list
 * list_ops pointer is owned by Statement object
 */
void Statement::SetOps(QList<Operation*>* list_ops)
{
    CleanOps();
    m_ListOps = list_ops;
}

void Statement::CleanOps()
{
    if (m_ListOps != 0)
    {
        foreach (Operation* op, *m_ListOps)
        {
            delete op;
        }
        m_ListOps->clear();
        delete m_ListOps;
        m_ListOps = 0;
    }
}
