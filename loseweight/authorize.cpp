#include "authorize.h"
#include "ui_authorize.h"
#include <QDateTime>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QPixmap>
#include <QImage>
#include <QSettings>
#include <QPainter>
#include <QFileDialog>

authorize::authorize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::authorize)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("ULSO"));
    QFont font;
    font.setPointSize(14);//字体大小
    this->setFont(font);

    //获得时间戳
    //ui->lE_timestamp->setText(QString::number(QDateTime::currentDateTime().toTime_t()));
    QString timeStamp = QString::number(QDateTime::currentDateTime().toTime_t());
    //获得MAC
    QString mac;
    QList<QNetworkInterface> networks = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface network, networks) {
        if(network.flags().testFlag(QNetworkInterface::IsUp)\
                &&network.flags().testFlag(QNetworkInterface::IsRunning)\
                &&!network.flags().testFlag(QNetworkInterface::IsLoopBack))
        mac = network.hardwareAddress();
    }

    //生成二维码
    QString srcData = QString("%1-%2").arg(timeStamp).arg(mac);
    QByteArray srcDataByteArray = srcData.toLocal8Bit();
    QByteArray base64Data = srcDataByteArray.toBase64();
    QByteArray xorData = GetXorEncryptDecrypt(base64Data, '-');
    QRcode *code = NULL;
    //code = QRcode_encodeString(xorData.toStdString().c_str(), 2, QR_ECLEVEL_L, QR_MODE_8, 0);
    code = QRcode_encodeString(xorData.toStdString().c_str(),1,QR_ECLEVEL_L,QR_MODE_8,1);
    if(code == NULL)
        QMessageBox::warning(this, tr("QRcode_encodeString error"), tr("QRcode_encodeString error"));
#if 0
    for(int i=0; i<code->width; i++)
    {
        for(int j=0; j<code->width; j++)
        {
            printf("%d ", (code->data[i*j]&0x01));
        }
        printf("\n");
    }
#endif

#if 1
    //生成二维码显示数据
    QColor white(255, 255, 255);
    QColor black(0, 0, 0);
    QImage image(code->width*10, code->width*10, QImage::Format_RGB888);
    for(int i=0; i<code->width; i++)
    {
        for(int j=0; j<code->width; j++)
        {
            for(int k=0; k<10; k++)
            {
                for(int l=0; l<10; l++)
                {
                    image.setPixelColor((i*10)+k, (j*10)+l, (code->data[i*code->width+j]&0x01)?black:white);
                }
            }
        }
    }
    QPixmap pixMap;
    pixMap=pixMap.fromImage(image);
#else
    QPixmap pixMap(code->width*5, code->width*5);
    pixMap.fill(Qt::white);

    QPainter painter(&pixMap);
    QColor foreground(Qt::black);
    painter.setBrush(foreground);
    const int qr_width = code->width > 0 ? code->width : 1;
    double scale_x = 5;
    double scale_y = 5;
    for( int y = 0; y < qr_width; y ++)
    {
       for(int x = 0; x < qr_width; x++)
       {
           unsigned char b = code->data[y * qr_width + x];
           if(b & 0x01)
           {
               QRectF r(x * scale_x, y * scale_y, scale_x, scale_y);
               painter.drawRects(&r, 1);
           }
       }
    }

#endif
    //显示二维码
    ui->label->resize(code->width*5, code->width*5);
    ui->label->setPixmap(pixMap);
    ui->label->show();
    //释放数据
    QRcode_free(code);
}

authorize::~authorize()
{
    delete ui;
}

QByteArray authorize::GetXorEncryptDecrypt(const QByteArray &str, const char &key)
{
#if 1
    QByteArray bs = str;

    for(int i=0; i<bs.size(); i++){
        bs[i] = bs[i] ^ key;
    }

    return bs;
#endif
    return str;
}

void authorize::on_pB_authorize_clicked()
{
    //1.解密数据
    QString fileName = QFileDialog::getOpenFileName(this, tr("秘钥文件"), "./", "*.bin");
    if(fileName.isEmpty())
        return;
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
        return;
    QByteArray byteArray = file.readAll();
    QByteArray xorData = GetXorEncryptDecrypt(byteArray, '-');
    QByteArray base64Data = QByteArray::fromBase64(xorData);

    QString data = QString::fromUtf8(base64Data);
    QStringList stringList = data.split("-");
    if(stringList.length()==4)
    {
        if(stringList.at(3)!="643871")
        {
            QMessageBox::warning(this, tr("密码错误"), tr("密码错误"));
        }
        else
        {
            int time = QDateTime::currentDateTime().toTime_t()-stringList.at(0).toUInt();
            qDebug()<<time;
            //当前时间-授权码时间 大于一定时间， 授权码失效
            if(time<=60*10)  //10分钟
            {
                //获得MAC
                QString mac;
                QList<QNetworkInterface> networks = QNetworkInterface::allInterfaces();
                foreach (QNetworkInterface network, networks) {
                    if(network.flags().testFlag(QNetworkInterface::IsUp)\
                            &&network.flags().testFlag(QNetworkInterface::IsRunning)\
                            &&!network.flags().testFlag(QNetworkInterface::IsLoopBack))
                    mac = network.hardwareAddress();
                }
                if(mac==stringList.at(1))
                {
                    int time = stringList.at(2).toUInt()-QDateTime::currentDateTime().toTime_t();
                    //授权时间 必须大于当前时间
                    if(time>0)
                    {
                        //写入注册表
                        QSettings settings("HKEY_CURRENT_USER\\Software\\ULSO",QSettings::NativeFormat);
                        settings.setValue("key", xorData);
                        QString value = settings.value("key").toString();
                        qDebug()<<value;

                        if(value==xorData)
                        {
                            QMessageBox::information(this, tr("授权成功"), tr("软件有效期至:%1").arg(QDateTime::fromTime_t(stringList.at(2).toUInt()).toString("yyyy-MM-dd")));
                            emit this->accept();
                            this->close();
                        }
                    }
                }
                else
                {
                    QMessageBox::warning(this, tr("授权设备出错！"), tr("授权设备出错！"));
                }
            }
            else
            {
                QMessageBox::warning(this, tr("授权码已过期！"), tr("授权码已过期！"));
            }
        }
    }
    else
    {
        QMessageBox::warning(this, tr("授权码错误！"), tr("授权码错误！"));
    }
}

void authorize::on_pushButton_clicked()
{
    //获得时间戳
    QString timeStamp = QString::number(QDateTime::currentDateTime().toTime_t());
    //获得MAC
    QString mac;
    QList<QNetworkInterface> networks = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface network, networks) {
        if(network.flags().testFlag(QNetworkInterface::IsUp)\
                &&network.flags().testFlag(QNetworkInterface::IsRunning)\
                &&!network.flags().testFlag(QNetworkInterface::IsLoopBack))
        mac = network.hardwareAddress();
    }

    //生成二维码
    QString srcData = QString("%1-%2").arg(timeStamp).arg(mac);
    QByteArray srcDataByteArray = srcData.toLocal8Bit();
    QByteArray base64Data = srcDataByteArray.toBase64();
    QByteArray xorData = GetXorEncryptDecrypt(base64Data, '-');

    QRcode *code = NULL;
    //code = QRcode_encodeString(xorData.toStdString().c_str(), 2, QR_ECLEVEL_L, QR_MODE_8, 0);
    code = QRcode_encodeString(xorData.toStdString().c_str(),1,QR_ECLEVEL_L,QR_MODE_8,1);
    if(code == NULL)
        QMessageBox::warning(this, tr("QRcode_encodeString error"), tr("QRcode_encodeString error"));

    //生成二维码显示数据
    QColor white(255, 255, 255);
    QColor black(0, 0, 0);
    QImage image(code->width*10, code->width*10, QImage::Format_RGB888);
    for(int i=0; i<code->width; i++)
    {
        for(int j=0; j<code->width; j++)
        {
            for(int k=0; k<10; k++)
            {
                for(int l=0; l<10; l++)
                {
                    image.setPixelColor((i*10)+k, (j*10)+l, (code->data[i*code->width+j]&0x01)?black:white);
                }
            }
        }
    }
    QPixmap pixMap;
    pixMap=pixMap.fromImage(image);

    //显示二维码
    ui->label->resize(code->width*5, code->width*5);
    ui->label->setPixmap(pixMap);
    ui->label->show();
    //释放数据
    QRcode_free(code);
}
