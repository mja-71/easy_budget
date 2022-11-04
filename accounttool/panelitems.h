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

#ifndef PANELITEMS_H
#define PANELITEMS_H

#include <QWidget>
#include <QModelIndex>

namespace Ui
{
class PanelItems;
}

class DataAccount;
class BudgetItem;
class ModelItems;
class QListWidgetItem;

class PanelItems : public QWidget
{
    Q_OBJECT

public:
    explicit PanelItems(QWidget* parent = 0);
    ~PanelItems();
    void UpdateData();

protected:
    void Update();
    void Apply(const QModelIndex& index);
    void InsertEmptyItem(int row);

    //--- overriden
    /// panel hidden : apply modifications
    virtual void hideEvent(QHideEvent* event);

private slots:

    void OnCurrentChanged(const QModelIndex& new_idx,
                          const QModelIndex& old_idx);

    /// Call on filter modification.
    void OnFilterChanged(QListWidgetItem* item);

    void on_btnNew_clicked();

    void on_btnDelete_clicked();

    void on_m_pEdtTitle_editingFinished();

private:
    Ui::PanelItems* ui;
    BudgetItem* m_pCurrItem;
    ModelItems* m_pModelItems;
};

#endif // PANELITEMS_H
