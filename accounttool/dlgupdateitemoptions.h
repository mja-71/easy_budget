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

#ifndef DLGUPDATEITEMOPTIONS_H
#define DLGUPDATEITEMOPTIONS_H

#include <QDialog>

namespace Ui {
class DlgUpdateItemOptions;
}

class DlgUpdateItemOptions : public QDialog
{
	Q_OBJECT

public:
	enum eOpt
	{
		OPT_NOT_ASSIGN,
		OPT_ALL,
		OPT_CLEAN
	} ;

	explicit DlgUpdateItemOptions(QWidget *parent = 0);
	~DlgUpdateItemOptions();

	eOpt Option() const;
	bool DoClean() const
	{
		eOpt opt = Option();
		return ((opt == OPT_CLEAN) || (opt == OPT_ALL));
	}
	bool DoAssign() const
	{
		eOpt opt = Option();
		return ((opt == OPT_ALL) || (opt == OPT_NOT_ASSIGN));
	}
	bool ApplyAllStats() const;

private slots:
	void on_btnOk_clicked();

	void on_btnCancel_clicked();

private:
	Ui::DlgUpdateItemOptions *ui;
};

#endif // DLGUPDATEITEMOPTIONS_H
