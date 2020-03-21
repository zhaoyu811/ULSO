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
#include <QSqlTableModel>
#include "./editablesqlmodel.h"
#include "./userdelegate.h"

class UserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserWidget(QWidget *parent = nullptr);

signals:

public slots:
    void UpdateDateEdit(int index);
    void UpdateTargetWeight(void);
    void AddUser();
    void QueryUserInfo(int index);
    void QueryUserName(const QString &text);
    void QueryPhoneNumber(const QString &text);
private:
    void InitAddUserTab(QWidget *widget);
    void InitQueryUserTab(QWidget *widget);
    void InitUpdateUserTab(QWidget *widget);

    //QSqlTableModel * queryModel;
    EditableSqlModel * queryModel;
    QTableView * tableView;

    QLineEdit * userNameLineEdit;
    QComboBox * genderComboBox;
    QDateEdit * dateofbirthEdit;

    QDateEdit * openHoleDateEdit;
    QLineEdit * openHoleWeigtEdit;

    QLineEdit * phoneLineEdit;
    QLineEdit * weightLineEdit;
    QLineEdit * heightLineEdit;
    QLineEdit * waistLineEdit;
    QLineEdit * targetweightLineEdit;
    QLineEdit * timeLineEdit;

    QRegExpValidator *validator1;
    QRegExpValidator *validator2;
    QRegExpValidator *validator3;
    QRegExpValidator *validator4;
    QRegExpValidator *validator5;
    QRegExpValidator *validator6;
    QRegExpValidator *validator7;
    QRegExpValidator *validator8;

    QLineEdit * queryUserNameLineEdit;
    QLineEdit * phoneNumberLineEdit;

    UserDelegate * userDelegate;
};

#endif // USERWIDGET_H
