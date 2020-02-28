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
        QMessageBox::critical(this, tr("错误"), cmd);
        return;
    }

    QList<double> data[4];
    int rate[4];
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

    cmd = QString(tr("select id, ABS(dateofbirth - '%1') from user where (ABS(dateofbirth - '%1')<=10 and gender='%2' and username!='%3' and phonenumber!='%4') order by ABS(dateofbirth - '%1') limit 1"))
            .arg(dateofbirth)
            .arg(gender)
            .arg(chartUserNameLineEdit->text())
            .arg(chartPhoneComboBox->currentText());
    if(query.exec(cmd))
    {
        int dataGroupNum=0;         //数据的组数
        int indexAndRate[2][3];     //数据的档案号和比例
        while(query.next())
        {
            //保存3组数据的档案编号以及所占的比例
            indexAndRate[0][dataGroupNum] = query.value(0).toInt();    //数据档案号
            if(query.value(1).toInt()<=1) //1 2 4 6 10
                indexAndRate[1][dataGroupNum] = 40;
            else if(query.value(1).toInt()<=2)
                indexAndRate[1][dataGroupNum] = 36;
            else if(query.value(1).toInt()<=4)
                indexAndRate[1][dataGroupNum] = 32;
            else if(query.value(1).toInt()<=6)
                indexAndRate[1][dataGroupNum] = 28;
            else if(query.value(1).toInt()<=10)
                indexAndRate[1][dataGroupNum] = 24;
            dataGroupNum++;

            textBrowser->append(tr("找到匹配数据:档案号为archive%1，年龄差为%2，预测数据占比%3")
                                .arg(query.value(0).toInt())
                                .arg(query.value(1).toInt())
                                .arg(indexAndRate[1][0]/100.0));
        }

        rate[0] = indexAndRate[1][0];

        if(dataGroupNum==0) //没有足够的样本
        {
            QMessageBox::critical(this, tr("没有足够的样本"), tr("没有足够的样本"));
            textBrowser->append(tr("没有找到匹配样本"));
            qDebug()<<cmd;
            return;
        }
        else
        {
            qDebug()<<tr("找到%1个样本").arg(dataGroupNum);
            for(int i=0; i<3; i++)
            {
                for(int j=0; j<2; j++)
                    qDebug()<<indexAndRate[j][i];
            }
        }

        QList<double> dataGroupValue[3];

        //有样本可以进行
        for(int i=0; i<dataGroupNum; i++)
        {
            dataGroupValue[i].append(weight);   //插入初始体重

            textBrowser->append(tr("样本值\t   预测值"));

            cmd = QString(tr("select weight from archive%1 order by datetime")).arg(indexAndRate[0][i]);
            if(query.exec(cmd))
            {
                double firstWeight;
                if(query.next())
                {
                    firstWeight = query.value(0).toDouble();     //得到了初始值
                    textBrowser->append(tr("%1\t%2").arg(firstWeight).arg(weight));
                }
                while(query.next())
                {
                    //预测出减少后的体重
                    dataGroupValue[i].append((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight);
                    data[0].append((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight);
                    textBrowser->append(tr("%1\t%2").arg(query.value(0).toDouble()).arg((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight));
                }
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), cmd);
                return;
            }
        }
        qDebug()<<dataGroupValue[0];
        //数据已经存在 QList<double> dataGroupValue[3]; 作平均
        //indexAndRate 中存放了每条曲线所占的比例  例如 40 36 3
    }
    else
    {
        QMessageBox::critical(this, tr("错误"), cmd);
        qDebug()<<cmd;
        return;
    }
    //2.1.2 以身高为参数找出一条曲线
    textBrowser->append(tr("3.以身高为参考找寻匹配数据"));
    cmd = QString(tr("select id, ABS(height - '%1') from user where (ABS(height - '%1')<=8 and gender='%2' and username!='%3' and phonenumber!='%4') order by ABS(height - '%1') limit 1"))
            .arg(height)
            .arg(gender)
            .arg(chartUserNameLineEdit->text())
            .arg(chartPhoneComboBox->currentText());
    if(query.exec(cmd))
    {
        if(query.next())
        {
            //找出档案号和比例
            //indexAndRate[0] = query.value(0).toInt();
            if(query.value(1).toDouble()<=2)
                rate[1] = 30;
            else if(query.value(1).toDouble()<=3)
                rate[1] = 26;
            else if(query.value(1).toDouble()<=5)
                rate[1] = 22;
            else if(query.value(1).toDouble()<=7)
                rate[1] = 18;
            else if(query.value(1).toDouble()<=8)
                rate[1] = 14;

            textBrowser->append(tr("找到匹配数据:档案号为archive%1，年龄差为%2，预测数据占比%3")
                                .arg(query.value(0).toInt())
                                .arg(query.value(1).toInt())
                                .arg(rate[1]/100.0));

            cmd = QString(tr("select weight from archive%1 order by datetime")).arg(query.value(0).toInt());
            if(query.exec(cmd))
            {
                double firstWeight;
                if(query.next())
                {
                    firstWeight = query.value(0).toDouble();     //得到了初始值
                }

                textBrowser->append(tr("样本值\t   预测值"));

                while(query.next())
                {
                    //预测出减少后的体重
                    data[1].append((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight);
                    textBrowser->append(tr("%1\t%2").arg(query.value(0).toDouble()).arg((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight));
                }
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), cmd);
                return;
            }
        }
        else
        {
            QMessageBox::information(this, tr("样本不足"), tr("以身高为参数找不到合适样本"));
            textBrowser->append(tr("以身高为参数找不到合适样本"));
            return;
        }
    }
    else
    {
        QMessageBox::critical(this, tr("错误"), cmd);
        qDebug()<<cmd;
        return;
    }

    //2.1.3 以体重为参数找出一条曲线
    textBrowser->append(tr("3.以体重为参考找寻匹配数据"));
    cmd = QString(tr("select id, ABS(weight - '%1') from user where (ABS(weight - '%1')<=8 and gender='%2' and username!='%3' and phonenumber!='%4') order by ABS(weight - '%1') limit 1"))
            .arg(weight)
            .arg(gender)
            .arg(chartUserNameLineEdit->text())
            .arg(chartPhoneComboBox->currentText());
    if(query.exec(cmd))
    {
        if(query.next())
        {
            //找出档案号和比例
            //indexAndRate[0] = query.value(0).toInt();
            if(query.value(1).toDouble()<=3)
                rate[2] = 30;
            else if(query.value(1).toDouble()<=5)
                rate[2] = 26;
            else if(query.value(1).toDouble()<=7)
                rate[2] = 22;
            else if(query.value(1).toDouble()<=10)
                rate[2] = 18;
            else if(query.value(1).toDouble()<=15)
                rate[2] = 14;

            textBrowser->append(tr("找到匹配数据:档案号为archive%1，年龄差为%2，预测数据占比%3")
                                .arg(query.value(0).toInt())
                                .arg(query.value(1).toInt())
                                .arg(rate[2]/100.0));

            cmd = QString(tr("select weight from archive%1 order by datetime")).arg(query.value(0).toInt());
            if(query.exec(cmd))
            {
                double firstWeight;
                if(query.next())
                {
                    firstWeight = query.value(0).toDouble();     //得到了初始值
                }
                textBrowser->append(tr("样本值\t   预测值"));
                while(query.next())
                {
                    //预测出减少后的体重
                    data[2].append((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight);
                    textBrowser->append(tr("%1\t%2").arg(query.value(0).toDouble()).arg((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight));
                }
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), cmd);
                return;
            }
        }
        else
        {
            QMessageBox::information(this, tr("样本不足"), tr("以身高为参数找不到合适样本"));
            textBrowser->append(tr("以体重为参数找不到合适样本"));
            return;
        }
    }
    else
    {
        QMessageBox::critical(this, tr("错误"), cmd);
        qDebug()<<cmd;
        return;
    }

    QList<double> finalData;
    if(rate[0]+rate[1]+rate[2] == 100)  //如果为 40 30 30 则无需进行BMI运算
    {
        textBrowser->append(tr("4.体重、身高、年龄占比之和达到%100"));
        // 找到最短的长度
        textBrowser->append(tr("5.数据按占比结合，得出最终数据"));
        textBrowser->append(tr("年龄占比%1，身高占比%2，体重占比%3").arg(rate[0]).arg(rate[1]).arg(rate[2]));
        int minCount;
        minCount = data[0].count();
        for(int i=1; i<3; i++)
        {
            if(data[i].count()<minCount)
            {
                minCount = data[i].count();
            }
        }
        // 按比例得出最终数据
        for(int i=0; i<minCount; i++)
            finalData.append(data[0].at(i)*rate[0]/100 + data[1].at(i)*rate[1]/100 + data[2].at(i)*rate[2]/100);
    }
    else
    {
        rate[3] = 100-rate[0]-rate[1]-rate[2];

        textBrowser->append(tr("4.体重、身高、年龄占比之和为%1%，由BMI做数据补充占比为%2%").arg(100-rate[0]-rate[1]-rate[2]).arg(rate[3]));
        //2.1.4 以bmi为参数找出一条曲线
        cmd = QString(tr("select id, ABS(bmi - '%1') from user where (ABS(bmi - '%1')<=8 and gender='%2' and username!='%3' and phonenumber!='%4') order by ABS(bmi - '%1') limit 1"))
                .arg(bmi)
                .arg(gender)
                .arg(chartUserNameLineEdit->text())
                .arg(chartPhoneComboBox->currentText());
        if(query.exec(cmd))
        {
            if(query.next())
            {
                //找出档案号和比例
                //indexAndRate[0] = query.value(0).toInt();
//                if(query.value(1).toDouble()<=0.5)
//                    rate[3] = 30;
//                else if(query.value(1).toDouble()<=1)
//                    rate[3] = 26;
//                else if(query.value(1).toDouble()<=2)
//                    rate[3] = 22;
//                else if(query.value(1).toDouble()<=3)
//                    rate[3] = 18;
//                else if(query.value(1).toDouble()<=5)
//                    rate[3] = 14;
                textBrowser->append(tr("找到匹配数据:档案号为archive%1，年龄差为%2，预测数据占比%3")
                                    .arg(query.value(0).toInt())
                                    .arg(query.value(1).toInt())
                                    .arg(rate[3]/100.0));

                cmd = QString(tr("select weight from archive%1 order by datetime")).arg(query.value(0).toInt());
                if(query.exec(cmd))
                {
                    double firstWeight;
                    if(query.next())
                    {
                        firstWeight = query.value(0).toDouble();     //得到了初始值
                    }
                    textBrowser->append(tr("样本值\t   预测值"));
                    while(query.next())
                    {
                        //预测出减少后的体重
                        data[3].append((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight);
                        textBrowser->append(tr("%1\t%2").arg(query.value(0).toDouble()).arg((1-((firstWeight-query.value(0).toDouble())/firstWeight))*weight));
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("错误"), cmd);
                    return;
                }
            }
            else
            {
                QMessageBox::information(this, tr("样本不足"), tr("以BMI为参数找不到合适样本"));
                textBrowser->append(tr("以BMI为参数找不到合适样本"));
                return;
            }

            textBrowser->append(tr("5.数据按占比结合，得出最终数据"));
            textBrowser->append(tr("年龄占比%1，身高占比%2，体重占比%3，BMI占比%4").arg(rate[0]).arg(rate[1]).arg(rate[2]).arg(rate[3]));

            int minCount;
            minCount = data[0].count();
            for(int i=1; i<4; i++)
            {
                if(data[i].count()<minCount)
                {
                    minCount = data[i].count();
                }
            }
            // 按比例得出最终数据
            for(int i=0; i<minCount; i++)
                finalData.append(data[0].at(i)*rate[0]/100 + data[1].at(i)*rate[1]/100 + data[2].at(i)*rate[2]/100 + data[3].at(i)*rate[3]/100);
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), cmd);
            qDebug()<<cmd;
            return;
        }

    }
    //3.显示曲线
    if(weightChartView->chart()!=NULL)
        weightChartView->chart()->removeAllSeries();
    weightSeries = new QLineSeries;
    weightScatterSeries = new QScatterSeries;

    textBrowser->append(tr("如果以今天为起点做预测，得出数据点"));
    textBrowser->append(tr("时间\t预计体重"));

    qint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    for(int i=0; i<finalData.count(); i++)
    {
        weightSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));
        weightScatterSeries->append(currentTime+(24*60*60*1000*i), finalData.at(i));

        textBrowser->append(tr("%1\t%2").arg(QDateTime::fromMSecsSinceEpoch(currentTime+(24*60*60*1000*i)).toString("yyyy-MM-dd")).arg(finalData.at(i)));
    }

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

    weightChartView->setChart(weightChart);
    weightChart->zoom(0.9);
}
