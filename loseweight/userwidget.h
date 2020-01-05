#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QTableWidget>
#include <QSqlQueryModel>
#include <QComboBox>
#include <QDateEdit>

class UserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserWidget(QWidget *parent = nullptr);

signals:

public slots:
    void AddUser();
    void QueryUserInfo(int index);
    void QueryUserName(const QString &text);
    void QueryPhoneNumber(const QString &text);
private:
    void InitAddUserTab(QWidget *widget);
    void InitQueryUserTab(QWidget *widget);
    void InitUpdateUserTab(QWidget *widget);

    QSqlQueryModel * queryModel;
    QTableView * tableView;

    QLineEdit * userNameLineEdit;
    QComboBox * genderComboBox;
    QDateEdit * dateofbirthEdit;
    QLineEdit * phoneLineEdit;
    QLineEdit * heightLineEdit;
    QLineEdit * targetweightLineEdit;
    QLineEdit * timeLineEdit;

    QRegExpValidator *validator1;
    QRegExpValidator *validator2;
    QRegExpValidator *validator3;
    QRegExpValidator *validator4;
    QRegExpValidator *validator5;

    QLineEdit * queryUserNameLineEdit;
    QLineEdit * phoneNumberLineEdit;
};

#endif // USERWIDGET_H
