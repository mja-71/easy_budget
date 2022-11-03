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

#ifndef PANELSYNTHESE_H
#define PANELSYNTHESE_H

#include <QWidget>
#include <QVector>
class Period;

namespace Ui
{
class PanelSynthese;
}

class BudgetItem;
class QTableWidgetItem;

/**
 * @brief The PanelSynthese class displays operation synthesis.
 * Sums is displayed by poste
 * m_pTableInput : display positive operations
 * m_pTableOutput : display negative operations
 */
class PanelSynthese : public QWidget
{
    Q_OBJECT

public:
    explicit PanelSynthese(QWidget* parent = 0);
    ~PanelSynthese();
    void UpdateData();
    void ExportSynthesis();

protected:
    void InitColumns();
    void InitRows(const QList<BudgetItem*>& list_items);
    void FillTables();
    QTableWidgetItem* CreateWgtItem(const QString& str, int type = 0);

private slots:
    void on_m_pListYears_itemSelectionChanged();

    void on_btnExport_clicked();

    void on_btnUpdateItem_clicked();

private:
    Ui::PanelSynthese* ui;
    Period* m_CurrYear;
};

#endif // PANELSYNTHESE_H
