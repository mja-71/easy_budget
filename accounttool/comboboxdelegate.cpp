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

#include "comboboxdelegate.h"


#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>

#include <iostream>

ComboBoxDelegate::ComboBoxDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{
}

void ComboBoxDelegate::SetList(const QStringList& list)
{
    m_ListItemLabels = list;
}

QWidget* ComboBoxDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem& /* option */,
    const QModelIndex& /* index */) const
{
    QComboBox* editor = new QComboBox(parent);
    editor->insertItems(0, m_ListItemLabels);
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget* editor,
                                     const QModelIndex& index) const
{
    QComboBox* pCmbx = static_cast<QComboBox*>(editor);
    QString name = index.model()->data(index, Qt::EditRole).toString();
    int idx = name.isEmpty() ? 0 : pCmbx->findText(name); // 0 = other
    if (idx != -1)
    {
        pCmbx->setCurrentIndex(idx);
    }
    else
    {
        pCmbx->insertItem(1, name); // 0 = other, use next index
        pCmbx->setCurrentIndex(1);
    }
}

void ComboBoxDelegate::setModelData(QWidget* editor,
                                    QAbstractItemModel* model,
                                    const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    if (comboBox->currentIndex() == 0)
    {
        model->setData(index, "", Qt::EditRole);
    }
    else
    {
        model->setData(index, comboBox->currentText(), Qt::EditRole);
    }
}

void ComboBoxDelegate::updateEditorGeometry(
    QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}

/*
void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem
&option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 myOption = option;
    QString text = "hello"; // Items[index.row()].c_str();

    myOption.text = text;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption,
painter);
}
*/
