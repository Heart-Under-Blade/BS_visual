#ifndef BEAMITEMMODEL_H
#define BEAMITEMMODEL_H

#include <QAbstractItemModel>
#include "BeamItem.h"

class BeamItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	BeamItemModel(const QStringList &headers, const QString &data,
				  QObject *parent = 0);
	~BeamItemModel();

	// QAbstractItemModel interface
public:
	QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex parent(const QModelIndex &child) const override;
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
	bool insertRows(int row, int count, const QModelIndex &parent) override;
	bool insertColumns(int column, int count, const QModelIndex &parent) override;
	bool removeRows(int row, int count, const QModelIndex &parent) override;
	bool removeColumns(int column, int count, const QModelIndex &parent) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	void setupModelData(const QStringList &lines, BeamItem *parent);

	BeamItem *getItem(const QModelIndex &index) const;

private:
	BeamItem *rootItem;


};

#endif // BEAMITEMMODEL_H
