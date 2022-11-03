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

#ifndef DATAACCOUNTFILE_H
#define DATAACCOUNTFILE_H

#include <QString>
#include <QMap>
#include <QList>
#include <QXmlStreamReader>

#include "operation.h"
#include "dataaccount.h"
#include "statement.h"

class Period;

/**
 * @brief The DataAccountFile class
 * Database is defined by a root path.
 * one folder by year
 * Data is stored in files : one file by month
 */
class DataAccountFile : public DataAccount
{
public:
    /// Constructor with folder path
    DataAccountFile(const QString& path);

    /// Destructor
    virtual ~DataAccountFile();

    virtual void Close(Period* p);

    /// Get Years
    virtual int Years(QList<Period*>& list_res);

    /// Get list of statements for one year
    virtual int BankStats(const QString& year, QList<Statement*>& list_rel);

    /// Get a statement
    virtual const Statement* BankStat(const QString& id);

    /// Create Bank statement.
    virtual bool CreateBankStat(const QString& year, int month);

    /// Get list of Operations of a statement
    virtual QList<Operation*>& Operations(const QString& stat);
    virtual void Operations(const QString& stat,
                            int status,
                            QList<Operation*>& list_ops);

    /// Get list of budget items defined in the account.
    virtual QList<BudgetItem*> Items(Period* p);
    virtual BudgetItem* InsertItem(const QString& label);

    /// Add operations into the db.
    virtual void SetTransfer(Period* p, Operation* op, bool save = true);
    virtual int AddOps(const QList<Operation*>& ops,
                       const QString& stat_id,
                       bool save = true);

    /// Add a reference position in db.
    virtual bool AddRefPosition(const StatePos& pos);

    /// Save statement in db.
    virtual bool UpdateStat(const QString& stat_id);
    /// Restore statement from db.
    virtual bool RestoreStat(const QString& stat_id);

    /// Save items in db.
    virtual bool UpdateItems();

protected:
    /// Load years from root folder.
    int LoadYears();
    /// Get period corresponding to the year \p year.
    Period* GetPeriod(const QString& year);
    /// Delete statements
    void DeleteStats();
    /// Load of Operations of a statement
    int LoadOps(Statement* st);
    /// Create operation from string.
    Operation* CreateOp(const QString& str,
                        QMap<unsigned long, BudgetItem*>& map_items);
    /// Get position from string.
    StatePos CreatePos(const QString& str);
    /// Get transfer from string.
    Operation* CreateTransfer(const QString& str);
    /// Save statement in file
    bool SaveOps(Statement* st);
    /// Load file with items
    int LoadItems(const QString& path_file,
                  QMap<unsigned long, BudgetItem*>& map_item);
    int LoadItem(QXmlStreamReader& xml, BudgetItem& p);
    /// Get a statement
    Statement* GetStat(const QString& str_id, bool load);
    /// Search operation by id
    Operation* Find(const QString& id, const QList<Operation*>& list_ops) const;

private:
    /// folder containing "statement", sorted by year
    QString m_Path;

    /// Period list, with year (YYYY) as key.
    QMap<QString, Period*> m_MapYear;

    /// Budget item list.
    QMap<Period*, QList<BudgetItem*>> m_ListPeriodItems;
    QMap<unsigned long, BudgetItem*>* m_MapItems; // current item list
};

#endif // DATAACCOUNTFILE_H
