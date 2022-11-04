#include "poste.h"
#include <QFile>
#include <QXmlStreamWriter>

Poste::Poste(const QString& name)
	: m_Label(name)
{
}

bool Poste::Include(const QString& op_name) const
{
	return (m_ListItem.indexOf(op_name) != -1);
}

void Poste::AddOp(const QString &op_label)
{
	m_ListItem.append(op_label);
}

void Poste::RemoveOp(const QString &op_label)
{
	m_ListItem.removeAll(op_label);
}





