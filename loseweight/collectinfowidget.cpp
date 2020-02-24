#include "collectinfowidget.h"
#include <QMessageBox>
#include <QDebug>

CollectInfoWidget::CollectInfoWidget(QWidget *parent) : QWidget(parent)
{
    //0. 初始化用户名所需的completer
    completer = new QCompleter(this);
    completer->setMaxVisibleItems(5);

    queryModel = new QSqlQueryModel(completer);
    UpdateQueryMode();
    completer->setModel(queryModel);

    treeView = new QTreeView;
    completer->setPopup(treeView);
    treeView->setRootIsDecorated(false);
    treeView->header()->hide();
    treeView->header()->setStretchLastSection(false);
    treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    //1. 新建TabWidget 和 Layout
    QGridLayout *mainLayout = new QGridLayout(this);
    tabWidget = new QTabWidget(this);
    connect(tabWidget, &QTabWidget::currentChanged, this, &UpdateDateEdit);
    mainLayout->addWidget(tabWidget);

    //2. 建立界面 采集信息的 曲线绘制的
    QWidget * inputWidget = new QWidget(tabWidget);
    InitInputWidget(inputWidget);
    tabWidget->addTab(inputWidget, tr("数据录入"));

    QWidget * chartWidget = new QWidget(tabWidget);
    InitChartWidget(chartWidget);
    tabWidget->addTab(chartWidget, tr("图表绘制"));

    this->setLayout(mainLayout);
}

void CollectInfoWidget::UpdateQueryMode()
{
    qDebug()<<"UpdateQueryMode";
    queryModel->setQuery("select distinct username from user");
    queryModel->query();
}

void CollectInfoWidget::InitInputWidget(QWidget *widget)
{
    //数据输入
    QLabel * userLabel = new QLabel(tr("用户名"), widget);
    QLabel * phoneLabel = new QLabel(tr("手机号"), widget);
    QLabel * timesLabel = new QLabel(tr("第几次"), widget);
    QLabel * weightLabel = new QLabel(tr("体重(斤)"), widget);
    QLabel * waistLabel = new QLabel(tr("腰围(cm)"), widget);
    QLabel * recipesLabel = new QLabel(tr("食谱"), widget);
    QLabel * dateLabel = new QLabel(tr("日期"), widget);
    QLabel * timeofdayLabel = new QLabel(tr("时间段"), widget);

    userLineEdit = new QLineEdit(widget);
    userLineEdit->setCompleter(completer);

    connect(userLineEdit, &QLineEdit::textEdited, this, &CollectInfoWidget::UserLineEdit);
    connect(userLineEdit, &QLineEdit::editingFinished, this, &CollectInfoWidget::UserLineEdit);

    phoneComboBox = new QComboBox(widget);
    connect(phoneComboBox, &QComboBox::currentTextChanged, this, &CollectInfoWidget::PhoneComboBoxIndexChanged);
    timesComboBox = new QComboBox(widget);
    weightLineEdit = new QLineEdit(widget);
    waistLineEdit = new QLineEdit(widget);
    recipesLineEdit = new QLineEdit(widget);
    dateEdit = new QDateEdit(widget);
    dateEdit->setDate(QDateTime::currentDateTime().date());
    timeofdayComboBox = new QComboBox(widget);
    timeofdayComboBox->addItem(tr("早(12点前)"));
    timeofdayComboBox->addItem(tr("中(16点前)"));
    timeofdayComboBox->addItem(tr("晚(20点前)"));
    //qDebug()<<QDateTime::currentDateTime().time().hour();
    int hour = QDateTime::currentDateTime().time().hour();
    if(hour<12)
    {
        timeofdayComboBox->setCurrentText(tr("早(12点前)"));
    }
    else if(hour>=12&&hour<16)
    {
        timeofdayComboBox->setCurrentText(tr("中(16点前)"));
    }
    else if(hour>=16)
    {
        timeofdayComboBox->setCurrentText(tr("晚(20点前)"));
    }

    QGridLayout * mainLayout = new QGridLayout(widget);
    mainLayout->addWidget(userLabel, 0, 0, 1, 1);
    mainLayout->addWidget(userLineEdit, 0, 1, 1, 1);
    mainLayout->addWidget(phoneLabel, 1, 0, 1, 1);
    mainLayout->addWidget(phoneComboBox, 1, 1, 1, 1);
    mainLayout->addWidget(timesLabel, 2, 0, 1, 1);
    mainLayout->addWidget(timesComboBox, 2, 1, 1, 1);
    mainLayout->addWidget(weightLabel, 3, 0, 1, 1);
    mainLayout->addWidget(weightLineEdit, 3, 1, 1, 1);
    mainLayout->addWidget(waistLabel, 4, 0, 1, 1);
    mainLayout->addWidget(waistLineEdit, 4, 1, 1, 1);
    mainLayout->addWidget(recipesLabel, 5, 0, 1, 1);
    mainLayout->addWidget(recipesLineEdit, 5, 1, 1, 1);
    mainLayout->addWidget(dateLabel, 6, 0, 1, 1);
    mainLayout->addWidget(dateEdit, 6, 1, 1, 1);
    mainLayout->addWidget(timeofdayLabel, 7, 0, 1, 1);
    mainLayout->addWidget(timeofdayComboBox, 7, 1, 1, 1);

    //mainLayout->setColumnStretch(0, 0);
    //mainLayout->setColumnStretch(1, 0);
    mainLayout->setColumnStretch(2, 1);

    QPushButton *pushButton = new QPushButton(widget);
    pushButton->setText(tr("录入"));
    connect(pushButton, &QPushButton::clicked, this, &CollectInfoWidget::InputButtonClicked);
    mainLayout->addWidget(pushButton, 8, 1, 1, 1, Qt::AlignCenter);

    mainLayout->setRowStretch(9, 1);

    mainLayout->setAlignment(widget, Qt::AlignLeft|Qt::AlignTop);
    widget->setLayout(mainLayout);
}

void CollectInfoWidget::UpdateDateEdit(int index)
{
    if(index == 0)
    {
        dateEdit->setDate(QDateTime::currentDateTime().date());

        int hour = QDateTime::currentDateTime().time().hour();
        if(hour<12)
        {
            timeofdayComboBox->setCurrentText(tr("早(12点前)"));
        }
        else if(hour>=12&&hour<16)
        {
            timeofdayComboBox->setCurrentText(tr("中(16点前)"));
        }
        else if(hour>=16)
        {
            timeofdayComboBox->setCurrentText(tr("晚(20点前)"));
        }
    }
}

void CollectInfoWidget::UserLineEdit()
{
    QString cmd;
    QSqlQuery query;

    if(userLineEdit->text().isEmpty())
    {
        phoneComboBox->clear();
        timesComboBox->clear();
        return;
    }

    cmd = QString("select distinct phonenumber from user where username like '%1%'").arg(userLineEdit->text());
    if(query.exec(cmd))
    {
        //将电话号码插入
        phoneComboBox->clear();
        while(query.next())
        {
            phoneComboBox->addItem(query.value(0).toString());
        }
        //查询当前电话的次数
        cmd = QString("select distinct times from user where username like '%1%' and phonenumber='%2'")
                .arg(userLineEdit->text())
                .arg(phoneComboBox->currentText());
        if(query.exec(cmd))
        {
            timesComboBox->clear();
            while(query.next())
            {
                timesComboBox->addItem(query.value(0).toString());
            }
        }
        else
        {
            QMessageBox::critical(this, tr("查询数据库失败"), cmd);
        }
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), cmd);
    }
}

void CollectInfoWidget::PhoneComboBoxIndexChanged(const QString &phone)
{
    QString cmd;
    QSqlQuery query;
    //查询当前电话的次数
    cmd = QString("select distinct times from user where username like '%1%' and phonenumber='%2'")
            .arg(userLineEdit->text())
            .arg(phoneComboBox->currentText());
    if(query.exec(cmd))
    {
        timesComboBox->clear();
        while(query.next())
        {
            timesComboBox->addItem(query.value(0).toString());
        }
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), cmd);
    }
}

void CollectInfoWidget::ChartPhoneComboBoxIndexChanged(const QString &phone)
{
    QString cmd;
    QSqlQuery query;
    //查询当前电话的次数
    cmd = QString("select distinct times from user where username like '%1%' and phonenumber='%2'")
            .arg(chartUserNameLineEdit->text())
            .arg(chartPhoneComboBox->currentText());
    if(query.exec(cmd))
    {
        chartTimesComboBox->clear();
        while(query.next())
        {
            chartTimesComboBox->addItem(query.value(0).toString());
        }
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), cmd);
    }
}

void CollectInfoWidget::InputButtonClicked()
{
    qDebug()<<"InputButtonClicked";
    QString cmd;
    QSqlQuery query;

    //1. 根据用户名和电话号码定位到档案号
    cmd = QString("select id from user where username = '%1' and phonenumber = '%2' and times = '%3'")
            .arg(userLineEdit->text())
            .arg(phoneComboBox->currentText())
            .arg(timesComboBox->currentText());
    if(query.exec(cmd))
    {
        if(query.next())
        {
            cmd = QString("insert into archive%1(weight, waist, recipes, datetime, timeofday)values('%2', '%3', '%4', '%5', '%6')")
                    .arg(query.value(0).toInt())
                    .arg(weightLineEdit->text())
                    .arg(waistLineEdit->text())
                    .arg(recipesLineEdit->text())
                    .arg(dateEdit->date().toString("yyyy-MM-dd"))
                    .arg(timeofdayComboBox->currentText());
            if(query.exec(cmd))
            {
                QMessageBox::information(this, tr("输入成功"), tr("输入成功"));
                //heightLineEdit->clear();
                weightLineEdit->clear();
                waistLineEdit->clear();
                recipesLineEdit->clear();
                //timeLineEdit->clear();
            }
            else
            {
                QMessageBox::information(this, tr("插入数据库失败"), cmd);
            }
        }
        else
        {
            QMessageBox::information(this, tr("定位不到用户信息"), tr("定位不到用户信息，检查用户名和手机号码"));
        }
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), cmd);
    }
}

void CollectInfoWidget::InitChartWidget(QWidget *widget)
{
    QFont font;
    font.setPointSize(14);//字体大小
    this->setFont(font);

    QGridLayout *mainLayout = new QGridLayout(widget);
    QGridLayout *archiveLayout = new QGridLayout();

    QLabel *chartUserNameLabel = new QLabel(tr("用户名"), widget);
    chartUserNameLabel ->setFont(font);//其他控件一样

    QLabel *chartPhoneLabel = new QLabel(tr("手机号码"), widget);
    QLabel *chartTimesLabel = new QLabel(tr("第几次"), widget);

    chartUserNameLineEdit = new QLineEdit(widget);
    chartUserNameLineEdit->setCompleter(completer);
    connect(chartUserNameLineEdit, &QLineEdit::textEdited, this, &CollectInfoWidget::ChartUserLineEdit);
    connect(chartUserNameLineEdit, &QLineEdit::editingFinished, this, &CollectInfoWidget::ChartUserLineEdit);

    chartPhoneComboBox = new QComboBox(widget);
    chartPhoneComboBox->setMinimumWidth(150);
    connect(chartPhoneComboBox, &QComboBox::currentTextChanged, this, &CollectInfoWidget::ChartPhoneComboBoxIndexChanged);
    chartTimesComboBox = new QComboBox(widget);

    QPushButton * queryButton = new QPushButton(tr("查询"), widget);
    connect(queryButton, &QPushButton::clicked, this, &CollectInfoWidget::QueryPushButtonClicked);

    archiveLayout->addWidget(chartUserNameLabel, 0, 0, 1, 1);
    archiveLayout->addWidget(chartUserNameLineEdit, 0, 1, 1, 1);
    archiveLayout->addWidget(chartPhoneLabel, 0, 2, 1, 1);
    archiveLayout->addWidget(chartPhoneComboBox, 0, 3, 1, 1);
    archiveLayout->addWidget(chartTimesLabel, 0, 4, 1, 1);
    archiveLayout->addWidget(chartTimesComboBox, 0, 5, 1, 1);
    archiveLayout->addWidget(queryButton, 0, 6, 1, 1);
    archiveLayout->setColumnStretch(7, 1);

    //体重曲线
    QFont font2;
    font2.setPointSize(14);//字体大小
    font2.setBold(true);

    weightChartView = new View(widget);
    weightChartView->text=tr("体重(斤)");
    weightChart = new QChart();
    weightChart->setTitle(tr("体重曲线"));
    weightChart->setTitleFont(font2);
    //weightChartView->setChart(weightChart);

    //腰围曲线
    waistChartView = new View(widget);
    waistChartView->text=tr("腰围(cm)");
    waistChart = new QChart();
    waistChart->setTitle(tr("腰围曲线"));
    waistChart->setTitleFont(font2);
    //waistChartView->setChart(waistChart);

    //体脂率曲线
    bodyFatRateChartView = new View(widget);
    bodyFatRateChartView->text=tr("体脂率(%)");
    bodyFatRateChart = new QChart();
    bodyFatRateChart->setTitle(tr("体脂率曲线"));
    bodyFatRateChart->setTitleFont(font2);
    //bodyFatRateChartView->setChart(bodyFatRateChart);

    //健康曲线
    healthIndexChartView = new View(widget);
    healthIndexChartView->text=tr("BMI(%)");
    healthIndexChart = new QChart();
    healthIndexChart->setTitle(tr("健康曲线"));
    healthIndexChart->setTitleFont(font2);
    //healthIndexChartView->setChart(healthIndexChart);

#if 0
    bodyFatRateView2 = new View(widget);
    bodyFatRateChart2 = new QChart();
    bodyFatRateChart2->setTitle("test");
    //bodyFatRateView2->setChart(bodyFatRateChart2);

    QLineSeries *series = new QLineSeries;
    series->append(QDateTime::fromSecsSinceEpoch(1578132063).toMSecsSinceEpoch(), 13);
    series->append(QDateTime::fromSecsSinceEpoch(1578218463).toMSecsSinceEpoch(), 15);
    series->append(QDateTime::fromSecsSinceEpoch(1578304863).toMSecsSinceEpoch(), 14.5);
    series->append(QDateTime::fromSecsSinceEpoch(1578391263).toMSecsSinceEpoch(), 11);
    series->append(QDateTime::fromSecsSinceEpoch(1578477663).toMSecsSinceEpoch(), 12);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+1*86400).toMSecsSinceEpoch(), 2);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+2*86400).toMSecsSinceEpoch(), 3);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+3*86400).toMSecsSinceEpoch(), 5);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+4*86400).toMSecsSinceEpoch(), 1);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+5*86400).toMSecsSinceEpoch(), 2);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+6*86400).toMSecsSinceEpoch(), 7);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+7*86400).toMSecsSinceEpoch(), 9);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+8*86400).toMSecsSinceEpoch(), 3);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+9*86400).toMSecsSinceEpoch(), 2);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+10*86400).toMSecsSinceEpoch(), 4);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+11*86400).toMSecsSinceEpoch(), 9);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+12*86400).toMSecsSinceEpoch(), 2);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+13*86400).toMSecsSinceEpoch(), 10);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+14*86400).toMSecsSinceEpoch(), 1);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+15*86400).toMSecsSinceEpoch(), 2);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+16*86400).toMSecsSinceEpoch(), 7);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+17*86400).toMSecsSinceEpoch(), 9);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+18*86400).toMSecsSinceEpoch(), 10);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+19*86400).toMSecsSinceEpoch(), 1);
//    series->append(QDateTime::fromSecsSinceEpoch(1578477663+20*86400).toMSecsSinceEpoch(), 2);
    bodyFatRateChart2->addSeries(series);
#if 0
    QScatterSeries *series2 = new QScatterSeries;
    series2->append(QDateTime::fromSecsSinceEpoch(1578132063).toMSecsSinceEpoch(), 3);
    series2->append(QDateTime::fromSecsSinceEpoch(1578218463).toMSecsSinceEpoch(), 5);
    series2->append(QDateTime::fromSecsSinceEpoch(1578304863).toMSecsSinceEpoch(), 4.5);
    series2->append(QDateTime::fromSecsSinceEpoch(1578391263).toMSecsSinceEpoch(), 1);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663).toMSecsSinceEpoch(), 2);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+1*86400).toMSecsSinceEpoch(), 2);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+2*86400).toMSecsSinceEpoch(), 3);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+3*86400).toMSecsSinceEpoch(), 5);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+4*86400).toMSecsSinceEpoch(), 1);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+5*86400).toMSecsSinceEpoch(), 2);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+6*86400).toMSecsSinceEpoch(), 7);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+7*86400).toMSecsSinceEpoch(), 9);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+8*86400).toMSecsSinceEpoch(), 3);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+9*86400).toMSecsSinceEpoch(), 2);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+10*86400).toMSecsSinceEpoch(), 4);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+11*86400).toMSecsSinceEpoch(), 9);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+12*86400).toMSecsSinceEpoch(), 2);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+13*86400).toMSecsSinceEpoch(), 10);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+14*86400).toMSecsSinceEpoch(), 1);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+15*86400).toMSecsSinceEpoch(), 2);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+16*86400).toMSecsSinceEpoch(), 7);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+17*86400).toMSecsSinceEpoch(), 9);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+18*86400).toMSecsSinceEpoch(), 10);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+19*86400).toMSecsSinceEpoch(), 1);
    series2->append(QDateTime::fromSecsSinceEpoch(1578477663+20*86400).toMSecsSinceEpoch(), 2);
    bodyFatRateChart2->addSeries(series2);
#endif
    bodyFatRateChart2->createDefaultAxes();

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("时间"));
    bodyFatRateChart2->setAxisX(axisX, series);
    bodyFatRateChart2->axisY()->setTitleText(tr("体脂率(%)"));
    bodyFatRateView2->setChart(bodyFatRateChart2);
#endif
    mainLayout->addLayout(archiveLayout, 0, 0, 1, 2);
    mainLayout->addWidget(weightChartView, 1, 0, 1, 1);
    mainLayout->addWidget(waistChartView, 1, 1, 1, 1);
    mainLayout->addWidget(bodyFatRateChartView, 2, 1, 1, 1);
    mainLayout->addWidget(healthIndexChartView, 2, 0, 1, 1);

    widget->setLayout(mainLayout);
}

void CollectInfoWidget::ChartUserLineEdit()
{
    QString cmd;
    QSqlQuery query;

    if(chartUserNameLineEdit->text().isEmpty())
    {
        chartPhoneComboBox->clear();
        chartTimesComboBox->clear();
        return;
    }

    cmd = QString("select distinct phonenumber from user where username like '%1%'").arg(chartUserNameLineEdit->text());
    if(query.exec(cmd))
    {
        //将电话号码插入
        chartPhoneComboBox->clear();
        while(query.next())
        {
            chartPhoneComboBox->addItem(query.value(0).toString());
        }
        //查询当前电话的次数
        cmd = QString("select distinct times from user where username like '%1%' and phonenumber='%2'")
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText());
        if(query.exec(cmd))
        {
            chartTimesComboBox->clear();
            while(query.next())
            {
                chartTimesComboBox->addItem(query.value(0).toString());
            }
        }
        else
        {
            QMessageBox::critical(this, tr("查询数据库失败"), cmd);
        }
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), cmd);
    }
}


void CollectInfoWidget::QueryPushButtonClicked()
{
    qDebug()<<"QueryPushButtonClicked";
    QString cmd;
    QSqlQuery query;

    QString gender;
    double height;
    int age;

    //1. 根据用户名和电话号码定位到档案号
#if 1
    cmd = QString("select id,gender,height,dateofbirth from user where username='%1' and phonenumber='%2' and times='%3'")
            .arg(chartUserNameLineEdit->text())
            .arg(chartPhoneComboBox->currentText())
            .arg(chartTimesComboBox->currentText());
#endif
    //cmd = QString("select id,gender,height,dateofbirth from user where username='赵瑜' and phonenumber='18404905403' and times='1'");
    if(query.exec(cmd))
    {
        if(query.next())
        {
            gender = query.value(1).toString();
            height = query.value(2).toDouble();
            age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
            qDebug()<<age;
            //2. 根据档案号查询到所有数据
            cmd = QString("select weight, waist, datetime from archive%1").arg(query.value(0).toInt());
            if(query.exec(cmd))
            {
                weightSeries = new QLineSeries();
                waistSeries = new QLineSeries();
                bodyFatRateSeries = new QLineSeries();
                healthIndexSeries = new QLineSeries();

                weightScatterSeries = new QScatterSeries();
                waistScatterSeries = new QScatterSeries();
                bodyFatRateScatterSeries = new QScatterSeries();
                healthIndexScatterSeries = new QScatterSeries();

                if(gender==tr("女"))
                {
                    while(query.next())
                    {
                        double weight, waist;
                        weight = query.value(0).toDouble()/2;
                        waist = query.value(1).toDouble();
                        double bmi = weight/height/height*10000;
                        double bfp = ((weight*0.74-(waist*0.082+34.89))/weight*100)/2+(1.2*bmi+0.23*age-5.4)/2;
                        weightSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), query.value(0).toDouble());
                        waistSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), query.value(1).toDouble());
                        healthIndexSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), bmi);
                        bodyFatRateSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), bfp);
                        weightScatterSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), query.value(0).toDouble());
                        waistScatterSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), query.value(1).toDouble());
                        bodyFatRateScatterSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), bfp);
                        healthIndexScatterSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), bmi);
                    }
                }
                else if(gender==tr("男"))
                {
                    while(query.next())
                    {
                        double weight, waist;
                        weight = query.value(0).toDouble()/2;
                        waist = query.value(1).toDouble();
                        double bmi = weight/height/height*10000;
                        double bfp = ((weight*0.74-(waist*0.082+34.89))/weight*100)/2+(1.2*bmi+0.23*age-5.4-10.8)/2;
                        weightSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), query.value(0).toDouble());
                        waistSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), query.value(1).toDouble());
                        healthIndexSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), bmi);
                        bodyFatRateSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), bfp);
                        weightScatterSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), query.value(0).toDouble());
                        waistScatterSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), query.value(1).toDouble());
                        bodyFatRateScatterSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), bfp);
                        healthIndexScatterSeries->append(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch(), bmi);
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("数据错误"), tr("性别为：").arg(gender));
                }

                QFont font;
                font.setPointSize(14);//字体大小
                font.setBold(true);

                if(weightChartView->chart()!=NULL)
                    weightChartView->chart()->removeAllSeries();
                weightChart->addSeries(weightSeries);
                weightChart->addSeries(weightScatterSeries);
                weightChart->createDefaultAxes();
                QDateTimeAxis *axisX = new QDateTimeAxis;
                axisX->setTickCount(6);
                axisX->setFormat("yyyy-MM-dd");
                axisX->setTitleText(tr("时间"));
                axisX->setTitleFont(font);
                weightChart->setAxisX(axisX, weightSeries);

                QValueAxis *axisY = new QValueAxis;
                axisY->setTitleText(tr("体重(斤)"));
                axisY->setTitleFont(font);
                axisY->setTickCount(8);
                weightChart->setAxisY(axisY, weightSeries);

                weightChartView->setChart(weightChart);
                weightChart->zoom(0.9);

                if(waistChartView->chart()!=NULL)
                    waistChartView->chart()->removeAllSeries();
                waistChart->addSeries(waistSeries);
                waistChart->addSeries(waistScatterSeries);
                waistChart->createDefaultAxes();
                axisX = new QDateTimeAxis;
                axisX->setFormat("yyyy-MM-dd");
                axisX->setTitleText(tr("时间"));
                axisX->setTitleFont(font);
                axisX->setTickCount(6);
                waistChart->setAxisX(axisX, waistSeries);

                axisY = new QValueAxis;
                axisY->setTitleText(tr("腰围(cm)"));
                axisY->setTitleFont(font);
                axisY->setTickCount(8);
                waistChart->setAxisY(axisY, waistSeries);

                waistChartView->setChart(waistChart);
                waistChart->zoom(0.9);

                if(healthIndexChartView->chart()!=NULL)
                    healthIndexChartView->chart()->removeAllSeries();
                healthIndexChart->addSeries(healthIndexSeries);
                healthIndexChart->addSeries(healthIndexScatterSeries);
                healthIndexChart->createDefaultAxes();
                axisX = new QDateTimeAxis;
                axisX->setFormat("yyyy-MM-dd");
                axisX->setTitleText(tr("时间"));
                axisX->setTitleFont(font);
                axisX->setTickCount(6);
                healthIndexChart->setAxisX(axisX, healthIndexSeries);

                axisY = new QValueAxis;
                axisY->setTitleText(tr("BMI(%)"));
                axisY->setTitleFont(font);
                axisY->setTickCount(8);
                healthIndexChart->setAxisY(axisY, healthIndexSeries);

                healthIndexChartView->setChart(healthIndexChart);
                healthIndexChart->zoom(0.9);

                if(bodyFatRateChartView->chart()!=NULL)
                    bodyFatRateChartView->chart()->removeAllSeries();
                bodyFatRateChart->addSeries(bodyFatRateSeries);
                bodyFatRateChart->addSeries(bodyFatRateScatterSeries);
                bodyFatRateChart->createDefaultAxes();
                axisX = new QDateTimeAxis;
                axisX->setFormat("yyyy-MM-dd");
                axisX->setTitleText(tr("时间"));
                axisX->setTitleFont(font);
                axisX->setTickCount(6);
                bodyFatRateChart->setAxisX(axisX, bodyFatRateSeries);

                axisY = new QValueAxis;
                axisY->setTitleText(tr("体脂率(%)"));
                axisY->setTitleFont(font);
                axisY->setTickCount(8);
                bodyFatRateChart->setAxisY(axisY, bodyFatRateSeries);

                bodyFatRateChartView->setChart(bodyFatRateChart);
                bodyFatRateChart->zoom(0.9);
            }
            else
            {
                QMessageBox::critical(this, tr("查询数据库失败"), cmd);
            }
        }
        else
        {
            QMessageBox::information(this, tr("查询不到档案"), tr("查询不到档案，检查用户名和手机号"));
        }
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), cmd);
    }
}
