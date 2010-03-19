/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Clock widget
*
*/

#include <QTimer>
#include "hsclockwidgettype_symbian.h"

#include <bacntf.h> // CEnvironmentChangeNotifier


namespace
{
    const char ANALOG[] = "analog";
    const char DIGITAL[] = "digital";
}

/*!
    \class HsClockWidgetTypeType
    \ingroup group_HsClockWidgetTypeplugin
    \brief Implementation for the observation of the system clock settings (analog/digital). 

*/


/*!
    Constructor
*/
HsClockWidgetType::HsClockWidgetType(QObject *parent)
    : QObject(parent),
      mDateTimeNotifier(0),
      mClockType(ANALOG)
{
    createObserver();
    mClockType = localeTypeString();
}



/*!
    Destructor.
*/
HsClockWidgetType::~HsClockWidgetType()
{
    delete mDateTimeNotifier;
}

/*!
    Returns the clock type ('analog'/'digital')
*/
QString HsClockWidgetType::type() const
{
    return mClockType;
}

/*!
    Callback function to receive system settings changes.
*/
TInt HsClockWidgetType::EnvironmentChanged( TAny* aSelf )
{
    HsClockWidgetType* self = static_cast<HsClockWidgetType*>(aSelf);
    if ( self ){
        TInt change = self->mDateTimeNotifier->Change();
        if ( change & EChangesLocale ) {
            self->onTypeChanged();
        }
    }
    return KErrNone;        
}

/*!
 Creates CEnvironmentChangeNotifier object to listen system settings changes.
*/
void HsClockWidgetType::createObserver() 
{
    if ( !mDateTimeNotifier ){
        mDateTimeNotifier = CEnvironmentChangeNotifier::NewL( 
            CActive::EPriorityLow,
            TCallBack( EnvironmentChanged, this ) );
        
        mDateTimeNotifier->Start();            
    }
}   

/*!
    Reads clock type from system locale settings
*/
QString HsClockWidgetType::localeTypeString() const
{
    QString clockType;
    TLocale locale;
    TClockFormat clockFormat = locale.ClockFormat();
    if ( clockFormat==EClockAnalog ) {
        return QString(ANALOG);
    } else {
        return QString(DIGITAL);
    }
}

/*!
*/
void HsClockWidgetType::onTypeChanged()
{
    QString clockType = localeTypeString();
    if ( clockType != mClockType ) {
        mClockType = clockType;
        emit typeChanged(mClockType);
    }
}

