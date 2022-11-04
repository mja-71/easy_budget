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

#ifndef PANELSTATE_H
#define PANELSTATE_H

#include <QWidget>
#include <QModelIndex>
#include <QStringList>
#include <QList>
#include <QVector>
#include "comboboxdelegate.h"
#include "editopdelegate.h"
#include "delegatemergeitem.h"

namespace Ui {
class PanelReleve;
}

class DataAccount;
class QListWidgetItem;
class QTableWidgetItem;
class QSortFilterProxyModel;
class BudgetItem;
class Operation;
class ModelOpList;
class ModelMergeList;
class DlgNewOp;
class Statement;
class Period;

/**
 * @brief The PanelState class displays Statement in table.
 * Statements are listed by year
 */
class PanelState : public QWidget
{
	Q_OBJECT
	
public:
	explicit PanelState(QWidget *parent = 0);
	~PanelState();

	/// Refresh data
	void UpdateData();

	/// Update items
	void UpdateOperationItem(bool clean, bool do_assign, bool all_stats=false);

protected:
	void InitStates();
	void UpdatePos(bool with_compute=false);
	void ResizeTable();
	void Apply();

//--- overriden
	/// panel hidden : apply modifications
	virtual void hideEvent(QHideEvent *event);
	/// panel shown : update datas
	virtual void showEvent(QShowEvent *event);

private slots:

    void on_m_pCbxYears_currentTextChanged(const QString &year);
	void on_m_pLstReleves_currentRowChanged(int row);
	void on_pbtnNew_clicked();
	void on_btnImport_clicked();
	void on_btnDelete_clicked();
	void on_btnNewState_clicked();	
	void OnNewOp(Operation* op);
	void on_pbtnMerge_toggled(bool checked);
	void OnMultiEdit(const QModelIndex & index);
	void on_pbtnUpdateItems_clicked();
	void on_pbtnClose_clicked();
	void OnDataChanged(QModelIndex first,QModelIndex last, QVector<int> v);

private:
	Ui::PanelReleve *ui;
	ModelOpList* m_pModelOpList;
	QSortFilterProxyModel* m_SortModel;
	ModelMergeList* m_pModelMerge;
	Period* m_CurrYear;
	Statement* m_CurrStat;
	QList<Statement*> m_ListStat;
	ComboBoxDelegate m_ItemDelegate;
	EditOpDelegate m_LineDelegate;
	DelegateMergeItem m_MergeDelegate;
	DlgNewOp* m_pDlgNew;
};

#endif // PANELSTATE_H
