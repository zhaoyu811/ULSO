#ifndef COLLECTINFOWIDGET_H
#define COLLECTINFOWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QTableWidget>
#include <QSqlQueryModel>
#include <QtCharts>
#include "callout/view.h"

QT_CHARTS_USE_NAMESPACE

class CollectInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CollectInfoWidget(QWidget *parent = nullptr);
    void UpdateQueryMode();

signals:

public slots:
    void InputButtonClicked();
    void QueryPushButtonClicked();
    void UpdateDateEdit(int index);
    void UserLineEdit();
    void ChartUserLineEdit();
    void PhoneComboBoxIndexChanged(const QString &phone);
    void ChartPhoneComboBoxIndexChanged(const QString &phone);

private:
    QTabWidget *tabWidget;
    void InitInputWidget(QWidget *widget);
    void InitChartWidget(QWidget *widget);

    QCompleter *completer;
    QSqlQueryModel *queryModel;
    QTreeView *treeView;

    QLineEdit * userLineEdit;
    QComboBox * phoneComboBox;
    QComboBox * timesComboBox;
    QLineEdit * weightLineEdit;
    QLineEdit * waistLineEdit;
    QLineEdit * recipesLineEdit;
    QDateEdit * dateEdit;
    QComboBox * timeofdayComboBox;

    QRegExpValidator *validator;

    QLineEdit * chartUserNameLineEdit;
    QComboBox * chartPhoneComboBox;
    QComboBox * chartTimesComboBox;
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


    QGraphicsTextItem * bmiTextItem=NULL;
    QGraphicsTextItem * bfpTextItem=NULL;
    //QGraphicsTextItem * weightTextItem=NULL;
    //QGraphicsTextItem * waistTextItem=NULL;
    QGraphicsItemGroup * waistItemGroup=NULL;
    QGraphicsItemGroup * weightItemGroup=NULL;
    void GenerateWaistTextItem(double height, QString gender);
    void GenerateWeightTextItem(double height, QString gender, int age);
};

#endif // COLLECTINFOWIDGET_H
