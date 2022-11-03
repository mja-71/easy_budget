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

#ifndef ACCOUNTMGR_H
#define ACCOUNTMGR_H

#include <QList>
#include <QMap>
#include <QDate>

#include "AccountToolData_global.h"

class DataAccount;
class Operation;
class BudgetItem;
class AccountSynthesis;
class Period;

class ACCOUNTTOOLDATASHARED_EXPORT AccountMgr
{
public:
    class ImportReport
    {
    public:
        int nb_ops_read;
        int nb_ops_added;
        QDate d1;
        QDate d2;
    };

    static AccountMgr* Mgr()
    {
        if (m_pMgr == 0)
        {
            m_pMgr = new AccountMgr;
        }
        return m_pMgr;
    }

    static void Release()
    {
        delete m_pMgr;
        m_pMgr = 0;
    }

    bool Import(const QString& str_path, ImportReport& report);

    /// Open a data base.
    bool Open(const QString& ident);

    /// get Database object.
    DataAccount* Db() { return m_pDb; }

    /// Update items
    void UpdateItems(QList<Operation*>& list_op,
                     const QList<BudgetItem*>& list_items,
                     bool clean);

    /// get Expert object.
    AccountSynthesis* Analyser(Period* year);

protected:
    AccountMgr();
    ~AccountMgr();

    /// delete synthesys objects.
    void ClearSynthesis();

private:
    /// singleton
    static AccountMgr* m_pMgr;

    /// Database
    DataAccount* m_pDb;

    /// experts
    QMap<Period*, AccountSynthesis*> m_MapSynthesis;
};

#endif // ACCOUNTMGR_H
