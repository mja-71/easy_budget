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

#include "operation.h"
#include <QStringList>

/// Default constructor.
Operation::Operation(bool user /*=user*/)
: m_Status(user ? S_USER : S_BANK), m_Item(0)
{
}


/// Copy constructor
Operation::Operation(const Operation& op)
{
    m_Id = op.m_Id;
    m_Label = op.m_Label;
    m_Date = op.m_Date;
    m_Value = op.m_Value;
    m_Item = op.m_Item;
    m_Comment = op.m_Comment;
    m_Status = op.m_Status;
}

/// Assignment operator
Operation& Operation::operator=(const Operation& op)
{
    m_Id = op.m_Id;
    m_Label = op.m_Label;
    m_Date = op.m_Date;
    m_Value = op.m_Value;
    m_Item = op.m_Item;
    m_Comment = op.m_Comment;
    m_Status = op.m_Status;
    return *this;
}
