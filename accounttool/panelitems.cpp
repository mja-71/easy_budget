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

#include "panelitems.h"
#include "ui_panelitems.h"
#include "modelitems.h"
#include "delegatefilteritem.h"
#include "accounttooldata/accountmgr.h"
#include "accounttooldata/dataaccount.h"
#include "accounttooldata/budgetitem.h"

#include <QModelIndex>

PanelItems::PanelItems(QWidget* parent)
: QWidget(parent),
  ui(new Ui::PanelItems),
  m_pCurrItem(0),
  m_pModelItems(new ModelItems(this))
{
    ui->setupUi(this);
    ui->splitter->setSizes(QList<int>() << 100 << 1000);
    connect(ui->m_pListFilters,
            SIGNAL(itemChanged(QListWidgetItem*)),
            this,
            SLOT(OnFilterChanged(QListWidgetItem*)));

    ui->m_pEdtTitle->setEnabled(false);
    ui->m_pListItems->setModel(m_pModelItems);
    connect(ui->m_pListItems->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            this,
            SLOT(OnCurrentChanged(QModelIndex, QModelIndex)));

    ui->m_pListFilters->setItemDelegate(new DelegateFilterItem(this));
}

PanelItems::~PanelItems()
{
    delete ui;
}

/// Define database.
void PanelItems::UpdateData()
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    m_pModelItems->SetDb(pData);

    QModelIndex index = m_pModelItems->index(0, 0);
    ui->m_pListItems->setCurrentIndex(index);
}

/// calls when an item is selected.
void PanelItems::OnCurrentChanged(const QModelIndex& new_idx,
                                  const QModelIndex& old_idx)
{
    // save
    Apply(old_idx);

    QVariant val = m_pModelItems->data(new_idx, Qt::UserRole);
    if (val.canConvert<BudgetItem*>())
    {
        m_pCurrItem = val.value<BudgetItem*>();
        ui->m_pEdtTitle->setEnabled(true);
    }
    else
    {
        m_pCurrItem = 0;
        ui->m_pEdtTitle->setEnabled(false);
    }
    Update();
}

void PanelItems::Update()
{
    if (m_pCurrItem)
    {
        ui->m_pEdtTitle->setText(m_pCurrItem->Name());
        if (m_pCurrItem->IsCredit())
        {
            ui->rdbCredit->setChecked(true);
            ui->rdbDebit->setChecked(false);
            ui->rdbNone->setChecked(false);
        }
        else if (m_pCurrItem->IsDebit())
        {
            ui->rdbCredit->setChecked(false);
            ui->rdbDebit->setChecked(true);
            ui->rdbNone->setChecked(false);
        }
        else
        {
            ui->rdbCredit->setChecked(false);
            ui->rdbDebit->setChecked(false);
            ui->rdbNone->setChecked(true);
        }
        ui->m_pListFilters->clear();
        ui->m_pListFilters->insertItems(0, m_pCurrItem->Ops());
        int row;
        for (row = 0; row < m_pCurrItem->Ops().size(); row++)
        {
            ui->m_pListFilters->item(row)->setFlags(Qt::ItemIsEnabled |
                                                    Qt::ItemIsEditable);
        }
        // last row is the empty row
        InsertEmptyItem(row);
        ui->m_pListFilters->item(row)->setFlags(Qt::ItemIsEnabled |
                                                Qt::ItemIsEditable);
    }
    else
    {
        // clear fields
        ui->m_pEdtTitle->setText("");
        ui->rdbCredit->setChecked(true);
        ui->rdbDebit->setChecked(false);
        ui->rdbNone->setChecked(false);
        ui->m_pListFilters->clear();
    }
}

/// Call after filter has changed in m_pListFilters.
/// Force empty row at list end otherwise delete it...
void PanelItems::OnFilterChanged(QListWidgetItem* item)
{
    int row = ui->m_pListFilters->currentIndex().row();
    if (row == (ui->m_pListFilters->count() - 1))
    {
        if (!item->text().isEmpty())
        {
            InsertEmptyItem(row + 1);
        }
        else
        {
            // item->setText(tr("enter new filter..."));
        }
    }
    else
    {
        if (item->text().isEmpty())
        {
            ui->m_pListFilters->model()->removeRow(row);
        }
    }
}

void PanelItems::Apply(const QModelIndex& index)
{
    if (index.isValid())
    {
        // apply name
        m_pModelItems->setData(index,
                               QVariant(ui->m_pEdtTitle->text()),
                               Qt::UserRole + ModelItems::COL_NAME);

        // apply type
        if (ui->rdbCredit->isChecked())
        {
            m_pModelItems->setData(index,
                                   QVariant(BudgetItem::OP_CREDIT),
                                   Qt::UserRole + ModelItems::COL_TYPE);
        }
        else if (ui->rdbDebit->isChecked())
        {
            m_pModelItems->setData(index,
                                   QVariant(BudgetItem::OP_DEBIT),
                                   Qt::UserRole + ModelItems::COL_TYPE);
        }
        else
        {
            m_pModelItems->setData(index,
                                   QVariant(BudgetItem::OP_NONE),
                                   Qt::UserRole + ModelItems::COL_TYPE);
        }
        // apply list of operations
        QStringList list_ops;
        for (int i = 0; i < ui->m_pListFilters->count() - 1; i++)
        {
            list_ops.append(ui->m_pListFilters->item(i)->text());
        }
        m_pModelItems->setData(
            index, QVariant(list_ops), Qt::UserRole + ModelItems::COL_FILTER);
    }
}

/// panel hidden : apply modifications
void PanelItems::hideEvent(QHideEvent* event)
{
    // apply current change and save items
    Apply(ui->m_pListItems->currentIndex());
    DataAccount* db = AccountMgr::Mgr()->Db();
    if (db != 0)
    {
        db->UpdateItems();
    }
}

void PanelItems::on_btnNew_clicked()
{
    int row = m_pModelItems->rowCount();
    m_pModelItems->insertRows(row, 1);
    // update item list
    ui->m_pListItems->setModel(m_pModelItems);
    ui->m_pListItems->show();
    // select item
    QModelIndex idx = m_pModelItems->index(row, 0);
    ui->m_pListItems->setCurrentIndex(idx);
}


void PanelItems::on_btnDelete_clicked()
{
    QModelIndex index = ui->m_pListItems->currentIndex();
    if (index.isValid())
    {
        m_pModelItems->removeRows(index.row(), 1);
        // select item
        if (m_pModelItems->rowCount() != 0)
        {
            QModelIndex idx = m_pModelItems->index(0, 0);
            ui->m_pListItems->setCurrentIndex(idx);
        }
        else
        {
            m_pCurrItem = 0;
            Update();
        }
    }
}

void PanelItems::InsertEmptyItem(int row)
{
    QListWidgetItem* item =
        new QListWidgetItem("enter pattern", ui->m_pListFilters);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->m_pListFilters->insertItem(row, item);
}

/// Item label is changed.
/// must update data
void PanelItems::on_m_pEdtTitle_editingFinished()
{
    // use model to update data and view ...
    QModelIndex index = ui->m_pListItems->currentIndex();
    if (index.isValid())
    {
        m_pModelItems->setData(index,
                               QVariant(ui->m_pEdtTitle->text()),
                               Qt::UserRole + ModelItems::COL_NAME);
    }
}
