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

#include "myinputpanel.h"
#include <QPoint>

//! [0]

MyInputPanel::MyInputPanel()
    : QWidget(0, Qt::Tool | Qt::WindowStaysOnTopHint),
      lastFocusedWidget(0)
{
    form.setupUi(this);

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(saveFocusWidget(QWidget*,QWidget*)));

    signalMapper.setMapping(form.panelButton_1, form.panelButton_1);
    signalMapper.setMapping(form.panelButton_2, form.panelButton_2);
    signalMapper.setMapping(form.panelButton_3, form.panelButton_3);
    signalMapper.setMapping(form.panelButton_4, form.panelButton_4);
    signalMapper.setMapping(form.panelButton_5, form.panelButton_5);
    signalMapper.setMapping(form.panelButton_6, form.panelButton_6);
    signalMapper.setMapping(form.panelButton_7, form.panelButton_7);
    signalMapper.setMapping(form.panelButton_8, form.panelButton_8);
    signalMapper.setMapping(form.panelButton_9, form.panelButton_9);
    signalMapper.setMapping(form.panelButton_star, form.panelButton_star);
    signalMapper.setMapping(form.panelButton_0, form.panelButton_0);
    signalMapper.setMapping(form.panelButton_hash, form.panelButton_hash);

    signalMapper.setMapping(form.panelButton_A, form.panelButton_A);
    signalMapper.setMapping(form.panelButton_B, form.panelButton_B);
    signalMapper.setMapping(form.panelButton_C, form.panelButton_C);
    signalMapper.setMapping(form.panelButton_D, form.panelButton_D);
    signalMapper.setMapping(form.panelButton_E, form.panelButton_E);
    signalMapper.setMapping(form.panelButton_F, form.panelButton_F);
    signalMapper.setMapping(form.panelButton_G, form.panelButton_G);
    signalMapper.setMapping(form.panelButton_H, form.panelButton_H);
    signalMapper.setMapping(form.panelButton_I, form.panelButton_I);
    signalMapper.setMapping(form.panelButton_J, form.panelButton_J);
    signalMapper.setMapping(form.panelButton_K, form.panelButton_K);
    signalMapper.setMapping(form.panelButton_L, form.panelButton_L);
    signalMapper.setMapping(form.panelButton_M, form.panelButton_M);
    signalMapper.setMapping(form.panelButton_N, form.panelButton_N);
    signalMapper.setMapping(form.panelButton_O, form.panelButton_O);
    signalMapper.setMapping(form.panelButton_P, form.panelButton_P);
    signalMapper.setMapping(form.panelButton_Q, form.panelButton_Q);
    signalMapper.setMapping(form.panelButton_R, form.panelButton_R);
    signalMapper.setMapping(form.panelButton_S, form.panelButton_S);
    signalMapper.setMapping(form.panelButton_T, form.panelButton_T);
    signalMapper.setMapping(form.panelButton_U, form.panelButton_U);
    signalMapper.setMapping(form.panelButton_V, form.panelButton_V);
    signalMapper.setMapping(form.panelButton_W, form.panelButton_W);
    signalMapper.setMapping(form.panelButton_X, form.panelButton_X);
    signalMapper.setMapping(form.panelButton_Y, form.panelButton_Y);
    signalMapper.setMapping(form.panelButton_Z, form.panelButton_Z);

    signalMapper.setMapping(form.panelButton_LT, form.panelButton_LT);
    signalMapper.setMapping(form.panelButton_GT, form.panelButton_GT);
    signalMapper.setMapping(form.panelButton_QUESTION, form.panelButton_QUESTION);
    signalMapper.setMapping(form.panelButton_NE, form.panelButton_NE);
    signalMapper.setMapping(form.panelButton_AT, form.panelButton_AT);
    signalMapper.setMapping(form.panelButton_ASH, form.panelButton_ASH);
    signalMapper.setMapping(form.panelButton_DOLLOR, form.panelButton_DOLLOR);
    signalMapper.setMapping(form.panelButton_MINUS, form.panelButton_MINUS);
    signalMapper.setMapping(form.panelButton_PLUS, form.panelButton_PLUS);
    signalMapper.setMapping(form.panelButton_MUL, form.panelButton_MUL);
    signalMapper.setMapping(form.panelButton_BO, form.panelButton_BO);
    signalMapper.setMapping(form.panelButton_BC, form.panelButton_BC);
    signalMapper.setMapping(form.panelButton_AND, form.panelButton_AND);
    signalMapper.setMapping(form.panelButton_CASE, form.panelButton_CASE);
    signalMapper.setMapping(form.panelButton_NEG, form.panelButton_NEG);
    signalMapper.setMapping(form.panelButton_COMMA, form.panelButton_COMMA);
    signalMapper.setMapping(form.panelButton_US, form.panelButton_US);
    signalMapper.setMapping(form.panelButton_BWS, form.panelButton_BWS);
    signalMapper.setMapping(form.panelButton_COLON, form.panelButton_COLON);
    signalMapper.setMapping(form.panelButton_SEMICOLON, form.panelButton_SEMICOLON);
    signalMapper.setMapping(form.panelButton_SQ, form.panelButton_SQ);
    signalMapper.setMapping(form.panelButton_FWS, form.panelButton_FWS);
    signalMapper.setMapping(form.panelButton_EQUAL, form.panelButton_EQUAL);

    connect(form.panelButton_A, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_B, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_C, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_D, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_E, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_F, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_G, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_H, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_I, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_J, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_K, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_L, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_M, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_N, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_O, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_P, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_Q, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_R, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_S, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_T, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_U, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_V, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_W, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_X, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_Y, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_Z, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));

    connect(form.panelButton_LT, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_GT, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_QUESTION, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_NE, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_AT, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_ASH, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_DOLLOR, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_MINUS, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_PLUS, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_MUL, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_BO, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_BC, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_AND, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_CASE, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_NEG, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_COMMA, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_US, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_BWS, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_COLON, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_SEMICOLON, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_SQ, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_FWS, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    /*connect(form.panelButton_FWS, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));*/
    connect(form.panelButton_EQUAL, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));


    connect(form.panelButton_1, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_2, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_3, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_4, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_5, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_6, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_7, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_8, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_9, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_star, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_0, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));
    connect(form.panelButton_hash, SIGNAL(clicked()),
            &signalMapper, SLOT(map()));

    connect(&signalMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(buttonClicked(QWidget*)));
    form.lineEdit->setText("qstrrrtr");

}

//! [0]

bool MyInputPanel::event(QEvent *e)
{
    switch (e->type()) {
//! [1]
    case QEvent::WindowActivate:
        if (lastFocusedWidget)
        {
            lastFocusedWidget->activateWindow();
        }
        break;
//! [1]
    default:
        break;
    }
    return QWidget::event(e);
}

//! [2]

void MyInputPanel::saveFocusWidget(QWidget */*oldFocus*/, QWidget *newFocus)
{
    if (newFocus != 0 && !this->isAncestorOf(newFocus))
    {
        lastFocusedWidget = newFocus;
    }
}

//! [2]

//! [3]
void MyInputPanel::buttonClicked(QWidget *w)
{
    static char lower_upper_flag;
    QChar chr = qvariant_cast<QChar>(w->property("buttonValue"));
    if((chr >= '0')&&(chr <= '9'));
    else
    {
        if(chr == '|')
        {
            lower_upper_flag ^= 1;
            if(lower_upper_flag == 1)
            {
                form.panelButton_A->setText("a");
                form.panelButton_B->setText("b");
                form.panelButton_C->setText("c");
                form.panelButton_D->setText("d");
                form.panelButton_E->setText("e");
                form.panelButton_F->setText("f");
                form.panelButton_G->setText("g");
                form.panelButton_H->setText("h");
                form.panelButton_I->setText("i");
                form.panelButton_J->setText("j");
                form.panelButton_K->setText("k");
                form.panelButton_L->setText("l");
                form.panelButton_M->setText("m");
                form.panelButton_N->setText("n");
                form.panelButton_O->setText("o");
                form.panelButton_P->setText("p");
                form.panelButton_Q->setText("q");
                form.panelButton_R->setText("r");
                form.panelButton_S->setText("s");
                form.panelButton_T->setText("t");
                form.panelButton_U->setText("u");
                form.panelButton_V->setText("v");
                form.panelButton_W->setText("w");
                form.panelButton_X->setText("x");
                form.panelButton_Y->setText("y");
                form.panelButton_Z->setText("z");
                form.panelButton_CASE->setText("AB");
            }
            else
            {
                form.panelButton_A->setText("A");
                form.panelButton_B->setText("B");
                form.panelButton_C->setText("C");
                form.panelButton_D->setText("D");
                form.panelButton_E->setText("E");
                form.panelButton_F->setText("F");
                form.panelButton_G->setText("G");
                form.panelButton_H->setText("H");
                form.panelButton_I->setText("I");
                form.panelButton_J->setText("J");
                form.panelButton_K->setText("K");
                form.panelButton_L->setText("L");
                form.panelButton_M->setText("M");
                form.panelButton_N->setText("N");
                form.panelButton_O->setText("O");
                form.panelButton_P->setText("P");
                form.panelButton_Q->setText("Q");
                form.panelButton_R->setText("R");
                form.panelButton_S->setText("S");
                form.panelButton_T->setText("T");
                form.panelButton_U->setText("U");
                form.panelButton_V->setText("V");
                form.panelButton_W->setText("W");
                form.panelButton_X->setText("X");
                form.panelButton_Y->setText("Y");
                form.panelButton_Z->setText("Z");
                form.panelButton_CASE->setText("ab");
            }
        }
        if((lower_upper_flag == 1)&&(chr >= 'A')&&(chr <= 'Z'))
            chr = chr.toAscii()+32;
    }
    if(chr != '|')
        emit characterGenerated(chr);
}

//! [3]

