#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "fuctionwidget.h"
#include "tool_button.h"
#include "userwidget.h"
#include "collectinfowidget.h"
#include "forecastwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void SlidePage(int index);
private:
    QStackedWidget *stackedWidget;
    FuctionWidget *fuctionWidget;
    ToolButton *toolButton;
    CollectInfoWidget * collectinfoWidget;
    ForecastWidget * forecastWidget;
};

#endif // MAINWINDOW_H
