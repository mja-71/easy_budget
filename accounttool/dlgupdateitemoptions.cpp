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

#include "dlgupdateitemoptions.h"
#include "ui_dlgupdateitemoptions.h"

DlgUpdateItemOptions::DlgUpdateItemOptions(QWidget* parent)
: QDialog(parent), ui(new Ui::DlgUpdateItemOptions)
{
    ui->setupUi(this);
}

DlgUpdateItemOptions::~DlgUpdateItemOptions()
{
    delete ui;
}

DlgUpdateItemOptions::eOpt DlgUpdateItemOptions::Option() const
{
    if (ui->rdbNotYetAssign->isChecked())
    {
        return OPT_NOT_ASSIGN;
    }
    else if (ui->rdbAllOps->isChecked())
    {
        return OPT_ALL;
    }
    else if (ui->rdbClean->isChecked())
    {
        return OPT_CLEAN;
    }
    return OPT_NOT_ASSIGN;
}

bool DlgUpdateItemOptions::ApplyAllStats() const
{
    return ui->ckbAllStats->isChecked();
}

void DlgUpdateItemOptions::on_btnOk_clicked()
{
    accept();
}

void DlgUpdateItemOptions::on_btnCancel_clicked()
{
    reject();
}
