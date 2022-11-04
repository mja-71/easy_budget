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

#ifndef DATAACCOUNT_H
#define DATAACCOUNT_H

#include <list>
#include <QString>
#include <QStringList>

#include "AccountToolData_global.h"
#include "period.h"
#include "operation.h"
#include "budgetitem.h"
#include "statement.h"

/**
 * @brief Generic account.
 */
class ACCOUNTTOOLDATASHARED_EXPORT DataAccount
{
public:
    enum eStatus
    {
        E_NONE = 0x0000,
        E_BANK = 0x0001,
        E_USER = 0x0002,
        E_VALID = 0x0004,
        E_ALL = 0xFFFF
    };
    //---------- Statement
    virtual void Close(Period* p) = 0;
    virtual int Years(QList<Period*>& list_res) = 0;
    virtual int BankStats(const QString& year, QList<Statement*>& list_rel) = 0;
    virtual const Statement* BankStat(const QString& id) = 0;
    virtual bool CreateBankStat(const QString& year, int month) = 0;
    virtual bool UpdateStat(const QString& stat_id) = 0;
    virtual bool RestoreStat(const QString& stat_id) = 0;
    //---------- Operations
    virtual void SetTransfer(Period* p, Operation* op, bool save = true) = 0;
    virtual int AddOps(const QList<Operation*>& lst,
                       const QString& stat_id,
                       bool save = true) = 0;
    virtual QList<Operation*>& Operations(const QString& stat) = 0;
    virtual void Operations(const QString& stat,
                            int status,
                            QList<Operation*>& list_ops) = 0;
    //---------- Positions
    virtual bool AddRefPosition(const StatePos& pos) = 0;
    //---------- Items
    virtual QList<BudgetItem*> Items(Period* p) = 0;
    virtual BudgetItem* InsertItem(const QString& label) = 0;
    virtual bool UpdateItems() = 0;
};

#endif // DATAACCOUNT_H
