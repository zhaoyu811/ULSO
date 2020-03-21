#ifndef USERDELEGATE_H
#define USERDELEGATE_H

#include <QModelIndex>
#include <QSize>
#include <QItemDelegate>

QT_FORWARD_DECLARE_CLASS(QPainter)

class UserDelegate : public QItemDelegate
{
public:
    UserDelegate(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const override;

private:
};

#endif
