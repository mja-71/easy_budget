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

#include "delegatemergeitem.h"


#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>
#include <QPainter>
#include <iostream>

#include "modelmergelist.h"
#include "accounttooldata/accountmerger.h"

DelegateMergeItem::DelegateMergeItem(QObject* parent)
: QStyledItemDelegate(parent)
{
}

QWidget* DelegateMergeItem::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem& /* option */,
    const QModelIndex& /* index */) const
{
    /*
        QComboBox* editor = new QComboBox(parent);
        editor->insertItems(0,m_ListItemLabels);
        return editor;
    */
    // return QStyledItemDelegate::
    return 0;
}

void DelegateMergeItem::setEditorData(QWidget* editor,
                                      const QModelIndex& index) const
{
    /*
        QComboBox *pCmbx = static_cast<QComboBox*>(editor);
        QString name = index.model()->data(index, Qt::EditRole).toString();
        int idx = pCmbx->findText(name);
        if (idx!=-1)
        {
            pCmbx->setCurrentIndex(idx);
        }
        else
        {
            pCmbx->setCurrentIndex(0);
        }
    */
}

void DelegateMergeItem::setModelData(QWidget* editor,
                                     QAbstractItemModel* model,
                                     const QModelIndex& index) const
{
    /*
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        if (comboBox->currentIndex()==0)
        {
            model->setData(index, "", Qt::EditRole);
        }
        else
        {
            model->setData(index, comboBox->currentText(), Qt::EditRole);
        }
    */
}

void DelegateMergeItem::updateEditorGeometry(
    QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}


void DelegateMergeItem::paint(QPainter* painter,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const
{
    QStyleOptionViewItem myOption = option;
    // initStyleOption(&myOption, index);
    // get operation
    QVariant data = index.model()->data(index, Qt::UserRole);
    if (data.canConvert<OperationMerged>())
    {
        OperationMerged op_m = data.value<OperationMerged>();
        Operation* op = (index.row() % 2) == 0 ? op_m.Op2() : op_m.Op1();

        if (!op_m.Merge())
        {
            if (op->IsUser())
            {
                myOption.palette.setBrush(QPalette::Text,
                                          QBrush(QColor(0, 0, 255)));
            }
            else
            {
                myOption.palette.setBrush(QPalette::Text,
                                          QBrush(QColor(128, 69, 0)));
            }
        }
        else
        {
            if (op->IsUser())
            {
                myOption.palette.setBrush(QPalette::Text,
                                          QBrush(QColor(50, 50, 50)));
                // myOption.font.setItalic(true);
                myOption.font.setStrikeOut(true);
            }
        }
    }
    QStyledItemDelegate::paint(painter, myOption, index);
    if ((index.row() % 2) == 1)
    {
        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
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
