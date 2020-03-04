#include "forecastwidget.h"
#include <QDebug>

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
    //textBrowser->show();
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

    QPushButton * queryButton = new QPushButton(tr("预测"), widget);
    connect(queryButton, &QPushButton::clicked, this, &ForecastWidget::QueryPushButtonClicked);

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

    mainLayout->addLayout(archiveLayout, 0, 0, 1, 2);
    mainLayout->addWidget(weightChartView, 1, 0, 1, 1);
    mainLayout->addWidget(waistChartView, 1, 1, 1, 1);
    mainLayout->addWidget(bodyFatRateChartView, 2, 1, 1, 1);
    mainLayout->addWidget(healthIndexChartView, 2, 0, 1, 1);

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
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), cmd);
    }
}

void ForecastWidget::QueryPushButtonClicked()
{
    qDebug()<<"ForecastWidget::QueryPushButtonClicked";
    textBrowser->clear();
    //1.新建曲线
    //weightSeries = new QLineSeries();
    //weightScatterSeries = new QScatterSeries();

    //2.得出预测数据并插入到曲线中
    //2.0 找出这个用户的基本信息
    QSqlQuery query;
    QString cmd;

    textBrowser->show();

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
    //double heightSectionAndScale[5][2]     = {{2, 40}, {3, 36}, {5, 32}, { 7, 28}, { 8, 24}};
    //double weightSectionAndScale[5][2]     = {{3, 40}, {5, 36}, {7, 32}, {10, 28}, {15, 24}};
    //double bmiSectionAndScale[5][2]        = {{0.5, 40}, {1, 36}, {2, 32}, {3, 28}, {5, 24}};

    QVector<int> ageSectionScaleId;
    QVector<int> ageDataCount;
    for(int i=0; i<5; i++)  //寻找可用数据
    {
        textBrowser->append(tr("寻找第%1分段数据样本，年龄差为±%2，占比%3%").arg(i).arg(ageSectionAndScale[i][0]).arg(ageSectionAndScale[i][1]));
        cmd = QString(tr("select id, ABS(dateofbirth - '%1'), datacount from user where (ABS(dateofbirth - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=15) order by ABS(dateofbirth - '%1') limit 50"))
                .arg(dateofbirth)
                .arg(ageSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText());
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
        cmd = QString(tr("select id, ABS(height - '%1'), datacount from user where (ABS(height - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=15) order by ABS(height - '%1') limit 50"))
                .arg(height)
                .arg(heightSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText());
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
        cmd = QString(tr("select id, ABS(weight - '%1'), datacount from user where (ABS(weight - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=15) order by ABS(weight - '%1') limit 50"))
                .arg(weight)
                .arg(weightSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText());
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
        cmd = QString(tr("select id, ABS(bmi - '%1'), datacount from user where (ABS(bmi - '%1')<=%2 and gender='%3' and username!='%4' and phonenumber!='%5' and (datacount-'0')>=15) order by ABS(bmi - '%1') limit 50"))
                .arg(bmi)
                .arg(bmiSectionAndScale[i][0])
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText());
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

    //3.显示曲线
    if(weightChartView->chart()!=NULL)
        weightChartView->chart()->removeAllSeries();
    weightSeries = new QLineSeries;
    weightScatterSeries = new QScatterSeries;

    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    double firstWeight = weight;
    for(int i=0; i<max6DataCount; i++)
    {
        weightSeries->append(currentTime+(24*60*60*1000*i), firstWeight*(1-finalData[i]));
        weightScatterSeries->append(currentTime+(24*60*60*1000*i), firstWeight*(1-finalData[i]));
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
    axisX->setTickCount(6);
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText(tr("时间"));
    //axisX->setTitleFont(font);
    weightChart->setAxisX(axisX, weightSeries);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText(tr("体重(斤)"));
    //axisY->setTitleFont(font);
    axisY->setTickCount(8);
    weightChart->setAxisY(axisY, weightSeries);

    //chart1.ChartAreas[0].AxisX.ScrollBar.Enabled = true;
    //chart1.ChartAreas[0].AxisX.ScrollBar.IsPositionedInside = true;
    //chart1.ChartAreas[0].AxisX.ScaleView.Size = 20;//可视区域数据点数

    weightChartView->setChart(weightChart);
    weightChart->zoom(0.9);
}

