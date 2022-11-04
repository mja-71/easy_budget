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

#ifndef STATEMENT_H
#define STATEMENT_H

#include "AccountToolData_global.h"
#include <QString>
#include <QDate>
#include "statepos.h"

class Operation;

/**
 * @brief Bank statement.
 */
class ACCOUNTTOOLDATASHARED_EXPORT Statement
{
public:
    Statement() {}
    ~Statement();
    Statement(const QString& id, const QString& label);
    Statement(const Statement& s);
    Statement& operator=(const Statement& s);
    QString Label() const { return m_Label; }
    QString Id() const { return m_Id; }
    QDate Date() const;
    const StatePos& Position() const { return m_Position; }
    void SetPosition(const StatePos& pos);
    QList<Operation*>* Ops() { return m_ListOps; }
    void SetOps(QList<Operation*>* list_ops);

protected:
    void CleanOps();

private:
    QString m_Id; ///< id is "YYYY-MM"
    QString m_Label;
    StatePos m_Position; ///< Position according bank
    QList<Operation*>* m_ListOps;
};

#endif // STATEMENT_H
