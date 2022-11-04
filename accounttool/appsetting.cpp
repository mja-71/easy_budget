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

#include "appsetting.h"

#include <QSettings>
#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QHeaderView>
#include <QSplitter>
#include <QStandardPaths>

AppSetting::AppSetting()
{
}

void AppSetting::SaveGeometry(const QWidget& w, const QString& name)
{
    QSettings settings("J-IHM", "EasyBudget");
    settings.setValue(name + "/geometry", w.saveGeometry());
}

void AppSetting::RestoreGeometry(QWidget& w, const QString& name)
{
    QSettings settings("J-IHM", "EasyBudget");
    w.restoreGeometry(settings.value(name + "/geometry").toByteArray());
}

void AppSetting::SaveState(const QMainWindow& w, const QString& name)
{
    QSettings settings("J-IHM", "EasyBudget");
    settings.setValue(name + "/geometry", w.saveGeometry());
    settings.setValue(name + "/windowState", w.saveState());
}

void AppSetting::RestoreState(QMainWindow& w, const QString& name)
{
    QSettings settings("J-IHM", "EasyBudget");
    w.restoreGeometry(settings.value(name + "/geometry").toByteArray());
    w.restoreState(settings.value(name + "/windowState").toByteArray());
}

void AppSetting::SaveState(const QHeaderView& w, const QString& name)
{
    QSettings settings("J-IHM", "EasyBudget");
    settings.setValue(name + "/geometry", w.saveGeometry());
    settings.setValue(name + "/windowState", w.saveState());
}

void AppSetting::RestoreState(QHeaderView& w, const QString& name)
{
    QSettings settings("J-IHM", "EasyBudget");
    w.restoreGeometry(settings.value(name + "/geometry").toByteArray());
    w.restoreState(settings.value(name + "/windowState").toByteArray());
}

void AppSetting::SaveState(const QSplitter& w, const QString& name)
{
    QSettings settings("J-IHM", "EasyBudget");
    settings.setValue(name + "/geometry", w.saveGeometry());
    settings.setValue(name + "/windowState", w.saveState());
}

void AppSetting::RestoreState(QSplitter& w, const QString& name)
{
    QSettings settings("J-IHM", "EasyBudget");
    w.restoreState(settings.value(name + "/windowState").toByteArray());
}

//------------------- FOLDERS -------------------------------------------------

QString AppSetting::ImportFolder()
{
    QSettings settings("J-IHM", "EasyBudget");
    QString path = settings.value("ImportFolder").toString();
    if (path.isEmpty())
    {
        path =
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    }
    return path;
}

void AppSetting::SaveImportFolder(const QString& folder)
{
    QSettings settings("J-IHM", "EasyBudget");
    settings.setValue("ImportFolder", QVariant(folder));
}
