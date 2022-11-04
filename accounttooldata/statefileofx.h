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

#ifndef STATEFILEOFX_H
#define STATEFILEOFX_H

#include <QDate>
#include <QTextStream>
#include "istatefile.h"
#include "operation.h"

/**
 * @brief Import file in Money format.
 * file contains the list of statement.
 */
class StateFileOfx : public IStateFile
{
public:
    StateFileOfx(const QString& path);
    virtual void Period(QDate& d_start, QDate& d_end) override;
    virtual int LoadOperations(QList<Operation*>& list_op) override;
    virtual bool Position(float& pos) const override
    {
        pos = m_Pos;
        return m_PosFound;
    }

protected:
    bool Load(const QString& path);
    Operation* CreateOp(QTextStream& xml);
    void Extract(const QString& line, QString& tag, QString& val);

private:
    QList<Operation*> m_List;
    QDate m_Start;
    QDate m_End;
    QString m_Path;
    bool m_Valid;
    bool m_Err;
    float m_Pos; ///< position at end period.
    bool m_PosFound;
};

#endif // STATEFILEOFX_H
