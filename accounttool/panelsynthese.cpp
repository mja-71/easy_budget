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

#include "panelsynthese.h"
#include "ui_panelsynthese.h"
#include <QDate>
#include <QStringList>
#include <QFileDialog>
#include <QLocale>
#include "accounttooldata/dataaccount.h"
#include "accounttooldata/accountsynthesis.h"
#include "accounttooldata/accountmgr.h"

PanelSynthese::PanelSynthese(QWidget* parent)
: QWidget(parent), ui(new Ui::PanelSynthese), m_CurrYear(0)
{
    ui->setupUi(this);
    ui->splitter_main->setSizes(QList<int>() << 100 << 1000);
    InitColumns();
}

PanelSynthese::~PanelSynthese()
{
    delete ui;
}

/// Define database.
void PanelSynthese::UpdateData()
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (pData != 0)
    {
        ui->m_pListYears->clear();
        // init years
        int curr_idx_year = -1, idx = 0;
        QList<Period*> list_years;
        Period* p;
        QString str;
        pData->Years(list_years);
        QListWidgetItem* item;
        QVariant data;
        foreach (p, list_years)
        {
            str = p->Name();
            data.setValue(p);
            item = new QListWidgetItem(str);
            item->setData(Qt::UserRole, data);
            ui->m_pListYears->addItem(item);
            if (p == m_CurrYear)
            {
                curr_idx_year = idx;
            }
            idx++;
        }
        // init rows with items

        // select current year
        if (curr_idx_year != -1)
        {
            ui->m_pListYears->setCurrentRow(curr_idx_year);
        }
        else if (idx > 0)
        {
            ui->m_pListYears->setCurrentRow(idx - 1);
        }
    }
    else // if (pData==0)
    {
        ui->m_pListYears->clear();
    }
}

void PanelSynthese::ExportSynthesis()
{
    int idx = ui->m_pListYears->currentRow();
    if (idx >= 0)
    {
        // QString year =  ui->m_pListYears->item(idx)->text();
        Period* p =
            ui->m_pListYears->item(idx)->data(Qt::UserRole).value<Period*>();
        // get destination file
        QString path = QFileDialog::getSaveFileName(
            this, tr("Export synthesis"), QString(), "CSV (*.csv)");
        // export
        if (!path.isEmpty())
        {
            AccountSynthesis* pSynthesis = AccountMgr::Mgr()->Analyser(p);
            pSynthesis->Export(path);
        }
    }
}

/// Init tables
void PanelSynthese::InitColumns()
{
    QStringList list_titles;
    ui->m_pTableInput->setColumnCount(14);
    ui->m_pTableOutput->setColumnCount(14);
    list_titles << "";
    QLocale def_loc;
    for (int i = 1; i <= 12; i++)
    {
        list_titles << def_loc.toString(QDate(2021, i, 1),
                                        "MMM"); // short month name
        ui->m_pTableInput->setColumnWidth(i, 80);
        ui->m_pTableOutput->setColumnWidth(i, 80);
    }
    list_titles << tr("Total");
    ui->m_pTableInput->setHorizontalHeaderLabels(list_titles);
    ui->m_pTableOutput->setHorizontalHeaderLabels(list_titles);
    // Total row
    // ui->m_pTableInput->setRowCount(1);
    // ui->m_pTableInput->setItem(0,0,CreateWgtItem(tr("Total")));
}

void PanelSynthese::InitRows(const QList<BudgetItem*>& list_items)
{
    int row_i = 0, row_o = 0;
    ui->m_pTableInput->setRowCount(0);
    ui->m_pTableOutput->setRowCount(0);
    foreach (const BudgetItem* pItem, list_items)
    {
        if (pItem->IsCredit())
        {
            ui->m_pTableInput->insertRow(row_i);
            ui->m_pTableInput->setItem(
                row_i++, 0, CreateWgtItem(pItem->Name(), 1));
        }
        else if (pItem->IsDebit())
        {
            ui->m_pTableOutput->insertRow(row_o);
            ui->m_pTableOutput->setItem(
                row_o++, 0, CreateWgtItem(pItem->Name(), 1));
        }
        else
        {
            ui->m_pTableInput->insertRow(row_i);
            ui->m_pTableInput->setItem(
                row_i++, 0, CreateWgtItem(pItem->Name(), 1));
            ui->m_pTableOutput->insertRow(row_o);
            ui->m_pTableOutput->setItem(
                row_o++, 0, CreateWgtItem(pItem->Name(), 1));
        }
    }
    ui->m_pTableInput->setRowCount(row_i + 2);
    ui->m_pTableOutput->setRowCount(row_o + 2);
    ui->m_pTableInput->setItem(row_i, 0, CreateWgtItem(tr("Other"), 1));
    ui->m_pTableOutput->setItem(row_o, 0, CreateWgtItem(tr("Other"), 1));
    ui->m_pTableInput->setItem(row_i + 1, 0, CreateWgtItem(tr("TOTAL"), 1));
    ui->m_pTableOutput->setItem(row_o + 1, 0, CreateWgtItem(tr("TOTAL"), 1));
}

void PanelSynthese::on_m_pListYears_itemSelectionChanged()
{
    FillTables();
}

void PanelSynthese::FillTables()
{
    int idx = ui->m_pListYears->currentRow(), i;
    QLocale def_locale;
    // update totals
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (idx >= 0 && pData != 0)
    {
        m_CurrYear = ui->m_pListYears->currentItem()
                         ->data(Qt::UserRole)
                         .value<Period*>();
        AccountSynthesis* pSynthesis = AccountMgr::Mgr()->Analyser(m_CurrYear);
        int row_i = 0, row_o = 0;
        QList<BudgetItem*> list_items = pData->Items(m_CurrYear);
        QString str_val;
        // update by item
        foreach (const BudgetItem* pItem, list_items)
        {
            if (pItem->IsCredit() || pItem->IsUndefined())
            {
                for (i = 1; i <= 12; i++)
                {
                    // str_val.setNum(pSynthesis->GetTotal(i-1,pItem->Name()),'f',2);
                    str_val = def_locale.toString(
                        pSynthesis->GetItemMonthTotal(
                            i - 1, pItem->Id(), AccountSynthesis::CAT_CREDIT),
                        'f',
                        2);
                    ui->m_pTableInput->setItem(
                        row_i, i, CreateWgtItem(str_val));
                }
                str_val = def_locale.toString(
                    pSynthesis->GetItemTotal(pItem->Id(),
                                             AccountSynthesis::CAT_CREDIT),
                    'f',
                    2);
                ui->m_pTableInput->setItem(
                    row_i, 13, CreateWgtItem(str_val, 2));
                row_i++;
            }
            if (pItem->IsDebit() || pItem->IsUndefined())
            {
                for (i = 1; i <= 12; i++)
                {
                    str_val = def_locale.toString(
                        pSynthesis->GetItemMonthTotal(
                            i - 1, pItem->Id(), AccountSynthesis::CAT_DEBIT),
                        'f',
                        2);
                    ui->m_pTableOutput->setItem(
                        row_o, i, CreateWgtItem(str_val));
                }
                str_val = def_locale.toString(
                    pSynthesis->GetItemTotal(pItem->Id(),
                                             AccountSynthesis::CAT_DEBIT),
                    'f',
                    2);
                ui->m_pTableOutput->setItem(
                    row_o, 13, CreateWgtItem(str_val, 2));
                row_o++;
            }
        }
        // update others
        for (i = 1; i <= 12; i++)
        {
            str_val =
                def_locale.toString(pSynthesis->GetOtherMonthTotal(
                                        i - 1, AccountSynthesis::CAT_CREDIT),
                                    'f',
                                    2);
            ui->m_pTableInput->setItem(row_i, i, CreateWgtItem(str_val));
            str_val =
                def_locale.toString(pSynthesis->GetOtherMonthTotal(
                                        i - 1, AccountSynthesis::CAT_DEBIT),
                                    'f',
                                    2);
            ui->m_pTableOutput->setItem(row_o, i, CreateWgtItem(str_val));
        }
        str_val = def_locale.toString(
            pSynthesis->GetOtherTotal(AccountSynthesis::CAT_CREDIT), 'f', 2);
        ui->m_pTableInput->setItem(row_i, 13, CreateWgtItem(str_val, 2));
        str_val = def_locale.toString(
            pSynthesis->GetOtherTotal(AccountSynthesis::CAT_DEBIT), 'f', 2);
        ui->m_pTableOutput->setItem(row_o, 13, CreateWgtItem(str_val, 2));
        // Update total by month
        for (i = 1; i <= 12; i++)
        {
            str_val =
                def_locale.toString(pSynthesis->GetTotalByMonth(
                                        i - 1, AccountSynthesis::CAT_CREDIT),
                                    'f',
                                    2);
            ui->m_pTableInput->setItem(row_i + 1, i, CreateWgtItem(str_val, 2));
            str_val = def_locale.toString(
                pSynthesis->GetTotalByMonth(i - 1, AccountSynthesis::CAT_DEBIT),
                'f',
                2);
            ui->m_pTableOutput->setItem(
                row_o + 1, i, CreateWgtItem(str_val, 2));
        }
        str_val = def_locale.toString(
            pSynthesis->GetTotal(AccountSynthesis::CAT_CREDIT), 'f', 2);
        ui->m_pTableInput->setItem(row_i + 1, 13, CreateWgtItem(str_val, 2));
        str_val = def_locale.toString(
            pSynthesis->GetTotal(AccountSynthesis::CAT_DEBIT), 'f', 2);
        ui->m_pTableOutput->setItem(row_o + 1, 13, CreateWgtItem(str_val, 2));
    }
    else
    {
        m_CurrYear = 0;
    }
}

QTableWidgetItem* PanelSynthese::CreateWgtItem(const QString& str,
                                               int type /*=0*/)
{
    QTableWidgetItem* pItem = new QTableWidgetItem(str);
    pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    if (type == 0 || type == 2)
    {
        pItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    if (type == 1)
    {
        pItem->setBackground(QColor(100, 100, 100));
        pItem->setForeground(QBrush(QColor(255, 255, 255)));
    }
    else if (type == 2)
    {
        pItem->setBackground(QColor(200, 200, 200));
        pItem->setForeground(QBrush(QColor(0, 0, 0)));
    }
    return pItem;
}

void PanelSynthese::on_btnExport_clicked()
{
    ExportSynthesis();
}

void PanelSynthese::on_btnUpdateItem_clicked()
{
    int idx = ui->m_pListYears->currentRow();
    if (idx != -1)
    {
        // update data
        AccountSynthesis* pSynthesis = AccountMgr::Mgr()->Analyser(m_CurrYear);
        AccountMgr* pMgr = AccountMgr::Mgr();
        DataAccount* pData = AccountMgr::Mgr()->Db();
        QList<BudgetItem*> list_items = pData->Items(m_CurrYear);
        QList<Statement*> list_stat;
        pData->BankStats(m_CurrYear->Name(), list_stat);
        foreach (Statement* s, list_stat)
        {
            QList<Operation*>& list_op = pData->Operations(s->Id());
            pMgr->UpdateItems(list_op, list_items, false);
        }
        pSynthesis->Update();
        // update tables
        InitRows(list_items);
        FillTables();
    }
}
