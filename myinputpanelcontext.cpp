/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>

#include "myinputpanelcontext.h"
#include <QCursor>

extern QString widget_name;

//! [0]
QString qstr11;
MyInputPanelContext::MyInputPanelContext()
{
    inputPanel = new MyInputPanel;
    connect(inputPanel, SIGNAL(characterGenerated(QChar)), SLOT(sendCharacter(QChar)));
    inputPanel->form.lineEdit->setText("qstr11");
}

//! [0]

MyInputPanelContext::~MyInputPanelContext()
{
    delete inputPanel;
}

//! [1]

bool MyInputPanelContext::filterEvent(const QEvent* event)
{
    if (event->type() == QEvent::RequestSoftwareInputPanel)
    {
        updatePosition();
        QWidget *w = focusWidget();
        qstr11 = dynamic_cast<QLineEdit*>(w)->text();
        if((widget_name =="lineEdit_pwd")||(widget_name =="lineEdit_88")||(widget_name =="lineEdit_24"))  //vinay
        {
            inputPanel->form.lineEdit->setEchoMode(QLineEdit::Password);
        }
        else
        {
            inputPanel->form.lineEdit->setEchoMode(QLineEdit::Normal);
        }

        inputPanel->show();
        inputPanel->form.lineEdit->setText(qstr11);
        return true;
    }
    else if ((event->type() == QEvent::CloseSoftwareInputPanel))
    {
        inputPanel->hide();
        return true;
    }
    return false;
}

//! [1]

QString MyInputPanelContext::identifierName()
{
    return "MyInputPanelContext";

}

void MyInputPanelContext::reset()
{
}

bool MyInputPanelContext::isComposing() const
{
    return false;
}

QString MyInputPanelContext::language()
{
    return "en_US";
}

//! [2]

void MyInputPanelContext::sendCharacter(QChar character)
{
    QPointer<QWidget> w = focusWidget();


    if (!w)
        return;
    if(character == '^')
    {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier, QString());
        QApplication::sendEvent(w, &keyPress);
    }
    else
    {
        QKeyEvent keyPress(QEvent::KeyPress, character.unicode(), Qt::NoModifier, QString(character));
        QApplication::sendEvent(w, &keyPress);
    }


    if (!w)
        return;

    if(character == '^')
    {
        QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_Backspace, Qt::NoModifier, QString());
        QApplication::sendEvent(w, &keyRelease);
    }
    else
    {
        QKeyEvent keyRelease(QEvent::KeyPress, character.unicode(), Qt::NoModifier, QString());
        QApplication::sendEvent(w, &keyRelease);
    }
    QWidget *w1 = focusWidget();
    qstr11 = dynamic_cast<QLineEdit*>(w1)->text();
    inputPanel->form.lineEdit->setText(qstr11);
}

//! [2]

//! [3]

void MyInputPanelContext::updatePosition()
{
    QWidget *widget = focusWidget();
    if (!widget)
        return;

    QRect widgetRect = widget->rect();
    QPoint panelPos;

    panelPos = QPoint(widgetRect.left(), widgetRect.bottom() + 2);
 //  the following 3 lines are not needed anymore
    qDebug() << "x,y = ("<< panelPos.x() <<","<< panelPos.y() << ")";
    panelPos = widget->mapToGlobal(panelPos);
    qDebug() << "x,y = ("<< panelPos.x() <<","<< panelPos.y() << ")";

    panelPos.setX(14);//307
    panelPos.setY(80); //193
    inputPanel->move(panelPos);

    QCursor c;
    c.setPos(0,0);
   }

//! [3]

