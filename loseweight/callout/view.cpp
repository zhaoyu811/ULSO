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

View::View(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      m_coordX(0),
      m_coordY(0),
      m_chart(0),
      m_tooltip(0)
{
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setRenderHint(QPainter::Antialiasing);

    this->setMouseTracking(true);
}

void View::setChart(QChart *chart)
{
    m_chart = chart;
    m_chart->createDefaultAxes();
    m_chart->setAcceptHoverEvents(true);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("时间"));
    m_chart->setAxisX(axisX);
    m_chart->axisY()->setTitleText(tr("体脂率(%)"));

    scene()->addItem(m_chart);

    connect(m_chart->series().at(0), SIGNAL(clicked(QPointF)), this, SLOT(keepCallout()));
    connect(m_chart->series().at(0), SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));
}

void View::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         m_chart->resize(event->size());

         foreach (Callout *callout, m_callouts)
             callout->updateGeometry();
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

    //1. 判断点是否是数据点
    //遍历曲线
    QLineSeries *series = (QLineSeries *)m_chart->series().at(0);
    foreach (QPointF point2, series->points()) {
        QPointF point3 = point-point2;
        if((abs(point3.rx())<=86500) && (abs(point3.ry())<=10))
        {
            if (state) {
                m_tooltip->setText(QString("时间: %1 \n体脂率: %2 ").arg(QDateTime::fromTime_t(point2.x()).toString("yyyy-MM-dd")).arg(point2.y()));
                m_tooltip->setAnchor(point2);
                m_tooltip->setZValue(11);
                m_tooltip->updateGeometry();
                m_tooltip->show();
            } else {
                m_tooltip->hide();
            }
            break;
            qDebug()<<point3;
        }
        else
        {
            qDebug()<<point<<point2;
        }
    }
}
