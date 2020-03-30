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
    qDebug()<<weightForecastData;
    ForecastWaist();
    qDebug()<<waistForecastData;

    qDebug()<<weightForecastData.count()<<waistForecastData.count();
    ForecastBMI();
    ForecastBFP();
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
        if(query.exec(cmd))
        {
            textBrowser->append(cmd);
            if(query.next())
            {
                //找到数据
                ageSectionScaleId.append(ageSectionAndScale[i][0]); //样本偏差
                ageDataCount.append(ageSectionAndScale[i][0]);
                ageSectionScaleId.append(ageSectionAndScale[i][1]); //所占比例
                ageDataCount.append(ageSectionAndScale[i][1]);
                ageSectionScaleId.append(query.size());             //数量
                ageDataCount.append(query.size());
                query.seek(-1);
                int count=0;
                while(query.next())
                {
                    //将数据的 分段 比例 数量 id 保存起来
                    ageSectionScaleId.append(query.value(0).toInt());
                    ageDataCount.append(query.value(2).toInt());
                    count++;
                }
                ageSectionScaleId[2] = count;
                ageDataCount[2] = count;
                //输出
                qDebug()<<ageSectionScaleId;
                qDebug()<<ageDataCount;
                break;
            }
            else
            {
                textBrowser->append(tr("没有找到可参考样本"));
                continue;   //找不到数据，以下一分段找寻数据
            }
        }
        else
        {
            textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
            return;
        }
    }
    //2.2 逐个取出样本，做预测
    textBrowser->append(tr("2.1逐个取出样本，做预测"));
    //找出最多有多少个数据点
    int maxDataCount = 0;
    for(int i=3; i<(ageDataCount.at(2)+3); i++)
    {
        if(ageDataCount.at(i)>maxDataCount)
            maxDataCount = ageDataCount.at(i);
    }
    textBrowser->append(tr("数据点数最大为%1").arg(maxDataCount));
    //申请空间存储各个点的数据量
    textBrowser->append(tr("申请%1个int存储各个点的数据量").arg(maxDataCount));
    int *ageEveryPointDataCount = new int[maxDataCount];
    for(int i=0; i<maxDataCount; i++)
        ageEveryPointDataCount[i] = 0;
    //计算各个点的数据量
    textBrowser->append(tr("计算每个预测点的数据量"));
    QString log;
    for(int i=3; i<(ageDataCount.at(2)+3); i++)    //遍历
    {
        //求出每个预测点的比例 预测1点有几个 预测N点有多少个
        for(int j=0; j<ageDataCount.at(i); j++)
        {
            ageEveryPointDataCount[j] = ageEveryPointDataCount[j]+1;
        }
    }
    log.clear();
    for(int i=0; i<maxDataCount; i++)
    {
        log.append(tr("%1 ").arg(ageEveryPointDataCount[i]));
    }
    textBrowser->append(log);
    //取出数据加到最终结果中
    //QVector<double> ageForecatDate;
    double *ageForecatDate = new double[maxDataCount];
    for(int i=0; i<maxDataCount; i++)
    {
        ageForecatDate[i] = 0;
    }
    for(int i=3; i<(ageSectionScaleId.at(2)+3); i++)
    {
        cmd = QString("select weight from archive%1").arg(ageSectionScaleId.at(i));
        textBrowser->append(tr("从archive%1中取出数据").arg(ageSectionScaleId.at(i)));
        if(query.exec(cmd))
        {
            double firstWeight;
            if(query.next())
                firstWeight = query.value(0).toDouble();
            query.seek(-1);
            int j=0;
            log.clear();
            while(query.next())
            {
                ageForecatDate[j] += ((firstWeight-query.value(0).toDouble())/firstWeight)/ageEveryPointDataCount[j];
                log.append(tr("%1 ").arg(((firstWeight-query.value(0).toDouble())/firstWeight)/ageEveryPointDataCount[j]));
                firstWeight = query.value(0).toDouble();
                j++;
            }
            textBrowser->append(log);
        }
        else
        {
            textBrowser->append(tr("SQL错误：%1").arg(cmd));
            return;
        }
    }
    //根据身高求出变化量
    textBrowser->append(tr("3.以身高为参考找寻匹配数据"));

    double heightSectionAndScale[5][2]     = {{2, 30}, {3, 26}, {5, 22}, { 7, 18}, { 8, 14}};

    QVector<int> heightSectionScaleId;
    QVector<int> heightDataCount;
    for(int i=0; i<5; i++)  //寻找可用数据
    {
        textBrowser->append(tr("寻找第%1分段数据样本，身高差为±%2，占比%3%").arg(i).arg(heightSectionAndScale[i][0]).arg(heightSectionAndScale[i][1]));
        cmd = QString(tr("select id, ABS(height - '%1'), datacount from user where (ABS(height - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=40) order by ABS(height - '%1') limit %6"))
                .arg(height)
                .arg(heightSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText())
                .arg(SampleSize);
        if(query.exec(cmd))
        {
            textBrowser->append(cmd);
            if(query.next())
            {
                //找到数据
                heightSectionScaleId.append(heightSectionAndScale[i][0]); //样本偏差
                heightDataCount.append(heightSectionAndScale[i][0]);
                heightSectionScaleId.append(heightSectionAndScale[i][1]); //所占比例
                heightDataCount.append(heightSectionAndScale[i][1]);
                heightSectionScaleId.append(query.size());             //数量
                heightDataCount.append(query.size());
                query.seek(-1);
                int count=0;
                while(query.next())
                {
                    //将数据的 分段 比例 数量 id 保存起来
                    heightSectionScaleId.append(query.value(0).toInt());
                    heightDataCount.append(query.value(2).toInt());
                    count++;
                }
                heightSectionScaleId[2] = count;
                heightDataCount[2] = count;
                //输出
                qDebug()<<heightSectionScaleId;
                qDebug()<<heightDataCount;
                break;
            }
            else
            {
                textBrowser->append(tr("没有找到可参考样本"));
                continue;   //找不到数据，以下一分段找寻数据
            }
        }
        else
        {
            textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
            return;
        }
    }
    //3.2 逐个取出样本，做预测
    textBrowser->append(tr("3.1逐个取出样本，做预测"));
    //找出最多有多少个数据点
    int max2DataCount = 0;
    for(int i=3; i<(heightDataCount.at(2)+3); i++)
    {
        if(heightDataCount.at(i)>max2DataCount)
            max2DataCount = heightDataCount.at(i);
    }
    textBrowser->append(tr("数据点数最大为%1").arg(max2DataCount));
    //申请空间存储各个点的数据量
    textBrowser->append(tr("申请%1个int存储各个点的数据量").arg(max2DataCount));
    int *heightEveryPointDataCount = new int[max2DataCount];
    for(int i=0; i<max2DataCount; i++)
        heightEveryPointDataCount[i] = 0;
    //计算各个点的数据量
    textBrowser->append(tr("计算每个预测点的数据量"));
    //QString log;
    log.clear();
    for(int i=3; i<(heightDataCount.at(2)+3); i++)    //遍历
    {
        //求出每个预测点的比例 预测1点有几个 预测N点有多少个
        for(int j=0; j<heightDataCount.at(i); j++)
        {
            heightEveryPointDataCount[j] = heightEveryPointDataCount[j]+1;
        }
    }
    log.clear();
    for(int i=0; i<max2DataCount; i++)
    {
        log.append(tr("%1 ").arg(heightEveryPointDataCount[i]));
    }
    textBrowser->append(log);
    //取出数据加到最终结果中
    //QVector<double> heightForecatDate;
    double *heightForecatDate = new double[max2DataCount];
    for(int i=0; i<max2DataCount; i++)
    {
        heightForecatDate[i] = 0;
    }
    for(int i=3; i<(heightSectionScaleId.at(2)+3); i++)
    {
        cmd = QString("select weight from archive%1").arg(heightSectionScaleId.at(i));
        textBrowser->append(tr("从archive%1中取出数据").arg(heightSectionScaleId.at(i)));
        if(query.exec(cmd))
        {
            double firstWeight;
            if(query.next())
                firstWeight = query.value(0).toDouble();
            query.seek(-1);
            int j=0;
            log.clear();
            while(query.next())
            {
                heightForecatDate[j] += ((firstWeight-query.value(0).toDouble())/firstWeight)/heightEveryPointDataCount[j];
                log.append(tr("%1 ").arg(((firstWeight-query.value(0).toDouble())/firstWeight)/heightEveryPointDataCount[j]));
                firstWeight = query.value(0).toDouble();
                j++;
            }
            textBrowser->append(log);
        }
        else
        {
            textBrowser->append(tr("SQL错误：%1").arg(cmd));
            return;
        }
    }

    //4 以体重找到1组数据
    textBrowser->append(tr("4.以体重为参考找寻匹配数据"));

    double weightSectionAndScale[5][2]        = {{3, 30}, {5, 26}, {7, 22}, {10, 18}, {15, 14}};

    QVector<int> weightSectionScaleId;
    QVector<int> weightDataCount;
    for(int i=0; i<5; i++)  //寻找可用数据
    {
        textBrowser->append(tr("寻找第%1分段数据样本，年龄差为±%2，占比%3%").arg(i).arg(weightSectionAndScale[i][0]).arg(weightSectionAndScale[i][1]));
        cmd = QString(tr("select id, ABS(weight - '%1'), datacount from user where (ABS(weight - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=40) order by ABS(weight - '%1') limit %6"))
                .arg(weight)
                .arg(weightSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText())
                .arg(SampleSize);
        if(query.exec(cmd))
        {
            if(query.next())
            {
                textBrowser->append(cmd);
                //找到数据
                weightSectionScaleId.append(weightSectionAndScale[i][0]); //样本偏差
                weightDataCount.append(weightSectionAndScale[i][0]);
                weightSectionScaleId.append(weightSectionAndScale[i][1]); //所占比例
                weightDataCount.append(weightSectionAndScale[i][1]);
                weightSectionScaleId.append(query.size());             //数量
                weightDataCount.append(query.size());
                query.seek(-1);
                int count=0;
                while(query.next())
                {
                    //将数据的 分段 比例 数量 id 保存起来
                    weightSectionScaleId.append(query.value(0).toInt());
                    weightDataCount.append(query.value(2).toInt());
                    count++;
                }
                weightSectionScaleId[2] = count;
                weightDataCount[2] = count;
                //输出
                qDebug()<<weightSectionScaleId;
                qDebug()<<weightDataCount;
                break;
            }
            else
            {
                textBrowser->append(tr("没有找到可参考样本"));
                continue;   //找不到数据，以下一分段找寻数据
            }
        }
        else
        {
            textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
            return;
        }
    }
    //2.2 逐个取出样本，做预测
    textBrowser->append(tr("4.1逐个取出样本，做预测"));
    //找出最多有多少个数据点
    int max3DataCount = 0;
    for(int i=3; i<(weightDataCount.at(2)+3); i++)
    {
        if(weightDataCount.at(i)>max3DataCount)
            max3DataCount = weightDataCount.at(i);
    }
    textBrowser->append(tr("数据点数最大为%1").arg(max3DataCount));
    //申请空间存储各个点的数据量
    textBrowser->append(tr("申请%1个int存储各个点的数据量").arg(max3DataCount));
    int *weightEveryPointDataCount = new int[max3DataCount];
    for(int i=0; i<max3DataCount; i++)
        weightEveryPointDataCount[i] = 0;
    //计算各个点的数据量
    textBrowser->append(tr("计算每个预测点的数据量"));
    //QString log;
    log.clear();
    for(int i=3; i<(weightDataCount.at(2)+3); i++)    //遍历
    {
        //求出每个预测点的比例 预测1点有几个 预测N点有多少个
        for(int j=0; j<weightDataCount.at(i); j++)
        {
            weightEveryPointDataCount[j] = weightEveryPointDataCount[j]+1;
        }
    }
    log.clear();
    for(int i=0; i<max3DataCount; i++)
    {
        log.append(tr("%1 ").arg(weightEveryPointDataCount[i]));
    }
    textBrowser->append(log);
    //取出数据加到最终结果中
    //QVector<double> weightForecatDate;
    double *weightForecatDate = new double[max3DataCount];
    for(int i=0; i<max3DataCount; i++)
    {
        weightForecatDate[i] = 0;
    }
    for(int i=3; i<(weightSectionScaleId.at(2)+3); i++)
    {
        cmd = QString("select weight from archive%1").arg(weightSectionScaleId.at(i));
        textBrowser->append(tr("从archive%1中取出数据").arg(weightSectionScaleId.at(i)));
        if(query.exec(cmd))
        {
            double firstWeight;
            if(query.next())
                firstWeight = query.value(0).toDouble();
            query.seek(-1);
            int j=0;
            log.clear();
            while(query.next())
            {
                weightForecatDate[j] += ((firstWeight-query.value(0).toDouble())/firstWeight)/weightEveryPointDataCount[j];
                log.append(tr("%1 ").arg(((firstWeight-query.value(0).toDouble())/firstWeight)/weightEveryPointDataCount[j]));
                firstWeight = query.value(0).toDouble();
                j++;
            }
            textBrowser->append(log);
        }
        else
        {
            textBrowser->append(tr("SQL错误：%1").arg(cmd));
            return;
        }
    }
#if 1
    //求出各个点的占比
    int max4DataCount=0;
    int maxDataCountArr[3];
    maxDataCountArr[0] = maxDataCount;
    maxDataCountArr[1] = max2DataCount;
    maxDataCountArr[2] = max3DataCount;
    for(int i=0; i<3; i++)
    {
        if(maxDataCountArr[i]>max4DataCount)
            max4DataCount = maxDataCountArr[i];
    }   //求出最大点数
    double *ageHeightWeightForestData = new double[max4DataCount];
    double *ageHeightWeightForestPointRate = new double[max4DataCount];
    for(int i=0; i<max4DataCount; i++)
    {
        ageHeightWeightForestData[i] = 0;
        ageHeightWeightForestPointRate[i] = 0;
    }

    for(int i=0; i<max4DataCount; i++)
    {
        if(i<maxDataCount)  //年龄预测的数据
        {
            ageHeightWeightForestData[i] += ageForecatDate[i]*ageDataCount[1]/100;
            ageHeightWeightForestPointRate[i] += ageDataCount[1];
        }
        if(i<max2DataCount)
        {
            ageHeightWeightForestData[i] += heightForecatDate[i]*heightDataCount[1]/100;
            ageHeightWeightForestPointRate[i] += heightDataCount[1];
        }
        if(i<max3DataCount)
        {
            ageHeightWeightForestData[i] += weightForecatDate[i]*weightDataCount[1]/100;
            ageHeightWeightForestPointRate[i] += weightDataCount[1];
        }
    }
    textBrowser->append(tr("5. 年龄 身高 体重三个参数预测点叠加 几个点比例数据"));
    log.clear();
    for(int i=0; i<max4DataCount; i++)
    {
        log.append(tr("%1 ").arg(ageHeightWeightForestData[i]));
    }
    textBrowser->append(log);
    log.clear();
    for(int i=0; i<max4DataCount; i++)
    {
        log.append(tr("%1 ").arg(ageHeightWeightForestPointRate[i]));
    }
    textBrowser->append(log);

#endif

    //4 以体重找到1组数据
    textBrowser->append(tr("5.以BMI为参考找寻匹配数据"));

    double bmiSectionAndScale[5][2]        = {{0.5, 30}, {1, 26}, {2, 22}, {3, 18}, {5, 14}};

    QVector<int> bmiSectionScaleId;
    QVector<int> bmiDataCount;
    for(int i=0; i<5; i++)  //寻找可用数据
    {
        textBrowser->append(tr("寻找第%1分段数据样本，BMI差为±%2，占比%3%").arg(i).arg(bmiSectionAndScale[i][0]).arg(bmiSectionAndScale[i][1]));
        cmd = QString(tr("select id, ABS(bmi - '%1'), datacount from user where (ABS(bmi - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=40) order by ABS(bmi - '%1') limit %6"))
                .arg(bmi)
                .arg(bmiSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText())
                .arg(SampleSize);
        if(query.exec(cmd))
        {
            textBrowser->append(cmd);
            if(query.next())
            {
                //找到数据
                bmiSectionScaleId.append(bmiSectionAndScale[i][0]); //样本偏差
                bmiDataCount.append(bmiSectionAndScale[i][0]);
                bmiSectionScaleId.append(bmiSectionAndScale[i][1]); //所占比例
                bmiDataCount.append(bmiSectionAndScale[i][1]);
                bmiSectionScaleId.append(query.size());             //数量
                bmiDataCount.append(query.size());
                query.seek(-1);
                int count=0;
                while(query.next())
                {
                    //将数据的 分段 比例 数量 id 保存起来
                    bmiSectionScaleId.append(query.value(0).toInt());
                    bmiDataCount.append(query.value(2).toInt());
                    count++;
                }
                bmiSectionScaleId[2] = count;
                bmiDataCount[2] = count;
                //输出
                qDebug()<<bmiSectionScaleId;
                qDebug()<<bmiDataCount;
                break;
            }
            else
            {
                textBrowser->append(tr("没有找到可参考样本"));
                continue;   //找不到数据，以下一分段找寻数据
            }
        }
        else
        {
            textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
            return;
        }
    }

    //2.2 逐个取出样本，做预测
    textBrowser->append(tr("4.1逐个取出样本，做预测"));
    //找出最多有多少个数据点
    int max5DataCount = 0;
    for(int i=3; i<(bmiDataCount.at(2)+3); i++)
    {
        if(bmiDataCount.at(i)>max5DataCount)
            max5DataCount = bmiDataCount.at(i);
    }
    textBrowser->append(tr("数据点数最大为%1").arg(max5DataCount));
    //申请空间存储各个点的数据量
    textBrowser->append(tr("申请%1个int存储各个点的数据量").arg(max5DataCount));
    int *bmiEveryPointDataCount = new int[max5DataCount];
    for(int i=0; i<max5DataCount; i++)
        bmiEveryPointDataCount[i] = 0;
    //计算各个点的数据量
    textBrowser->append(tr("计算每个预测点的数据量"));
    //QString log;
    log.clear();
    for(int i=3; i<(bmiDataCount.at(2)+3); i++)    //遍历
    {
        //求出每个预测点的比例 预测1点有几个 预测N点有多少个
        for(int j=0; j<bmiDataCount.at(i); j++)
        {
            bmiEveryPointDataCount[j] = bmiEveryPointDataCount[j]+1;
        }
    }
    log.clear();
    for(int i=0; i<max5DataCount; i++)
    {
        log.append(tr("%1 ").arg(bmiEveryPointDataCount[i]));
    }
    textBrowser->append(log);
    //取出数据加到最终结果中
    //QVector<double> bmiForecatDate;
    double *bmiForecatDate = new double[max5DataCount];
    for(int i=0; i<max5DataCount; i++)
    {
        bmiForecatDate[i] = 0;
    }
    for(int i=3; i<(bmiSectionScaleId.at(2)+3); i++)
    {
        cmd = QString("select weight from archive%1").arg(bmiSectionScaleId.at(i));
        textBrowser->append(tr("从archive%1中取出数据").arg(bmiSectionScaleId.at(i)));
        if(query.exec(cmd))
        {
            double firstbmi;
            if(query.next())
                firstbmi = query.value(0).toDouble();
            query.seek(-1);
            int j=0;
            log.clear();
            while(query.next())
            {
                bmiForecatDate[j] += ((firstbmi-query.value(0).toDouble())/firstbmi)/bmiEveryPointDataCount[j];
                log.append(tr("%1 ").arg(((firstbmi-query.value(0).toDouble())/firstbmi)/bmiEveryPointDataCount[j]));
                firstbmi = query.value(0).toDouble();
                j++;
            }
            textBrowser->append(log);
        }
        else
        {
            textBrowser->append(tr("SQL错误：%1").arg(cmd));
            return;
        }
    }

    //double *ageHeightWeightForestData = new double[max4DataCount];
    //double *ageHeightWeightForestPointRate = new double[max4DataCount];
    // max5DataCount BMI数据大小        bmiForecatDate
    // 四个参数做融合 （年龄 身高 体重） bmi 做融合
    // 求出样本最大长度
    int max6DataCount = max4DataCount;
    if(max6DataCount<max5DataCount)
    {
        max6DataCount = max5DataCount;
    }
    double *finalData = new double[max6DataCount];
    for(int i=0; i<max6DataCount; i++)
    {
        if((i<max4DataCount)&&(i<max5DataCount))   //（年龄 身高 体重） bmi都有数据
        {
            finalData[i] = ageHeightWeightForestData[i]*ageHeightWeightForestPointRate[i]/100 + bmiForecatDate[i]*(100-ageHeightWeightForestPointRate[i])/100;
        }
        else if((i>=max4DataCount)&&(i<max5DataCount))  //bmi有数据 则赞比100%
        {
            finalData[i] = bmiForecatDate[i];
        }
        else if((i<max4DataCount)&&(i>=max5DataCount))
        {
            finalData[i] = ageHeightWeightForestData[i]/ageHeightWeightForestPointRate[i]*100;  //按比例放大
        }
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
    double bmi=weight/2/height/height*10000;


    textBrowser->append(tr("1.用户基本信息: 姓名:%1 性别:%2 出生日期:%3 体重:%4 身高:%5 腰围:%6 目标体重:%7 bmi:%8")
                        .arg(chartUserNameLineEdit->text())
                        .arg(gender)
                        .arg(dateofbirth)
                        .arg(weight)
                        .arg(height)
                        .arg(waist)
                        .arg(targetWeight)
                        .arg(bmi));
#if 0
    QString dateofbirth;
    QString gender;
    double weight;
    double height;
    double waist;
    double targetWeight;
    double bmi;

    cmd = QString("select * from user where username='%1' and phonenumber='%2' and times='%3'")
            .arg(chartUserNameLineEdit->text())
            .arg(chartPhoneComboBox->currentText())
            .arg(chartTimesComboBox->currentText());
    if(query.exec(cmd))
    {
        if(query.next())
        {
            dateofbirth = query.value(3).toString();
            //age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
            gender = query.value(2).toString();
            weight = query.value(10).toDouble();
            height = query.value(5).toDouble();
            waist = query.value(11).toDouble();
            targetWeight = query.value(6).toDouble();
            bmi = query.value(12).toDouble();

            textBrowser->append(tr("1.用户基本信息: 姓名:%1 性别:%2 出生日期:%3 体重:%4 身高:%5 腰围:%6 目标体重:%7 bmi:%8")
                                .arg(chartUserNameLineEdit->text())
                                .arg(gender)
                                .arg(dateofbirth)
                                .arg(weight)
                                .arg(height)
                                .arg(waist)
                                .arg(targetWeight)
                                .arg(bmi));
        }
    }
    else
    {
        textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
        return;
    }
#endif
    //2.1 找出3条曲线, 与预测用户类似的
    /*
        1.	性别：同性别对比、参考
        2.	年龄：①±1  ②±2  ③±4  ④±6  ⑤±10       以年龄作为参数找3条曲线， 以年龄排序，找到附近的
        3.	身高(cm)：①±2 ②±3 ③±5 ④±7 ⑤±8        以身高作为参数找3条曲线   以身高排序，找到附近的
        5.	腰围(cm)：①±2 ②±4 ③±7 ④±10 ⑤±15

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
        if(query.exec(cmd))
        {
            textBrowser->append(cmd);
            if(query.next())
            {
                //找到数据
                ageSectionScaleId.append(ageSectionAndScale[i][0]); //样本偏差
                ageDataCount.append(ageSectionAndScale[i][0]);
                ageSectionScaleId.append(ageSectionAndScale[i][1]); //所占比例
                ageDataCount.append(ageSectionAndScale[i][1]);
                ageSectionScaleId.append(query.size());             //数量
                ageDataCount.append(query.size());
                query.seek(-1);
                int count=0;
                while(query.next())
                {
                    //将数据的 分段 比例 数量 id 保存起来
                    ageSectionScaleId.append(query.value(0).toInt());
                    ageDataCount.append(query.value(2).toInt());
                    count++;
                }
                ageSectionScaleId[2] = count;
                ageDataCount[2] = count;
                //输出
                qDebug()<<ageSectionScaleId;
                qDebug()<<ageDataCount;
                break;
            }
            else
            {
                textBrowser->append(tr("没有找到可参考样本"));
                continue;   //找不到数据，以下一分段找寻数据
            }
        }
        else
        {
            textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
            return;
        }
    }
    //2.2 逐个取出样本，做预测
    textBrowser->append(tr("2.1逐个取出样本，做预测"));
    //找出最多有多少个数据点
    int maxDataCount = 0;
    for(int i=3; i<(ageDataCount.at(2)+3); i++)
    {
        if(ageDataCount.at(i)>maxDataCount)
            maxDataCount = ageDataCount.at(i);
    }
    textBrowser->append(tr("数据点数最大为%1").arg(maxDataCount));
    //申请空间存储各个点的数据量
    textBrowser->append(tr("申请%1个int存储各个点的数据量").arg(maxDataCount));
    int *ageEveryPointDataCount = new int[maxDataCount];
    for(int i=0; i<maxDataCount; i++)
        ageEveryPointDataCount[i] = 0;
    //计算各个点的数据量
    textBrowser->append(tr("计算每个预测点的数据量"));
    QString log;
    for(int i=3; i<(ageDataCount.at(2)+3); i++)    //遍历
    {
        //求出每个预测点的比例 预测1点有几个 预测N点有多少个
        for(int j=0; j<ageDataCount.at(i); j++)
        {
            ageEveryPointDataCount[j] = ageEveryPointDataCount[j]+1;
        }
    }
    log.clear();
    for(int i=0; i<maxDataCount; i++)
    {
        log.append(tr("%1 ").arg(ageEveryPointDataCount[i]));
    }
    textBrowser->append(log);
    //取出数据加到最终结果中
    //QVector<double> ageForecatDate;
    double *ageForecatDate = new double[maxDataCount];
    for(int i=0; i<maxDataCount; i++)
    {
        ageForecatDate[i] = 0;
    }
    for(int i=3; i<(ageSectionScaleId.at(2)+3); i++)
    {
        cmd = QString("select waist from archive%1").arg(ageSectionScaleId.at(i));
        textBrowser->append(tr("从archive%1中取出数据").arg(ageSectionScaleId.at(i)));
        if(query.exec(cmd))
        {
            double firstWeight;
            if(query.next())
                firstWeight = query.value(0).toDouble();
            query.seek(-1);
            int j=0;
            log.clear();
            while(query.next())
            {
                ageForecatDate[j] += ((firstWeight-query.value(0).toDouble())/firstWeight)/ageEveryPointDataCount[j];
                log.append(tr("%1 ").arg(((firstWeight-query.value(0).toDouble())/firstWeight)/ageEveryPointDataCount[j]));
                firstWeight = query.value(0).toDouble();
                j++;
            }
            textBrowser->append(log);
        }
        else
        {
            textBrowser->append(tr("SQL错误：%1").arg(cmd));
            return;
        }
    }
    //根据身高求出变化量
    textBrowser->append(tr("3.以身高为参考找寻匹配数据"));

    double heightSectionAndScale[5][2]     = {{2, 30}, {3, 26}, {5, 22}, { 7, 18}, { 8, 14}};

    QVector<int> heightSectionScaleId;
    QVector<int> heightDataCount;
    for(int i=0; i<5; i++)  //寻找可用数据
    {
        textBrowser->append(tr("寻找第%1分段数据样本，身高差为±%2，占比%3%").arg(i).arg(heightSectionAndScale[i][0]).arg(heightSectionAndScale[i][1]));
        cmd = QString(tr("select id, ABS(height - '%1'), datacount from user where (ABS(height - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=40) order by ABS(height - '%1') limit %6"))
                .arg(height)
                .arg(heightSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText())
                .arg(SampleSize);
        if(query.exec(cmd))
        {
            textBrowser->append(cmd);
            if(query.next())
            {
                //找到数据
                heightSectionScaleId.append(heightSectionAndScale[i][0]); //样本偏差
                heightDataCount.append(heightSectionAndScale[i][0]);
                heightSectionScaleId.append(heightSectionAndScale[i][1]); //所占比例
                heightDataCount.append(heightSectionAndScale[i][1]);
                heightSectionScaleId.append(query.size());             //数量
                heightDataCount.append(query.size());
                query.seek(-1);
                int count=0;
                while(query.next())
                {
                    //将数据的 分段 比例 数量 id 保存起来
                    heightSectionScaleId.append(query.value(0).toInt());
                    heightDataCount.append(query.value(2).toInt());
                    count++;
                }
                heightSectionScaleId[2] = count;
                heightDataCount[2] = count;
                //输出
                qDebug()<<heightSectionScaleId;
                qDebug()<<heightDataCount;
                break;
            }
            else
            {
                textBrowser->append(tr("没有找到可参考样本"));
                continue;   //找不到数据，以下一分段找寻数据
            }
        }
        else
        {
            textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
            return;
        }
    }
    //3.2 逐个取出样本，做预测
    textBrowser->append(tr("3.1逐个取出样本，做预测"));
    //找出最多有多少个数据点
    int max2DataCount = 0;
    for(int i=3; i<(heightDataCount.at(2)+3); i++)
    {
        if(heightDataCount.at(i)>max2DataCount)
            max2DataCount = heightDataCount.at(i);
    }
    textBrowser->append(tr("数据点数最大为%1").arg(max2DataCount));
    //申请空间存储各个点的数据量
    textBrowser->append(tr("申请%1个int存储各个点的数据量").arg(max2DataCount));
    int *heightEveryPointDataCount = new int[max2DataCount];
    for(int i=0; i<max2DataCount; i++)
        heightEveryPointDataCount[i] = 0;
    //计算各个点的数据量
    textBrowser->append(tr("计算每个预测点的数据量"));
    //QString log;
    log.clear();
    for(int i=3; i<(heightDataCount.at(2)+3); i++)    //遍历
    {
        //求出每个预测点的比例 预测1点有几个 预测N点有多少个
        for(int j=0; j<heightDataCount.at(i); j++)
        {
            heightEveryPointDataCount[j] = heightEveryPointDataCount[j]+1;
        }
    }
    log.clear();
    for(int i=0; i<max2DataCount; i++)
    {
        log.append(tr("%1 ").arg(heightEveryPointDataCount[i]));
    }
    textBrowser->append(log);
    //取出数据加到最终结果中
    //QVector<double> heightForecatDate;
    double *heightForecatDate = new double[max2DataCount];
    for(int i=0; i<max2DataCount; i++)
    {
        heightForecatDate[i] = 0;
    }
    for(int i=3; i<(heightSectionScaleId.at(2)+3); i++)
    {
        cmd = QString("select waist from archive%1").arg(heightSectionScaleId.at(i));
        textBrowser->append(tr("从archive%1中取出数据").arg(heightSectionScaleId.at(i)));
        if(query.exec(cmd))
        {
            double firstWeight;
            if(query.next())
                firstWeight = query.value(0).toDouble();
            query.seek(-1);
            int j=0;
            log.clear();
            while(query.next())
            {
                heightForecatDate[j] += ((firstWeight-query.value(0).toDouble())/firstWeight)/heightEveryPointDataCount[j];
                log.append(tr("%1 ").arg(((firstWeight-query.value(0).toDouble())/firstWeight)/heightEveryPointDataCount[j]));
                firstWeight = query.value(0).toDouble();
                j++;
            }
            textBrowser->append(log);
        }
        else
        {
            textBrowser->append(tr("SQL错误：%1").arg(cmd));
            return;
        }
    }

    //4 以体重找到1组数据
    textBrowser->append(tr("4.以体重为参考找寻匹配数据"));

    double weightSectionAndScale[5][2]        = {{2, 30}, {4, 26}, {7, 22}, {10, 18}, {15, 14}};

    QVector<int> weightSectionScaleId;
    QVector<int> weightDataCount;
    for(int i=0; i<5; i++)  //寻找可用数据
    {
        textBrowser->append(tr("寻找第%1分段数据样本，年龄差为±%2，占比%3%").arg(i).arg(weightSectionAndScale[i][0]).arg(weightSectionAndScale[i][1]));
        cmd = QString(tr("select id, ABS(waist - '%1'), datacount from user where (ABS(waist - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=40) order by ABS(waist - '%1') limit %6"))
                .arg(waist)
                .arg(weightSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText())
                .arg(SampleSize);
        if(query.exec(cmd))
        {
            if(query.next())
            {
                textBrowser->append(cmd);
                //找到数据
                weightSectionScaleId.append(weightSectionAndScale[i][0]); //样本偏差
                weightDataCount.append(weightSectionAndScale[i][0]);
                weightSectionScaleId.append(weightSectionAndScale[i][1]); //所占比例
                weightDataCount.append(weightSectionAndScale[i][1]);
                weightSectionScaleId.append(query.size());             //数量
                weightDataCount.append(query.size());
                query.seek(-1);
                int count=0;
                while(query.next())
                {
                    //将数据的 分段 比例 数量 id 保存起来
                    weightSectionScaleId.append(query.value(0).toInt());
                    weightDataCount.append(query.value(2).toInt());
                    count++;
                }
                weightSectionScaleId[2] = count;
                weightDataCount[2] = count;
                //输出
                qDebug()<<weightSectionScaleId;
                qDebug()<<weightDataCount;
                break;
            }
            else
            {
                textBrowser->append(tr("没有找到可参考样本"));
                continue;   //找不到数据，以下一分段找寻数据
            }
        }
        else
        {
            textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
            return;
        }
    }
    //2.2 逐个取出样本，做预测
    textBrowser->append(tr("4.1逐个取出样本，做预测"));
    //找出最多有多少个数据点
    int max3DataCount = 0;
    for(int i=3; i<(weightDataCount.at(2)+3); i++)
    {
        if(weightDataCount.at(i)>max3DataCount)
            max3DataCount = weightDataCount.at(i);
    }
    textBrowser->append(tr("数据点数最大为%1").arg(max3DataCount));
    //申请空间存储各个点的数据量
    textBrowser->append(tr("申请%1个int存储各个点的数据量").arg(max3DataCount));
    int *weightEveryPointDataCount = new int[max3DataCount];
    for(int i=0; i<max3DataCount; i++)
        weightEveryPointDataCount[i] = 0;
    //计算各个点的数据量
    textBrowser->append(tr("计算每个预测点的数据量"));
    //QString log;
    log.clear();
    for(int i=3; i<(weightDataCount.at(2)+3); i++)    //遍历
    {
        //求出每个预测点的比例 预测1点有几个 预测N点有多少个
        for(int j=0; j<weightDataCount.at(i); j++)
        {
            weightEveryPointDataCount[j] = weightEveryPointDataCount[j]+1;
        }
    }
    log.clear();
    for(int i=0; i<max3DataCount; i++)
    {
        log.append(tr("%1 ").arg(weightEveryPointDataCount[i]));
    }
    textBrowser->append(log);
    //取出数据加到最终结果中
    //QVector<double> weightForecatDate;
    double *weightForecatDate = new double[max3DataCount];
    for(int i=0; i<max3DataCount; i++)
    {
        weightForecatDate[i] = 0;
    }
    for(int i=3; i<(weightSectionScaleId.at(2)+3); i++)
    {
        cmd = QString("select waist from archive%1").arg(weightSectionScaleId.at(i));
        textBrowser->append(tr("从archive%1中取出数据").arg(weightSectionScaleId.at(i)));
        if(query.exec(cmd))
        {
            double firstWeight;
            if(query.next())
                firstWeight = query.value(0).toDouble();
            query.seek(-1);
            int j=0;
            log.clear();
            while(query.next())
            {
                weightForecatDate[j] += ((firstWeight-query.value(0).toDouble())/firstWeight)/weightEveryPointDataCount[j];
                log.append(tr("%1 ").arg(((firstWeight-query.value(0).toDouble())/firstWeight)/weightEveryPointDataCount[j]));
                firstWeight = query.value(0).toDouble();
                j++;
            }
            textBrowser->append(log);
        }
        else
        {
            textBrowser->append(tr("SQL错误：%1").arg(cmd));
            return;
        }
    }
#if 1
    //求出各个点的占比
    int max4DataCount=0;
    int maxDataCountArr[3];
    maxDataCountArr[0] = maxDataCount;
    maxDataCountArr[1] = max2DataCount;
    maxDataCountArr[2] = max3DataCount;
    for(int i=0; i<3; i++)
    {
        if(maxDataCountArr[i]>max4DataCount)
            max4DataCount = maxDataCountArr[i];
    }   //求出最大点数
    double *ageHeightWeightForestData = new double[max4DataCount];
    double *ageHeightWeightForestPointRate = new double[max4DataCount];
    for(int i=0; i<max4DataCount; i++)
    {
        ageHeightWeightForestData[i] = 0;
        ageHeightWeightForestPointRate[i] = 0;
    }

    for(int i=0; i<max4DataCount; i++)
    {
        if(i<maxDataCount)  //年龄预测的数据
        {
            ageHeightWeightForestData[i] += ageForecatDate[i]*ageDataCount[1]/100;
            ageHeightWeightForestPointRate[i] += ageDataCount[1];
        }
        if(i<max2DataCount)
        {
            ageHeightWeightForestData[i] += heightForecatDate[i]*heightDataCount[1]/100;
            ageHeightWeightForestPointRate[i] += heightDataCount[1];
        }
        if(i<max3DataCount)
        {
            ageHeightWeightForestData[i] += weightForecatDate[i]*weightDataCount[1]/100;
            ageHeightWeightForestPointRate[i] += weightDataCount[1];
        }
    }
    textBrowser->append(tr("5. 年龄 身高 体重三个参数预测点叠加 几个点比例数据"));
    log.clear();
    for(int i=0; i<max4DataCount; i++)
    {
        log.append(tr("%1 ").arg(ageHeightWeightForestData[i]));
    }
    textBrowser->append(log);
    log.clear();
    for(int i=0; i<max4DataCount; i++)
    {
        log.append(tr("%1 ").arg(ageHeightWeightForestPointRate[i]));
    }
    textBrowser->append(log);

#endif

    //4 以体重找到1组数据
    textBrowser->append(tr("5.以BMI为参考找寻匹配数据"));

    double bmiSectionAndScale[5][2]        = {{0.5, 30}, {1, 26}, {2, 22}, {3, 18}, {5, 14}};

    QVector<int> bmiSectionScaleId;
    QVector<int> bmiDataCount;
    for(int i=0; i<5; i++)  //寻找可用数据
    {
        textBrowser->append(tr("寻找第%1分段数据样本，BMI差为±%2，占比%3%").arg(i).arg(bmiSectionAndScale[i][0]).arg(bmiSectionAndScale[i][1]));
        cmd = QString(tr("select id, ABS(bmi - '%1'), datacount from user where (ABS(bmi - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=40) order by ABS(bmi - '%1') limit %6"))
                .arg(bmi)
                .arg(bmiSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText())
                .arg(SampleSize);
        if(query.exec(cmd))
        {
            textBrowser->append(cmd);
            if(query.next())
            {
                //找到数据
                bmiSectionScaleId.append(bmiSectionAndScale[i][0]); //样本偏差
                bmiDataCount.append(bmiSectionAndScale[i][0]);
                bmiSectionScaleId.append(bmiSectionAndScale[i][1]); //所占比例
                bmiDataCount.append(bmiSectionAndScale[i][1]);
                bmiSectionScaleId.append(query.size());             //数量
                bmiDataCount.append(query.size());
                query.seek(-1);
                int count=0;
                while(query.next())
                {
                    //将数据的 分段 比例 数量 id 保存起来
                    bmiSectionScaleId.append(query.value(0).toInt());
                    bmiDataCount.append(query.value(2).toInt());
                    count++;
                }
                bmiSectionScaleId[2] = count;
                bmiDataCount[2] = count;
                //输出
                qDebug()<<bmiSectionScaleId;
                qDebug()<<bmiDataCount;
                break;
            }
            else
            {
                textBrowser->append(tr("没有找到可参考样本"));
                continue;   //找不到数据，以下一分段找寻数据
            }
        }
        else
        {
            textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
            return;
        }
    }

    //2.2 逐个取出样本，做预测
    textBrowser->append(tr("4.1逐个取出样本，做预测"));
    //找出最多有多少个数据点
    int max5DataCount = 0;
    for(int i=3; i<(bmiDataCount.at(2)+3); i++)
    {
        if(bmiDataCount.at(i)>max5DataCount)
            max5DataCount = bmiDataCount.at(i);
    }
    textBrowser->append(tr("数据点数最大为%1").arg(max5DataCount));
    //申请空间存储各个点的数据量
    textBrowser->append(tr("申请%1个int存储各个点的数据量").arg(max5DataCount));
    int *bmiEveryPointDataCount = new int[max5DataCount];
    for(int i=0; i<max5DataCount; i++)
        bmiEveryPointDataCount[i] = 0;
    //计算各个点的数据量
    textBrowser->append(tr("计算每个预测点的数据量"));
    //QString log;
    log.clear();
    for(int i=3; i<(bmiDataCount.at(2)+3); i++)    //遍历
    {
        //求出每个预测点的比例 预测1点有几个 预测N点有多少个
        for(int j=0; j<bmiDataCount.at(i); j++)
        {
            bmiEveryPointDataCount[j] = bmiEveryPointDataCount[j]+1;
        }
    }
    log.clear();
    for(int i=0; i<max5DataCount; i++)
    {
        log.append(tr("%1 ").arg(bmiEveryPointDataCount[i]));
    }
    textBrowser->append(log);
    //取出数据加到最终结果中
    //QVector<double> bmiForecatDate;
    double *bmiForecatDate = new double[max5DataCount];
    for(int i=0; i<max5DataCount; i++)
    {
        bmiForecatDate[i] = 0;
    }
    for(int i=3; i<(bmiSectionScaleId.at(2)+3); i++)
    {
        cmd = QString("select waist from archive%1").arg(bmiSectionScaleId.at(i));
        textBrowser->append(tr("从archive%1中取出数据").arg(bmiSectionScaleId.at(i)));
        if(query.exec(cmd))
        {
            double firstbmi;
            if(query.next())
                firstbmi = query.value(0).toDouble();
            query.seek(-1);
            int j=0;
            log.clear();
            while(query.next())
            {
                bmiForecatDate[j] += ((firstbmi-query.value(0).toDouble())/firstbmi)/bmiEveryPointDataCount[j];
                log.append(tr("%1 ").arg(((firstbmi-query.value(0).toDouble())/firstbmi)/bmiEveryPointDataCount[j]));
                firstbmi = query.value(0).toDouble();
                j++;
            }
            textBrowser->append(log);
        }
        else
        {
            textBrowser->append(tr("SQL错误：%1").arg(cmd));
            return;
        }
    }

    //double *ageHeightWeightForestData = new double[max4DataCount];
    //double *ageHeightWeightForestPointRate = new double[max4DataCount];
    // max5DataCount BMI数据大小        bmiForecatDate
    // 四个参数做融合 （年龄 身高 体重） bmi 做融合
    // 求出样本最大长度
    int max6DataCount = max4DataCount;
    if(max6DataCount<max5DataCount)
    {
        max6DataCount = max5DataCount;
    }
    double *finalData = new double[max6DataCount];
    for(int i=0; i<max6DataCount; i++)
    {
        if((i<max4DataCount)&&(i<max5DataCount))   //（年龄 身高 体重） bmi都有数据
        {
            finalData[i] = ageHeightWeightForestData[i]*ageHeightWeightForestPointRate[i]/100 + bmiForecatDate[i]*(100-ageHeightWeightForestPointRate[i])/100;
        }
        else if((i>=max4DataCount)&&(i<max5DataCount))  //bmi有数据 则赞比100%
        {
            finalData[i] = bmiForecatDate[i];
        }
        else if((i<max4DataCount)&&(i>=max5DataCount))
        {
            finalData[i] = ageHeightWeightForestData[i]/ageHeightWeightForestPointRate[i]*100;  //按比例放大
        }
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
    double firstWaist = waist;
    waistForecastData.clear();
    for(int i=0; i<max6DataCount; i++)
    {
        waistSeries->append(currentTime+(24*60*60*1000*i), firstWaist*(1-finalData[i]));
        waistScatterSeries->append(currentTime+(24*60*60*1000*i), firstWaist*(1-finalData[i]));
        //if(firstWaist*(1-finalData[i])<targetWeight)
        //    break;
        waistForecastData.append(firstWaist);
        if(i>=weightForecastData.count()-1)
            break;
        firstWaist = firstWaist*(1-finalData[i]);
    }

    textBrowser->append(tr("如果以今天为起点做预测，得出数据点"));
    textBrowser->append(tr("时间\t预计腰围"));
/*
    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    for(int i=0; i<finalData.count(); i++)
    {
        waistSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
        waistScatterSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));

        textBrowser->append(tr("%1\t%2").arg(QDateTime::fromMSecsSinceEpoch(currentTime+(24*60*60*1000*i)).toString("yyyy-MM-dd")).arg(finalData.at(i)));
    }
*/
    textBrowser->append(tr("6.生成曲线"));
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
#if 0
    QSqlQuery query;
    QString cmd;

    QString dateofbirth;
    QString gender;
    double weight;
    double height;
    double waist;
    double targetWeight;
    double bmi;
    int age;

    cmd = QString("select * from user where username='%1' and phonenumber='%2' and times='%3'")
           .arg(chartUserNameLineEdit->text())
           .arg(chartPhoneComboBox->currentText())
           .arg(chartTimesComboBox->currentText());
    if(query.exec(cmd))
    {
       if(query.next())
       {
           dateofbirth = query.value(3).toString();
           age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
           gender = query.value(2).toString();
           weight = query.value(10).toDouble();
           height = query.value(5).toDouble();
           waist = query.value(11).toDouble();
           targetWeight = query.value(6).toDouble();
           bmi = query.value(12).toDouble();

           textBrowser->append(tr("1.用户基本信息: 姓名:%1 性别:%2 出生日期:%3 体重:%4 身高:%5 腰围:%6 目标体重:%7 bmi:%8")
                               .arg(chartUserNameLineEdit->text())
                               .arg(gender)
                               .arg(dateofbirth)
                               .arg(weight)
                               .arg(height)
                               .arg(waist)
                               .arg(targetWeight)
                               .arg(bmi));
       }
    }
    else
    {
       textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
       return;
    }
#endif
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

    textBrowser->append(tr("如果以今天为起点做预测，得出数据点"));
    textBrowser->append(tr("时间\t预计体脂率"));
/*
    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    for(int i=0; i<finalData.count(); i++)
    {
        waistSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
        waistScatterSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));

        textBrowser->append(tr("%1\t%2").arg(QDateTime::fromMSecsSinceEpoch(currentTime+(24*60*60*1000*i)).toString("yyyy-MM-dd")).arg(finalData.at(i)));
    }
*/
    textBrowser->append(tr("6.生成曲线"));
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
    //textItem->setZValue(1);
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
#if 0
    QSqlQuery query;
    QString cmd;

    QString dateofbirth;
    QString gender;
    double weight;
    double height;
    double waist;
    double targetWeight;
    double bmi;
    int age;

    cmd = QString("select * from user where username='%1' and phonenumber='%2' and times='%3'")
           .arg(chartUserNameLineEdit->text())
           .arg(chartPhoneComboBox->currentText())
           .arg(chartTimesComboBox->currentText());
    if(query.exec(cmd))
    {
       if(query.next())
       {
           dateofbirth = query.value(3).toString();
           age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();
           gender = query.value(2).toString();
           weight = query.value(10).toDouble();
           height = query.value(5).toDouble();
           waist = query.value(11).toDouble();
           targetWeight = query.value(6).toDouble();
           bmi = query.value(12).toDouble();

           textBrowser->append(tr("1.用户基本信息: 姓名:%1 性别:%2 出生日期:%3 体重:%4 身高:%5 腰围:%6 目标体重:%7 bmi:%8")
                               .arg(chartUserNameLineEdit->text())
                               .arg(gender)
                               .arg(dateofbirth)
                               .arg(weight)
                               .arg(height)
                               .arg(waist)
                               .arg(targetWeight)
                               .arg(bmi));
       }
    }
    else
    {
       textBrowser->append(tr("SQL执行错误 %1").arg(cmd));
       return;
    }
#endif
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

    textBrowser->append(tr("如果以今天为起点做预测，得出数据点"));
    textBrowser->append(tr("时间\t预计体脂率"));
/*
    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    for(int i=0; i<finalData.count(); i++)
    {
        waistSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
        waistScatterSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));

        textBrowser->append(tr("%1\t%2").arg(QDateTime::fromMSecsSinceEpoch(currentTime+(24*60*60*1000*i)).toString("yyyy-MM-dd")).arg(finalData.at(i)));
    }
*/
    textBrowser->append(tr("6.生成曲线"));
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
    //textItem->setZValue(1);
    bmiTextItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    QFont font3;
    font3.setPointSize(14);
    bmiTextItem->setFont(font3);
    healthIndexChartView->scene()->addItem(bmiTextItem);
    //qDebug()<<bmiForecastData;
}

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


