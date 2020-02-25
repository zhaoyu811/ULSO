#include "fuctionwidget.h"
#include <QDebug>

FuctionWidget::FuctionWidget(QWidget *parent) : QWidget(parent)
{
    QGridLayout *button_layout = new QGridLayout(this);

    userButton = new ToolButton(QString(":/img/fuc/resource/img/fucimg/user.png"), this);
    userButton->setText("用户");
    connect(userButton, &ToolButton::clicked, this, [&](){emit ToolButtonClicked(1);});
    button_layout->addWidget(userButton, 0, 0);

    collectInfButton = new ToolButton(QString(":/img/fuc/resource/img/fucimg/collectinf"), this);
    collectInfButton->setText("信息采集");
    connect(collectInfButton, &ToolButton::clicked, this, [&](){emit ToolButtonClicked(2);});
    button_layout->addWidget(collectInfButton, 1, 0);

    forecastButton = new ToolButton(QString(":/img/fuc/resource/img/fucimg/forecast"), this);
    forecastButton->setText("数据预测");
    connect(forecastButton, &ToolButton::clicked, this, [&](){emit ToolButtonClicked(3);});
    button_layout->addWidget(forecastButton, 2, 0);

    button_layout->setColumnStretch(1,1);
    button_layout->setRowStretch(3,1);

    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(Qt::white));
    this->setPalette(palette);
    this->setAutoFillBackground(true);
    this->setLayout(button_layout);
}

void FuctionWidget::resizeEvent(QResizeEvent *event)
{
    qDebug()<<"hera"<<event->size();
}

