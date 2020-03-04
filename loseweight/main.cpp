#include "mainwindow.h"
#include <QApplication>
#include "authorize.h"
#include <QDateTime>
#include <QNetworkInterface>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QString>
#include <register.h>
#include <login.h>

static bool CreateConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./loseweight.db");
    db.setUserName("hello");
    db.setPassword("world");
    if(!db.open()){
       return false;
    }

    QSqlQuery query;
    //1. 表一 管理员账号密码
    query.exec("create table admin(admin varchar, password varchar)");
    //2. 表二 用户信息表
    query.exec("create table user(id integer primary key autoincrement, username varchar, gender varchar, dateofbirth varchar, phonenumber varchar, height varchar, targetweight varchar, times varchar, openholetime varchar, openholeweight varchar, weight varchar, waist varchar, bmi varchar, bfp varchar, datacount varchar, state varchar)");
    //3. 表三 每个用户对应一张表
    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //打开数据库
    if(!CreateConnection())
    {
        QMessageBox::critical(NULL, QString("打开数据库失败"), QString("打开数据失败"));
        return -1;
    }
    MainWindow w;
    //获得MAC
    QString mac;
    QList<QNetworkInterface> networks = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface network, networks) {
        if(network.flags().testFlag(QNetworkInterface::IsUp)\
                &&network.flags().testFlag(QNetworkInterface::IsRunning)\
                &&!network.flags().testFlag(QNetworkInterface::IsLoopBack))
        mac = network.hardwareAddress();
    }

    //从注册表中读取数据
    QSettings settings("HKEY_CURRENT_USER\\Software\\loseweight",QSettings::NativeFormat);
    QString value = settings.value("key").toString();
    qDebug()<<value;
    if(!value.isEmpty())    //如果不为空
    {
        QByteArray base64Data1 = QByteArray::fromBase64(value.toLocal8Bit());
        qDebug()<<base64Data1;
        QString data = QString::fromLocal8Bit(base64Data1);
        QStringList stringList = data.split("-");
        //校验字符串二 和 字符串三
        if(stringList.at(1)==mac&&(stringList.at(2).toUInt()-QDateTime::currentDateTime().toTime_t()>0))    //mac相同 并且授权没有过期
        {
            QString cmd;
            QSqlQuery query;
            cmd = QString("select * from admin");
            if(query.exec(cmd))
            {
                if(query.next())    //存在数据，说明存在账号， 显示登录界面
                {
                    LogIn login;
                    if(login.exec() == QDialog::Accepted)   //登录成功
                    {
                        w.showMaximized();
                        return a.exec();
                    }
                }
                else    //注册
                {
                    Register reg;
                    if(reg.exec() == QDialog::Accepted) //注册成功 显示登录界面
                    {
                        LogIn login;
                        if(login.exec() == QDialog::Accepted)
                        {
                             w.showMaximized();
                            return a.exec();
                        }
                    }
                }
            }
            else
            {
                QMessageBox::critical(NULL, QString("查询数据库失败"), QString("查询数据库失败"));
            }
        }
        else
        {
            authorize login;
            if (login.exec() == QDialog::Accepted)//调用login.exec()，阻塞主控制流，直到完成返回，继续执行主控制流
            {
                Register reg;
                if(reg.exec() == QDialog::Accepted)
                {
                    LogIn login;
                    if(login.exec() == QDialog::Accepted)
                    {
                         w.showMaximized();
                        return a.exec();
                    }
                }
            }
        }
    }
    else
    {
        authorize login;
        if (login.exec() == QDialog::Accepted)//调用login.exec()，阻塞主控制流，直到完成返回，继续执行主控制流
        {
            Register reg;
            if(reg.exec() == QDialog::Accepted)
            {
                LogIn login;
                if(login.exec() == QDialog::Accepted)
                {
                     w.showMaximized();
                    return a.exec();
                }
            }
        }
    }

    return 0;
}
