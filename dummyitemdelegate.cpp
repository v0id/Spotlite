#include "dummyitemdelegate.h"
#include <QPainter>

DummyItemDelegate::DummyItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void DummyItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    if (QStyleOptionViewItemV4 *o4 = qstyleoption_cast<QStyleOptionViewItemV4 *>(option))
    {
        o4->text.clear();
    }
}


/*void DummyItemDelegate::paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &idx) const
{
    QStyleOptionViewItemV4 o4 = QStyleOptionViewItemV4(option);
    o4.text.clear();
    QStyledItemDelegate::paint(painter, o4, idx);
}*/

