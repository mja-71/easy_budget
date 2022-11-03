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

#include "delegatefilteritem.h"
#include <QLineEdit>
#include <QApplication>
#include <QString>
#include <QPainter>

DelegateFilterItem::DelegateFilterItem(QObject* parent)
: QStyledItemDelegate(parent)
{
}

QWidget* DelegateFilterItem::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem& /* option */,
    const QModelIndex& /* index */) const
{
    QLineEdit* edit = new QLineEdit(parent);
    edit->setPlaceholderText(tr("enter pattern"));
    // return QStyledItemDelegate::
    return edit;
}

void DelegateFilterItem::setEditorData(QWidget* editor,
                                       const QModelIndex& index) const
{
    QLineEdit* edit = static_cast<QLineEdit*>(editor);
    QString name = index.model()->data(index, Qt::EditRole).toString();
    if (name != tr("enter pattern"))
    {
        QString name = index.model()->data(index, Qt::EditRole).toString();
        edit->setText(name);
    }
}

void DelegateFilterItem::setModelData(QWidget* editor,
                                      QAbstractItemModel* model,
                                      const QModelIndex& index) const
{
    QLineEdit* edit = static_cast<QLineEdit*>(editor);
    QString label = edit->text();
    if (!label.isEmpty())
    {
        model->setData(index, label, Qt::EditRole);
    }
    else if (index.row() != (model->rowCount() - 1))
    {
        model->setData(index, "", Qt::EditRole);
    }
}

void DelegateFilterItem::updateEditorGeometry(
    QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}


void DelegateFilterItem::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    QStyleOptionViewItem myOption = option;

    initStyleOption(&myOption, index);
    // get label
    QString label = index.model()->data(index).toString();
    if (label == tr("enter pattern"))
    {
        myOption.font.setItalic(true);
        myOption.palette.setBrush(QPalette::Text,
                                  QBrush(QColor(150, 150, 150)));
        // myOption.text = tr("validate");
        // QApplication::style()->drawControl(QStyle::CE_ItemViewItem,
        // &myOption, painter);
        QStyledItemDelegate::paint(painter, myOption, index);
    }
    else
    {
        QStyledItemDelegate::paint(painter, myOption, index);
    }

    /*
        QStyleOptionButton opt;
        opt.palette = QPalette(Qt::red);
        opt.state = QStyle::State_Active | QStyle::State_Enabled;
        opt.rect = option.rect;
        opt.text = tr("validate");
        QApplication::style()->drawControl(QStyle::CE_PushButton, &opt,
       painter);
    */
}
