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

#include <QEvent>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QTranslator>
#include <QtGlobal>
#include <QSettings>
#include <hbinstance.h>
#include <hbview.h>
#include "hsapptranslator.h"

#ifdef Q_OS_SYMBIAN
#include <f32file.h>
#endif //Q_OS_SYMBIAN

/*!
    \class HsAppTranslator
    \ingroup group_hsapplication
    \brief Homescreen application main class.
    Loads a runtime from a runtime provider plugin. Manages 
    the runtime execution. 
*/

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Stores the translator instance.
*/
HsAppTranslator *HsAppTranslator::mInstance = 0;

/*!
    Constructor.
    \a parent Parent object.
*/
HsAppTranslator::HsAppTranslator(QObject *parent)
  : QObject(parent)
{  
}

/*!
    Returns the hstranlator instance.
*/
HsAppTranslator *HsAppTranslator::instance()
{
    if (!mInstance) {
        mInstance = new HsAppTranslator();
    }
    return mInstance;
}

/*!
    Destructor.
*/
HsAppTranslator::~HsAppTranslator()
{
    removeTranslators();
}

/*!
    \copydoc QObject::eventFilter(QObject *watched, QEvent *event) 
*/
bool HsAppTranslator::eventFilter(QObject *watched, QEvent *event) 
{
    Q_UNUSED(watched);    
    if (event->type() == QEvent::LocaleChange) {
        setLanguage();
        emit languageChanged();
        return true;
    }

    return false;
}

void HsAppTranslator::setLanguage()
{
    removeTranslators();

#ifdef Q_OS_SYMBIAN    
    QString locale = QLocale::system().name();
#else
    QString locale;
    QFile file("hslocale.txt");
    QTextStream stream(&file);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QString word;
        stream >> word;
        if (!word.isEmpty()) {
            locale = word;
        } else {
            locale = "en_US";
            file.seek(0);
            stream << locale;
            file.close();
        }
    } else {
        locale = QLocale::system().name();
    }
#endif //Q_OS_SYMBIAN 

    loadTranslators(locale);
}

void HsAppTranslator::loadTranslators(const QString &locale)
{
    QString path = QDir::currentPath() + QString("/hsresources/loc");
    QDir currentDir = QDir(path);

    QString localeLowerCase = locale.toLower();
    QString localeUpperCase = locale.toUpper();
    QString language = locale.left(2);
    QString languageUpperCase = language.toUpper();
    QStringList nameFilters;
    nameFilters << "*_" + locale + ".qm" 
                << "*_" + localeLowerCase + ".qm" 
                << "*_" + localeUpperCase + ".qm" 
                << "*_" + language + ".qm" 
                << "*_" + languageUpperCase + ".qm";

    QStringList locFiles = currentDir.entryList(nameFilters, QDir::Files | QDir::NoSymLinks);
 
    for (int i = 0; i < locFiles.size(); ++i) {
        QString locFile = locFiles[i];
        bool loaded = false;
        QTranslator *translator = new QTranslator();
#ifdef Q_OS_SYMBIAN    
        TRAP_IGNORE(loaded = translator->load(locFile, path));
#else
        loaded = translator->load(locFile, path); 
#endif //Q_OS_SYMBIAN    
        if (!loaded) {
            QString debugMsg = "ERROR: QTranslator::load() failed for " + locFile;
            HSDEBUG(debugMsg);
        }
        qApp->installTranslator(translator);
        mTranslatorList << translator;
    }
}

void HsAppTranslator::removeTranslators()
{
    while (!mTranslatorList.isEmpty()) { 
        QTranslator *translator = mTranslatorList.takeFirst();
        qApp->removeTranslator(translator);
        delete translator;
        translator = 0;
    }
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
