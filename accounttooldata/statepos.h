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

#ifndef STATEPOS_H
#define STATEPOS_H

#include <QDate>

/**
 * @brief Position at a date.
 */
class StatePos
{
public:
    StatePos();
    StatePos(const QDate& d, float val);
    StatePos& operator=(const StatePos& r);
    float Pos() const { return m_Pos; }
    QString StrPos() const { return QString("%1").arg(m_Pos, 0, 'f', 2); }
    QString StateId() const
    {
        return (m_Date.isValid() ? m_Date.toString("yyyy-MM") : "0000-00");
    }
    const QDate& Date() const { return m_Date; }
    bool IsEndState() const
    {
        return (m_Date.daysTo(QDate(m_Date.year(), m_Date.month(), 1)) == 1);
    }

private:
    QDate m_Date;
    float m_Pos;
};

#endif // STATEPOS_H
