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

#include "appconfig.h"

#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

/** Constructor. */
AppConfig::AppConfig()
{
}

/** Destructor. */
AppConfig::~AppConfig()
{
}

void AppConfig::Init(const QString& p)
{
    QFileInfo fi(p);
    // add app dir if relative
    if (fi.isRelative())
    {
        QString abs_path = QCoreApplication::applicationDirPath() + "/" + p;
        fi.setFile(abs_path);
    }
    m_CfgPath = fi.absoluteFilePath();

    // create user appdata folder
    QString abs_path =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
        "/JihmSoftware/AccountTool/";
    QDir data_dir;
    data_dir.mkpath(abs_path);
}

/** Load config file.
    read xml file as a stream and store data
*/
int AppConfig::Load()
{
    QFile cfg_file(m_CfgPath);
    // open file
    if (!cfg_file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    QXmlStreamReader xml;
    xml.setDevice(&cfg_file);
    bool found = false;
    QString name;
    // go to SOFT_ACCOUNT
    while (!found && !xml.atEnd())
    {
        xml.readNextStartElement();
        found = (xml.name().compare(QString("SOFT_ACCOUNT")) == 0);
    }
    if (xml.hasError() || !found)
    {
        return -2;
    }
    // read fields
    while (!xml.atEnd())
    {
        xml.readNextStartElement();
        name = xml.name().toString();
        if (xml.name().compare(QString("CURRENT_BDD")) == 0)
        {
            m_CurrDb = xml.readElementText();
        }
    }
    return 0;
}
