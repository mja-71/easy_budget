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

#include "panelreleve.h"
#include "ui_panelreleve.h"
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QLocale>
#include <QCheckBox>
#include <QSortFilterProxyModel>
#include "modeloplist.h"
#include "modelmergelist.h"
#include "dlgnewop.h"
#include "dlgnewstate.h"
#include "dlgupdateitemoptions.h"
#include "accounttooldata/dataaccount.h"
#include "accounttooldata/accountmgr.h"
#include "accounttooldata/accountsynthesis.h"
#include "appsetting.h"

PanelState::PanelState(QWidget* parent)
: QWidget(parent),
  ui(new Ui::PanelReleve),
  m_pModelOpList(0),
  m_SortModel(0),
  m_pModelMerge(new ModelMergeList(this)),
  m_CurrYear(0),
  m_CurrStat(0),
  m_ItemDelegate(this),
  m_LineDelegate(this),
  m_MergeDelegate(this),
  m_pDlgNew(0)
{
    ui->setupUi(this);
    setLocale(QLocale::system());
    m_pModelOpList = new ModelOpList(this);
    m_SortModel = new QSortFilterProxyModel(this);
    m_SortModel->setSourceModel(m_pModelOpList);
    // Init tree view
    ui->splitter->setSizes(QList<int>() << 100 << 1000);
    ui->m_pListOp->setModel(m_SortModel);
    ui->m_pListOp->setEditTriggers(QAbstractItemView::CurrentChanged);
    ui->m_pListOp->setAlternatingRowColors(true);

    // restore size
    AppSetting::RestoreState(*(ui->splitter), "states_splitter");
    // AppSetting::RestoreGeometry(*(ui->m_pListOp),"states_op_list");
    AppSetting::RestoreState(*(ui->m_pListOp->header()), "states_header");

    // dialog to add operation
    m_pDlgNew = new DlgNewOp(m_SortModel, this);
    connect(
        m_pDlgNew, SIGNAL(NewOp(Operation*)), this, SLOT(OnNewOp(Operation*)));
    connect(&m_LineDelegate,
            SIGNAL(modelDataEdited(const QModelIndex&)),
            this,
            SLOT(OnMultiEdit(const QModelIndex&)));
    connect(m_pModelOpList,
            SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
            this,
            SLOT(OnDataChanged(QModelIndex, QModelIndex, QVector<int>)));
    AppSetting::RestoreState(*(ui->m_pListOp->header()), "states_header");
}

PanelState::~PanelState()
{
    AppSetting::SaveState(*(ui->splitter), "states_splitter");
    AppSetting::SaveState(*(ui->m_pListOp->header()), "states_header");
    delete m_pDlgNew;
    delete ui;
}

/// Define database.
void PanelState::UpdateData()
{
    // keep previous selection if any
    // no need to update since all modification
    // should be done in this panel ...
    // --> except Items : use a dedicated button
    if (m_CurrYear == 0)
    {
        InitStates();
        // select last year
        int idx = ui->m_pCbxYears->count() - 1;
        if (idx >= 0)
        {
            ui->m_pCbxYears->setCurrentIndex(idx);
            on_m_pCbxYears_currentTextChanged(ui->m_pCbxYears->currentText());
        }
    }
}

/// Update items
void PanelState::UpdateOperationItem(bool do_clean,
                                     bool do_assign,
                                     bool all_stats)
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (pData == 0)
        return;

    m_LineDelegate.SetItemList(pData->Items(m_CurrYear));

    // get items to assign
    QList<BudgetItem*> list_items;
    if (do_assign)
    {
        list_items = pData->Items(m_CurrYear);
    }

    // get ops to update
    QStringList list_stats;
    if (all_stats)
    {
        foreach (Statement* s, m_ListStat)
        {
            list_stats.push_back(s->Id());
        }
    }
    else
    {
        QListWidgetItem* item = ui->m_pLstReleves->currentItem();
        if (item != 0)
        {
            list_stats.push_back(item->data(Qt::UserRole).toString());
        }
    }
    ui->m_pListOp->hide();
    foreach (QString str_stat, list_stats)
    {
        QList<Operation*>& list_op = pData->Operations(str_stat);
        AccountMgr::Mgr()->UpdateItems(list_op, list_items, do_clean);
    }
    ui->m_pListOp->show();
}


//------------------------- Protected -----------------------------------------

void PanelState::InitStates()
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (pData == 0)
        return;

    ui->m_pCbxYears->clear();
    ui->m_pLstReleves->clear();
    m_CurrYear = 0;
    // init years
    QList<Period*> list_years;
    Period* p;
    pData->Years(list_years);
    ui->m_pCbxYears->blockSignals(true);
    foreach (p, list_years)
    {
        QVariant val;
        // val.setValue<Period*>(p);
        val.setValue(p);
        ui->m_pCbxYears->addItem(p->Name(), val);
    }
    ui->m_pCbxYears->blockSignals(false);
}

void PanelState::UpdatePos(bool with_compute /*=false*/)
{
    QLocale def_loc;
    // update total credit and debit
    int row = ui->m_pLstReleves->currentIndex().row();
    Statement* s = m_ListStat.at(row);
    int idx_month = s->Date().month() - 1;
    AccountSynthesis* pSynthesis = AccountMgr::Mgr()->Analyser(m_CurrYear);
    if (with_compute)
    {
        pSynthesis->Update(idx_month);
    }
    float v =
        pSynthesis->GetTotalByMonth(idx_month, AccountSynthesis::CAT_CREDIT);
    ui->m_edtCredit->setText(def_loc.toString(v, 'f', 2));
    v = pSynthesis->GetTotalByMonth(idx_month, AccountSynthesis::CAT_DEBIT);
    ui->m_EdtDebit->setText(def_loc.toString(v, 'f', 2));

    // update position
    v = pSynthesis->GetCalPos(idx_month);
    ui->m_edtCurrPos->setText(def_loc.toString(v, 'f', 2));
    if (s->Position().Date().isValid())
    {
        v = pSynthesis->GetEndPosition(s->Position());
        ui->m_EdtPos->setText(def_loc.toString(v, 'f', 2));
        float diff = 0;
        if (pSynthesis->CheckPosition(idx_month, v, diff))
        {
            ui->m_edtStatus->setText(
                tr("Position error : %1").arg(def_loc.toString(diff, 'f', 2)));
        }
        else
        {
            ui->m_edtStatus->setText("");
        }
    }
    else
    {
        ui->m_EdtPos->setText(tr("undefined"));
        ui->m_edtStatus->setText("");
    }
}

/// Resize table column according the selected mode
void PanelState::ResizeTable()
{
    /*
        if (ui->pbtnMerge->isChecked())
        {
            ui->m_pListOp->resizeColumnToContents(ModelMergeList::E_MERGE);
            ui->m_pListOp->resizeColumnToContents(ModelMergeList::E_DATE);
            ui->m_pListOp->resizeColumnToContents(ModelMergeList::E_LABEL);
            ui->m_pListOp->resizeColumnToContents(ModelMergeList::E_VAL);
            ui->m_pListOp->resizeColumnToContents(ModelMergeList::E_ITEM);
        }
        else
        {
            ui->m_pListOp->resizeColumnToContents(ModelOpList::E_VALIDITY);
            ui->m_pListOp->resizeColumnToContents(ModelOpList::E_DATE);
            ui->m_pListOp->resizeColumnToContents(ModelOpList::E_LABEL);
            ui->m_pListOp->resizeColumnToContents(ModelOpList::E_VAL);
            ui->m_pListOp->resizeColumnToContents(ModelOpList::E_ITEM);
        }
    */
}

void PanelState::Apply()
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (ui->pbtnMerge->isChecked())
    {
        // apply merge
        m_pModelMerge->Apply();
    }
    else
    {
        // update selected state
        if (pData && m_CurrStat != 0)
        {
            pData->UpdateStat(m_CurrStat->Id());
        }
    }
}

/// panel hidden : apply modifications
void PanelState::hideEvent(QHideEvent* event)
{
    Apply();
}

/// panel shown : update datas
void PanelState::showEvent(QShowEvent* event)
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (pData)
    {
        m_LineDelegate.SetItemList(pData->Items(m_CurrYear));
    }
}

//------------------------- Protected Slots -----------------------------------

/// Called after year selecting : update releve list.
void PanelState::on_m_pCbxYears_currentTextChanged(const QString& year)
{
    QDate today = QDate::currentDate();
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (pData == 0)
        return;
    int index = ui->m_pCbxYears->findText(year);
    if (index != -1)
    {
        QVariant var = ui->m_pCbxYears->itemData(index);
        Period* p = var.value<Period*>();
        QListWidgetItem* pItem;
        m_ListStat.clear();
        Statement* s;
        ui->m_pLstReleves->clear();
        pData->BankStats(year, m_ListStat);
        foreach (s, m_ListStat)
        {
            pItem = new QListWidgetItem();
            pItem->setText(s->Label());
            pItem->setData(Qt::UserRole, s->Id());
            ui->m_pLstReleves->addItem(pItem);
        }
        m_CurrYear = p;
        m_pModelMerge->SetState(pData, year);

        // select a state
        if (m_CurrYear->Name() == today.toString("yyyy"))
        {
            ui->m_pLstReleves->setCurrentRow(today.month() - 1);
        }
        else
        {
            ui->m_pLstReleves->setCurrentRow(0);
        }
    }
    // update GUI
    bool is_close = (m_CurrYear == 0 || m_CurrYear->IsClose());
    ui->pbtnClose->setEnabled(!is_close);
    ui->btnDelete->setEnabled(!is_close);
    ui->pbtnNew->setEnabled(!is_close);
    ui->pbtnUpdateItems->setEnabled(!is_close);
    ui->pbtnMerge->setEnabled(!is_close);
}

/// Called after Statement selecting : fill table with operation list.
void PanelState::on_m_pLstReleves_currentRowChanged(int row)
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (row == -1)
    {
        m_CurrStat = 0;
    }
    else
    {
        Apply(); // save modification on previous selected statement
        m_CurrStat = m_ListStat.at(row);

        // update operations
        m_pModelOpList->SetState(pData, m_CurrYear, m_CurrStat);
        m_LineDelegate.SetItemList(pData->Items(m_CurrYear));
        ui->m_pListOp->setItemDelegate(&m_LineDelegate);

        ui->pbtnMerge->setChecked(false);
        m_pDlgNew->SetDate(m_CurrStat->Date());

        // update total credit and debit
        UpdatePos();
    }
}


void PanelState::on_pbtnNew_clicked()
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    m_pDlgNew->SetItems(pData->Items(m_CurrYear));
    m_pDlgNew->show();
}

void PanelState::on_btnImport_clicked()
{
    QString folder_import = AppSetting::ImportFolder();
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Select one or more files to import"),
        folder_import,
        tr("CSV files (*.csv *.txt);;OFX files (*.ofx)"));
    bool res;
    AccountMgr::ImportReport report;
    foreach (QString path, files)
    {
        res = AccountMgr::Mgr()->Import(path, report);
        if (!res)
        {
            QMessageBox::warning(
                this, tr("Import"), tr("Cannot import %1").arg(path));
            break;
        }
        else
        {
            QMessageBox::information(
                this,
                tr("Import"),
                tr("Read %1 operations (%2 to %3)\n%4 operations imported.")
                    .arg(report.nb_ops_read)
                    .arg(report.d1.toString(QLocale().dateFormat()))
                    .arg(report.d2.toString(QLocale().dateFormat()))
                    .arg(report.nb_ops_added));
            folder_import = QFileInfo(path).absoluteFilePath();
        }
    }
    // save user import folder
    AppSetting::SaveImportFolder(folder_import);
    // refresh : select imported state
    if (report.d2.isValid())
    {
        QString curr_year = report.d2.toString("yyyy");
        InitStates();
        if (!curr_year.isEmpty())
        {
            int idx = ui->m_pCbxYears->findText(curr_year);
            if (idx != -1)
            {
                ui->m_pCbxYears->setCurrentIndex(idx);
                on_m_pCbxYears_currentTextChanged(curr_year);
                ui->m_pLstReleves->setCurrentRow(report.d2.month() - 1);
            }
        }
        else
        {
            // select last year
            int idx = ui->m_pCbxYears->count() - 1;
            if (idx >= 0)
            {
                ui->m_pCbxYears->setCurrentIndex(idx);
            }
        }
        // update total credit and debit
        // update account position
        UpdatePos(true);
    }
}

void PanelState::on_btnDelete_clicked()
{
    QModelIndex idx = ui->m_pListOp->currentIndex();
    if (idx.row() != -1)
    {
        m_SortModel->removeRow(idx.row());
        UpdatePos(true);
    }
}

void PanelState::on_btnNewState_clicked()
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    DlgNewState dlg(m_CurrYear->Name(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QDate d = dlg.GetDate();
        pData->CreateBankStat(d.toString("yyyy"), d.month());
        InitStates();
        // select year
        int idx = ui->m_pCbxYears->findText(d.toString("yyyy"));
        if (idx != -1)
        {
            ui->m_pCbxYears->setCurrentIndex(idx);
        }
        // select month
        QList<QListWidgetItem*> list =
            ui->m_pLstReleves->findItems(d.toString("MMMM"), Qt::MatchExactly);
        if (!list.empty())
        {
            ui->m_pLstReleves->setCurrentItem(list.first());
        }
    }
}

void PanelState::OnNewOp(Operation* op)
{
    // change current state
    if (op->StatId() != m_CurrStat->Id())
    {
        ui->m_pLstReleves->setCurrentRow(op->Date().month() - 1);
    }
    // update tree view
    m_pModelOpList->AddOperation(op);
    // update account position
    UpdatePos(true);
}

void PanelState::on_pbtnMerge_toggled(bool checked)
{
    DataAccount* pData = AccountMgr::Mgr()->Db();

    // toggle table
    ui->m_pListOp->hide();
    // go to merge mode
    if (checked)
    {
        ui->m_pListOp->setDragEnabled(true);
        ui->m_pListOp->setAcceptDrops(true);
        ui->m_pListOp->setDropIndicatorShown(true);
        ui->m_pListOp->setSortingEnabled(false);
        m_pModelMerge->SetState(pData, m_CurrYear->Name());
        ui->m_pListOp->setModel(m_pModelMerge);
        ui->m_pListOp->setItemDelegate(&m_MergeDelegate);
        ui->m_pListOp->setItemDelegateForColumn(ModelOpList::E_ITEM, 0);
        ui->pbtnMerge->setText(tr("END MERGE"));
    }
    else
    {
        // apply merge
        m_pModelMerge->Apply();
        ui->m_pListOp->setDragEnabled(false);
        ui->m_pListOp->setAcceptDrops(false);
        ui->m_pListOp->setDropIndicatorShown(false);
        ui->m_pListOp->setSortingEnabled(true);
        ui->m_pListOp->setModel(m_SortModel);
        m_pModelOpList->SetState(pData, m_CurrYear, m_CurrStat);
        ui->m_pListOp->setItemDelegate(&m_LineDelegate);
        ui->pbtnMerge->setText(tr("MERGE"));
        UpdatePos(true);
    }
    // ResizeTable();
    ui->m_pListOp->show();
    // enable/disable button
    ui->btnDelete->setEnabled(!checked);
    ui->pbtnNew->setEnabled(!checked);
}

/**
 * @brief Call when cell is edited.
 * For validity (checkbox) column, update all selected rows.
 * @param index index of the edited cell
 */
void PanelState::OnMultiEdit(const QModelIndex& index)
{
    if (!ui->pbtnMerge->isChecked())
    {
        if (index.column() == ModelOpList::E_VALIDITY &&
            ui->m_pListOp->selectionModel()->isSelected(index))
        {
            QVariant v = m_pModelOpList->data(index, Qt::CheckStateRole);
            foreach (const QModelIndex& i,
                     ui->m_pListOp->selectionModel()->selectedIndexes())
            {
                m_pModelOpList->setData(i, v, Qt::CheckStateRole);
            }
        }
    }
}


void PanelState::on_pbtnUpdateItems_clicked()
{
    DlgUpdateItemOptions dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        UpdateOperationItem(dlg.DoClean(), dlg.DoAssign(), dlg.ApplyAllStats());
    }
}

void PanelState::on_pbtnClose_clicked()
{
    DataAccount* pData = AccountMgr::Mgr()->Db();
    if (m_CurrYear != 0)
    {
        pData->Close(m_CurrYear);
        QMessageBox::information(
            this,
            tr("Close"),
            tr("Period %1 is closed.").arg(m_CurrYear->Name()));
    }
}

void PanelState::OnDataChanged(QModelIndex first,
                               QModelIndex last,
                               QVector<int> v)
{
    UpdatePos(false);
}
