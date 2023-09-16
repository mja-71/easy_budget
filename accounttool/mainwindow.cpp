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
#include "ui_mainwindow.h"
#include "accounttooldata/accountmgr.h"
#include "accounttooldata/dataaccount.h"
#include "panelreleve.h"
#include "panelsynthese.h"
#include "panelitems.h"
#include "appconfig.h"
#include "appsetting.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QLocale>

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent), ui(new Ui::MainWindow)
{
    m_pConfig = new AppConfig;
    m_pConfig->Init("config.xml");
    m_pConfig->Load();

    ui->setupUi(this);
    m_pMgr = AccountMgr::Mgr();

    // open current DB
    QString curr_db = m_pConfig->GetCurrDb();
    if (!curr_db.isEmpty())
    {
        m_pMgr->Open(curr_db);
    }

    // init gui
    m_pPanelReleve = new PanelState(this);
    ui->m_pContainer->addWidget(m_pPanelReleve);
    m_pPanelSynthese = new PanelSynthese(this);
    ui->m_pContainer->addWidget(m_pPanelSynthese);
    m_pPanelItems = new PanelItems(this);
    ui->m_pContainer->addWidget(m_pPanelItems);

    // states panel by default
    m_pPanelReleve->UpdateData();
    ui->m_pContainer->setCurrentIndex(0);

    // hide menubar, use only toolbar that must not be hidden
    ui->menuBar->hide();
    ui->mainToolBar->toggleViewAction()->setVisible(false);

    setLocale(QLocale::system());

    AppSetting::RestoreState(*this, "main");
}

MainWindow::~MainWindow()
{
    delete m_pPanelReleve;
    AccountMgr::Release();
    delete ui;
}

void MainWindow::on_actionIMPORT_triggered()
{
    // select a CSV file
    // todo : remember import dir
    /*
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to import",
                            "",
                            "CSV files (*.csv *.txt)");
    bool res;
    foreach(QString path, files)
    {
        res = m_pMgr->Import(path);
        if(!res)
        {
            QMessageBox::warning(this,tr("Import"),tr("Cannot import
    %1").arg(path)); break;
        }
    }
    */
}

/// Select a database.
/// \todo init the list of available database
void MainWindow::on_actionOPEN_triggered()
{
    // delete m_pDatabase;
    // m_pDatabase = m_pMgr->Create("FILE:./bdd");

    // display : should update only the current view :
    // \todo use signal or model/view ?
    // m_pPanelReleve->SetData(m_pDatabase);
}

void MainWindow::on_actionDisplayReleve_triggered()
{
    m_pPanelReleve->UpdateData();
    ui->m_pContainer->setCurrentIndex(0);
}

void MainWindow::on_actionDisplaySynthese_triggered()
{
    m_pPanelSynthese->UpdateData();
    ui->m_pContainer->setCurrentIndex(1);
}

void MainWindow::on_actionUPDATE_ITEM_triggered()
{
    /*
    if (ui->m_pContainer->currentIndex()==0)
    {
        m_pPanelReleve->UpdateOperationItem();
    }
    */
}

void MainWindow::on_actionEXPORT_triggered()
{
    if (ui->m_pContainer->currentIndex() == 1)
    {
        m_pPanelSynthese->ExportSynthesis();
    }
}

void MainWindow::on_actionDisplayItems_triggered()
{
    m_pPanelItems->UpdateData();
    ui->m_pContainer->setCurrentIndex(2);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    AppSetting::SaveState(*this, "main");
    QWidget::closeEvent(event);
}

void MainWindow::on_actionHELP_triggered()
{
    QString cmd = "assistant";
    QStringList arguments;
    arguments << "-collectionFile"
              << "help/easybudget.qhc";
    m_Assistant.start(cmd, arguments);
}
