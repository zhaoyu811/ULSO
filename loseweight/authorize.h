#ifndef AUTHORIZE_H
#define AUTHORIZE_H

#include <QDialog>
#include "QRencode/include/qrencode.h"

namespace Ui {
class authorize;
}

class authorize : public QDialog
{
    Q_OBJECT

public:
    explicit authorize(QWidget *parent = 0);
    ~authorize();

private slots:
    void on_pB_authorize_clicked();

private:
    Ui::authorize *ui;
    QByteArray GetXorEncryptDecrypt(const QByteArray &str, const char &key);
};

#endif // AUTHORIZE_H
