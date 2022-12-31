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

#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QList>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/bourse.png"));

    // translations
    QStringList modules;
    modules << "accountdata"
            << "account"
            << "qtbase";
    // QList<QTranslator> translators(modules.size());
    QTranslator translators[4];
    QTranslator* ptr_tr = translators;
    foreach (QString m, modules)
    {
        if (!ptr_tr->load(QLocale::system(), m, "_", ":/i18n", ".qm"))
        {
            qDebug("No translation file found for %s",
                   m.toLatin1().constData());
        }
        else
        {
            a.installTranslator(ptr_tr);
        }
        ptr_tr++;
    }

    // display main window
    MainWindow w;
    w.show();
    // w.setWindowState(Qt::WindowMaximized);
    return a.exec();
}
