#ifndef POSTE_H
#define POSTE_H

#include <QString>
#include <QStringList>

/**
 * Poste is composed by a list of operation names.
 */
class Poste
{
public:
	Poste(const QString& name);
	QString Name() const { return m_Label; }
	bool Include(const QString& op_name) const;
	void AddOp(const QString& op_label);
	void RemoveOp(const QString& op_label);

private:
	/// Nom du poste
	QString m_Label;
	/// liste des operations rattachées au poste
	QStringList m_ListItem;

};

#endif // POSTE_H
