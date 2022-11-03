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

#include "accountsynthesis.h"
#include "operation.h"
#include "dataaccount.h"
#include <QFile>
#include <QTextStream>

AccountSynthesis::AccountSynthesis(DataAccount* pDb, Period* year)
: m_pDatabase(pDb), m_Year(year), m_NeedUpdate(true)
{
    // init maps
    for (int i = 0; i < 12; i++)
    {
        m_Totals[i][0] = new QMap<unsigned long, float>;
        m_Totals[i][1] = new QMap<unsigned long, float>;
        m_TotalOther[i][0] = 0.;
        m_TotalOther[i][1] = 0.;
        m_TotalByMonth[i][0] = 0.;
        m_TotalByMonth[i][1] = 0.;
    }
}

AccountSynthesis::~AccountSynthesis()
{
    // clean maps
    for (int i = 0; i < 12; i++)
    {
        delete m_Totals[i][0];
        delete m_Totals[i][1];
    }
}


float AccountSynthesis::GetItemMonthTotal(int month,
                                          unsigned long item_id,
                                          eCat cat)
{
    if (m_NeedUpdate)
    {
        Update();
    }
    return (*m_Totals[month][cat])[item_id];
}


float AccountSynthesis::GetItemTotal(unsigned long item_id, eCat cat)
{
    float s = 0;
    if (m_NeedUpdate)
    {
        Update();
    }
    for (int i = 0; i < 12; i++)
    {
        s += (*m_Totals[i][cat])[item_id];
    }
    return s;
}


float AccountSynthesis::GetOtherMonthTotal(int month, eCat cat)
{
    if (m_NeedUpdate)
    {
        Update();
    }
    return m_TotalOther[month][cat];
}

float AccountSynthesis::GetOtherTotal(eCat cat)
{
    float s = 0;
    if (m_NeedUpdate)
    {
        Update();
    }
    for (int i = 0; i < 12; i++)
    {
        s += m_TotalOther[i][cat];
    }
    return s;
}

float AccountSynthesis::GetTotalByMonth(int month, eCat cat)
{
    if (m_NeedUpdate)
    {
        Update();
    }
    return m_TotalByMonth[month][cat];
}

void AccountSynthesis::Update(int month)
{
    int begin = 0, end = 12;
    if (month != -1)
    {
        begin = month;
        end = month + 1;
    }
    // clear maps
    for (int i = begin; i < end; i++)
    {
        (*m_Totals[i][0]).clear();
        (*m_Totals[i][1]).clear();
        m_TotalOther[i][0] = 0.;
        m_TotalOther[i][1] = 0.;
        m_TotalByMonth[i][0] = 0.;
        m_TotalByMonth[i][1] = 0.;
    }
    for (int i = begin; i < end; i++)
    {
        const QList<Operation*>& list_ops =
            m_pDatabase->Operations(QString("%1-%2")
                                        .arg(m_Year->Name())
                                        .arg(i + 1, 2, 10, QLatin1Char('0')));
        ComputeTotal(i, list_ops);

        // update position
    }
    if (month == -1)
    {
        m_NeedUpdate = false;
    }
}

float AccountSynthesis::GetTotal(eCat cat)
{
    float s = 0;
    if (m_NeedUpdate)
    {
        Update();
    }
    for (int i = 0; i < 12; i++)
    {
        s += m_TotalByMonth[i][cat];
    }
    return s;
}


/// return position at statement end.
float AccountSynthesis::GetEndPosition(const StatePos& ref)
{
    // trivial case : ref is at the end of the statement
    if (ref.IsEndState())
    {
        return ref.Pos();
    }
    // get statement
    const QList<Operation*>& list_ops = m_pDatabase->Operations(ref.StateId());
    // compute diff
    float pos = ref.Pos();
    foreach (Operation* op, list_ops)
    {
        if ((op->Date() > ref.Date()) && op->IsBank())
        {
            pos += op->Value();
        }
    }
    return pos;
}

bool AccountSynthesis::CheckPosition(int month, float pos_val, float& diff)
{
    float prev_pos = 0.0;
    // get previous position
    if (month > 0)
    {
        /*
        QString s_id =  QString("%1-%2").arg(m_Year->Name())
                                        .arg(month,2,10,QChar('0'));
        const Statement* s = m_pDatabase->BankStat(s_id);
        prev_pos = s->Position().Pos();
        */
        prev_pos = GetCalPos(month - 1);
    }
    else if (month == 0)
    {
        Operation* transfer = m_Year->Transfer();
        if (transfer)
        {
            prev_pos = transfer->Value();
        }
    }
    // get difference
    float d = GetTotalByMonth(month, CAT_DEBIT),
          c = GetTotalByMonth(month, CAT_CREDIT);
    diff = pos_val - (prev_pos + c + d);
    return ((diff > 0.01) || (diff < -0.01));
}

/// Calculate position using total credit and debit.
float AccountSynthesis::GetCalPos(int month)
{
    float pos = 0.0;
    pos =
        GetTotalByMonth(month, CAT_DEBIT) + GetTotalByMonth(month, CAT_CREDIT);
    // add transfer (first month)
    if (month == 0)
    {
        Operation* transfer = m_Year->Transfer();
        if (transfer)
        {
            pos += transfer->Value();
        }
    }
    // add report of previous month
    else if (month > 0)
    {
        pos += GetCalPos(month - 1);
    }
    return pos;
}

bool AccountSynthesis::Export(const QString& path)
{
    QFile f(path);
    if (!f.open(QFile::WriteOnly | QFile::Truncate))
    {
        return false;
    }
    QTextStream stream(&f);
    stream.setLocale(QLocale()); // use default locale
    // doc title
    stream << QObject::tr("Year %1\n").arg(m_Year->Name());
    // Credit array
    ExportArray(stream, CAT_CREDIT);
    stream << "\n\n";
    // Debit array
    ExportArray(stream, CAT_DEBIT);

    return true;
}

//----------------- PROTECTED
//---------------------------------------------------

void AccountSynthesis::ComputeTotal(int month,
                                    const QList<Operation*>& list_ops)
{
    foreach (Operation* op, list_ops)
    {
        if (op->Item() == 0)
        {
            if (op->Value() > 0.)
            {
                m_TotalOther[month][0] += op->Value();
                m_TotalByMonth[month][0] += op->Value();
            }
            else
            {
                m_TotalOther[month][1] += op->Value();
                m_TotalByMonth[month][1] += op->Value();
            }
        }
        else
        {
            if (op->Item()->IsCredit())
            {
                (*m_Totals[month][0])[op->Item()->Id()] += op->Value();
                m_TotalByMonth[month][0] += op->Value();
            }
            else if (op->Item()->IsDebit())
            {
                (*m_Totals[month][1])[op->Item()->Id()] += op->Value();
                m_TotalByMonth[month][1] += op->Value();
            }
            else
            {
                if (op->Value() > 0.)
                {
                    (*m_Totals[month][0])[op->Item()->Id()] += op->Value();
                    m_TotalByMonth[month][0] += op->Value();
                }
                else
                {
                    (*m_Totals[month][1])[op->Item()->Id()] += op->Value();
                    m_TotalByMonth[month][1] += op->Value();
                }
            }
        }
    }
}

void AccountSynthesis::ExportArray(QTextStream& stream, eCat cat)
{
    int i;

    // ---- titles
    stream << ";";
    for (i = 1; i <= 12; i++)
    {
        // stream << QDate::shortMonthName(i) << ";";
        stream << QDate(2021, i, 1).toString("MMM") << ";";
    }
    stream << QObject::tr("Total") << "\n";
    // ---- row = item
    QList<BudgetItem*> list_items = m_pDatabase->Items(m_Year);
    foreach (const BudgetItem* item, list_items)
    {
        if ((cat == CAT_CREDIT && item->IsCredit()) ||
            (cat == CAT_DEBIT && item->IsDebit()) || (item->IsUndefined()))
        {
            if (!item->IsDeprecated())
            {
                stream << item->Name() << ";";
                for (i = 0; i < 12; i++)
                {
                    stream << GetItemMonthTotal(i, item->Id(), cat) << ";";
                }
                stream << GetItemTotal(item->Id(), cat) << "\n";
            }
        }
    }
    //---- Other
    stream << QObject::tr("other") << ";";
    for (i = 0; i < 12; i++)
    {
        stream << m_TotalOther[i][cat] << ";";
    }
    stream << GetOtherTotal(cat) << "\n";

    //---- Total
    stream << QObject::tr("Total") << ";";
    for (i = 0; i < 12; i++)
    {
        stream << m_TotalByMonth[i][cat] << ";";
    }
    stream << GetTotal(cat) << "\n";
}
