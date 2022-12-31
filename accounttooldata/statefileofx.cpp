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

#include "statefileofx.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "operation.h"

StateFileOfx::StateFileOfx(const QString& path)
: m_Start(QDate::currentDate()),
  m_End(0, 0, 0),
  m_Path(path),
  m_Err(false),
  m_Pos(0.f),
  m_PosFound(false)
{
}

void StateFileOfx::Period(QDate& d_start, QDate& d_end)
{
    d_start = m_Start;
    d_end = m_End;
}

int StateFileOfx::LoadOperations(QList<Operation*>& list_op)
{
    if (!m_Err && m_List.empty())
    {
        if (!Load(m_Path))
        {
            m_Err = true;
        }
    }
    else if (m_Err)
    {
        return -1;
    }
    // return the list
    list_op = m_List;
    return m_List.size();
}

//----- PROTECTED ----------------------------------------------------------

bool StateFileOfx::Load(const QString& file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    // Parse file
    QString line, tag, val;
    QLocale def_loc;
    QTextStream in(&file);
    Operation* op;
    // parse file
    while (!in.atEnd())
    {
        line = in.readLine();
        // transaction
        if (line == "<STMTTRN>")
        {
            op = CreateOp(in);
            if (op != 0)
            {
                m_List.push_back(op);
                // update period
                QDate d = op->Date();
                if (d < m_Start)
                {
                    m_Start = d;
                }
                if (d > m_End)
                {
                    m_End = d;
                }
            }
        }
        // balance
        else if (line.startsWith("<BALAMT>"))
        {
            Extract(line, tag, val);
            m_Pos = def_loc.toFloat(val);
            m_PosFound = true;
        }
    }
    return true;
}

/// Set op with string.
Operation* StateFileOfx::CreateOp(QTextStream& in)
{
    Operation* op = 0;
    QDate date;
    float val = 0.f;
    bool debit = false;
    QString id, label, tag, text, line;
    line = in.readLine();
    QLocale def_loc;
    while (!in.atEnd() && line != "</STMTTRN>")
    {
        debit = false;
        Extract(line, tag, text);
        if (tag == "TRNAMT")
        {
            if (text.contains('.'))
            {
                val = text.toFloat();
            }
            else
            {
                val = def_loc.toFloat(text);
            }
        }
        else if (tag == "FITID")
        {
            id = text;
        }
        else if (tag == "NAME")
        {
            label = text;
        }
        else if (tag == "TRNTYPE")
        {
            if (text == "DEBIT")
            {
                debit = true;
            }
        }
        else if (tag == "DTPOSTED")
        {
            date = QDate::fromString(text, "yyyyMMdd");
        }
        line = in.readLine();
    }
    if (line == "</STMTTRN>")
    {
        op = new Operation(false);
        if (debit && val > 0.f)
        {
            val *= -1;
        }
        op->Init(id, date, val, label);
    }
    return op;
}

/**
 * @brief Extract tag name and text
 * @param line as "<tag> text"
 * @param tag is the tag name
 * @param val is text after tag
 */
void StateFileOfx::Extract(const QString& line, QString& tag, QString& val)
{
    int idx1, idx2;
    idx1 = line.indexOf('<');
    idx2 = line.indexOf('>');
    if (idx1 != -1 && idx2 > idx1)
    {
        idx1 += 1;
        tag = line.mid(idx1, idx2 - idx1);
    }
    if (idx2 != -1 && idx2 != line.length())
    {
        idx1 = line.indexOf('<');
        if (idx1 == -1)
        {
            val = line.mid(idx2 + 1);
        }
        else
        {
            val = line.mid(idx2 + 1, idx1 - idx2);
        }
    }
}
