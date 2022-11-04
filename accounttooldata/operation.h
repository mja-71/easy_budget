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

#ifndef OPERATION_H
#define OPERATION_H

#include "AccountToolData_global.h"
#include <QString>
#include <QDate>
#include <QLocale>
#include "budgetitem.h"

/**
 * @brief The Operation class contains all information of a operation.
 * Information can be read from string
 */
class ACCOUNTTOOLDATASHARED_EXPORT Operation
{
public:
    enum eStatus
    {
        S_USER = 0x0001,
        S_BANK = 0x0002,
        S_REPORT = 0x0004,
        S_VALID = 0x0010,
        S_TRANSFER = 0x0020
    };

    /// default contructor.
    Operation(bool user = true);
    /// Copy constructor
    Operation(const Operation& op);
    /// Assignment operator
    Operation& operator=(const Operation& op);
    /// set data.
    void Init(const QString& id, const QDate& d, float v, const QString label)
    {
        m_Id = id;
        m_Date = d;
        m_Value = v;
        m_Label = label;
    }

    //--- Accessor
    /// date
    QString StringDate() const
    {
        QLocale def_loc;
        return def_loc.toString(m_Date, QLocale::ShortFormat);
    }
    const QDate& Date() const { return m_Date; }
    /// Label
    QString Label() const { return m_Label; }
    /// Value
    QString Val() const
    {
        QLocale def_loc;
        return def_loc.toString(m_Value, 'f', 2);
    }
    float Value() const { return m_Value; }
    /// Item
    BudgetItem* Item() const { return m_Item; }
    /// Comment
    QString Comment() const { return m_Comment; }
    /// Id
    QString Id() const { return m_Id; }
    /// Statement id.
    QString StatId() const { return m_Date.toString("yyyy-MM"); }
    /// Validity status
    bool IsValid() const { return ((m_Status & S_VALID) != 0); }
    /// Origin Status
    bool IsUser() const { return ((m_Status & S_USER) != 0); }
    bool IsBank() const { return ((m_Status & S_BANK) != 0); }
    bool IsTransfer() const { return ((m_Status & S_TRANSFER) != 0); }
    //--- Modifier
    /// Set item.
    void SetItem(BudgetItem* item) { m_Item = item; }
    /// Set user comment.
    void SetComment(const QString& str_comment) { m_Comment = str_comment; }
    /// Set status
    void SetStatus(bool valid)
    {
        m_Status = valid ? (m_Status | S_VALID) : (m_Status & ~S_VALID);
    }
    /// Set status
    void SetTransfer(bool is_transfer)
    {
        m_Status =
            is_transfer ? (m_Status | S_TRANSFER) : (m_Status & ~S_TRANSFER);
    }
    /// set label
    void SetLabel(const QString& str_label) { m_Label = str_label; }
    /// set value
    void SetValue(float val) { m_Value = val; }
    /// set date
    void SetDate(const QDate& d) { m_Date = d; }

private:
    QString m_Id;
    QString m_Label;
    QDate m_Date;
    float m_Value;
    BudgetItem* m_Item; ///< Item, null if "other"
    QString m_Comment;  ///< User comment.
    unsigned short m_Status;
};

#endif // OPERATION_H
