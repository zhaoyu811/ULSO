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

    cmd = QString("select id,gender,height,dateofbirth from user where username = '%1' and phonenumber = '%2' and times = '%3'")
            .arg(userLineEdit->text())
            .arg(phoneComboBox->currentText())
            .arg(timesComboBox->currentText());
    if(query.exec(cmd))
    {
        if(query.next())
        {
            //找到了对应的档案，计算出各个数据
            double weight = weightLineEdit->text().toDouble();  //单位为 斤
            double waist = waistLineEdit->text().toDouble();
            double bmi, bfp;
            double height = query.value(2).toDouble();
            int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
            if(query.value(1)==tr("男"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4-10.8)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
            }
            else if(query.value(1)==tr("女"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
            }

            cmd = QString("insert into archive%1(weight, waist, recipes, datetime, timeofday, bmi, bfp)values('%2', '%3', '%4', '%5', '%6', '%7', '%8')")
                    .arg(query.value(0).toInt())
                    .arg(weight)
                    .arg(waist)
                    .arg(recipesLineEdit->text())
                    .arg(dateEdit->date().toString("yyyy-MM-dd"))
                    .arg(timeofdayComboBox->currentText())
                    .arg(QString::number(bmi, 'f', 2))
                    .arg(QString::number(bfp, 'f', 2));
            if(query.exec(cmd))
            {
                QMessageBox::information(this, tr("输入成功"), tr("输入成功"));
                //heightLineEdit->clear();
                weightLineEdit->clear();
                waistLineEdit->clear();
                recipesLineEdit->clear();
                //timeLineEdit->clear();
                cmd = QString("update user set datacount=datacount+1 where username = '%1' and phonenumber = '%2' and times = '%3'")
                        .arg(userLineEdit->text())
                        .arg(phoneComboBox->currentText())
                        .arg(timesComboBox->currentText());
                if(query.exec(cmd)==false)
                {
                    QMessageBox::information(this, tr("SQL错误"), cmd);
                }
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

    QScrollArea * scrollArea = new QScrollArea(widget);
    QWidget * widget2 = new QWidget(scrollArea);
    QVBoxLayout * vBoxLayout = new QVBoxLayout(widget2);
    //体重曲线
    QFont font2;
    font2.setPointSize(20);//字体大小
    font2.setBold(true);

    weightChartView = new View(widget2);
    weightChartView->text=tr("体重(斤)");
    weightChart = new QChart();
    weightChart->setTitle(tr("体重曲线"));
    weightChart->setTitleFont(font2);
    //weightChartView->setChart(weightChart);
    weightChartView->setFixedSize(1660, 900);

    //腰围曲线
    waistChartView = new View(widget2);
    waistChartView->text=tr("腰围(cm)");
    waistChart = new QChart();
    waistChart->setTitle(tr("腰围曲线"));
    waistChart->setTitleFont(font2);
    //waistChartView->setChart(waistChart);
    waistChartView->setFixedSize(1660, 900);


    //体脂率曲线
    bodyFatRateChartView = new View(widget2);
    bodyFatRateChartView->text=tr("体脂率(%)");
    bodyFatRateChart = new QChart();
    bodyFatRateChart->setTitle(tr("体脂率曲线"));
    bodyFatRateChart->setTitleFont(font2);
    //bodyFatRateChartView->setChart(bodyFatRateChart);
    bodyFatRateChartView->setFixedSize(1660, 900);

    //健康曲线
    healthIndexChartView = new View(widget2);
    healthIndexChartView->text=tr("BMI(%)");
    healthIndexChart = new QChart();
    healthIndexChart->setTitle(tr("身体质量指数曲线"));
    healthIndexChart->setTitleFont(font2);
    //healthIndexChartView->setChart(healthIndexChart);
    healthIndexChartView->setFixedSize(1660, 900);

    vBoxLayout->addWidget(weightChartView);
    vBoxLayout->addWidget(waistChartView);
    vBoxLayout->addWidget(bodyFatRateChartView);
    vBoxLayout->addWidget(healthIndexChartView);

    widget2->setLayout(vBoxLayout);
    scrollArea->setWidget(widget2);

    mainLayout->addLayout(archiveLayout, 0, 0, 1, 2);
    mainLayout->addWidget(scrollArea);
    //mainLayout->addWidget(weightChartView, 1, 0, 1, 1);
    //mainLayout->addWidget(waistChartView, 1, 1, 1, 1);
    //mainLayout->addWidget(bodyFatRateChartView, 2, 1, 1, 1);
    //mainLayout->addWidget(healthIndexChartView, 2, 0, 1, 1);

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

    //1. 根据用户名和电话号码定位到档案号
    cmd = QString("select id,gender,dateofbirth,height,openholetime from user where username='%1' and phonenumber='%2' and times='%3'")
            .arg(chartUserNameLineEdit->text())
            .arg(chartPhoneComboBox->currentText())
            .arg(chartTimesComboBox->currentText());
    if(query.exec(cmd))
    {
        if(query.next())
        {
            QString gender = query.value(1).toString();
            int age = QDateTime::fromString(query.value(4).toString(), "yyyy-MM-dd").toString("yyyy").toInt()-QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
            double height = query.value(3).toDouble();
            //2. 根据档案号查询到所有数据
            cmd = QString("select weight, waist, datetime, bmi, bfp from archive%1").arg(query.value(0).toInt());
            if(query.exec(cmd))
            {
                weightSeries = new QLineSeries();
                waistSeries = new QLineSeries();
                bodyFatRateSeries = new QLineSeries();
                healthIndexSeries = new QLineSeries();

                weightScatterSeries = new QScatterSeries();
                weightScatterSeries->setMarkerSize(10);
                waistScatterSeries = new QScatterSeries();
                waistScatterSeries->setMarkerSize(10);
                bodyFatRateScatterSeries = new QScatterSeries();
                bodyFatRateScatterSeries->setMarkerSize(10);
                healthIndexScatterSeries = new QScatterSeries();
                healthIndexScatterSeries->setMarkerSize(10);

                if(bmiTextItem!=NULL)
                    healthIndexChartView->scene()->removeItem(bmiTextItem);
                if(bfpTextItem!=NULL)
                    bodyFatRateChartView->scene()->removeItem(bfpTextItem);

                while(query.next())
                {
                    double weight, waist;
                    weight = query.value(0).toDouble();
                    waist = query.value(1).toDouble();
                    double bmi, bfp;

                    if(gender==tr("男"))
                    {
                        bmi = weight/2/height/height*10000;
                        //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4-10.8)*0.4;
                        bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
                    }
                    else if(gender==tr("女"))
                    {
                        bmi = weight/2/height/height*10000;
                        //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4)*0.4;
                        bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
                    }

                    //bmi = query.value(3).toDouble();
                    //bfp = query.value(4).toDouble();
                    qint64 date = QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toMSecsSinceEpoch();
                    weightSeries->append(date, weight);
                    weightScatterSeries->append(date, weight);
                    waistSeries->append(date, waist);
                    waistScatterSeries->append(date, waist);
                    bodyFatRateSeries->append(date, bfp);
                    bodyFatRateScatterSeries->append(date, bfp);
                    healthIndexSeries->append(date, bmi);
                    healthIndexScatterSeries->append(date, bmi);
                }

                QFont font;
                font.setPointSize(18);//字体大小
                font.setBold(true);

                if(weightChartView->chart()!=NULL)
                {
                    weightChartView->chart()->removeAllSeries();
                }
                weightChart->addSeries(weightSeries);
                weightChart->addSeries(weightScatterSeries);
                weightChart->createDefaultAxes();
                QDateTimeAxis *axisX = new QDateTimeAxis;
                axisX->setTickCount(12);
                axisX->setFormat("yyyy-MM-dd");
                axisX->setTitleText(tr("时间"));
                axisX->setTitleFont(font);
                weightChart->setAxisX(axisX, weightSeries);

                QValueAxis *axisY = new QValueAxis;
                axisY->setTitleText(tr("体重(斤)"));
                axisY->setTitleFont(font);
                axisY->setTickCount(16);
                weightChart->setAxisY(axisY, weightSeries);

                weightChartView->setChart(weightChart);
                weightChart->zoom(0.9);

                if(waistChartView->chart()!=NULL)
                {
                    waistChartView->chart()->removeAllSeries();
                }
                waistChart->addSeries(waistSeries);
                waistChart->addSeries(waistScatterSeries);
                waistChart->createDefaultAxes();
                axisX = new QDateTimeAxis;
                axisX->setFormat("yyyy-MM-dd");
                axisX->setTitleText(tr("时间"));
                axisX->setTitleFont(font);
                axisX->setTickCount(12);
                waistChart->setAxisX(axisX, waistSeries);

                axisY = new QValueAxis;
                axisY->setTitleText(tr("腰围(cm)"));
                axisY->setTitleFont(font);
                axisY->setTickCount(16);
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
                axisX->setTickCount(12);
                healthIndexChart->setAxisX(axisX, healthIndexSeries);

                axisY = new QValueAxis;
                axisY->setTitleText(tr("BMI(%)"));
                axisY->setTitleFont(font);
                axisY->setTickCount(16);
                healthIndexChart->setAxisY(axisY, healthIndexSeries);

                healthIndexChartView->setChart(healthIndexChart);
                healthIndexChart->zoom(0.9);

                bmiTextItem = new QGraphicsTextItem;
                bmiTextItem->setPos(900,120);
                QTextDocument * textDocument = new QTextDocument(bmiTextItem);
                QFile file;
                file.setFileName(tr(":/resource/html/bmi.html"));
                file.open(QIODevice::ReadOnly);
                textDocument->setHtml(file.readAll());
                file.close();
                bmiTextItem->setDocument(textDocument);
                bmiTextItem->setZValue(1);
                bmiTextItem->setFlag(QGraphicsItem::ItemIsMovable, true);
                QFont font3;
                font3.setPointSize(14);
                bmiTextItem->setFont(font3);
                healthIndexChartView->scene()->addItem(bmiTextItem);

                if(bodyFatRateChartView->chart()!=NULL)
                    bodyFatRateChartView->chart()->removeAllSeries();
                bodyFatRateChart->addSeries(bodyFatRateSeries);
                bodyFatRateChart->addSeries(bodyFatRateScatterSeries);
                bodyFatRateChart->createDefaultAxes();
                axisX = new QDateTimeAxis;
                axisX->setFormat("yyyy-MM-dd");
                axisX->setTitleText(tr("时间"));
                axisX->setTitleFont(font);
                axisX->setTickCount(12);
                bodyFatRateChart->setAxisX(axisX, bodyFatRateSeries);

                axisY = new QValueAxis;
                axisY->setTitleText(tr("体脂率(%)"));
                axisY->setTitleFont(font);
                axisY->setTickCount(16);
                bodyFatRateChart->setAxisY(axisY, bodyFatRateSeries);

                bodyFatRateChartView->setChart(bodyFatRateChart);
                bodyFatRateChart->zoom(0.9);

                bfpTextItem = new QGraphicsTextItem;
                bfpTextItem->setPos(900,120);
                textDocument = new QTextDocument(bfpTextItem);
                QFile file2;
                if(gender == tr("男"))
                    file2.setFileName(tr(":/resource/html/male.html"));
                else
                    file2.setFileName(tr(":/resource/html/female.html"));
                file2.open(QIODevice::ReadOnly);
                textDocument->setHtml(file2.readAll());
                file2.close();
                bfpTextItem->setDocument(textDocument);
                bfpTextItem->setZValue(1);
                bfpTextItem->setFlag(QGraphicsItem::ItemIsMovable, true);
                bfpTextItem->setFont(font3);
                bodyFatRateChartView->scene()->addItem(bfpTextItem);
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
