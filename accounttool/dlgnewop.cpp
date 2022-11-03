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

#include "dlgnewop.h"
#include "ui_dlgnewop.h"
#include "accounttooldata/operation.h"
#include "accounttooldata/budgetitem.h"
#include "modeloplist.h"

int l_counter = 0;

DlgNewOp::DlgNewOp(QAbstractItemModel* pModel, QWidget* parent)
: QWidget(parent), ui(new Ui::DlgNewOp), m_pModel(pModel)
{
    ui->setupUi(this);
    ui->m_pEdtDate->setDate(QDate::currentDate());
    ui->m_EdtValue->setMaximum(
        1e15); // for euro or dollar, should be enough ...
    ui->m_EdtValue->setMinimum(-1e15);
    setWindowFlags(Qt::Tool);
}

DlgNewOp::~DlgNewOp()
{
    delete ui;
}

void DlgNewOp::SetItems(const QList<BudgetItem*>& list)
{
    int i = 0;
    ui->m_pCbxItem->clear();
    ui->m_pCbxItem->insertItem(i++, tr("other"));
    foreach (BudgetItem* bi, list)
    {
        QVariant val;
        if (!bi->IsDeprecated())
        {
            val.setValue(bi);
            ui->m_pCbxItem->insertItem(i++, bi->Name(), val);
        }
    }
}

void DlgNewOp::SetDate(const QDate& date)
{
    // user cannot change the current year
    ui->m_pEdtDate->setMinimumDate(QDate(date.year(), 1, 1));
    ui->m_pEdtDate->setMaximumDate(QDate(date.year(), 12, 31));
    ui->m_pEdtDate->setDate(date);
}

void DlgNewOp::on_btnAdd_clicked()
{
    Operation* op = new Operation();
    op->Init(QString("%1_%2")
                 .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
                 .arg(l_counter++),
             ui->m_pEdtDate->date(),
             ui->m_EdtValue->value(),
             ui->m_pEdtLabel->text());
    op->SetComment(ui->m_pEdtComment->text());
    if (ui->m_chkbxReport->isChecked())
    {
        op->SetTransfer(true);
    }
    else
    {
        QVariant var_bi = ui->m_pCbxItem->currentData();
        if (var_bi.canConvert<BudgetItem*>())
        {
            op->SetItem(var_bi.value<BudgetItem*>());
        }
    }
    emit NewOp(op);
}

void DlgNewOp::on_btnClose_clicked()
{
    hide();
}

void DlgNewOp::on_m_chkbxReport_stateChanged(int arg1)
{
    ui->m_pCbxItem->setEnabled(arg1 != Qt::Checked);
    ui->m_pEdtDate->setEnabled(arg1 != Qt::Checked);
    if (arg1 == Qt::Checked)
    {
        QDate d = ui->m_pEdtDate->date();
        d.setDate(d.year(), 1, 1);
        ui->m_pEdtDate->setDate(d);
        ui->m_pEdtLabel->setText(tr("TRANSFER"));
    }
}
