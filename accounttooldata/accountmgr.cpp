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

#include "accountmgr.h"

#include <QFileInfo>

// database managers
#include "dataaccountfile.h"
#include "accountsynthesis.h"

// Import
#include "statefilefactory.h"
#include "statefilece.h"
#include "statefileofx.h"

AccountMgr* AccountMgr::m_pMgr = 0;

AccountMgr::AccountMgr() : m_pDb(0)
{
}

AccountMgr::~AccountMgr()
{
    ClearSynthesis();
    delete m_pDb;
}

/// Open a data base.
/// ident can be :
/// file database : "FILE:path"
bool AccountMgr::Open(const QString& ident)
{
    ClearSynthesis();
    delete m_pDb;
    m_pDb = 0;

    // parse ident
    if (ident.left(4) == "FILE")
    {
        m_pDb = new DataAccountFile(ident.mid(5));
    }

    return (m_pDb != 0);
}

/// Import operations from file to a database.
bool AccountMgr::Import(const QString& str_path, ImportReport& report)
{
    // todo : check the file format.
    QFileInfo info(str_path);

    // db must be opened.
    if (m_pDb == 0)
    {
        return false;
    }

    // Read operations
    StateFileFactory factory;
    IStateFile* parser = factory.Parser(str_path);
    QDate d1, d2;
    QList<Operation*> lst;
    StatePos pos;
    float new_pos;
    if (parser != 0)
    {
        if (parser->LoadOperations(lst) < 0)
        {
            delete parser;
            return false;
        }
        parser->Period(d1, d2);
        if (parser->Position(new_pos))
        {
            pos = StatePos(d2, new_pos);
            m_pDb->AddRefPosition(pos);
        }
        delete parser;

        // fill the report
        report.nb_ops_read = lst.count();
        report.d1 = d1;
        report.d2 = d2;

        // link ops with items
        QList<BudgetItem*> list_items = m_pDb->Items(0);
        UpdateItems(lst, list_items, true);

        // add operations into the database
        int nb_ops;
        nb_ops = m_pDb->AddOps(lst, QString("")); // add and save data
        report.nb_ops_added = nb_ops;
        return (nb_ops != -1);
    }
    else
    {
        return false;
    }
}

/// Find item of each operation.
void AccountMgr::UpdateItems(QList<Operation*>& list_op,
                             const QList<BudgetItem*>& list_items,
                             bool clean)
{
    QList<Operation*>::iterator it_o;
    QList<BudgetItem*>::const_iterator it_i;
    for (it_o = list_op.begin(); it_o != list_op.end(); ++it_o)
    {
        if ((*it_o)->Item() != 0)
        {
            if (clean)
            {
                (*it_o)->SetItem(0);
            }
            else
            {
                continue;
            }
        }

        for (it_i = list_items.constBegin(); it_i != list_items.constEnd();
             ++it_i)
        {
            if ((*it_i)->Include((*it_o)->Label()))
            {
                if (((*it_i)->IsCredit() && ((*it_o)->Value() > 0)) ||
                    ((*it_i)->IsDebit() && ((*it_o)->Value() < 0)) ||
                    (*it_i)->IsUndefined())
                {
                    (*it_o)->SetItem((*it_i));
                }
            }
        }
    }
}

/// get Expert object.
AccountSynthesis* AccountMgr::Analyser(Period* year)
{
    AccountSynthesis* res = 0;
    QMap<Period*, AccountSynthesis*>::const_iterator it =
        m_MapSynthesis.find(year);
    if (it == m_MapSynthesis.constEnd())
    {
        res = new AccountSynthesis(m_pDb, year);
        m_MapSynthesis[year] = res;
    }
    else
    {
        res = it.value();
    }
    return res;
}

/// delete synthesys objects.
void AccountMgr::ClearSynthesis()
{
    foreach (AccountSynthesis* pSynthesis, m_MapSynthesis)
    {
        delete pSynthesis;
    }
    m_MapSynthesis.clear();
}
