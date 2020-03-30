#ifndef FORECASTWIDGET_H
#define FORECASTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QtCharts>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTextBrowser>
#include "callout/view.h"

QT_CHARTS_USE_NAMESPACE

class ForecastWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ForecastWidget(QWidget *parent = nullptr);
    void UpdateQueryMode();

public slots:
    void ChartUserLineEdit();
    void ChartPhoneComboBoxIndexChanged(const QString &phone);
    void QueryPushButtonClicked();
    void ChartTimesComboBoxIndexChanged();

private:
    void InitChartWidget(QWidget *widget);

    QCompleter *completer;
    QSqlQueryModel *queryModel;
    QTreeView *treeView;
    QLineEdit * chartUserNameLineEdit;
    QComboBox * chartPhoneComboBox;
    QComboBox * chartTimesComboBox;
    QComboBox * genderCombox;
    //QLineEdit * ageLineEdit;
    QDateEdit * dateEdit;
    QLineEdit * heightLineEdit;
    QLineEdit * weightLineEdit;
    QLineEdit * waistLineEdit;
    QLineEdit * openHoleWeightLineEdit;
    View *weightChartView;
    View *waistChartView;
    View *bodyFatRateChartView;
    View *healthIndexChartView;
    QChart *weightChart;
    QChart *waistChart;
    QChart *bodyFatRateChart;
    QChart *healthIndexChart;

    QLineSeries *weightSeries;
    QLineSeries *waistSeries;
    QLineSeries *bodyFatRateSeries;
    QLineSeries *healthIndexSeries;

    QScatterSeries *weightScatterSeries;
    QScatterSeries *waistScatterSeries;
    QScatterSeries *bodyFatRateScatterSeries;
    QScatterSeries *healthIndexScatterSeries;

    QTextBrowser *textBrowser;

    void ForecastWeight();
    void ForecastWaist();
    void ForecastBMI();
    void ForecastBFP();

    QVector<double> weightForecastData;
    QVector<double> waistForecastData;
    QVector<double> bmiForecastData;
    QVector<double> bfpForecastData;

    void ShowUserInfo();

    QGraphicsTextItem * bmiTextItem=NULL;
    QGraphicsTextItem * bfpTextItem=NULL;
    QGraphicsItemGroup * waistItemGroup=NULL;
    QGraphicsItemGroup * weightItemGroup=NULL;

    void GenerateWaistTextItem(double height, QString gender);
    void GenerateWeightTextItem(double height, QString gender, int age);
};

#endif // FORECASTWIDGET_H
