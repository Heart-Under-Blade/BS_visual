#include "BeamItem.h"

BeamItem::BeamItem(const QVector<QVariant> &data, BeamItem *parent)
{
	parentItem = parent;
	itemData = data;
}

BeamItem::~BeamItem()
{
	qDeleteAll(childItems);
}

BeamItem *BeamItem::child(int number)
{
	return childItems.value(number);
}

void BeamItem::appendChild(BeamItem *item)
{
	childItems.append(item);
}

int BeamItem::childCount() const
{
	return childItems.count();
}

int BeamItem::columnCount() const
{
	return itemData.count();
}

int BeamItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<BeamItem*>(this));

	return 0;
}

QVariant BeamItem::data(int column) const
{
	return itemData.value(column);
}

bool BeamItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(columns);
		BeamItem *item = new BeamItem(data, this);
		childItems.insert(position, item);
	}

	return true;
}

bool BeamItem::insertColumns(int position, int columns)
{
	if (position < 0 || position > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.insert(position, QVariant());

	foreach (BeamItem *child, childItems)
		child->insertColumns(position, columns);

	return true;
}

BeamItem *BeamItem::parent()
{
	return parentItem;
}

bool BeamItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}

int BeamItem::childNumber() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<BeamItem*>(this));

	return 0;
}

bool BeamItem::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= itemData.size())
		return false;

	itemData[column] = value;
	return true;
}
