/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Homescreen application translator class.
*
*/

#ifndef HSAPPTRANSLATOR_H
#define HSAPPTRANSLATOR_H

#include <QObject>
#include <QList>
#include "hsutils_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(t_hsUtils)

class QEvent;
class QTranslator;

class HSUTILS_EXPORT HsAppTranslator : public QObject
{
    Q_OBJECT

public:
    ~HsAppTranslator();
    static HsAppTranslator *instance();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

public:
    void setLanguage();

private:
    void loadTranslators(const QString &locale);
    void removeTranslators();

signals:

    void languageChanged();

private:
    HsAppTranslator(QObject* aParent = 0);
    Q_DISABLE_COPY(HsAppTranslator)
    static HsAppTranslator *mInstance;
    QList<QTranslator *> mTranslatorList;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif
