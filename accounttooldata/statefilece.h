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

#ifndef STATEFILECE_H
#define STATEFILECE_H

#include "istatefile.h"
#include <QDate>
#include "operation.h"

/**
 * @brief Caisse Epargne format.
 * file contains the list of statement.
 */
class StateFileCE : public IStateFile
{
public:
    StateFileCE(const QString& path);
    virtual void Period(QDate& d_start, QDate& d_end) override;
    virtual int LoadOperations(QList<Operation*>& list_op) override;
    virtual bool Position(float& pos) const override
    {
        pos = m_Pos;
        return (m_Ver == 1);
    }

protected:
    bool Load(const QString& path);
    bool Parse1(QTextStream& in);
    bool Parse2(QTextStream& in);
    bool ParsePosition(const QString& line);
    Operation* CreateOp(const QString& str);
    Operation* CreateOp2(const QString& str, int counter);

private:
    QList<Operation*> m_List;
    float m_Pos; ///< position at end period.
    QDate m_Start;
    QDate m_End;
    QString m_Path;
    bool m_Valid;
    bool m_Err;
    int m_Ver;
};

#endif // STATEFILECE_H
