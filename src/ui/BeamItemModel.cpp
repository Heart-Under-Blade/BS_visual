#include "BeamItemModel.h"
#include <QStringList>

BeamItemModel::BeamItemModel(const QStringList &headers, const QString &data,
							 QObject *parent)
{
	QVector<QVariant> rootData;
	foreach (QString header, headers)
		rootData << header;

	rootItem = new BeamItem(rootData);
	setupModelData(data.split(QString("\n")), rootItem);
}

BeamItemModel::~BeamItemModel()
{
	delete rootItem;
}

QModelIndex BeamItemModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	BeamItem *parentItem = getItem(parent);

	BeamItem *childItem = parentItem->child(row);

	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex BeamItemModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	BeamItem *childItem = getItem(index);
	BeamItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}

int BeamItemModel::rowCount(const QModelIndex &parent) const
{
	BeamItem *parentItem = getItem(parent);

	return parentItem->childCount();
}

int BeamItemModel::columnCount(const QModelIndex &parent) const
{
	return rootItem->columnCount();
}

QVariant BeamItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	BeamItem *item = static_cast<BeamItem*>(index.internalPointer());

	return item->data(index.column());
}

bool BeamItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
}

QVariant BeamItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

bool BeamItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
}

bool BeamItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
}

bool BeamItemModel::insertColumns(int column, int count, const QModelIndex &parent)
{
}

bool BeamItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
}

bool BeamItemModel::removeColumns(int column, int count, const QModelIndex &parent)
{
}

bool BeamItemModel::hasChildren(const QModelIndex &parent) const
{
	return getItem(parent)->childCount() != 0;
}

Qt::ItemFlags BeamItemModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

BeamItem *BeamItemModel::getItem(const QModelIndex &index) const
{
	if (index.isValid())
	{
		BeamItem *item = static_cast<BeamItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return rootItem;
}

void BeamItemModel::setupModelData(const QStringList &lines, BeamItem *parent)
{
	QList<BeamItem*> parents;
	QList<int> indentations;
	parents << parent;
	indentations << 0;

	int number = 0;

	while (number < lines.count())
	{
		int position = 0;
		while (position < lines[number].length())
		{
			if (lines[number].mid(position, 1) != " ")
				break;
			position++;
		}

		QString lineData = lines[number].mid(position).trimmed();

		if (!lineData.isEmpty())
		{
			// Считываем значения столбцов из остатка строки.
			QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
			QVector<QVariant> columnData;
			for (int column = 0; column < columnStrings.count(); ++column)
				columnData << columnStrings[column];

			if (position > indentations.last())
			{
				// Последний потомок текущего родителя теперь новый родитель
				// пока текущий родитель не имеет потомков.

				if (parents.last()->childCount() > 0)
				{
					parents << parents.last()->child(parents.last()->childCount()-1);
					indentations << position;
				}
			}
			else
			{
				while (position < indentations.last() && parents.count() > 0)
				{
					parents.pop_back();
					indentations.pop_back();
				}
			}

			// Добавляем новый элемент в список потомков текущего родителя.
			parents.last()->appendChild(new BeamItem(columnData, parents.last()));
		}

		number++;
	}
}
