/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "view.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsTextItem>
#include "callout.h"
#include <QtGui/QMouseEvent>
#include <QtCharts/QDateTimeAxis>
#include <QDateTime>
#include <QtCharts/QXYSeries>
#include <QDebug>
#include <QtCharts/QScatterSeries>
#include <QTextDocument>
#include <QGraphicsProxyWidget>

View::View(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      m_coordX(0),
      m_coordY(0),
      m_chart(0),
      m_tooltip(0)
{
    setStyleSheet("padding:0px;border:0px");
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setRenderHint(QPainter::Antialiasing);

    this->setMouseTracking(true);

    QFont font2;
    font2.setPointSize(10);//字体大小
    this->setFont(font2);
    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->setSingleShot(false);
    connect(timer, &QTimer::timeout, this, &View::TimerTimeOut);
}

void View::setChart(QChart *chart)
{
    if(pixmapItem!=NULL)
    {
        this->scene()->removeItem(pixmapItem);
        pixmapItem=NULL;
    }

    if(m_chart!=NULL)
    {
        scene()->update();
        QScatterSeries *series = (QScatterSeries *)m_chart->series().at(1);
        //connect(series, SIGNAL(clicked(QPointF)), this, SLOT(keepCallout()));
        connect(series, SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));
    }
    else
    {
        m_chart = chart;
        m_chart->setAcceptHoverEvents(true);
        scene()->addItem(m_chart);
        QScatterSeries *series = (QScatterSeries *)m_chart->series().at(1);
        //connect(series, SIGNAL(clicked(QPointF)), this, SLOT(keepCallout()));
        connect(series, SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));

        m_chart->resize(this->size());
    }

    if(pushButton!=NULL)
    {
        delete pushButton;
        pushButton = NULL;
    }

    pushButton = new QPushButton(tr("动态显示"));
    QFont font2;
    font2.setPointSize(14);//字体大小
    pushButton->setFont(font2);
    QGraphicsProxyWidget * tmp = this->scene()->addWidget(pushButton);
    tmp->setPos(1500, 50);
    connect(pushButton, &QPushButton::clicked, this, &View::PushButtonClicked);
}

void View::resizeEvent(QResizeEvent *event)
{
    qDebug()<<event->size();
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        if(m_chart!=nullptr)
        {
            m_chart->resize(event->size());
            foreach (Callout *callout, m_callouts)
                callout->updateGeometry();
        }
    }
    QGraphicsView::resizeEvent(event);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

void View::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(m_chart);
}

void View::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);

    if (state)
    {
        int i=1;
        QScatterSeries *series = (QScatterSeries *)m_chart->series().at(1);
        foreach (QPointF point2, series->points())
        {
            if(point2 == point)
                break;
            i++;
        }

        m_tooltip->setText(QString("第%1次数据\n时间: %2 \n%3: %4").arg(i).arg(QDateTime::fromTime_t(point.x()/1000).toString("yyyy-MM-dd")).arg(text).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
        ShowImage(point.ry());
    }
    else
    {
        m_tooltip->hide();
    }
}

void View::ShowImage(double value)
{
    if(pixmapItem!=NULL)
    {
        this->scene()->removeItem(pixmapItem);
        pixmapItem=NULL;
    }

    QString fileName;
    if(text == tr("BMI(%)"))
    {
        if(gender==tr("男"))
        {
            //BMI 男
            if(value<18.5)
                fileName=tr("BMIM1.jpg");
            else if(value>=18.5 && value<24.9)
                fileName=tr("BMIM2.jpg");
            else if(value>=24.9 && value<29.9)
                fileName=tr("BMIM3.jpg");
            else if(value>=29.9 && value<34.9)
                fileName=tr("BMIM4.jpg");
            else if(value>=34.9 && value<39.9)
                fileName=tr("BMIM5.jpg");
            else if(value>=39.9)
                fileName=tr("BMIM6.jpg");
        }
        else
        {
            //BMI 女
            if(value<15)
                fileName=tr("BMIW1.jpg");
            else if(value>=15 && value<18.5)
                fileName=tr("BMIW2.jpg");
            else if(value>=18.5 && value<24.9)
                fileName=tr("BMIW3.jpg");
            else if(value>=24.9 && value<29.9)
                fileName=tr("BMIW4.jpg");
            else if(value>=29.9)
                fileName=tr("BMIW5.jpg");
        }

        if(!fileName.isEmpty())
        {
            QString prefix=tr(":/img/bmi/resource/img/bmi/");
            fileName = prefix+fileName;
            qDebug()<<fileName;

            QPixmap pixmap(fileName);
            pixmapItem = new QGraphicsPixmapItem(pixmap);
            pixmapItem->setPos(150, 420);
            pixmapItem->setFlag(QGraphicsItem::ItemIsMovable, true);
            this->scene()->addItem(pixmapItem);
        }
    }
    else if(text == tr("体脂率(%)"))
    {
        if(gender==tr("男"))
        {
            //体脂率 男
            if(value>=3 && value<=4)
                fileName=tr("M1.jpg");
            else if(value>=6 && value<=7)
                fileName=tr("M2.jpg");
            else if(value>=10 && value<=12)
                fileName=tr("M3.jpg");
            else if(value>=14 && value<=16)
                fileName=tr("M4.jpg");
            else if(value>=19 && value<=21)
                fileName=tr("M5.jpg");
            else if(value>=24 && value<=26)
                fileName=tr("M6.jpg");
            else if(value>=29 && value<=31)
                fileName=tr("M7.jpg");
            else if(value>=34 && value<=36)
                fileName=tr("M8.jpg");
            else if(value>=39 && value<=41)
                fileName=tr("M9.jpg");
        }
        else
        {
            //体脂率  女
            if(value>=49 && value<=51)
                fileName=tr("W1.jpg");
            else if(value>=44 && value<=46)
                fileName=tr("W2.jpg");
            else if(value>=39 && value<=41)
                fileName=tr("W3.jpg");
            else if(value>=34 && value<=36)
                fileName=tr("W4.jpg");
            else if(value>=29 && value<=31)
                fileName=tr("W5.jpg");
            else if(value>=24 && value<=26)
                fileName=tr("W6.jpg");
            else if(value>=20 && value<=22)
                fileName=tr("W7.jpg");
            else if(value>=15 && value<=17)
                fileName=tr("W8.jpg");
            else if(value>=10 && value<=12)
                fileName=tr("W9.jpg");
        }

        if(!fileName.isEmpty())
        {
            QString prefix=tr(":/img/bfp/resource/img/bfp/");
            fileName = prefix+fileName;
            qDebug()<<fileName;
            QPixmap pixmap(fileName);
            pixmapItem = new QGraphicsPixmapItem(pixmap);
            pixmapItem->setPos(150, 420);
            pixmapItem->setFlag(QGraphicsItem::ItemIsMovable, true);
            this->scene()->addItem(pixmapItem);
        }
    }
    else
    {
        return;
    }
}

void View::PushButtonClicked()
{
    if(pushButton->text()==tr("动态显示"))
    {
        pushButton->setText(tr("停止"));
        timer->start();
    }
    else
    {
        pushButton->setText(tr("动态显示"));
        timer->stop();
    }
}

void View::TimerTimeOut()
{
    if(m_chart!=NULL)
    {
        if (m_tooltip == 0)
            m_tooltip = new Callout(m_chart);

        QScatterSeries *series = (QScatterSeries *)m_chart->series().at(1);
        if(i<series->points().count())
        {
            QPointF point = series->points().at(i++);

            m_tooltip->setText(QString("第%1次数据\n时间: %2 \n%3: %4").arg(i).arg(QDateTime::fromTime_t(point.x()/1000).toString("yyyy-MM-dd")).arg(text).arg(QString::number(point.y(), 'f', 2)));
            m_tooltip->setAnchor(point);
            m_tooltip->setZValue(11);
            m_tooltip->updateGeometry();
            m_tooltip->show();
            ShowImage(point.ry());
        }
        else
        {
            pushButton->setText(tr("动态显示"));
            timer->stop();
            i=0;
        }
    }
}
