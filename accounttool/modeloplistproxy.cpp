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

#include "modeloplistproxy.h"
#include "modeloplist.h"

ModelOpListProxy::ModelOpListProxy(QObject* parent /*= Q_NULLPTR*/)
: QSortFilterProxyModel(parent)
{
}

ModelOpListProxy::~ModelOpListProxy()
{
}

bool ModelOpListProxy::lessThan(const QModelIndex& source_left,
                                const QModelIndex& source_right) const
{
    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
