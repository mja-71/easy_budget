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

#include "dataaccountfile.h"
//#include <QDesktopServices>
#include <QDir>
#include <QDate>
#include <QTextStream>
#include <QFile>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QStandardPaths>

#include "operation.h"
#include "period.h"

/// Constructor with folder path
DataAccountFile::DataAccountFile(const QString& path)
: m_Path(path), m_MapItems(0)
{
    // get absolute path
    QFileInfo info_dir(path);
    if (info_dir.isRelative())
    {
        QString data_dir = QStandardPaths::writableLocation(
            QStandardPaths::GenericDataLocation);
        // 		+ "/data/organization/application"

        data_dir += "/JihmSoftware/AccountTool/";
        info_dir.setFile(data_dir + path);
        m_Path = info_dir.absoluteFilePath();
    }
    // init period (= year) list
    int n = LoadYears();
    // if empty, initialize with current year
    if (n == 0)
    {
        //  create bdd folder and current year folder
        QString curr_year = QString("%1").arg(QDate::currentDate().year());
        QDir bdd_dir;
        bdd_dir.mkpath(m_Path + "/" + curr_year);
        for (int month = 0; month < 12; month++)
        {
            CreateBankStat(curr_year, month + 1);
        }
    }
}

DataAccountFile::~DataAccountFile()
{
    DeleteStats();
    delete m_MapItems;
}

void DataAccountFile::Close(Period* p)
{
    p->SetClose(true);
    // rename period folder
    QDir root_folder(m_Path);
    root_folder.rename(p->Name(), p->Name() + "_c");
    // copy Item file
    QString new_name = QString("%1/%2_c/postes.xml").arg(m_Path).arg(p->Name());
    QFile file_items(m_Path + "/postes.xml");
    file_items.copy(m_Path + "/postes.xml", new_name);
    // Force items loading
    Items(p);
}


/// Get Years
int DataAccountFile::Years(QList<Period*>& list_res)
{
    if (m_MapYear.size() == 0)
    {
        LoadYears();
    }
    list_res = m_MapYear.values();
    return (m_MapYear.size());
}

/// Get Releve for a year
int DataAccountFile::BankStats(const QString& year, QList<Statement*>& list_rel)
{
    QString str_id;
    int n = 0;
    for (int m = 1; m <= 12; m++)
    {
        str_id = QString("%1-%2").arg(year).arg(m, 2, 10, QChar('0'));
        Statement* st = GetStat(str_id, false);
        list_rel.append(st);
    }
    return n;
}

/// Get a statement
const Statement* DataAccountFile::BankStat(const QString& id)
{
    return GetStat(id, false);
}
/// Create Bank statement.
bool DataAccountFile::CreateBankStat(const QString& year, int month)
{
    bool res = false;
    QString stat_id = QString("%1-%2").arg(year).arg(month, 2, 10, QChar('0'));

    // check if statement already exists
    Period* p = GetPeriod(year);
    Statement* st = p->GetStatement(stat_id); // = GetStat(stat_id,false);
    // save in file system, if not yet created
    QString file_path =
        QString("%1/%2/%3.csv").arg(m_Path).arg(year).arg(stat_id);
    QFile f(file_path);
    if (!f.exists())
    {
        res = SaveOps(st);
    }
    return res;
}

/// Get Operation of a Statement
QList<Operation*>& DataAccountFile::Operations(const QString& stat)
{
    Statement* st = GetStat(stat, true);
    return (*st->Ops());
}

void DataAccountFile::Operations(const QString& stat,
                                 int status,
                                 QList<Operation*>& list_ops)
{
    Statement* st = GetStat(stat, true);
    // add op if status matche
    foreach (Operation* op, *st->Ops())
    {
        if (((status & E_BANK) && op->IsBank()) ||
            ((status & E_USER) && op->IsUser()))
        {
            if (status & E_VALID)
            {
                if (op->IsValid())
                {
                    list_ops.push_back(op);
                }
            }
            else
            {
                if (!op->IsValid())
                {
                    list_ops.push_back(op);
                }
            }
        }
    }
}

void DataAccountFile::SetTransfer(Period* p, Operation* op, bool save /*=true*/)
{
    p->SetTransfer(op);
    if (save)
    {
        Statement* st = GetStat(QString("%1_01").arg(p->Name()), true);
        SaveOps(st);
    }
}

/// Add operations into the db.
/// operations are added in m_MapOps and saved on disk if \p save is true
int DataAccountFile::AddOps(const QList<Operation*>& ops,
                            const QString& stat_id,
                            bool save /*=true*/
)
{
    QString curr_stat_id;
    QList<Operation*>::const_iterator it = ops.constBegin(),
                                      it_end = ops.constEnd();
    QList<Statement*> list_stats;
    QList<Operation*>* list_ops = 0;
    Statement* st = 0;
    int nb_ops = 0;
    if (!stat_id.isEmpty())
    {
        st = GetStat(stat_id, true);
        list_ops = st->Ops();
    }
    while (it != it_end)
    {
        // if no default statement id, search statement
        if (stat_id.isEmpty() && (stat_id != (*it)->StatId()))
        {
            curr_stat_id = (*it)->StatId();
            st = GetStat(curr_stat_id, true);
            list_ops = st->Ops();
            list_stats.push_back(st);
        }
        // add op in statement
        if ((list_ops != nullptr) && (Find((*it)->Id(), *list_ops) == 0))
        {
            list_ops->push_back((*it));
            nb_ops++;
        }
        else
        {
            delete (*it);
        }
        ++it;
    }
    // save statements
    if (save)
    {
        foreach (Statement* st, list_stats)
        {
            if (!SaveOps(st))
            {
                return -1;
            }
        }
    }
    return nb_ops;
}

/// Add a reference position in db.
bool DataAccountFile::AddRefPosition(const StatePos& pos)
{
    Statement* st = GetStat(pos.StateId(), false);
    st->SetPosition(pos);
    return true;
}

/// Get list of budget items defined in the account.
/// \p p is a pointer on a Period
/// p is 0 to get current items
/// if not null and period closed, get archived items of the period
QList<BudgetItem*> DataAccountFile::Items(Period* p)
{
    QMap<unsigned long, BudgetItem*>* map;
    if (p != 0 && p->IsClose()) // use archived items
    {
        map = p->Items();
        if (map == 0)
        {
            QString path_file =
                QString("%1/%2_c/postes.xml").arg(m_Path).arg(p->Name());
            map = new QMap<unsigned long, BudgetItem*>;
            LoadItems(path_file, *map);
            p->SetItems(map);
        }
        return map->values();
    }
    else
    {
        if (m_MapItems == 0)
        {
            m_MapItems = new QMap<unsigned long, BudgetItem*>;
            LoadItems(m_Path + "/postes.xml", *m_MapItems);
        }
        return m_MapItems->values();
    }
}

BudgetItem* DataAccountFile::InsertItem(const QString& label)
{
    BudgetItem* bi = new BudgetItem(label);
    m_MapItems->insert(bi->Id(), bi);
    return bi;
}

bool DataAccountFile::UpdateStat(const QString& stat_id)
{
    return SaveOps(GetStat(stat_id, true));
}

bool DataAccountFile::RestoreStat(const QString& stat_id)
{
    // remove previons
    Statement* st = GetStat(stat_id, true);
    QList<Operation*>& list_op = *st->Ops();
    foreach (Operation* op, list_op)
    {
        delete op;
    }
    list_op.clear();
    // reload
    return (LoadOps(st) >= 0);
}

/// Save items in db.
/// Only current items can be modified and save
bool DataAccountFile::UpdateItems()
{
    QString path_file(m_Path + "/postes.xml");
    QFile file(path_file);
    // open file
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("soft_account");
    xml.writeAttribute("last_id", QString::number(BudgetItem::LastId()));
    // list of items
    QMap<unsigned long, BudgetItem*>::const_iterator it = m_MapItems->cbegin();
    for (; it != m_MapItems->cend(); ++it)
    {
        BudgetItem* bi = it.value();
        if (!bi->IsDeprecated())
        {
            xml.writeStartElement("poste");
            xml.writeAttribute("name", bi->Name());
            if (bi->IsCredit())
                xml.writeAttribute("type", "+");
            else if (bi->IsDebit())
                xml.writeAttribute("type", "-");
            else
                xml.writeAttribute("type", "*");
            xml.writeAttribute("id", QString::number(bi->Id()));
            // list of op, included into the item
            foreach (QString str_op, bi->Ops())
            {
                xml.writeTextElement("op", str_op);
            }
            xml.writeEndElement(); // end poste
        }
    }
    xml.writeEndElement(); // end soft_account
    xml.writeEndDocument();
    return true;
}


//------------------ PROTECTED ----------------------------------------

/// Load Years
int DataAccountFile::LoadYears()
{
    QDir dir_releve(m_Path);
    QStringList list_files, filters;
    QString str, str_year, str_curr = "";
    bool ok;
    int n = 0;
    Period* p;
    // get folders
    filters << "*";
    list_files = dir_releve.entryList(filters, QDir::Dirs, QDir::Name);
    foreach (str, list_files)
    {
        str_year = str.left(4);
        // check if number
        str_year.toUInt(&ok);
        if (ok && str_year != str_curr)
        {
            if (str.endsWith("_c"))
            {
                p = new Period(str_year, false);
            }
            else
            {
                p = new Period(str_year, true);
            }
            m_MapYear.insert(str_year, p);
            str_curr = str_year;
            n++;
        }
    }
    return n;
}

Period* DataAccountFile::GetPeriod(const QString& year)
{
    Period* p = 0;
    QMap<QString, Period*>::iterator it = m_MapYear.find(year);
    if (it != m_MapYear.end())
    {
        p = it.value();
    }
    else
    {
        p = new Period(year);
        m_MapYear.insert(year, p);
    }
    return p;
}

/// Delete operations
void DataAccountFile::DeleteStats()
{
    foreach (Period* p, m_MapYear)
    {
        delete p;
    }
    m_MapYear.clear();
}

/// Load and parse file : the file is in "year" folder and named "YYYY-MM.csv".
int DataAccountFile::LoadOps(Statement* st)
{
    QMap<unsigned long, BudgetItem*>* items;
    QList<Operation*>* list_ops = new QList<Operation*>;
    QString year = st->Id().mid(0, 4);
    Period* p = GetPeriod(year);
    Items(p); // force items loading
    if (p->IsClose())
    {
        items = p->Items();
        year += "_c";
    }
    else
    {
        items = m_MapItems;
    }
    // open file
    QString file_path =
        QString("%1/%2/%3.csv").arg(m_Path).arg(year).arg(st->Id());
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }
    // Parse file
    int n = 0;
    QString line;
    Operation* op;
    QTextStream in(&file);
    // skip title
    line = in.readLine();
    if (line.startsWith("EASYBUDGET"))
    {
        in.readLine();        // header
        line = in.readLine(); // position
        StatePos pos = CreatePos(line);
        st->SetPosition(pos);
        line = in.readLine();        // transfer
        if (st->Date().month() == 1) // transfer only define for first month
        {
            Operation* transfer = CreateTransfer(line);
            p->SetTransfer(transfer);
        }
    }
    else // version 0. (test)
    {
        in.readLine(); // header
    }
    // read ops
    while (!in.atEnd())
    {
        // read operation
        line = in.readLine();
        if (!line.isEmpty())
        {
            op = CreateOp(line, *items);
            if (op != 0)
            {
                list_ops->push_back(op);
                n++;
            }
        }
    }
    file.close();
    st->SetOps(list_ops);
    return n;
}

/// Create operation from string.
/// DATE;ID;LABEL;VALUE;CATEGORIE;COMMENT
Operation* DataAccountFile::CreateOp(
    const QString& str, QMap<unsigned long, BudgetItem*>& map_items)
{
    Operation* op = 0;
    QStringList fields = str.split(';');
    QDate date;
    float val;
    QString id, label;
    bool user = false, valid = false;
    if (fields.size() >= 6)
    {
        if (fields.size() >= 8)
        {
            user = (fields.at(6) == "U");
            valid = (fields.at(7) == "1");
        }
        // date
        date = QDate::fromString(fields.at(0), QString("dd-MM-yyyy"));
        // id
        id = fields.at(1);
        // label
        label = fields.at(2);
        // value
        val = fields.at(3).toFloat();
        // create operation
        op = new Operation(user);
        op->Init(id, date, val, label);
        op->SetComment(fields.at(5));
        op->SetStatus(valid);

        QMap<unsigned long, BudgetItem*>::iterator it =
            map_items.find(fields.at(4).toULong());
        if (it != map_items.end())
        {
            op->SetItem(*it);
        }
    }
    return op;
}

/// Get position from string.
/// DATE;"POSITION";"POSITION";VALUE
StatePos DataAccountFile::CreatePos(const QString& str)
{
    QStringList fields = str.split(';');
    QDate date;
    float val;
    if (str == "UNDEFINED POSITION")
    {
        return StatePos();
    }
    if (fields.size() == 4)
    {
        // date
        date = QDate::fromString(fields.at(0), QString("dd-MM-yyyy"));
        // value
        val = fields.at(3).toFloat();
    }
    return StatePos(date, val);
}
/// Get transfer from string.
/// "TRANSFER";VALUE
Operation* DataAccountFile::CreateTransfer(const QString& str)
{
    Operation* op = 0;
    QStringList fields = str.split(';');
    QDate date;
    float val;
    QString id, label;
    if (fields.size() >= 6)
    {
        // date
        date = QDate::fromString(fields.at(0), QString("dd-MM-yyyy"));
        // id
        id = fields.at(1);
        // label
        label = fields.at(2);
        // value
        val = fields.at(3).toFloat();
        // create operation
        op = new Operation();
        op->Init(id, date, val, label);
        op->SetComment(fields.at(5));
        op->SetTransfer(true);
    }
    return op;
}

/// Save file : the file is in "year" folder and named "YYYY-MM.csv".
bool DataAccountFile::SaveOps(Statement* st)
{
    QDate date_stat = st->Date();
    // create year folder
    QString year = st->Id().left(4);
    QDir root_dir(m_Path);
    Period* p = GetPeriod(year);
    if (p->IsClose())
    {
        year += "_c";
    }
    root_dir.mkdir(year);
    // open file
    QString file_path =
        QString("%1/%2/%3.%4").arg(m_Path).arg(year).arg(st->Id()).arg("csv");
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    // write op in file
    QTextStream out(&file);
    out << QString("EASYBUDGET DB VERSION 1.0\n");
    out << QString("DATE;ID;LABEL;VALUE;CATEGORIE;COMMENT\n");
    // position
    if (st->Position().Date().isValid())
    {
        out << st->Position().Date().toString("dd-MM-yyyy") << ";";
        out << "POSITION;"
            << "POSITION;";
        out << st->Position().StrPos() << "\n";
    }
    else
    {
        out << "UNDEFINED POSITION"
            << "\n";
    }
    // transfer
    if (date_stat.month() == 1 && p->Transfer())
    {
        const Operation* op = p->Transfer();
        out << op->Date().toString("dd-MM-yyyy") << ";";
        out << "_TRANSFER_; " << op->Label() << ";";
        out << QString("%1").arg(op->Value(), 0, 'f', 2) << ";";
        out << "0"
            << ";" << op->Comment() << ";U;1\n";
    }
    else
    {
        out << "TRANSFER;"
            << "\n";
    }
    // operations
    if (st->Ops() != 0)
    {
        QList<Operation*>& list_ops = *st->Ops();
        foreach (Operation* op, list_ops)
        {
            out << op->Date().toString("dd-MM-yyyy") << ";";
            out << op->Id() << ";";
            out << op->Label() << ";";
            out << QString("%1").arg(op->Value(), 0, 'f', 2) << ";";
            if (op->Item() != 0)
            {
                out << op->Item()->Id() << ";";
            }
            else
            {
                out << "0"
                    << ";";
            }
            out << op->Comment() << ";";
            out << (op->IsUser() ? "U;" : "B;");
            out << (op->IsValid() ? "1\n" : "0\n");
        }
    }
    file.close();
    return true;
}

/// Load item definition file
int DataAccountFile::LoadItems(const QString& path_file,
                               QMap<unsigned long, BudgetItem*>& map_item)
{
    map_item.clear();

    QFile file(path_file);
    QString name, type;
    unsigned long id;
    // open file
    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    QXmlStreamReader xml;
    xml.setDevice(&file);

    bool found = false;
    // go to doc root
    while (!found && !xml.atEnd())
    {
        xml.readNextStartElement();
        found = (xml.qualifiedName().compare(QString("soft_account")) == 0);
    }
    if (xml.hasError() || !found)
    {
        name = xml.errorString();
        return -1; // wrong format
    }

    // read last item number
    unsigned long last_id = xml.attributes().value("last_id").toULong();
    BudgetItem::SetLastId(last_id);

    // read item list
    int n = 0;

    while (!xml.atEnd())
    {
        xml.readNextStartElement();
        if (xml.name().compare(QString("poste")) == 0)
        {
            name = xml.attributes().value("name").toString();
            type = xml.attributes().value("type").toString();
            id = xml.attributes().value("id").toULong();
            BudgetItem* bi = new BudgetItem(id);
            bi->SetData(name, type);
            LoadItem(xml, *bi);
            map_item.insert(bi->Id(), bi);
            n++;
        }
    }
    return n;
}

//--- XML

/// init p from xml stream.
int DataAccountFile::LoadItem(QXmlStreamReader& xml, BudgetItem& p)
{
    int n = 0;
    QString str_op;
    QXmlStreamReader::TokenType t = xml.readNext();
    while (xml.name().compare(QString("poste")) != 0)
    {
        if (t == QXmlStreamReader::StartElement &&
            xml.name().compare(QString("op")) == 0)
        {
            str_op = xml.readElementText();
            p.AddOp(str_op);
            n++;
        }
        t = xml.readNext();
    }
    return n;
}

/// Get a statement
Statement* DataAccountFile::GetStat(const QString& str_id, bool load)
{
    QString year = str_id.left(4);
    Period* p = GetPeriod(year);
    Statement* st = p->GetStatement(str_id);

    // create on file system if not yet done
    if (p->IsClose())
    {
        year += "_c";
    }
    QString file_path =
        QString("%1/%2/%3.csv").arg(m_Path).arg(year).arg(str_id);
    QFile f(file_path);
    if (!f.exists())
    {
        st->SetOps(new QList<Operation*>);
        SaveOps(st);
    }
    // load if required and if not yet done
    else if (load && st->Ops() == 0)
    {
        LoadOps(st);
    }

    return st;
}

/// Search operation by id
inline Operation* DataAccountFile::Find(const QString& id,
                                        const QList<Operation*>& list_ops) const
{
    foreach (Operation* op, list_ops)
    {
        if (op->Id() == id)
        {
            return op;
        }
    }
    return 0;
}
