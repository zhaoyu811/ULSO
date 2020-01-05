#include "login.h"
#include "ui_login.h"
#include <QSqlQuery>
#include <QMessageBox>

LogIn::LogIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogIn)
{
    ui->setupUi(this);
    ui->lE_password->setEchoMode(QLineEdit::Password);
}

LogIn::~LogIn()
{
    delete ui;
}

void LogIn::on_pB_login_clicked()
{
    QString cmd;
    QSqlQuery query;

    cmd = QString("select * from admin where admin='%1' and password='%2'")\
            .arg(ui->lE_username->text())
            .arg(ui->lE_password->text());
    if(query.exec(cmd))
    {
        if(query.next())
        {
            emit this->accept();
            this->close();
        }
        else
        {
            QMessageBox::information(this, tr("用户或密码错误"), tr("用户或密码错误"));
        }
    }
    else
    {
        QMessageBox::critical(this, tr("查询数据库失败"), tr("查询数据库失败"));
    }
}
