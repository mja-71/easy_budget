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

#include "period.h"
#include "statement.h"
#include "operation.h"
#include <QDate>

Period::Period(const QString& name, bool open)
: m_Name(name), m_Status(open ? S_OPEN : S_CLOSE), m_MapItems(0), m_Transfer(0)
{
}

Period::~Period()
{
    DeleteStatements();
    delete m_MapItems;
    delete m_Transfer;
}

void Period::SetClose(bool is_close)
{
    m_Status = is_close ? S_CLOSE : S_OPEN;
}

void Period::SetTransfer(Operation* op)
{
    delete m_Transfer;
    m_Transfer = op;
}

Statement* Period::GetStatement(const QString& str_id)
{
    Statement* st = 0;
    QMap<QString, Statement*>::iterator it = m_MapStat.find(str_id);
    QLocale def_loc;
    if (it == m_MapStat.end())
    {
        int mm = str_id.mid(5, 2).toInt();
        // st = new Statement(str_id, QDate(2021, mm, 1).toString("MMMM"));
        st =
            new Statement(str_id, def_loc.toString(QDate(2021, mm, 1), "MMMM"));
        m_MapStat.insert(str_id, st);
    }
    else
    {
        st = it.value();
    }
    return (st);
}

void Period::DeleteStatements()
{
    foreach (Statement* s, m_MapStat)
    {
        delete s;
    }
    m_MapStat.clear();
}
