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

#include "editopdelegate.h"
#include <QApplication>
#include <QCheckBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include "accounttooldata/operation.h"
#include "modeloplist.h"


EditOpDelegate::EditOpDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

void EditOpDelegate::SetItemList(const QList<BudgetItem*>& list)
{
    m_ListItems = list;
}

QWidget* EditOpDelegate::createEditor(QWidget* parent,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const
{
    if (index.isValid())
    {
        if (index.column() == ModelOpList::E_DATE)
        {
            QDateEdit* editor = new QDateEdit(parent);
            return editor;
        }
        else if (index.column() == ModelOpList::E_VAL)
        {
            QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
            editor->setMaximum(
                1e15); // for euro or dollar, should be enough ...
            editor->setMinimum(-1e15);
            return editor;
        }
        else if (index.column() == ModelOpList::E_ITEM)
        {
            QVariant val;
            QComboBox* editor = new QComboBox(parent);
            int i = 0;
            editor->insertItem(i++, tr("other"), val);
            foreach (BudgetItem* bi, m_ListItems)
            {
                if (!bi->IsDeprecated())
                {
                    val.setValue(bi);
                    editor->insertItem(i++, bi->Name(), val);
                }
            }
            return editor;
        }
        else
        {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
    return 0;
}

void EditOpDelegate::setEditorData(QWidget* editor,
                                   const QModelIndex& index) const
{
    if (index.isValid())
    {
        if (index.column() == ModelOpList::E_DATE)
        {
            QDateEdit* edt_date = static_cast<QDateEdit*>(editor);
            QDate d = index.model()->data(index, Qt::EditRole).toDate();
            edt_date->setDate(d);
        }
        else if (index.column() == ModelOpList::E_VAL)
        {
            QDoubleSpinBox* edt_val = static_cast<QDoubleSpinBox*>(editor);
            float v = index.model()->data(index, Qt::EditRole).toFloat();
            edt_val->setValue(v);
        }
        else if (index.column() == ModelOpList::E_ITEM)
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
                // may be deprecated : name always associate with BudgetItem
                // object
                pCmbx->setCurrentIndex(0);
            }
        }
        else
        {
            QStyledItemDelegate::setEditorData(editor, index);
        }
    }
}

void EditOpDelegate::setModelData(QWidget* editor,
                                  QAbstractItemModel* model,
                                  const QModelIndex& index) const
{
    if (index.isValid())
    {
        if (index.column() == ModelOpList::E_DATE)
        {
            QDateEdit* edt_date = static_cast<QDateEdit*>(editor);
            QDate d = edt_date->date();
            model->setData(index, d);
        }
        else if (index.column() == ModelOpList::E_VAL)
        {
            QDoubleSpinBox* edt_value = static_cast<QDoubleSpinBox*>(editor);
            float v = (float)edt_value->value();
            model->setData(index, v);
        }
        else if (index.column() == ModelOpList::E_ITEM)
        {
            QComboBox* comboBox = static_cast<QComboBox*>(editor);
            if (comboBox->currentIndex() == 0) // other -> no item
            {
                model->setData(index, QVariant(), Qt::EditRole);
            }
            else
            {
                model->setData(index, comboBox->currentData(), Qt::EditRole);
            }
        }
        else
        {
            QStyledItemDelegate::setModelData(editor, model, index);
        }
    }
}

/*!
  \reimp
*/
bool EditOpDelegate::editorEvent(QEvent* event,
                                 QAbstractItemModel* model,
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index)
{
    bool res = QStyledItemDelegate::editorEvent(event, model, option, index);
    if (res)
    {
        emit modelDataEdited(index);
    }
    return res;
}

// void EditOpDelegate::updateEditorGeometry(QWidget *editor, const
// QStyleOptionViewItem &option, const QModelIndex &/* index */) const
//{
//	editor->setGeometry(option.rect);
// }


void EditOpDelegate::paint(QPainter* painter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index) const
{
    QStyleOptionViewItem myOption = option;
    // initStyleOption(&myOption, index);
    // get operation
    QVariant data = index.model()->data(index, Qt::UserRole);
    if (data.canConvert<Operation*>())
    {
        Operation* op = data.value<Operation*>();
        if (!op->IsValid())
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
    }

    QStyledItemDelegate::paint(painter, myOption, index);

    // QString text = "toto"; // Items[index.row()].c_str();
    // myOption.text = text;
    // QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption,
    // painter);
    //	 QApplication::style()->drawItemText(painter,myOption.rect,Qt::AlignLeft,myOption.palette,
    //										 true,text,QPalette::WindowText);
}
