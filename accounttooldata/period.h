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

#ifndef PERIOD_H
#define PERIOD_H

#include <QString>
#include <QMap>
#include <qmetatype.h>

class Statement;
class BudgetItem;
class Operation;

/**
 * @brief Period is an accounting period.
 */
class Period
{
public:
    Period(const QString& name, bool open = true);
    virtual ~Period();
    QString Name() const { return m_Name; }
    bool IsClose() const { return m_Status == S_CLOSE; }
    void SetClose(bool is_close);
    void SetTransfer(Operation* op);
    Operation* Transfer() const { return m_Transfer; }
    Statement* GetStatement(const QString& str_id);
    QMap<unsigned long, BudgetItem*>* Items() { return m_MapItems; }
    void SetItems(QMap<unsigned long, BudgetItem*>* items)
    {
        m_MapItems = items;
    }

protected:
    void DeleteStatements();

private:
    enum eStatus
    {
        S_OPEN,
        S_CLOSE
    };

    eStatus m_Status;
    QString m_Name;

    /// Statement list.
    /// store operations : key is statement id (yyyy-MM)
    QMap<QString, Statement*> m_MapStat;

    /// Budget item list.
    QMap<unsigned long, BudgetItem*>* m_MapItems;

    /// Transfer from previous period.
    Operation* m_Transfer;
};

// to use Period* as QVariant
Q_DECLARE_METATYPE(Period*)

#endif // PERIOD_H
