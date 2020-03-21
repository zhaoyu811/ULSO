/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "userdelegate.h"

#include <QtWidgets>

UserDelegate::UserDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *UserDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    if ((index.column() == 1))  //username
    {
        QLineEdit *le = new QLineEdit(parent);

        QRegExp rx("^[\u4e00-\u9fa5A-Za-z_]{2,20}$");
        QRegExpValidator *validator = new QRegExpValidator(rx, le);
        le->setValidator(validator);

        return le;
    }
    else if(index.column() == 2) //gender
    {
        QComboBox *cb = new QComboBox(parent);

        cb->addItem(tr("男"));
        cb->addItem(tr("女"));
        cb->setCurrentText(index.data().toString());

        return cb;
    }
    else if(index.column() == 3) //birthday
    {
        QDateEdit * de = new QDateEdit(parent);
        de->setDate(index.data().toDate());
        return de;
    }
    else if(index.column() == 4) //电话号码
    {
        QLineEdit *le = new QLineEdit(parent);

        QRegExp rx("^[0-9]{11}$");
        QRegExpValidator *validator = new QRegExpValidator(rx, le);
        le->setValidator(validator);

        return le;
    }
    else if(index.column() == 5) //height
    {
        QLineEdit *le = new QLineEdit(parent);

        QRegExp rx("^[1-9][0-9]{1,2}[\\.]{0,1}[0-9]{0,2}$");
        QRegExpValidator *validator = new QRegExpValidator(rx, le);
        le->setValidator(validator);

        return le;
    }
    else if(index.column() == 7)  //times
    {
        // for editing the year, return a spinbox with a range from -1000 to 2100.
        QSpinBox *sb = new QSpinBox(parent);
        sb->setFrame(false);
        sb->setMinimum(1);
        sb->setMaximum(10);
        return sb;
    }
    else if(index.column() == 8)    //openholetime
    {
        QDateEdit * de = new QDateEdit(parent);
        de->setDate(index.data().toDate());
        return de;
    }
    else if(index.column() == 9)    //openholeweight
    {
        QLineEdit *le = new QLineEdit(parent);

        QRegExp rx("^[1-9][0-9]{1,2}[\\.]{0,1}[0-9]{0,2}$");
        QRegExpValidator *validator = new QRegExpValidator(rx, le);
        le->setValidator(validator);

        return le;
    }
    else if(index.column() == 10)   //体重
    {
        QLineEdit *le = new QLineEdit(parent);

        QRegExp rx("^[1-9][0-9]{1,2}[\\.]{0,1}[0-9]{0,2}$");
        QRegExpValidator *validator = new QRegExpValidator(rx, le);
        le->setValidator(validator);

        return le;
    }
    else if(index.column() == 11)   //waist
    {
        QLineEdit *le = new QLineEdit(parent);

        QRegExp rx("^[1-9][0-9]{1,2}[\\.]{0,1}[0-9]{0,2}$");
        QRegExpValidator *validator = new QRegExpValidator(rx, le);
        le->setValidator(validator);

        return le;
    }
    else if(index.column() == 15)   //state
    {
        QComboBox *cb = new QComboBox(parent);

        cb->addItem(tr("减肥中"));
        cb->addItem(tr("减肥完成"));
        cb->setCurrentText(index.data().toString());

        return cb;
    }
    else
    {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

