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

    QRegExp rx("^[1-9][0-9]{1,2}[\\.]{0,1}[0-9]{0,2}$");
    validator = new QRegExpValidator(rx, this);
    weightLineEdit = new QLineEdit(widget);
    weightLineEdit->setValidator(validator);
    waistLineEdit = new QLineEdit(widget);
    waistLineEdit->setValidator(validator);

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
    int pos;
    //确认腰围和体重
    QString weight = weightLineEdit->text();
    if(validator->validate(weight, pos) != QValidator::Acceptable)
    {
        QMessageBox::critical(this, tr("体重输入错误"), tr("体重输入错误"));
        weightLineEdit->setFocus();
        return;
    }
    QString waist = waistLineEdit->text();
    if(validator->validate(waist, pos) != QValidator::Acceptable)
    {
        QMessageBox::critical(this, tr("腰围输入错误"), tr("腰围输入错误"));
        waistLineEdit->setFocus();
        return;
    }

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
                if(waistItemGroup!=NULL)
                    waistChartView->scene()->removeItem(waistItemGroup);
                if(weightItemGroup!=NULL)
                    weightChartView->scene()->removeItem(weightItemGroup);

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
                GenerateWeightTextItem(height, gender, age);

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
                GenerateWaistTextItem(height, gender);

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
                healthIndexChartView->gender=gender;
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
                bodyFatRateChartView->gender=gender;
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

void CollectInfoWidget::GenerateWaistTextItem(double height, QString gender)
{
    QMap<int, QString> maleWaist;   //男
    QMap<int, QString> femaleWaist; //女

    maleWaist.insert(150,"60.8 ~ 67.2");
    maleWaist.insert(151,"64.5 ~ 67.7");
    maleWaist.insert(152,"65.0 ~ 68.3");
    maleWaist.insert(153,"65.5 ~ 68.8");
    maleWaist.insert(154,"66.0 ~ 69.3");
    maleWaist.insert(155,"66.5 ~ 69.8");
    maleWaist.insert(156,"67.0 ~ 70.4");
    maleWaist.insert(157,"67.5 ~ 70.9");
    maleWaist.insert(158,"68.0 ~ 71.4");
    maleWaist.insert(159,"68.5 ~ 71.9");
    maleWaist.insert(160,"69.0 ~ 72.5");
    maleWaist.insert(161,"69.5 ~ 73.0");
    maleWaist.insert(162,"70.0 ~ 73.5");
    maleWaist.insert(163,"70.5 ~ 74.0");
    maleWaist.insert(164,"71.0 ~ 74.6");
    maleWaist.insert(165,"71.5 ~ 75.1");
    maleWaist.insert(166,"72.0 ~ 75.6");
    maleWaist.insert(167,"72.5 ~ 76.1");
    maleWaist.insert(168,"73.0 ~ 76.7");
    maleWaist.insert(169,"73.5 ~ 77.2");
    maleWaist.insert(170,"74.0 ~ 77.7");
    maleWaist.insert(171,"74.5 ~ 78.2");
    maleWaist.insert(172,"75.0 ~ 78.8");
    maleWaist.insert(173,"75.5 ~ 79.3");
    maleWaist.insert(174,"76.0 ~ 79.8");
    maleWaist.insert(175,"76.5 ~ 80.3");
    maleWaist.insert(176,"77.0 ~ 80.9");
    maleWaist.insert(177,"77.5 ~ 81.4");
    maleWaist.insert(178,"78.0 ~ 81.9");
    maleWaist.insert(179,"78.5 ~ 82.4");
    maleWaist.insert(180,"79.0 ~ 83.0");
    maleWaist.insert(181,"79.5 ~ 83.5");
    maleWaist.insert(182,"80.0 ~ 84.0");
    maleWaist.insert(183,"80.5 ~ 84.5");
    maleWaist.insert(184,"81.0 ~ 85.1");
    maleWaist.insert(185,"81.5 ~ 85.6");

    femaleWaist.insert(150,"58.0 ~ 64.1");
    femaleWaist.insert(151,"58.4 ~ 61.5");
    femaleWaist.insert(152,"58.9 ~ 62.0");
    femaleWaist.insert(153,"59.4 ~ 62.5");
    femaleWaist.insert(154,"59.9 ~ 63.0");
    femaleWaist.insert(155,"60.3 ~ 63.5");
    femaleWaist.insert(156,"60.8 ~ 64.0");
    femaleWaist.insert(157,"61.3 ~ 64.5");
    femaleWaist.insert(158,"61.8 ~ 65.0");
    femaleWaist.insert(159,"62.2 ~ 65.5");
    femaleWaist.insert(160,"62.7 ~ 66.0");
    femaleWaist.insert(161,"63.2 ~ 67.0");
    femaleWaist.insert(162,"63.7 ~ 67.0");
    femaleWaist.insert(163,"64.1 ~ 67.5");
    femaleWaist.insert(164,"64.6 ~ 68.0");
    femaleWaist.insert(165,"65.1 ~ 68.5");
    femaleWaist.insert(166,"65.6 ~ 69.0");
    femaleWaist.insert(167,"66.0 ~ 69.5");
    femaleWaist.insert(168,"66.5 ~ 70.0");
    femaleWaist.insert(169,"67.0 ~ 70.5");
    femaleWaist.insert(170,"67.5 ~ 71.0");
    femaleWaist.insert(171,"67.9 ~ 71.5");
    femaleWaist.insert(172,"68.4 ~ 72.0");
    femaleWaist.insert(173,"68.9 ~ 72.5");
    femaleWaist.insert(174,"69.4 ~ 73.0");
    femaleWaist.insert(175,"69.8 ~ 73.5");
    femaleWaist.insert(176,"70.3 ~ 74.0");
    femaleWaist.insert(177,"70.8 ~ 74.5");
    femaleWaist.insert(178,"71.3 ~ 75.0");
    femaleWaist.insert(179,"71.7 ~ 75.5");
    femaleWaist.insert(180,"72.2 ~ 76.0");
    femaleWaist.insert(181,"72.7 ~ 76.5");
    femaleWaist.insert(182,"73.2 ~ 77.0");
    femaleWaist.insert(183,"73.6 ~ 77.5");
    femaleWaist.insert(184,"74.1 ~ 78.0");
    femaleWaist.insert(185,"74.6 ~ 78.5");

    //定位五条数据
    int index=0;
    if(height<=155)     //0 1 2 3 4
    {
        index=150;
    }
    else if(height>155&&height<=180)    //151 152 153 154 155
    {
        index=height-4;
    }
    else
    {
        index=176;
    }

    //生成html
    waistItemGroup = new QGraphicsItemGroup;
    if(gender==tr("男"))
    {
        QGraphicsSimpleTextItem * textItem = new QGraphicsSimpleTextItem(waistItemGroup);
        textItem->setText(tr("身高(cm)  男腰围(cm)"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        textItem->setFont(font);
        waistItemGroup->addToGroup(textItem);
        font.setBold(false);
        for(int i=0; i<10; i++)
        {
            textItem = new QGraphicsSimpleTextItem(waistItemGroup);
            textItem->setText(tr("%1        %2").arg(index+i).arg(maleWaist.value(index+i)));
            textItem->setPos(0, 20*(i+1));
            textItem->setFont(font);
            waistItemGroup->addToGroup(textItem);
        }
    }
    else
    {
        QGraphicsSimpleTextItem * textItem = new QGraphicsSimpleTextItem(waistItemGroup);
        textItem->setText(tr("身高(cm)  女腰围(cm)"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        textItem->setFont(font);
        waistItemGroup->addToGroup(textItem);
        font.setBold(false);
        for(int i=0; i<10; i++)
        {
            textItem = new QGraphicsSimpleTextItem(waistItemGroup);
            textItem->setText(tr("%1        %2").arg(index+i).arg(femaleWaist.value(index+i)));
            textItem->setPos(0, 20*(i+1));
            textItem->setFont(font);
            waistItemGroup->addToGroup(textItem);
        }
    }
    //显示HTML

    //waistTextItem = new QGraphicsTextItem;
    waistItemGroup->setPos(900,120);
    waistItemGroup->setZValue(1);
    waistItemGroup->setFlag(QGraphicsItem::ItemIsMovable, true);

    //waistItemGroup->setFont(font);
    waistChartView->scene()->addItem(waistItemGroup);
}
void CollectInfoWidget::GenerateWeightTextItem(double height, QString gender, int age)
{
    weightItemGroup = new QGraphicsItemGroup;
    if(gender==tr("男"))
    {
        QMap<int, QString> maleWeight;

        maleWeight.insert(19,"100 104 104 108 112 116 122 128 134 140");
        maleWeight.insert(21,"102 106 108 110 114 120 124 130 138 144");
        maleWeight.insert(23,"104 106 110 112 116 120 126 132 140 146");
        maleWeight.insert(25,"104 108 110 114 118 121 126 134 142 148");
        maleWeight.insert(27,"104 108 110 114 118 121 128 134 142 148");
        maleWeight.insert(29,"106 110 112 114 118 121 128 134 142 148");
        maleWeight.insert(31,"106 110 112 116 120 124 130 136 144 150");
        maleWeight.insert(33,"108 112 114 116 120 126 130 136 144 150");
        maleWeight.insert(35,"108 112 114 118 121 126 132 138 146 152");
        maleWeight.insert(37,"110 112 116 118 121 126 132 138 146 152");
        maleWeight.insert(39,"110 114 116 120 121 128 132 140 148 154");
        maleWeight.insert(41,"110 114 116 120 124 128 134 140 148 154");
        maleWeight.insert(43,"112 114 116 120 124 128 134 140 148 154");
        maleWeight.insert(45,"112 114 118 120 124 128 134 140 148 154");
        maleWeight.insert(47,"112 116 118 121 126 130 134 142 150 156");
        maleWeight.insert(49,"112 116 118 121 126 130 136 142 150 156");
        maleWeight.insert(51,"114 116 118 121 126 130 136 142 150 156");
        maleWeight.insert(53,"114 116 118 121 126 130 136 142 150 156");
        maleWeight.insert(55,"112 116 118 121 126 130 136 142 150 156");
        maleWeight.insert(57,"112 114 118 120 124 130 134 140 148 154");
        maleWeight.insert(59,"112 114 116 120 124 128 134 140 148 154");
        maleWeight.insert(61,"112 114 116 120 124 128 134 140 148 154");
        maleWeight.insert(63,"112 114 116 120 124 128 134 140 148 154");
        maleWeight.insert(65,"112 114 116 120 124 128 134 140 148 154");
        maleWeight.insert(67,"112 114 116 120 124 128 134 140 148 154");
        maleWeight.insert(69,"112 114 116 120 124 128 134 140 148 154");

        //根据身高定位10个数据
        //9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34
        //(age-1)/2
        int index;
        if((age-1)/2<=23)
            index = 11;
        else if((age-1)/2>31)
            index = 30;
        else
            index=(age-1)/2-2;

        //显示
        QGraphicsSimpleTextItem * textItem = new QGraphicsSimpleTextItem(weightItemGroup);
        textItem->setText(tr("年龄\\身高(cm) 152 156 160 164 168 172 176 180 184 188"));
        QFont font;
        font.setPointSize(14);
        //font.setBold(true);
        textItem->setFont(font);
        weightItemGroup->addToGroup(textItem);
        font.setBold(false);
        for(int i=0; i<10; i++)
        {
            textItem = new QGraphicsSimpleTextItem(weightItemGroup);
            textItem->setText(tr("%1            %2").arg((index+i)*2+1).arg(maleWeight.value((index+i)*2+1)));
            textItem->setPos(0, 20*(i+1));
            textItem->setFont(font);
            weightItemGroup->addToGroup(textItem);
        }
    }
    else
    {
        QMap<int, QString> femaleWeight;

        femaleWeight.insert(150,"99.0         90.0         85.6 ");
        femaleWeight.insert(152,"101.6        92.4         87.8 ");
        femaleWeight.insert(154,"104.4        94.8         90.2 ");
        femaleWeight.insert(156,"107.0        97.4         92.4 ");
        femaleWeight.insert(158,"109.8        98.8         94.8 ");
        femaleWeight.insert(160,"112.6        102.4        97.2 ");
        femaleWeight.insert(162,"115.4        105.0        99.8 ");
        femaleWeight.insert(164,"118.4        107.6        102.2");
        femaleWeight.insert(166,"121.2        110.2        104.8");
        femaleWeight.insert(168,"124.2        112.8        107.2");
        femaleWeight.insert(170,"127.2        115.6        109.8");
        femaleWeight.insert(172,"130.2        118.4        112.4");
        femaleWeight.insert(174,"133.2        121.2        115.0");
        femaleWeight.insert(176,"136.2        124.0        117.8");
        femaleWeight.insert(178,"139.4        126.8        120.4");
        femaleWeight.insert(180,"142.6        129.6        123.2");

        //根据身高定位10个数据
        //75 76 77 78 79 89 81 82 83 84 85 86 87 88 89 90
        int index;
        if(height/2<=79)
            index = 75;
        else if(height/2>85)
            index = 85;
        else
            index=height/2-4;

        //显示
        QGraphicsSimpleTextItem * textItem = new QGraphicsSimpleTextItem(weightItemGroup);
        textItem->setText(tr    ("身高(cm) 标准体重(斤) 美体体重(斤) 模特体重(斤)"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        textItem->setFont(font);
        weightItemGroup->addToGroup(textItem);
        font.setBold(false);
        for(int i=0; i<10; i++)
        {
            textItem = new QGraphicsSimpleTextItem(weightItemGroup);
            textItem->setText(tr("%1       %2").arg((index+i)*2).arg(femaleWeight.value((index+i)*2)));
            textItem->setPos(0, 20*(i+1));
            textItem->setFont(font);
            weightItemGroup->addToGroup(textItem);
        }
    }

    weightItemGroup->setPos(900,120);
    weightItemGroup->setZValue(1);
    weightItemGroup->setFlag(QGraphicsItem::ItemIsMovable, true);
    weightChartView->scene()->addItem(weightItemGroup);
}
