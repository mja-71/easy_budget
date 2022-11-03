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

#include "dlgnewstate.h"
#include "ui_dlgnewstate.h"

DlgNewState::DlgNewState(const QString& year, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DlgNewState)
{
	QDate d;
	ui->setupUi(this);
	if (year.isEmpty())
	{
		ui->m_pDtePeriod->setDate(QDate::currentDate());
	}
	else
	{
		d.setDate(year.toInt(),1,1);
		ui->m_pDtePeriod->setDate(d);
	}
}

DlgNewState::~DlgNewState()
{
	delete ui;
}

QDate DlgNewState::GetDate() const
{
	return ui->m_pDtePeriod->date();
}

void DlgNewState::on_btnOK_clicked()
{
	accept();
}

void DlgNewState::on_btnCancel_clicked()
{
	reject();
}
