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

#include "statefilece.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QLocale>
#include "operation.h"

StateFileCE::StateFileCE(const QString& path)
: m_Pos(0.f),
  m_Start(QDate::currentDate()),
  m_End(0, 0, 0),
  m_Path(path),
  m_Err(false),
  m_Ver(2)
{
}

void StateFileCE::Period(QDate& d_start, QDate& d_end)
{
    d_start = m_Start;
    d_end = m_End;
}

int StateFileCE::LoadOperations(QList<Operation*>& list_op)
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

bool StateFileCE::Load(const QString& file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    // Parse file
    QString line;
    QTextStream in(&file);
    bool res = false;

    // check header
    line = in.readLine();
    if (line.startsWith("Code de la banque"))
    {
        m_Ver = 1;
        for (int i = 0; i < 2; i++)
        {
            in.readLine();
        }
        res = Parse1(in);
    }
    else
    {
        res = Parse2(in);
    }
    return res;
}

bool StateFileCE::Parse1(QTextStream& in)
{
    int n = 0;
    QString line;
    Operation* op;

    // read position
    ParsePosition(in.readLine());
    // skip list header
    in.readLine();
    while (!in.atEnd())
    {
        // read operation
        line = in.readLine();
        if (!line.isEmpty() && !line.startsWith("Solde"))
        {
            op = CreateOp(line);
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
                n++;
            }
        }
    }
    return true;
}

bool StateFileCE::Parse2(QTextStream& in)
{
    int n = 0;
    QString line;
    Operation* op;
    int counter = 0;
    while (!in.atEnd())
    {
        // read operation
        line = in.readLine();
        if (!line.isEmpty())
        {
            op = CreateOp2(line, counter++);
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
                n++;
            }
        }
    }
    return true;
}

bool StateFileCE::ParsePosition(const QString& line)
{
    bool res = false;
    QStringList fields = line.split(';');
    QLocale def_loc;
    if (fields.size() > 4)
    {
        m_Pos = def_loc.toFloat(fields.at(4));
        res = true;
    }
    return res;
}

/// Set data with string.
/// Date;Numéro d'opération;Libellé;Débit;Crédit;Détail;
Operation* StateFileCE::CreateOp(const QString& str)
{
    Operation* op = 0;
    QStringList fields = str.split(';');
    QDate date;
    float val;
    QString id, label;
    QLocale def_loc;
    if (fields.size() > 4)
    {
        date = QDate::fromString(fields.at(0), QString("dd/MM/yy"));
        date = date.addYears(100); // start at 2000 instead of 1900
        // m_Date.setDate(2013,1,1);
        // id
        id = fields.at(1);
        // value : debit or credit
        if (!fields.at(3).isEmpty())
        {
            val = def_loc.toFloat(fields.at(3));
        }
        else
        {
            val = def_loc.toFloat(fields.at(4));
        }
        // label
        label = fields.at(2);
        op = new Operation(false);
        op->Init(id, date, val, label);
    }
    return op;
}

/// Set data with string (new format at 2022-04)
/// Date1\t Date2\t Category\t Type\t Label\t Short label\t detail\t value\t
/// check
Operation* StateFileCE::CreateOp2(const QString& str, int counter)
{
    Operation* op = 0;
    QDate date;
    float val;
    QString id, label;
    QLocale def_loc;
    QStringList fields = str.split('\t');
    if (fields.size() <= 1)
    {
        fields = str.split(';');
    }
    // may 2022
    if (fields.size() == 7)
    {
        date = QDate::fromString(fields.at(0), QString("yyyy-MM-dd"));
        // id = date + counter
        id = QString("%1_%2").arg(fields.at(1)).arg(counter);
        // values
        val = def_loc.toFloat(fields.at(5));
        // label
        label = fields.at(3);
        if (label.isEmpty())
        {
            label = fields.at(4); // try short label
        }
        op = new Operation(false);
        op->Init(id, date, val, label);
    }
    // june 2022
    else if (fields.size() == 9)
    {
        date = QDate::fromString(fields.at(0), QString("dd/MM/yyyy"));
        // id = date + counter
        id = QString("%1_%2").arg(fields.at(1)).arg(counter);
        // values
        val = def_loc.toFloat(fields.at(7));
        // label
        label = fields.at(4);
        if (label.isEmpty())
        {
            label = fields.at(5); // try short label
        }
        op = new Operation(false);
        op->Init(id, date, val, label);
    }
    // august 2022
    else if (fields.size() == 13)
    {
        date = QDate::fromString(fields.at(0), QString("dd/MM/yyyy"));
        // id = date + counter
        id = QString("%1_%2").arg(fields.at(1)).arg(counter);
        // values (debit or credit
        if (!fields.at(8).isEmpty())
        {
            val = def_loc.toFloat(fields.at(8));
        }
        else
        {
            val = def_loc.toFloat(fields.at(9));
        }

        // label
        label = fields.at(1);
        op = new Operation(false);
        op->Init(id, date, val, label);
    }
    return op;
}
