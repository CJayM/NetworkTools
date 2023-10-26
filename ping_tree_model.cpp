#include "ping_tree_model.h"

PingTreeModel::PingTreeModel(QObject *parent) : QAbstractItemModel(parent) {
  root_ = new PingTreeItem("root", nullptr, false);

  QStringList items = {"193.0.12.11", "193.0.12.12", "193.0.12.13",
                       "193.0.12.14", "193.0.12.21", "193.0.12.22",
                       "193.0.12.41"};
  for (const auto &item : items) {
    root_->insertChildern(0, new PingTreeItem(item));
  }
}

PingTreeModel::~PingTreeModel() {
  delete root_;
  root_ = nullptr;
}

QVariant PingTreeModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 0)
      return "IP";
    return "---";
  }

  return QVariant();
}

QModelIndex PingTreeModel::index(int row, int column,
                                 const QModelIndex &parent) const {
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  PingTreeItem *parentItem = getItem(parent);
  if (!parentItem)
    return QModelIndex();

  PingTreeItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  return QModelIndex();
}

QModelIndex PingTreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return QModelIndex();

  PingTreeItem *childItem = getItem(index);
  PingTreeItem *parentItem = childItem ? childItem->parent() : nullptr;

  if (parentItem == root_ || !parentItem)
    return QModelIndex();

  return createIndex(parentItem->pos(), 0, parentItem);
}

int PingTreeModel::rowCount(const QModelIndex &parent) const {
  const PingTreeItem *parentItem = getItem(parent);
  return parentItem ? parentItem->childCount() : 0;
}

int PingTreeModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 2;
}

QVariant PingTreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    auto item = getItem(index);
    if (item == 0)
      return QVariant();

    if (index.column() == 0)
      return item->value;

    return "--";
  }

  return QVariant();
}

bool PingTreeModel::setData(const QModelIndex &index, const QVariant &value,
                            int role) {
  if (role != Qt::EditRole)
    return false;

  auto item = getItem(index);
  if (item == nullptr)
    return false;

  switch (index.column()) {
  case 0:
    auto newValue = value.toString();
    if (item->value == newValue)
      return false;
    item->value = newValue;
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
  }
  return false;
}

Qt::ItemFlags PingTreeModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

bool PingTreeModel::insertRows(int row, int count, const QModelIndex &parent) {
  PingTreeItem *parentItem = getItem(parent);
  if (!parentItem)
    return false;

  auto end = row + count - 1;
  beginInsertRows(parent, row, end);

  for (int i = row; i < end; ++i)
    parentItem->insertChildern(i, new PingTreeItem("127.0.0.1"));
  endInsertRows();

  return true;
}

bool PingTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
  auto start = row;
  auto end = start + count;

  auto parentItem = getItem(parent);
  if (parentItem == nullptr)
    return false;

  if (start >= parentItem->childCount() && end >= parentItem->childCount())
    return false;

  beginRemoveRows(parent, start, end - 1);
  parentItem->removeChildren(row, count);
  endRemoveRows();
}

PingTreeItem *PingTreeModel::getItem(const QModelIndex &index) const {
  if (index.isValid()) {
    PingTreeItem *item = static_cast<PingTreeItem *>(index.internalPointer());
    if (item)
      return item;
  }
  return root_;
}

PingTreeItem::PingTreeItem(QString name, PingTreeItem *parentItem, bool isLeaf)
    : value(name), isLeaf_(isLeaf), parent_(parentItem) {}

PingTreeItem::~PingTreeItem() { qDeleteAll(children_); }

PingTreeItem *PingTreeItem::child(int pos) {
  if (pos < 0 || pos >= children_.size())
    return nullptr;

  return children_.at(pos);
}

int PingTreeItem::childCount() const {
  if (isLeaf_)
    return 0;
  return children_.size();
}

bool PingTreeItem::insertChildern(int pos, PingTreeItem *item) {
  if (pos < 0 || pos > children_.size())
    return false;

  children_.insert(pos, item);
  item->parent_ = this;
  return true;
}

PingTreeItem *PingTreeItem::parent() { return parent_; }

bool PingTreeItem::removeChildren(int pos, int count) {
  if (pos < 0 || pos + count > children_.size())
    return false;

  for (int row = 0; row < count; ++row)
    delete children_.takeAt(pos);
}

int PingTreeItem::pos() const {
  if (parent_)
    return parent_->children_.indexOf(const_cast<PingTreeItem *>(this));
  return 0;
}
