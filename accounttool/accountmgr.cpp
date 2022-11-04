#include "accountmgr.h"
#include "poste.h"
#include <QFile>

AccountMgr::AccountMgr()
{
}

AccountMgr::~AccountMgr()
{
	ClearOperations();
}

bool AccountMgr::LoadPoste(const QString &path_file)
{
	QFile file(path_file);
	// open file
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QXmlStreamReader xml;
	xml.setDevice(&file);

	bool found = false;
	// go to doc root
	while (!found && !xml.atEnd())
	{
		xml.readNextStartElement();
		found = (xml.name().compare("MJA:soft:account")==0);
	}
	if (xml.hasError() || !found)
	{
		return false; // wrong format
	}
	// read poste list
	Poste* pElt;
	QString name;
	while (!xml.atEnd())
	{
		xml.readNextStartElement();
		name = xml.name().toString();
		if (xml.name().compare("poste")==0)
		{
			xml.attributes().value("name").toString();
			pElt = new Poste(name);
			LoadOp(xml,pElt);
			m_ListPoste.append(pElt);
		}
	}
	return true;
}

//--- XML

int AccountMgr::LoadOp(QXmlStreamReader& xml, Poste* p)
{
	int n=0;
	QString str_op;
	QXmlStreamReader::TokenType t =xml.readNext();
	while (xml.name().compare("poste")!=0)
	{
		if (t==QXmlStreamReader::StartElement
			&& xml.name().compare("op")==0)
		{
			str_op =  xml.readElementText();
			p->AddOp(str_op);
			n++;
		}
		t = xml.readNext();
	}
	return n;
}

void AccountMgr::ClearOperations()
{
	while (!m_ListPoste.isEmpty())
	{
		delete m_ListPoste.takeFirst();
	}
}
