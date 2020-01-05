#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout * mainLayout = new QGridLayout(this);
    //新建停靠窗口
    stackedWidget = new QStackedWidget(this);
    //新建用户窗口
    UserWidget * userWidget = new UserWidget(stackedWidget);
    userWidget->setFixedHeight(800);
    stackedWidget->addWidget(userWidget);
    //新建采集信息窗口
    collectinfoWidget = new CollectInfoWidget(stackedWidget);
    stackedWidget->addWidget(collectinfoWidget);
    //新建数据预测窗口
    QWidget * forecastWidget = new QWidget(stackedWidget);
    QLabel * forecastLabel = new QLabel("预测信息", forecastWidget);
    QGridLayout *layout3 = new QGridLayout(forecastWidget);
    forecastWidget->setLayout(layout3);
    forecastWidget->layout()->addWidget(forecastLabel);
    stackedWidget->addWidget(forecastWidget);
    stackedWidget->setFixedWidth(1200);

    //界面刷白
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(Qt::white));
    stackedWidget->setPalette(palette);
    this->setPalette(palette);

    //创建3个按键
    fuctionWidget = new FuctionWidget(this);
    mainLayout->addWidget(fuctionWidget, 0, 0, 1, 1, Qt::AlignTop|Qt::AlignLeft);
    mainLayout->addWidget(stackedWidget, 0, 1, 1, 1, Qt::AlignVCenter|Qt::AlignTop);
    mainLayout->setColumnStretch(1, 0);
    mainLayout->setRowStretch(2, 0);
    connect(fuctionWidget, &FuctionWidget::ToolButtonClicked, this, &MainWindow::SlidePage);

    this->setLayout(mainLayout);
}

MainWindow::~MainWindow()
{

}

void MainWindow::SlidePage(int index)
{
    qDebug()<<"显示的页码为："<<index;
    stackedWidget->setCurrentIndex(index-1);
    if(index==2)
        collectinfoWidget->UpdateQueryMode();
}