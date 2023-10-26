#pragma once

#include <QAbstractItemModel>

class PingTreeItem {
public:
  explicit PingTreeItem(QString name, PingTreeItem *parent = nullptr,
                        bool isLeaf = true);
  ~PingTreeItem();

  PingTreeItem *child(int pos);
  int childCount() const;
  bool insertChildern(int pos, PingTreeItem *item);
  PingTreeItem *parent();

  bool removeChildren(int pos, int count);
  int pos() const;
  QString value;

private:
  QList<PingTreeItem *> children_;
  PingTreeItem *parent_ = nullptr;
  bool isLeaf_ = true;
};

class PingTreeModel : public QAbstractItemModel {
  Q_OBJECT

public:
  explicit PingTreeModel(QObject *parent = nullptr);
  ~PingTreeModel();

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  //  bool setHeaderData(int section, Qt::Orientation orientation,
  //                     const QVariant &value, int role = Qt::EditRole)
  //                     override;

  // Basic functionality:
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  // Editable:
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole) override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  // Add data:
  bool insertRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;
  //  bool insertColumns(int column, int count,
  //                     const QModelIndex &parent = QModelIndex()) override;

  // Remove data:
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;
  //  bool removeColumns(int column, int count,
  //                     const QModelIndex &parent = QModelIndex()) override;

  // Items:
  PingTreeItem *getItem(const QModelIndex &index) const;

private:
  PingTreeItem *root_;
};
