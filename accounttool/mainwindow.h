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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui
{
class MainWindow;
}

class AccountMgr;
class DataAccount;
class PanelState;
class PanelSynthese;
class PanelItems;
class AppConfig;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_actionIMPORT_triggered();

    void on_actionOPEN_triggered();

    void on_actionDisplayReleve_triggered();

    void on_actionDisplaySynthese_triggered();

    void on_actionUPDATE_ITEM_triggered();

    void on_actionEXPORT_triggered();

    void on_actionDisplayItems_triggered();

    void on_actionHELP_triggered();

protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    Ui::MainWindow* ui;
    // panels
    PanelState* m_pPanelReleve;
    PanelSynthese* m_pPanelSynthese;
    PanelItems* m_pPanelItems;
    AppConfig* m_pConfig;
    AccountMgr* m_pMgr; ///< sys facade
    // DataAccount* m_pDatabase;   ///< account database
    QProcess m_Assistant;
};

#endif // MAINWINDOW_H
