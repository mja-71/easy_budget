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

#ifndef ACCOUNTSYNTHESIS_H
#define ACCOUNTSYNTHESIS_H

#include "AccountToolData_global.h"
#include "statepos.h"
#include <QMap>
#include <QString>
#include <QTextStream>

class DataAccount;
class Operation;
class Period;

class ACCOUNTTOOLDATASHARED_EXPORT AccountSynthesis
{
public:
    enum eCat
    {
        CAT_CREDIT = 0,
        CAT_DEBIT = 1
    };

    AccountSynthesis(DataAccount* pDb, Period* year);
    virtual ~AccountSynthesis();

    void Update(int month = -1);

    float GetItemMonthTotal(int month, unsigned long item_id, eCat cat);
    float GetItemTotal(unsigned long item_id, eCat cat);
    float GetOtherMonthTotal(int month, eCat cat);
    float GetOtherTotal(eCat cat);
    float GetTotalByMonth(int month, eCat cat);
    float GetTotal(eCat cat);

    /// return position at statement end.
    float GetEndPosition(const StatePos& ref);
    /// Check position according credit and debit of the month
    bool CheckPosition(int month, float pos_val, float& diff);
    /// Calculate position using total credit and debit.
    float GetCalPos(int month);
    /// export annual synthesys.
    bool Export(const QString& path);

protected:
    void ComputeTotal(int month, const QList<Operation*>& list_ops);
    void ExportArray(QTextStream& stream, eCat cat);

private:
    DataAccount* m_pDatabase;
    Period* m_Year;
    /// Total by item
    QMap<unsigned long, float>* m_Totals[12][2];
    ///
    float m_TotalOther[12][2];
    float m_TotalByMonth[12][2];

    bool m_NeedUpdate;
};

#endif // ACCOUNTSYNTHESIS_H
