#include "register.h"
#include "ui_register.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>

Register::Register(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    ui->lE_password->setEchoMode(QLineEdit::Password);
    ui->lE_confirm->setEchoMode(QLineEdit::Password);
    this->setWindowTitle(tr("ULSO"));
    QFont font;
    font.setPointSize(14);//字体大小
    this->setFont(font);
}

Register::~Register()
{
    delete ui;
}

void Register::on_pB_confirm_clicked()
{
    //1.先判断用户名账号是否符合要求
    if(ui->lE_username->text().isEmpty())
    {
        QMessageBox::information(this, tr("用户名为空"), tr("用户名为空"));
        return;
    }
    if(ui->lE_username->text().isEmpty())
    {
        QMessageBox::information(this, tr("密码为空"), tr("密码为空"));
        return;
    }
    if(ui->lE_confirm->text().isEmpty())
    {
        QMessageBox::information(this, tr("未确认密码"), tr("未确认密码"));
        return;
    }
    if(ui->lE_confirm->text()!=ui->lE_password->text())
    {
        QMessageBox::information(this, tr("密码确认失败"), tr("密码确认失败"));
        return;
    }
    //2. 校验通过，插入数据库
    QString cmd;
    QSqlQuery query;

    cmd = QString("insert into admin (admin, password) values('%1', '%2')")\
            .arg(ui->lE_username->text())
            .arg(ui->lE_password->text());
    if(query.exec(cmd))
    {
        QMessageBox::information(this, tr("注册成功"), tr("注册成功"));
        emit this->accept();
        this->close();
    }
    else
    {
        QMessageBox::critical(this, tr("注册失败"), tr("注册失败"));
        qDebug()<<cmd;
    }
}
