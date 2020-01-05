#include "userwidget.h"
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

UserWidget::UserWidget(QWidget *parent) : QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout(this);

    QTabWidget *tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);

    QWidget * addUserTab = new QWidget(this);
    InitAddUserTab(addUserTab);

    QWidget * queryUserTab = new QWidget(this);
    InitQueryUserTab(queryUserTab);
    connect(tabWidget, &QTabWidget::tabBarClicked, this, &UserWidget::QueryUserInfo);

    QWidget * updateUserTab = new QWidget(this);
    InitUpdateUserTab(updateUserTab);
    tabWidget->addTab(addUserTab, tr("添加用户"));
    tabWidget->addTab(queryUserTab, tr("查询用户"));
    tabWidget->addTab(updateUserTab, tr("更新用户"));

    this->setLayout(mainLayout);
}

void UserWidget::InitAddUserTab(QWidget *widget)
{
    QGridLayout * mainLayout = new QGridLayout(widget);
    QGroupBox * groupBox = new QGroupBox(widget);
    QGridLayout * gridLayout = new QGridLayout(groupBox);

    QLabel * userNameLabel      = new QLabel(tr("姓名"), groupBox);
    QLabel * genderLabel        = new QLabel(tr("性别"), groupBox);
    QLabel * dateofbirthLabel   = new QLabel(tr("出生日期"), groupBox);
    QLabel * phoneLabel         = new QLabel(tr("电话号码"), groupBox);
    QLabel * heightLabel        = new QLabel(tr("身高(cm)"), groupBox);
    QLabel * targetweightLabel  = new QLabel(tr("目标体重(kg)"), groupBox);
    QLabel * timeLabel          = new QLabel(tr("第几次减肥"), groupBox);

    userNameLineEdit        = new QLineEdit(groupBox);
    QRegExp rx1("^[\u4e00-\u9fa5A-Za-z_]{2,20}$");
    validator1 = new QRegExpValidator(rx1, userNameLineEdit);
    userNameLineEdit->setValidator(validator1);


    genderComboBox          = new QComboBox(groupBox);
    genderComboBox->addItem(tr("男"));
    genderComboBox->addItem(tr("女"));

    dateofbirthEdit = new QDateEdit(groupBox);

    phoneLineEdit           = new QLineEdit(groupBox);
    QRegExp rx3("^(13[0-9]|14[5|7]|15[0|1|2|3|4|5|6|7|8|9]|18[0|1|2|3|4|5|6|7|8|9])[0-9]{8}$");
    validator3 = new QRegExpValidator(rx3, phoneLineEdit);
    phoneLineEdit->setValidator(validator3);

    heightLineEdit = new QLineEdit(groupBox);
    QRegExp rx2("^[1-9][0-9]{1,2}$");
    validator2 = new QRegExpValidator(rx2, heightLineEdit);
    heightLineEdit->setValidator(validator2);

    targetweightLineEdit    = new QLineEdit(groupBox);
    QRegExp rx4("^[1-9][0-9]{0,2}$");
    validator4 = new QRegExpValidator(rx4, targetweightLineEdit);
    targetweightLineEdit->setValidator(validator4);

    timeLineEdit            = new QLineEdit(groupBox);
    QRegExp rx5("^[1-9]$");
    validator5 = new QRegExpValidator(rx5, timeLineEdit);
    timeLineEdit->setValidator(validator5);

    gridLayout->addWidget(userNameLabel, 0, 0, 1, 1);
    gridLayout->addWidget(userNameLineEdit, 0, 1, 1, 1);
    gridLayout->addWidget(genderLabel, 1, 0, 1, 1);
    gridLayout->addWidget(genderComboBox, 1, 1, 1, 1);
    gridLayout->addWidget(dateofbirthLabel, 2, 0, 1, 1);
    gridLayout->addWidget(dateofbirthEdit, 2, 1, 1, 1);
    gridLayout->addWidget(phoneLabel, 3, 0, 1, 1);
    gridLayout->addWidget(phoneLineEdit, 3, 1, 1, 1);
    gridLayout->addWidget(heightLabel, 4, 0, 1, 1);
    gridLayout->addWidget(heightLineEdit, 4, 1, 1, 1);
    gridLayout->addWidget(targetweightLabel, 5, 0, 1, 1);
    gridLayout->addWidget(targetweightLineEdit, 5, 1, 1, 1);
    gridLayout->addWidget(timeLabel, 6, 0, 1, 1);
    gridLayout->addWidget(timeLineEdit, 6, 1, 1, 1);

    QPushButton * addUserPushButton = new QPushButton(tr("添加用户"), groupBox);
    gridLayout->addWidget(addUserPushButton, 7, 1, 1, 1);
    connect(addUserPushButton, &QPushButton::clicked, this, &UserWidget::AddUser);
    gridLayout->setColumnStretch(2, 1);
    gridLayout->setRowStretch(8, 1);
    //groupBox->setFixedSize(200, 300);
    mainLayout->setAlignment(widget, Qt::AlignLeft);
    mainLayout->addWidget(groupBox);
    widget->setLayout(mainLayout);
}

void UserWidget::AddUser()
{
    qDebug()<<"AddUser";
    QString cmd;
    QSqlQuery query;

    QString userName=userNameLineEdit->text();
    int pos;
    if(validator1->validate(userName, pos)!=QValidator::Acceptable)
    {
        QMessageBox::critical(this, tr("用户名不符合"), tr("用户名不符合:2-20个字符"));
        userNameLineEdit->setFocus();
        return;
    }

    QString height=heightLineEdit->text();
    if(validator2->validate(height, pos)!=QValidator::Acceptable)
    {
        QMessageBox::critical(this, tr("身高输入错误"), tr("身高输入错误"));
        heightLineEdit->setFocus();
        return;
    }

    QString phone=phoneLineEdit->text();
    if(validator3->validate(phone, pos)!=QValidator::Acceptable)
    {
        QMessageBox::critical(this, tr("电话号码有误"), tr("电话号码有误"));
        phoneLineEdit->setFocus();
        return;
    }

    QString targetweight=targetweightLineEdit->text();
    if(validator4->validate(targetweight, pos)!=QValidator::Acceptable)
    {
        QMessageBox::critical(this, tr("目标体重有误"), tr("目标体重有误"));
        targetweightLineEdit->setFocus();
        return;
    }

    QString time=timeLineEdit->text();
    if(validator5->validate(time, pos)!=QValidator::Acceptable)
    {
        QMessageBox::critical(this, tr("次数有误"), tr("次数有误"));
        timeLineEdit->setFocus();
        return;
    }

    cmd = QString("insert into user(username, gender, dateofbirth, phonenumber, height, targetweight, times) values('%1', '%2', '%3', '%4', '%5', '%6', '%7')")
            .arg(userNameLineEdit->text())
            .arg(genderComboBox->currentText())
            .arg(dateofbirthEdit->date().toString("yyyy-MM-dd"))
            .arg(phoneLineEdit->text())
            .arg(heightLineEdit->text())
            .arg(targetweightLineEdit->text())
            .arg(timeLineEdit->text());
    if(query.exec(cmd))
    {
        cmd = QString("select id from user limit 1 offset (select count(*) - 1 from user)");
        if(query.exec(cmd))
        {
            if(query.next())
            {
                int id = query.value(0).toInt();
                cmd = QString("create table archive%1(id integer primary key autoincrement, weight varchar, waist varchar, recipes varchar, datetime varchar, timeofday varchar)").arg(id);
                if(query.exec(cmd))
                {
                    QMessageBox::information(this, tr("建立用户信息成功"), tr("建立用户信息成功"));
                    userNameLineEdit->clear();
                    dateofbirthEdit->clear();
                    phoneLineEdit->clear();
                    heightLineEdit->clear();
                    targetweightLineEdit->clear();
                    timeLineEdit->clear();
                }
                else
                {
                    QMessageBox::critical(this, tr("建立用户减肥表失败"), cmd);
                }
            }
            else
            {
                QMessageBox::critical(this, tr("查询数据为空"), cmd);
            }
        }
        else
        {
            QMessageBox::critical(this, tr("查询数据库失败"), cmd);
        }
    }
    else
    {
        QMessageBox::critical(this, tr("插入数据失败"), cmd);
    }
}

void UserWidget::InitQueryUserTab(QWidget *widget)
{
    QGridLayout *mainLayout = new QGridLayout(widget);
    QGridLayout *gridLayout = new QGridLayout();

    QLabel * userNameLabel = new QLabel(tr("用户名"), widget);
    QLabel * phoneNumberLabel = new QLabel(tr("手机号码"), widget);

    queryUserNameLineEdit = new QLineEdit(widget);
    connect(queryUserNameLineEdit, &QLineEdit::textEdited, this, &UserWidget::QueryUserName);
    phoneNumberLineEdit = new QLineEdit(widget);
    connect(phoneNumberLineEdit, &QLineEdit::textEdited, this, &UserWidget::QueryPhoneNumber);


    gridLayout->addWidget(userNameLabel, 0, 0, 1, 1);
    gridLayout->addWidget(queryUserNameLineEdit, 0, 1, 1, 1);
    gridLayout->addWidget(phoneNumberLabel, 0, 2, 1, 1);
    gridLayout->addWidget(phoneNumberLineEdit, 0, 3, 1, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(3, 1);
    gridLayout->setColumnStretch(4, 4);

    tableView = new QTableView(widget);
    queryModel = new QSqlQueryModel(tableView);

    queryModel->setQuery("select * from user");
    queryModel->setHeaderData(0, Qt::Horizontal, tr("档案号"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("性别"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("出生日期"));
    queryModel->setHeaderData(4, Qt::Horizontal, tr("电话号码"));
    queryModel->setHeaderData(5, Qt::Horizontal, tr("身高"));
    queryModel->setHeaderData(6, Qt::Horizontal, tr("目标体重"));
    queryModel->setHeaderData(7, Qt::Horizontal, tr("第几次减肥"));
    queryModel->query();
    tableView->setModel(queryModel);

    mainLayout->addLayout(gridLayout, 0, 0, 1, 1);
    mainLayout->addWidget(tableView, 1, 0, 1, 1);
    widget->setLayout(mainLayout);
}

void UserWidget::QueryUserName(const QString &text)
{
    QString cmd;
    cmd = QString("select * from user where username like '%1%'").arg(text);

    queryModel->setQuery(cmd);
    queryModel->setHeaderData(0, Qt::Horizontal, tr("档案号"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("性别"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("出生日期"));
    queryModel->setHeaderData(4, Qt::Horizontal, tr("电话号码"));
    queryModel->setHeaderData(5, Qt::Horizontal, tr("身高"));
    queryModel->setHeaderData(6, Qt::Horizontal, tr("目标体重"));
    queryModel->setHeaderData(7, Qt::Horizontal, tr("第几次减肥"));
    queryModel->query();
    tableView->setModel(queryModel);
}

void UserWidget::QueryPhoneNumber(const QString &text)
{
    QString cmd;
    cmd = QString("select * from user where phonenumber like '%1%'").arg(text);

    queryModel->setQuery(cmd);
    queryModel->setHeaderData(0, Qt::Horizontal, tr("档案号"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("性别"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("出生日期"));
    queryModel->setHeaderData(4, Qt::Horizontal, tr("电话号码"));
    queryModel->setHeaderData(5, Qt::Horizontal, tr("身高"));
    queryModel->setHeaderData(6, Qt::Horizontal, tr("目标体重"));
    queryModel->setHeaderData(7, Qt::Horizontal, tr("第几次减肥"));
    queryModel->query();
    tableView->setModel(queryModel);
}

void UserWidget::QueryUserInfo(int index)
{
    qDebug()<<"QueryUserInfo index="<<index;
    if(index == 1)
    {
        queryModel->setQuery("select * from user");
        queryModel->setHeaderData(0, Qt::Horizontal, tr("档案号"));
        queryModel->setHeaderData(1, Qt::Horizontal, tr("姓名"));
        queryModel->setHeaderData(2, Qt::Horizontal, tr("性别"));
        queryModel->setHeaderData(3, Qt::Horizontal, tr("出生日期"));
        queryModel->setHeaderData(4, Qt::Horizontal, tr("电话号码"));
        queryModel->setHeaderData(5, Qt::Horizontal, tr("身高"));
        queryModel->setHeaderData(6, Qt::Horizontal, tr("目标体重"));
        queryModel->setHeaderData(7, Qt::Horizontal, tr("第几次减肥"));
        queryModel->query();
        tableView->setModel(queryModel);
    }
}

void UserWidget::InitUpdateUserTab(QWidget *widget)
{

}
