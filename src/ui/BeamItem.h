#pragma once

#include <QVector>
#include <QVariant>

class BeamItem
{
public:
	explicit BeamItem(const QVector<QVariant> &data, BeamItem *parent = 0);
	~BeamItem();

	BeamItem *child(int number);
	void appendChild(BeamItem *item);
	int childCount() const;
	int columnCount() const;
	int row() const;
	QVariant data(int column) const;
	bool insertChildren(int position, int count, int columns);
	bool insertColumns(int position, int columns);
	BeamItem *parent();
	bool removeChildren(int position, int count);
	bool removeColumns(int position, int columns);
	int childNumber() const;
	bool setData(int column, const QVariant &value);

private:
	QList<BeamItem*> childItems;
	QVector<QVariant> itemData;
	BeamItem *parentItem;
};
