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

#ifndef APPSETTING_H
#define APPSETTING_H

class QWidget;
class QString;
class QMainWindow;
class QHeaderView;
class QSplitter;

/**
 * @brief The AppSetting class manage Application user settings.
 */
class AppSetting
{
public:
	AppSetting();

	//--- HMI
	static void SaveGeometry(const QWidget &w, const QString &name);
	static void RestoreGeometry(QWidget& w, const QString &name);
	static void SaveState(const QMainWindow &w, const QString &name);
	static void RestoreState(QMainWindow& w, const QString &name);
	static void SaveState(const QHeaderView &w, const QString &name);
	static void RestoreState(QHeaderView &w, const QString &name);
	static void SaveState(const QSplitter &w, const QString &name);
	static void RestoreState(QSplitter& w, const QString &name);
	// --- FOLDER
	static void SaveImportFolder(const QString& folder);
	static QString ImportFolder();
};

#endif // APPSETTING_H
