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

#ifndef DLGNEWOP_H
#define DLGNEWOP_H

#include <QWidget>
#include <QDate>

namespace Ui {
class DlgNewOp;
}
class Operation;
class BudgetItem;
class QAbstractItemModel;

class DlgNewOp : public QWidget
{
	Q_OBJECT
	
public:
	explicit DlgNewOp(QAbstractItemModel* pModel, QWidget *parent = 0);
	~DlgNewOp();
	void SetItems(const QList<BudgetItem*>& list);
	/// define the month and year of the state.
	void SetDate(const QDate& date);

signals:
	void NewOp(Operation* op);

private slots:
	void on_btnAdd_clicked();
	void on_btnClose_clicked();
	void on_m_chkbxReport_stateChanged(int arg1);

private:
	Ui::DlgNewOp *ui;
	QAbstractItemModel* m_pModel;
};

#endif // DLGNEWOP_H
