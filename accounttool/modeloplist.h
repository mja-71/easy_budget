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

#ifndef MODELOPLIST_H
#define MODELOPLIST_H

#include <QAbstractListModel>
#include <QStringList>
#include "accounttooldata/dataaccount.h"

class AccountSynthesis;

// to use Operation* as QVariant
Q_DECLARE_METATYPE(Operation*)

class ModelOpList : public QAbstractListModel
{
    Q_OBJECT

public:
    enum eCOL
    {
        E_VALIDITY = 0,
        E_DATE = 1,
        E_LABEL = 2,
        E_VAL = 3,
        E_ITEM = 4,
        E_DESC = 5,
        E_MAX = 6
    };

    ModelOpList(QObject* parent);
    void SetState(DataAccount* pData, Period* year, Statement* state);
    void AddOperation(Operation* op);
    // subclassing
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual bool insertRows(int row,
                            int count,
                            const QModelIndex& parent = QModelIndex());
    virtual bool removeRows(int row,
                            int count,
                            const QModelIndex& parent = QModelIndex());
    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index,
                         const QVariant& value,
                         int role = Qt::EditRole);
    virtual QVariant headerData(int section,
                                Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;

protected:
    void MoveOp(Operation* op);
    Operation* GetOp(int row) const;

private:
    Period* m_Year;
    DataAccount* m_pData;
    Statement* m_CurrStat;
    QStringList m_Titles;
    Operation* m_Transfer;
    AccountSynthesis* m_Synthesis;
};

#endif // MODELOPLIST_H
