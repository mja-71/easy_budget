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

#ifndef ACCOUNTTOOLDATA_GLOBAL_H
#define ACCOUNTTOOLDATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ACCOUNTTOOLDATA_LIBRARY)
#define ACCOUNTTOOLDATASHARED_EXPORT Q_DECL_EXPORT
#else
#define ACCOUNTTOOLDATASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ACCOUNTTOOLDATA_GLOBAL_H
