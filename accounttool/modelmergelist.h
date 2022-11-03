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

#ifndef MODELMERGELIST_H
#define MODELMERGELIST_H

#include <QAbstractListModel>
#include <QStringList>
#include "accounttooldata/accountmerger.h"
#include <QVector>

#include "accounttooldata/dataaccount.h"

// to use OperationMerged* as QVariant
Q_DECLARE_METATYPE(OperationMerged)

class ModelMergeList : public QAbstractListModel
{
    Q_OBJECT

public:
    enum eCOL
    {
        E_MERGE = 0,
        E_DATE = 1,
        E_LABEL = 2,
        E_VAL = 3,
        E_ITEM = 4,
        E_DESC = 5,
        E_MAX = 6
    };

    ModelMergeList(QObject* parent = 0);
    void SetState(DataAccount* pData, const QString& str_year);
    void Restore(const QString& stat);
    void Apply();

    // subclassing
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual bool insertRows(int row,
                            int count,
                            const QModelIndex& parent = QModelIndex());
    virtual bool removeRows(int row,
                            int count,
                            const QModelIndex& parent = QModelIndex());
    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index,
                         const QVariant& value,
                         int role = Qt::EditRole);
    virtual QVariant headerData(int section,
                                Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    // D&D features
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;
    QMimeData* mimeData(const QModelIndexList& indexes) const;
    bool dropMimeData(const QMimeData* data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex& parent);

protected:
private:
    DataAccount* m_pData;
    QString m_Year;
    static QList<Operation*> m_EmptyList;
    QVector<OperationMerged> m_List;
    QStringList m_Titles;
};

#endif // MODELMERGELIST_H
