#ifndef DUMMYITEMDELEGATE_H
#define DUMMYITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

class DummyItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DummyItemDelegate(QObject *parent = 0);
    //void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;

signals:

public slots:

};

#endif // DUMMYITEMDELEGATE_H
