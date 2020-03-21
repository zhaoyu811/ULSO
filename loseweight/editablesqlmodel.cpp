/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtSql>

#include "editablesqlmodel.h"

EditableSqlModel::EditableSqlModel(QObject *parent)
    :QSqlQueryModel(parent)
{

}

//! [0]
Qt::ItemFlags EditableSqlModel::flags(
        const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if ((index.column() >=1 && index.column() <=11)|| index.column() == 15)
        flags |= Qt::ItemIsEditable;
    return flags;
}
//! [0]

//! [1]
bool EditableSqlModel::setData(const QModelIndex &index, const QVariant &value, int /* role */)
{
    if (index.column() < 1 || index.column() == 6 || index.column() > 15 || (index.column()>11&&index.column()<15))
        return false;

    QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
    int id = data(primaryKeyIndex).toInt();

    clear();

    bool ok;

    if(index.column() == 1)
    {
        ok = setName(id, value.toString());
    }
    else if(index.column() == 2)
    {
        ok = setGender(id, value.toString());
    }
    else if(index.column() == 3)
    {
        ok = setBirthday(id, value.toString());
    }
    else if(index.column() == 4)
    {
        ok = setPhoneNumber(id, value.toString());
    }
    else if(index.column() == 5)
    {
        ok = setHeight(id, value.toString());
    }
    else if(index.column() == 7)
    {
        ok = setTimes(id, value.toString());
    }
    else if(index.column() == 8)
    {
        ok = setOpenHoleTime(id, value.toString());
    }
    else if(index.column() == 9)
    {
        ok = setOpenHoleWeight(id, value.toString());
    }
    else if(index.column() == 10)
    {
        ok = setWeight(id, value.toString());
    }
    else if(index.column() == 11)
    {
        ok = setWaist(id, value.toString());
    }
    else if(index.column() == 15)
    {
        ok = setState(id, value.toString());
    }
    refresh();
    return ok;
}
//! [1]

void EditableSqlModel::setQueryCmd(const QString &query, const QSqlDatabase &db)
{
    cmd = query;
    setQuery(query, db);
}

void EditableSqlModel::refresh()
{
    setQuery(cmd, QSqlDatabase());
    setHeaderData(0, Qt::Horizontal, tr("档案号"));
    setHeaderData(1, Qt::Horizontal, tr("姓名"));
    setHeaderData(2, Qt::Horizontal, tr("性别"));
    setHeaderData(3, Qt::Horizontal, tr("出生日期"));
    setHeaderData(4, Qt::Horizontal, tr("电话号码"));
    setHeaderData(5, Qt::Horizontal, tr("身高"));
    setHeaderData(6, Qt::Horizontal, tr("目标体重"));
    setHeaderData(7, Qt::Horizontal, tr("第几次减肥"));
    setHeaderData(8, Qt::Horizontal, tr("开穴日期"));
    setHeaderData(9, Qt::Horizontal, tr("开穴斤数"));
    setHeaderData(10, Qt::Horizontal, tr("体重"));
    setHeaderData(11, Qt::Horizontal, tr("腰围"));
    setHeaderData(12, Qt::Horizontal, tr("BMI"));
    setHeaderData(13, Qt::Horizontal, tr("体脂率"));
    setHeaderData(14, Qt::Horizontal, tr("累积次数"));
    setHeaderData(15, Qt::Horizontal, tr("状态"));
}

//! [2]
bool EditableSqlModel::setName(int id, const QString &name)
{
    QSqlQuery query;
    query.prepare("update user set username = ? where id = ?");
    query.addBindValue(name);
    query.addBindValue(id);
    return query.exec();
}
//! [2]

bool EditableSqlModel::setGender(int id, const QString &gender)
{
    //年龄改变 会引起bfp的变化
    QSqlQuery query;
    QString val;
    if(query.exec(tr("select weight, waist, height, dateofbirth from user where id = %1 limit 1").arg(id)))
    {
        if(query.next())
        {
            double weight, waist, height;
            double bmi, bfp;
            int age;
            weight = query.value(0).toDouble();
            waist = query.value(1).toDouble();
            height = query.value(2).toDouble();
            age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("yyyy").toInt();

            if(gender==tr("男"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4-10.8)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
            }
            else if(gender==tr("女"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
            }
            else
            {
                bfp = 0;
            }
            val = QString::number(bfp, 'f', 2);
        }
    }
    else
    {
        return false;
    }
    query.prepare("update user set gender = ? , bfp = ? where id = ?");
    query.addBindValue(gender);
    query.addBindValue(val);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setBirthday(int id, const QString &birthday)
{
    QSqlQuery query;
    QString val;
    if(query.exec(tr("select weight, waist, height, gender from user where id = %1 limit 1").arg(id)))
    {
        if(query.next())
        {
            double weight, waist, height;
            double bmi, bfp;
            QString gender;
            weight = query.value(0).toDouble();
            waist = query.value(1).toDouble();
            height = query.value(2).toDouble();
            gender = query.value(3).toString();
            int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(birthday, "yyyy-MM-dd").toString("yyyy").toInt();

            if(gender==tr("男"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4-10.8)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
            }
            else if(gender==tr("女"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
            }
            else
            {
                bfp = 0;
            }
            val = QString::number(bfp, 'f', 2);
        }
    }
    else
    {
        return false;
    }
    query.prepare("update user set dateofbirth = ?, bfp = ? where id = ?");
    query.addBindValue(birthday);
    query.addBindValue(val);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setPhoneNumber(int id, const QString &phone)
{
    QSqlQuery query;
    query.prepare("update user set phonenumber = ? where id = ?");
    query.addBindValue(phone);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setHeight(int id, const QString &height1)
{
    QSqlQuery query;
    QString val, val2;
    if(query.exec(tr("select weight, waist, dateofbirth, gender from user where id = %1 limit 1").arg(id)))
    {
        if(query.next())
        {
            double weight, waist, height;
            double bmi, bfp;
            QString gender;
            weight = query.value(0).toDouble();
            waist = query.value(1).toDouble();
            gender = query.value(3).toString();
            int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toString("yyyy").toInt();

            height = height1.toDouble();
            if(gender==tr("男"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4-10.8)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
            }
            else if(gender==tr("女"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
            }
            else
            {
                bmi = 0;
                bfp = 0;
            }
            val = QString::number(bfp, 'f', 2);
            val2 = QString::number(bmi, 'f', 2);
        }
    }
    else
    {
        return false;
    }
    query.prepare("update user set height = ?, bfp = ?, bmi = ? where id = ?");
    query.addBindValue(height1);
    query.addBindValue(val);
    query.addBindValue(val2);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setTimes(int id, const QString &times)
{
    QSqlQuery query;
    query.prepare("update user set times = ? where id = ?");
    query.addBindValue(times);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setOpenHoleTime(int id, const QString &openHoleTime)
{
    QSqlQuery query;
    query.prepare("update user set openholetime = ? where id = ?");
    query.addBindValue(openHoleTime);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setOpenHoleWeight(int id, const QString &openHoleWeight)
{
    QSqlQuery query;
    query.prepare("update user set openholeweight = ?, targetweight=weight-? where id = ?");
    query.addBindValue(openHoleWeight);
    query.addBindValue(openHoleWeight);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setWeight(int id, const QString &weight1)
{
    QSqlQuery query;
    QString val, val2;
    if(query.exec(tr("select height, waist, dateofbirth, gender from user where id = %1 limit 1").arg(id)))
    {
        if(query.next())
        {
            double weight, waist, height;
            double bmi, bfp;
            QString gender;
            height = query.value(0).toDouble();
            waist = query.value(1).toDouble();
            gender = query.value(3).toString();
            int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toString("yyyy").toInt();

            weight = weight1.toDouble();
            if(gender==tr("男"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4-10.8)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
            }
            else if(gender==tr("女"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
            }
            else
            {
                bmi = 0;
                bfp = 0;
            }
            val = QString::number(bfp, 'f', 2);
            val2 = QString::number(bmi, 'f', 2);
        }
    }
    else
    {
        return false;
    }
    query.prepare("update user set weight = ?, bfp = ?, bmi = ?, targetweight=?-openholeweight where id = ?");
    query.addBindValue(weight1);
    query.addBindValue(val);
    query.addBindValue(val2);
    query.addBindValue(weight1);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setWaist(int id, const QString &waist1)
{
    QSqlQuery query;
    QString val, val2;
    if(query.exec(tr("select height, weight, dateofbirth, gender from user where id = %1 limit 1").arg(id)))
    {
        if(query.next())
        {
            double weight, waist, height;
            double bmi, bfp;
            QString gender;
            height = query.value(0).toDouble();
            weight = query.value(1).toDouble();
            gender = query.value(3).toString();
            int age = QDateTime::currentDateTime().toString("yyyy").toInt()-QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd").toString("yyyy").toInt();

            waist = waist1.toDouble();
            if(gender==tr("男"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4-10.8)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89+9.85))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4-10.8)*0.7);
            }
            else if(gender==tr("女"))
            {
                bmi = weight/2/height/height*10000;
                //bfp = ((weight/2*0.74-(waist*0.082+34.89))/weight/2*100)*0.6+(1.2*bmi+0.23*age-5.4)*0.4;
                bfp = ((((waist*0.74)-(weight/2*0.082+34.89))/(weight/2)*100)*0.3)+(((1.2*bmi)+(0.23*age)-5.4)*0.7);
            }
            else
            {
                bmi = 0;
                bfp = 0;
            }
            val = QString::number(bfp, 'f', 2);
            val2 = QString::number(bmi, 'f', 2);
        }
    }
    else
    {
        return false;
    }
    query.prepare("update user set waist = ?, bfp = ?, bmi = ? where id = ?");
    query.addBindValue(waist1);
    query.addBindValue(val);
    query.addBindValue(val2);
    query.addBindValue(id);
    return query.exec();
}

bool EditableSqlModel::setState(int id, const QString &state)
{
    QSqlQuery query;
    query.prepare("update user set state = ? where id = ?");
    query.addBindValue(state);
    query.addBindValue(id);
    return query.exec();
}
