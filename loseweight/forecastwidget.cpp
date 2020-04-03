#include "forecastwidget.h"
#include <QDebug>

#define SampleSize 3

ForecastWidget::ForecastWidget(QWidget *parent) : QWidget(parent)
{
    qDebug()<<"ForecastWidget";
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
    InitChartWidget(this);

    textBrowser = new QTextBrowser;
    textBrowser->append(tr("test"));
}

void ForecastWidget::UpdateQueryMode()
{
    qDebug()<<"UpdateQueryMode";
    queryModel->setQuery("select distinct username from user");
    queryModel->query();
}

void ForecastWidget::InitChartWidget(QWidget *widget)
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
    connect(chartUserNameLineEdit, &QLineEdit::textEdited, this, &ForecastWidget::ChartUserLineEdit);
    connect(chartUserNameLineEdit, &QLineEdit::editingFinished, this, &ForecastWidget::ChartUserLineEdit);

    chartPhoneComboBox = new QComboBox(widget);
    chartPhoneComboBox->setMinimumWidth(150);
    connect(chartPhoneComboBox, &QComboBox::currentTextChanged, this, &ForecastWidget::ChartPhoneComboBoxIndexChanged);
    chartTimesComboBox = new QComboBox(widget);
    connect(chartTimesComboBox, &QComboBox::currentTextChanged, this, &ForecastWidget::ChartTimesComboBoxIndexChanged);

    QLabel * genderLabel = new QLabel(tr("性别"), widget);
    QLabel * ageLabel = new QLabel(tr("出生日期"), widget);
    QLabel * heightLabel = new QLabel(tr("身高(cm)"), widget);
    QLabel * weightLabel = new QLabel(tr("体重(斤)"), widget);
    QLabel * waistLabel = new QLabel(tr("腰围(cm)"), widget);
    QLabel * openHoleWeightLabel = new QLabel(tr("开穴斤数(斤)"), widget);

    genderCombox = new QComboBox(widget);
    genderCombox->addItem(tr("男"));
    genderCombox->addItem(tr("女"));
    dateEdit = new QDateEdit(widget);
    dateEdit->setDate(QDateTime::currentDateTime().date());
    heightLineEdit = new QLineEdit(widget);
    weightLineEdit = new QLineEdit(widget);
    waistLineEdit = new QLineEdit(widget);
    openHoleWeightLineEdit = new QLineEdit(widget);

    QPushButton * queryButton = new QPushButton(tr("预测"), widget);
    connect(queryButton, &QPushButton::clicked, this, &ForecastWidget::QueryPushButtonClicked);

    archiveLayout->addWidget(chartUserNameLabel, 0, 0, 1, 1);
    archiveLayout->addWidget(chartUserNameLineEdit, 0, 1, 1, 1);
    archiveLayout->addWidget(chartPhoneLabel, 0, 2, 1, 1);
    archiveLayout->addWidget(chartPhoneComboBox, 0, 3, 1, 1);
    archiveLayout->addWidget(chartTimesLabel, 0, 4, 1, 1);
    archiveLayout->addWidget(chartTimesComboBox, 0, 5, 1, 1);
    archiveLayout->addWidget(genderLabel, 0, 6, 1, 1);
    archiveLayout->addWidget(genderCombox, 0, 7, 1, 1);
    archiveLayout->addWidget(ageLabel, 0, 8, 1, 1);
    archiveLayout->addWidget(dateEdit, 0, 9, 1, 1);
    archiveLayout->addWidget(heightLabel, 0, 10, 1, 1);
    archiveLayout->addWidget(heightLineEdit, 0, 11, 1, 1);
    archiveLayout->addWidget(weightLabel, 0, 12, 1, 1);
    archiveLayout->addWidget(weightLineEdit, 0, 13, 1, 1);
    archiveLayout->addWidget(waistLabel, 0, 14, 1, 1);
    archiveLayout->addWidget(waistLineEdit, 0, 15, 1, 1);
    archiveLayout->addWidget(openHoleWeightLabel, 0, 16, 1, 1);
    archiveLayout->addWidget(openHoleWeightLineEdit, 0, 17, 1, 1);

    archiveLayout->addWidget(queryButton, 0, 18, 1, 1);
    archiveLayout->setColumnStretch(19, 1);

    QScrollArea * scrollArea = new QScrollArea(widget);
    QWidget * widget2 = new QWidget(scrollArea);
    QVBoxLayout * vBoxLayout = new QVBoxLayout(widget2);

    //体重曲线
    QFont font2;
    font2.setPointSize(20);//字体大小
    font2.setBold(true);

    weightChartView = new View(widget);
    weightChartView->text=tr("体重(斤)");
    weightChart = new QChart();
    weightChart->setTitle(tr("体重曲线"));
    weightChart->setTitleFont(font2);
    //weightChartView->setChart(weightChart);
    weightChartView->setFixedSize(1660, 900);

    //腰围曲线
    waistChartView = new View(widget);
    waistChartView->text=tr("腰围(cm)");
    waistChart = new QChart();
    waistChart->setTitle(tr("腰围曲线"));
    waistChart->setTitleFont(font2);
    //waistChartView->setChart(waistChart);
    waistChartView->setFixedSize(1660, 900);

    //体脂率曲线
    bodyFatRateChartView = new View(widget);
    bodyFatRateChartView->text=tr("体脂率(%)");
    bodyFatRateChart = new QChart();
    bodyFatRateChart->setTitle(tr("体脂率曲线"));
    bodyFatRateChart->setTitleFont(font2);
    //bodyFatRateChartView->setChart(bodyFatRateChart);
    bodyFatRateChartView->setFixedSize(1660, 900);

    //健康曲线
    healthIndexChartView = new View(widget);
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

    widget->setLayout(mainLayout);
}

void ForecastWidget::ChartUserLineEdit()
{
    QString cmd;
    QSqlQuery query;

    if(chartUserNameLineEdit->text().isEmpty())
    {
        chartPhoneComboBox->clear();
        chartTimesComboBox->clear();
        genderCombox->setCurrentIndex(0);
        dateEdit->setDate(QDateTime::currentDateTime().date());
        heightLineEdit->clear();
        weightLineEdit->clear();
        waistLineEdit->clear();
        openHoleWeightLineEdit->clear();
        return;
    }

    cmd = QString("select distinct phonenumber from user where username = '%1'").arg(chartUserNameLineEdit->text());
    if(query.exec(cmd))
    {
        //将电话号码插入
        chartPhoneComboBox->clear();
        while(query.next())
        {
            chartPhoneComboBox->addItem(query.value(0).toString());
        }
        //查询当前电话的次数
        cmd = QString("select distinct times from user where username = '%1' and phonenumber='%2'")
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText());
        if(query.exec(cmd))
        {
            chartTimesComboBox->clear();
            while(query.next())
            {
                chartTimesComboBox->addItem(query.value(0).toString());
            }

            //将第一个用户的数据显示出来
            ShowUserInfo();
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

void ForecastWidget::ShowUserInfo()
{
    QSqlQuery query;
    QString cmd;
    //将第一个用户的数据显示出来
    cmd = QString("select gender, dateofbirth, height, weight, waist,openholeweight from user where username='%1' and phonenumber='%2' and times='%3'")
            .arg(chartUserNameLineEdit->text())
            .arg(chartPhoneComboBox->currentText())
            .arg(chartTimesComboBox->currentText());
    if(query.exec(cmd))
    {
        if(query.next())
        {
            genderCombox->setCurrentText(query.value(0).toString());
            dateEdit->setDate(QDateTime::fromString(query.value(1).toString(), "yyyy-MM-dd").date());
            heightLineEdit->setText(query.value(2).toString());
            weightLineEdit->setText(query.value(3).toString());
            waistLineEdit->setText(query.value(4).toString());
            openHoleWeightLineEdit->setText(query.value(5).toString());
        }
    }
}

void ForecastWidget::ChartPhoneComboBoxIndexChanged(const QString &phone)
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
        ShowUserInfo();
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), cmd);
    }
}

void ForecastWidget::ChartTimesComboBoxIndexChanged()
{
    ShowUserInfo();
}

void ForecastWidget::QueryPushButtonClicked()
{
    qDebug()<<"ForecastWidget::QueryPushButtonClicked";
    textBrowser->clear();
    //textBrowser->show();
    //1.新建曲线
    //2.得出预测数据并插入到曲线中
    //2.0 找出这个用户的基本信息

    ForecastWeight();
    //qDebug()<<weightForecastData;
    ForecastWaist();
    //qDebug()<<waistForecastData;

    //()<<weightForecastData.count()<<waistForecastData.count();
    ForecastBMI();
    ForecastBFP();
}
#if 0
void ForecastWidget::ForecastWeight()
{
    QSqlQuery query;
    QString cmd;

    QString dateofbirth = dateEdit->date().toString("yyyy-MM-dd");
    QString gender = genderCombox->currentText();
    double weight = weightLineEdit->text().toDouble();
    double height = heightLineEdit->text().toDouble();
    double waist = waistLineEdit->text().toDouble();
    double targetWeight = weight-openHoleWeightLineEdit->text().toDouble();
    double bmi=weight/2/height/height*10000;
    int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(dateofbirth, "yyyy-MM-dd").toString("yyyy").toInt();

    textBrowser->append(tr("1.用户基本信息: 姓名:%1 性别:%2 出生日期:%3 体重:%4 身高:%5 腰围:%6 目标体重:%7 bmi:%8")
                        .arg(chartUserNameLineEdit->text())
                        .arg(gender)
                        .arg(dateofbirth)
                        .arg(weight)
                        .arg(height)
                        .arg(waist)
                        .arg(targetWeight)
                        .arg(bmi));

    //2.1 找出3条曲线, 与预测用户类似的
    /*
        1.	性别：同性别对比、参考
        2.	年龄：①±1  ②±2  ③±4  ④±6  ⑤±10       以年龄作为参数找3条曲线， 以年龄排序，找到附近的
        3.	身高(cm)：①±2 ②±3 ③±5 ④±7 ⑤±8        以身高作为参数找3条曲线   以身高排序，找到附近的
        4.	体重(斤)： ①±3 ②±5 ③±7 ④±10 ⑤±15     以体重作为参数找3条曲线   以体重排序，找到附近的

        7.	BMI：①±0.5 ②±1 ③±2 ④±3 ⑤±5           以BMI作为参考找3条曲线   以BMI排序，找到附近的
    */
    //2.1.1 以年龄找到1组数据
    textBrowser->append(tr("2.以年龄为参考找寻匹配数据"));

    double ageSectionAndScale[5][2]        = {{1, 40}, {2, 36}, {4, 32}, { 6, 28}, {10, 24}};

    QVector<int> ageSectionScaleId;
    QVector<int> ageDataCount;
    for(int i=0; i<5; i++)  //寻找可用数据
    {
        textBrowser->append(tr("寻找第%1分段数据样本，年龄差为±%2，占比%3%").arg(i).arg(ageSectionAndScale[i][0]).arg(ageSectionAndScale[i][1]));
        cmd = QString(tr("select id, ABS(dateofbirth - '%1'), datacount from user where (ABS(dateofbirth - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=40) order by ABS(dateofbirth - '%1') limit %6"))
                .arg(dateofbirth)
                .arg(ageSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText())
                .arg(SampleSize);

    QFont font;
    font.setPointSize(18);//字体大小
    font.setBold(true);

    //3.显示曲线
    if(weightChartView->chart()!=NULL)
        weightChartView->chart()->removeAllSeries();
    weightSeries = new QLineSeries;
    weightScatterSeries = new QScatterSeries;
    weightScatterSeries->setMarkerSize(10);

    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    double firstWeight = weight;
    weightForecastData.clear();
    for(int i=0; i<max6DataCount; i++)
    {
        weightSeries->append(currentTime+(24*60*60*1000*i), firstWeight*(1-finalData[i]));
        weightScatterSeries->append(currentTime+(24*60*60*1000*i), firstWeight*(1-finalData[i]));
        weightForecastData.append(firstWeight);
        if(firstWeight*(1-finalData[i])<targetWeight)
            break;
        firstWeight = firstWeight*(1-finalData[i]);
    }

    textBrowser->append(tr("如果以今天为起点做预测，得出数据点"));
    textBrowser->append(tr("时间\t预计体重"));
/*
    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    for(int i=0; i<finalData.count(); i++)
    {
        weightSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
        weightScatterSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));

        textBrowser->append(tr("%1\t%2").arg(QDateTime::fromMSecsSinceEpoch(currentTime+(24*60*60*1000*i)).toString("yyyy-MM-dd")).arg(finalData.at(i)));
    }
*/
    textBrowser->append(tr("6.生成曲线"));
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

    if(weightItemGroup!=NULL)
        weightChartView->scene()->removeItem(weightItemGroup);
    GenerateWeightTextItem(height, gender, age);
}
#endif
void ForecastWidget::GenerateWaistTextItem(double height, QString gender)
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

void ForecastWidget::GenerateWeightTextItem(double height, QString gender, int age)
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

void ForecastWidget::ForecastWeight()
{
    QSqlQuery query;
    QString cmd;

    QString dateofbirth = dateEdit->date().toString("yyyy-MM-dd");
    QString gender = genderCombox->currentText();
    double weight = weightLineEdit->text().toDouble();
    double height = heightLineEdit->text().toDouble();
    double waist = waistLineEdit->text().toDouble();
    double targetWeight = weight-openHoleWeightLineEdit->text().toDouble();
    double openHoleWeight = openHoleWeightLineEdit->text().toDouble();
    double bmi=weight/2/height/height*10000;
    int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(dateofbirth, "yyyy-MM-dd").toString("yyyy").toInt();
    double bfp = 0;
    if(gender==tr("男"))
    {
        bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
    }
    else if(gender==tr("女"))
    {
        bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
    }

    int id = 0;
    QString sampleGender;
    int sampleAge;
    double sampleWeight;
    double sampleHeight;
    double sampleWaist;
    double sampleBfp;
    cmd = QString("select * from user where %1-0.5<=bfp and bfp<=%1+1 and datacount-40>=0 order by datacount desc").arg(bfp);
    qDebug()<<cmd;
    if(query.exec(cmd))
    {
        if(query.next())
        {
            id = query.value(0).toInt();
            sampleGender = query.value(2).toString();
            sampleAge = QDateTime::fromString(query.value(8).toString(), "yyyy-MM-dd").toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
            sampleWeight = query.value(10).toDouble();
            sampleHeight = query.value(5).toDouble();
            sampleWaist = query.value(11).toDouble();
            sampleBfp = query.value(13).toDouble();
        }
        else
        {
            cmd = QString("select * from user where bfp>%1 and datacount-40>=0 order by abs(bfp-%1)").arg(bfp);
            if(query.exec(cmd))
            {
                if(query.next())
                {
                    id = query.value(0).toInt();
                    sampleGender = query.value(2).toString();
                    sampleAge = QDateTime::fromString(query.value(8).toString(), "yyyy-MM-dd").toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
                    sampleWeight = query.value(10).toDouble();
                    sampleHeight = query.value(5).toDouble();
                    sampleWaist = query.value(11).toDouble();
                    sampleBfp = query.value(13).toDouble();
                }
                else
                {
                    cmd = QString("select * from user where bfp<%1 and datacount-40>=0 order by abs(bfp-%1)").arg(bfp);
                    if(query.exec(cmd))
                    {
                        if(query.next())
                        {
                            id = query.value(0).toInt();
                            sampleGender = query.value(2).toString();
                            sampleAge = QDateTime::fromString(query.value(8).toString(), "yyyy-MM-dd").toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
                            sampleWeight = query.value(10).toDouble();
                            sampleHeight = query.value(5).toDouble();
                            sampleWaist = query.value(11).toDouble();
                            sampleBfp = query.value(13).toDouble();
                        }
                    }
                    else
                    {
                        qDebug()<<tr("SQL错误")<<cmd;
                    }
                }
            }
            else
            {
                qDebug()<<tr("SQL错误")<<cmd;
            }
        }
    }
    else
    {
        qDebug()<<tr("SQL错误")<<cmd;
    }
    qDebug()<<id;
    //找到ID后, 求出权重
    int weightDataWeight = 0;       //没有%， 带入运算时除以100
    //qDebug()<<"年龄"<<age<<"样本年龄"<<sampleAge;
    if((age-sampleAge)>0)
    {
        weightDataWeight = (age-sampleAge)/3 * 5;
    }
    else
    {
        weightDataWeight = -(sampleAge-age)/3 * 5;
    }
    qDebug()<<"年龄计算权重"<<weightDataWeight;
    if((height-sampleHeight)>0)
    {
        weightDataWeight+= (int)((height-sampleHeight)/2)*3;
    }
    else
    {
        weightDataWeight+= -((int)((sampleHeight-height)/2)*3);
    }
    qDebug()<<"年龄+身高计算权重"<<weightDataWeight;

    if((weight-sampleWeight)>0)
    {
        weightDataWeight+= (int)((weight-sampleWeight)/3)*2;
    }
    else
    {
        weightDataWeight+= -((int)((sampleWeight-weight)/3)*2);
    }
    weightDataWeight = (100+weightDataWeight);
    qDebug()<<"权重"<<weightDataWeight;
    // 取出样本所有的数据
    QVector<double> forecastWeight;
    cmd = QString("select weight from archive%1").arg(id);
    if(query.exec(cmd))
    {
        int i=0;
        forecastWeight.append(weight);
        double sampleLastWeight = sampleWeight;
        while(query.next())
        {
            forecastWeight.append(forecastWeight.at(i)+((query.value(0).toDouble()-sampleLastWeight)*weightDataWeight/100));
            i++;
            sampleWeightDataCount = i;
            sampleLastWeight = query.value(0).toDouble();
            if(forecastWeight.at(i)<=targetWeight)
                break;
        }
    }
    else
    {
        qDebug()<<"SQL错误"<<cmd;
    }
#if 1
    qDebug()<<forecastWeight.last()<<targetWeight;
    int len = forecastWeight.length();
    //如果没有到达目标体重，则补齐数据
    if(forecastWeight.last()>targetWeight)
    {
        for(int i=1;forecastWeight.length()<=170; i++)
        {
            double value1, value2, value3;
            value1 = forecastWeight.at(len/2+i-1);      //循环时加了1
            value2 = forecastWeight.at(len/2+i);
            value3 = forecastWeight.at(len/2+i+1);
            if(i>=1 && i<=9)
            {
                //qDebug()<<forecastWeight.last()<<(((value2-value1)/value1+(value3-value2)/value2)*0.85)<<forecastWeight.last()+(((value2-value1)/value1+(value3-value2)/value2)*0.85);
                forecastWeight.append(forecastWeight.last()+(((value2-value1)+(value3-value2))*0.85));

            }
            else if(i>=10 && i<=19)
            {
                forecastWeight.append(forecastWeight.last()+(((value2-value1)+(value3-value2))*0.75));
            }
            else if(i>=20 && i<=29)
            {
                forecastWeight.append(forecastWeight.last()+(((value2-value1)+(value3-value2))*0.65));
            }
            else if(i>=30 && i<=39)
            {
                forecastWeight.append(forecastWeight.last()+(((value2-value1)+(value3-value2))*0.55));
            }
            else
            {
                forecastWeight.append(forecastWeight.last()+(((value2-value1)+(value3-value2))*0.50));
            }
            if(forecastWeight.last()<targetWeight)
                break;
        }
    }

    //得到预测的点数
    forecastWeightDataCount = forecastWeight.length();

#endif

    //将曲线进行缩放
    QVector<double> finalData;
    if(forecastWeight.length() < openHoleWeight*1.7)
        ResizeDataCount(forecastWeight, finalData, forecastWeight.length(), openHoleWeight*2.0);
    else if(forecastWeight.length() > openHoleWeight*2.2)
        ResizeDataCount(forecastWeight, finalData, forecastWeight.length(), openHoleWeight*2.1);
    else
    {
        for(int i=0; i<forecastWeight.length(); i++)
            finalData.append(forecastWeight.at(i));
    }
    resizeWeightDataCount = finalData.length();

    weightForecastData.clear();
    for(int i=0; i<finalData.length(); i++)
    {
        weightForecastData.append(finalData.at(i));
    }

    QFont font;
    font.setPointSize(18);//字体大小
    font.setBold(true);

    //3.显示曲线
    if(weightChartView->chart()!=NULL)
        weightChartView->chart()->removeAllSeries();
    weightSeries = new QLineSeries;
    weightScatterSeries = new QScatterSeries;
    weightScatterSeries->setMarkerSize(10);

#if 0
    QLineSeries *originWeightSeries = new QLineSeries;
    QScatterSeries *originWeightScatterSeries = new QScatterSeries;
    originWeightScatterSeries->setMarkerSize(10);
#endif

    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    for(int i=0; i<finalData.length(); i++)
    {
        weightSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
        weightScatterSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
    }
#if 0
    for(int i=0; i<forecastWeight.length(); i++)
    {
        originWeightSeries->append(currentTime+(24*60*60*1000*i), forecastWeight.at(i));
        originWeightScatterSeries->append(currentTime+(24*60*60*1000*i), forecastWeight.at(i));
    }

    weightChart->addSeries(originWeightSeries);
    weightChart->addSeries(originWeightScatterSeries);
#endif
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

    if(weightItemGroup!=NULL)
        weightChartView->scene()->removeItem(weightItemGroup);
    GenerateWeightTextItem(height, gender, age);
}

void ForecastWidget::ResizeDataCount(QVector<double> &src, QVector<double> &dst, int srcNum, int dstNum)
{
    //1. 前20个点不进行处理
    for(int i=0; i<20; i++)
    {
        dst.append(src.at(i));
    }
    //2. 将后边的点数进行缩放  60 80  40 60   src/dst  40/60=  0.67  0.67取一个点
    double val = ((double)srcNum-20.0)/((double)dstNum-20);
    for(int i=20; i<dstNum; i++)
    {
        //0.67取点  1.34 取点  2.01取点
        // (i-20+1)*0.67
        double val2 = (i-20.0+1.0)*val+20.0;
        if(src.length()<=(int)(val2+0.5))
            break;
        double lastValue = src.at((int)(val2-0.5));
        double nextValue = src.at((int)(val2+0.5));
        dst.append(lastValue*(1.0-(val2-(int)(val2)))+nextValue*(val2-(int)(val2)));
    }
    //dst.append(src.last());
}

void ForecastWidget::ForecastWaist()
{
    QSqlQuery query;
    QString cmd;

    QString dateofbirth = dateEdit->date().toString("yyyy-MM-dd");
    QString gender = genderCombox->currentText();
    double weight = weightLineEdit->text().toDouble();
    double height = heightLineEdit->text().toDouble();
    double waist = waistLineEdit->text().toDouble();
    double targetWeight = weight-openHoleWeightLineEdit->text().toDouble();
    double openHoleWeight = openHoleWeightLineEdit->text().toDouble();
    double bmi=weight/2/height/height*10000;
    int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(dateofbirth, "yyyy-MM-dd").toString("yyyy").toInt();
    double bfp = 0;
    if(gender==tr("男"))
    {
        bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
    }
    else if(gender==tr("女"))
    {
        bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
    }

    int id = 0;
    QString sampleGender;
    int sampleAge;
    double sampleWeight;
    double sampleHeight;
    double sampleWaist;
    double sampleBfp;
    cmd = QString("select * from user where %1-0.5<=bfp and bfp<=%1+1 and datacount-40>=0 order by datacount desc").arg(bfp);
    if(query.exec(cmd))
    {
        if(query.next())
        {
            id = query.value(0).toInt();
            sampleGender = query.value(2).toString();
            sampleAge = QDateTime::fromString(query.value(8).toString(), "yyyy-MM-dd").toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
            sampleWeight = query.value(10).toDouble();
            sampleHeight = query.value(5).toDouble();
            sampleWaist = query.value(11).toDouble();
            sampleBfp = query.value(13).toDouble();
        }
        else
        {
            cmd = QString("select * from user where bfp>%1 and datacount-40>=0 order by abs(bfp-%1)").arg(bfp);
            if(query.exec(cmd))
            {
                if(query.next())
                {
                    id = query.value(0).toInt();
                    sampleGender = query.value(2).toString();
                    sampleAge = QDateTime::fromString(query.value(8).toString(), "yyyy-MM-dd").toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
                    sampleWeight = query.value(10).toDouble();
                    sampleHeight = query.value(5).toDouble();
                    sampleWaist = query.value(11).toDouble();
                    sampleBfp = query.value(13).toDouble();
                }
                else
                {
                    cmd = QString("select * from user where bfp<%1 and datacount-40>=0 order by abs(bfp-%1)").arg(bfp);
                    if(query.exec(cmd))
                    {
                        if(query.next())
                        {
                            id = query.value(0).toInt();
                            sampleGender = query.value(2).toString();
                            sampleAge = QDateTime::fromString(query.value(8).toString(), "yyyy-MM-dd").toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
                            sampleWeight = query.value(10).toDouble();
                            sampleHeight = query.value(5).toDouble();
                            sampleWaist = query.value(11).toDouble();
                            sampleBfp = query.value(13).toDouble();
                        }
                    }
                    else
                    {
                        qDebug()<<tr("SQL错误")<<cmd;
                    }
                }
            }
            else
            {
                qDebug()<<tr("SQL错误")<<cmd;
            }
        }
    }
    else
    {
        qDebug()<<tr("SQL错误")<<cmd;
    }
    qDebug()<<id;

    int waistDataWeight = 0;       //没有%， 带入运算时除以100
    if((waist-sampleWaist)>0)
    {
        waistDataWeight = (int)((waist-sampleWaist)/2) * 4;
    }
    else
    {
        waistDataWeight = -(int)((waist-sampleWaist)/2) * 4;
    }

    if((bfp-sampleBfp)>0)
    {
        waistDataWeight+= (int)((bfp-sampleBfp)/0.5)*2;
    }
    else
    {
        waistDataWeight+= -((int)((bfp-sampleBfp)/0.5)*2);
    }

    waistDataWeight = (100+waistDataWeight);
    qDebug()<<"权重"<<waistDataWeight;
    // 取出样本所有的数据
    QVector<double> forecastWaist;
    cmd = QString("select waist from archive%1").arg(id);
    if(query.exec(cmd))
    {
        int i=0;
        forecastWaist.append(waist);
        double sampleLastWaist = sampleWaist;
        while(query.next())
        {
            forecastWaist.append(forecastWaist.at(i)+((query.value(0).toDouble()-sampleLastWaist)*waistDataWeight/100));
            i++;
            sampleLastWaist = query.value(0).toDouble();
            if(i>=sampleWeightDataCount)    //取得和体重相同的点数
                break;
        }
    }
    else
    {
        qDebug()<<"SQL错误"<<cmd;
    }

    int len = forecastWaist.length();
    //如果没有到达到与体重相同的点数，则补齐数据
    if(forecastWaist.length()<forecastWeightDataCount)
    {
        for(int i=1;forecastWaist.length()<=170; i++)
        {
            double value1, value2, value3;
            value1 = forecastWaist.at(len/2+i-1);      //循环时加了1
            value2 = forecastWaist.at(len/2+i);
            value3 = forecastWaist.at(len/2+i+1);
            if(i>=1 && i<=9)
            {
                forecastWaist.append(forecastWaist.last()+(((value2-value1)+(value3-value2))*0.85));
            }
            else if(i>=10 && i<=19)
            {
                forecastWaist.append(forecastWaist.last()+(((value2-value1)+(value3-value2))*0.75));
            }
            else if(i>=20 && i<=29)
            {
                forecastWaist.append(forecastWaist.last()+(((value2-value1)+(value3-value2))*0.65));
            }
            else if(i>=30 && i<=39)
            {
                forecastWaist.append(forecastWaist.last()+(((value2-value1)+(value3-value2))*0.55));
            }
            else
            {
                forecastWaist.append(forecastWaist.last()+(((value2-value1)+(value3-value2))*0.50));
            }
            if(forecastWeightDataCount==forecastWaist.length())
                break;
        }
    }

    //将曲线进行缩放
    QVector<double> finalData;
    if(forecastWaist.length() != resizeWeightDataCount)
    {
        ResizeDataCount(forecastWaist, finalData, forecastWaist.length(), resizeWeightDataCount+1);
    }
    else
    {
        for(int i=0; i<forecastWaist.length(); i++)
            finalData.append(forecastWaist.at(i));
    }

    waistForecastData.clear();
    for(int i=0; i<finalData.length(); i++)
    {
        waistForecastData.append(finalData.at(i));
    }

    QFont font;
    font.setPointSize(18);//字体大小
    font.setBold(true);
    //3.显示曲线
    if(waistChartView->chart()!=NULL)
        waistChartView->chart()->removeAllSeries();
    waistSeries = new QLineSeries;
    waistScatterSeries = new QScatterSeries;
    waistScatterSeries->setMarkerSize(10);

    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    for(int i=0; i<finalData.length(); i++)
    {
        waistSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
        waistScatterSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
    }

    waistChart->addSeries(waistSeries);
    waistChart->addSeries(waistScatterSeries);

    waistChart->createDefaultAxes();
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(12);
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("时间"));
    axisX->setTitleFont(font);
    waistChart->setAxisX(axisX, waistSeries);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText(tr("腰围(cm)"));
    axisY->setTitleFont(font);
    axisY->setTickCount(16);
    waistChart->setAxisY(axisY, waistSeries);

    waistChartView->setChart(waistChart);
    waistChart->zoom(0.9);
    if(waistItemGroup!=NULL)
    {
        waistChartView->scene()->removeItem(waistItemGroup);
    }
    GenerateWaistTextItem(height, gender);
}

void ForecastWidget::ForecastBMI()
{
    QString dateofbirth = dateEdit->date().toString("yyyy-MM-dd");
    QString gender = genderCombox->currentText();
    double weight = weightLineEdit->text().toDouble();
    double height = heightLineEdit->text().toDouble();
    double waist = waistLineEdit->text().toDouble();
    double targetWeight = weight-openHoleWeightLineEdit->text().toDouble();
    double bmi=weight/2/height/height*10000;
    int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(dateEdit->date().toString("yyyy-MM-dd"), "yyyy-MM-dd").toString("yyyy").toInt();

    textBrowser->append(tr("1.用户基本信息: 姓名:%1 性别:%2 出生日期:%3 体重:%4 身高:%5 腰围:%6 目标体重:%7 bmi:%8")
                        .arg(chartUserNameLineEdit->text())
                        .arg(gender)
                        .arg(dateofbirth)
                        .arg(weight)
                        .arg(height)
                        .arg(waist)
                        .arg(targetWeight)
                        .arg(bmi));

    //取出 weight waist 的最小值
    double bfp;
    bfpForecastData.clear();
    if(weightForecastData.count()>waistForecastData.count())
    {
        for(int i=0; i<waistForecastData.count(); i++)
        {
            bmi = weightForecastData.at(i)/2/height/height*10000;
            if(gender==tr("男"))
                //bfp = (((1.2*bmi)+(0.23*age)-5.4-10.8)*0.4)/0.4;
                //bfp = ((((waistForecastData.at(i)*0.74)-(weightForecastData.at(i)/2*0.082+34.89+9.85))/(weightForecastData.at(i)/2)*100)*0.6)/0.6;
                bfp = ((((waistForecastData.at(i)*0.74)-(weightForecastData.at(i)/2*0.082+34.89+9.85))/(weightForecastData.at(i)/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
            else
                //bfp = (((1.2*bmi)+(0.23*age)-5.4)*0.4)/0.4;
                //bfp = ((((waistForecastData.at(i)*0.74)-(weightForecastData.at(i)/2*0.082+34.89))/(weightForecastData.at(i)/2)*100)*0.6)/0.6;
                bfp = ((((waistForecastData.at(i)*0.74)-(weightForecastData.at(i)/2*0.082+34.89))/(weightForecastData.at(i)/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
            bfpForecastData.append(bfp);
        }
    }
    else
    {
        for(int i=0; i<weightForecastData.count(); i++)
        {
            bmi = weightForecastData.at(i)/2/height/height*10000;
            if(gender==tr("男"))
                //bfp = (((1.2*bmi)+(0.23*age)-5.4-10.8)*0.4)/0.4;
                //bfp = ((((waistForecastData.at(i)*0.74)-(weightForecastData.at(i)/2*0.082+34.89+9.85))/(weightForecastData.at(i)/2)*100)*0.6)/0.6;
                bfp = ((((waistForecastData.at(i)*0.74)-(weightForecastData.at(i)/2*0.082+34.89+9.85))/(weightForecastData.at(i)/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
            else
                //bfp = (((1.2*bmi)+(0.23*age)-5.4)*0.4)/0.4;
                //bfp = ((((waistForecastData.at(i)*0.74)-(weightForecastData.at(i)/2*0.082+34.89))/(weightForecastData.at(i)/2)*100)*0.6)/0.6;
                bfp = ((((waistForecastData.at(i)*0.74)-(weightForecastData.at(i)/2*0.082+34.89))/(weightForecastData.at(i)/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
            bfpForecastData.append(bfp);
        }
    }

    QFont font;
    font.setPointSize(18);//字体大小
    font.setBold(true);

    //3.显示曲线
    if(bodyFatRateChartView->chart()!=NULL)
        bodyFatRateChartView->chart()->removeAllSeries();
    bodyFatRateSeries = new QLineSeries;
    bodyFatRateScatterSeries = new QScatterSeries;
    bodyFatRateScatterSeries->setMarkerSize(10);

    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    for(int i=0; i<bfpForecastData.count(); i++)
    {
        bodyFatRateSeries->append(currentTime+(24*60*60*1000*i), bfpForecastData.at(i));
        bodyFatRateScatterSeries->append(currentTime+(24*60*60*1000*i), bfpForecastData.at(i));
    }

    bodyFatRateChart->addSeries(bodyFatRateSeries);
    bodyFatRateChart->addSeries(bodyFatRateScatterSeries);

    bodyFatRateChart->createDefaultAxes();
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(12);
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("时间"));
    axisX->setTitleFont(font);
    bodyFatRateChart->setAxisX(axisX, bodyFatRateSeries);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText(tr("体脂率(%)"));
    axisY->setTitleFont(font);
    axisY->setTickCount(16);
    bodyFatRateChart->setAxisY(axisY, bodyFatRateSeries);

    bodyFatRateChartView->setChart(bodyFatRateChart);
    bodyFatRateChartView->gender=gender;
    bodyFatRateChart->zoom(0.9);

    if(bfpTextItem!=NULL)
        bodyFatRateChartView->scene()->removeItem(bfpTextItem);
    bfpTextItem = new QGraphicsTextItem;
    bfpTextItem->setPos(900,120);
    QTextDocument * textDocument = new QTextDocument(bfpTextItem);
    QFile file;
    if(gender == tr("男"))
        file.setFileName(tr(":/resource/html/male.html"));
    else
        file.setFileName(tr(":/resource/html/female.html"));
    file.open(QIODevice::ReadOnly);
    textDocument->setHtml(file.readAll());
    file.close();
    bfpTextItem->setDocument(textDocument);
    bfpTextItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    QFont font3;
    font3.setPointSize(14);
    bfpTextItem->setFont(font3);
    bodyFatRateChartView->scene()->addItem(bfpTextItem);
}

void ForecastWidget::ForecastBFP()
{
    QString dateofbirth = dateEdit->date().toString("yyyy-MM-dd");
    QString gender = genderCombox->currentText();
    double weight = weightLineEdit->text().toDouble();
    double height = heightLineEdit->text().toDouble();
    double waist = waistLineEdit->text().toDouble();
    double targetWeight = weight-openHoleWeightLineEdit->text().toDouble();
    double bmi=weight/2/height/height*10000;
    int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(dateEdit->date().toString("yyyy-MM-dd"), "yyyy-MM-dd").toString("yyyy").toInt();

    textBrowser->append(tr("1.用户基本信息: 姓名:%1 性别:%2 出生日期:%3 体重:%4 身高:%5 腰围:%6 目标体重:%7 bmi:%8")
                        .arg(chartUserNameLineEdit->text())
                        .arg(gender)
                        .arg(dateofbirth)
                        .arg(weight)
                        .arg(height)
                        .arg(waist)
                        .arg(targetWeight)
                        .arg(bmi));

    bmiForecastData.clear();

    for(int i=0; i<weightForecastData.count(); i++)
    {
        bmi = weightForecastData.at(i)/2/height/height*10000;
        bmiForecastData.append(bmi);
    }

    QFont font;
    font.setPointSize(18);//字体大小
    font.setBold(true);
    //3.显示曲线
    if(healthIndexChartView->chart()!=NULL)
        healthIndexChartView->chart()->removeAllSeries();
    healthIndexSeries = new QLineSeries;
    healthIndexScatterSeries = new QScatterSeries;
    healthIndexScatterSeries->setMarkerSize(10);

    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    for(int i=0; i<bmiForecastData.count(); i++)
    {
        healthIndexSeries->append(currentTime+(24*60*60*1000*i), bmiForecastData.at(i));
        healthIndexScatterSeries->append(currentTime+(24*60*60*1000*i), bmiForecastData.at(i));
    }

    healthIndexChart->addSeries(healthIndexSeries);
    healthIndexChart->addSeries(healthIndexScatterSeries);

    healthIndexChart->createDefaultAxes();
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(12);
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("时间"));
    axisX->setTitleFont(font);
    healthIndexChart->setAxisX(axisX, healthIndexSeries);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText(tr("BMI(%)"));
    axisY->setTitleFont(font);
    axisY->setTickCount(16);
    healthIndexChart->setAxisY(axisY, healthIndexSeries);

    healthIndexChartView->setChart(healthIndexChart);
    healthIndexChartView->gender=gender;
    healthIndexChart->zoom(0.9);

    if(bmiTextItem!=NULL)
        healthIndexChartView->scene()->removeItem(bmiTextItem);
    bmiTextItem = new QGraphicsTextItem;
    bmiTextItem->setPos(900, 120);
    QTextDocument * textDocument = new QTextDocument(bmiTextItem);
    QFile file(tr(":/resource/html/bmi.html"));
    file.open(QIODevice::ReadOnly);
    textDocument->setHtml(file.readAll());
    file.close();
    bmiTextItem->setDocument(textDocument);
    bmiTextItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    QFont font3;
    font3.setPointSize(14);
    bmiTextItem->setFont(font3);
    healthIndexChartView->scene()->addItem(bmiTextItem);
}


