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
#include "hsclocksettingsnotifier_symbian.h"

#include <bacntf.h> // CEnvironmentChangeNotifier
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <clockdomaincrkeys.h>


namespace
{
    const char ANALOG[] = "analog";
    const char DIGITAL[] = "digital";
    const char TIME12[] = "TIME12";
    const char TIME24[] = "TIME24";
}

/*!
    \class HsClockSettingsNotifierType
    \ingroup group_HsClockSettingsNotifierplugin
    \brief Implementation for the observation of the system clock settings (analog/digital). 

*/


/*!
    Constructor
*/
HsClockSettingsNotifier::HsClockSettingsNotifier(QObject *parent)
    : QObject(parent),
      mDateTimeNotifier(0)
{
    mTimeFormat = timeFormatString();
    createSystemChangeObserver();
    
	// Connect for the clock type changes to refresh the view
	mSettingsManager = new XQSettingsManager();
	mClockTypeSettingsKey = new XQSettingsKey(
							XQSettingsKey::TargetCentralRepository,
							KCRUidClockApp,
							KClockType);
	
    mClockType = clockTypeString();
	
	mSettingsManager->startMonitoring(*mClockTypeSettingsKey);
	connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
				this, SLOT(onClockTypeChanged(XQSettingsKey, QVariant)));

}



/*!
    Destructor.
*/
HsClockSettingsNotifier::~HsClockSettingsNotifier()
{
    delete mDateTimeNotifier;
    mSettingsManager->stopMonitoring(*mClockTypeSettingsKey);
    delete mClockTypeSettingsKey;
    delete mSettingsManager;
}

/*!
    Returns the clock format ('analog'/'digital')
*/
QString HsClockSettingsNotifier::clockType() const
{
    return mClockType;
}

/*!
    Returns the time format ('12'/'24')
*/
QString HsClockSettingsNotifier::timeFormat() const
{
    return mTimeFormat;
}

/*!
    Callback function to receive system settings changes.
*/
TInt HsClockSettingsNotifier::EnvironmentChanged( TAny* aSelf )
{
    HsClockSettingsNotifier* self = static_cast<HsClockSettingsNotifier*>(aSelf);
    if ( self ){
        TInt change = self->mDateTimeNotifier->Change();
        if ( change & EChangesLocale ) {
            self->onSettingsChanged();
        }
    }
    return KErrNone;        
}

/*!
	Slot which is called when the value changes in cenrep.

	\param key The key which got changed in cenrep.
	\param value The new value of that key.
 */
void HsClockSettingsNotifier::onClockTypeChanged( const XQSettingsKey& key, const QVariant& value )
{
    Q_UNUSED(value)
	if (key.uid() == KCRUidClockApp && key.key() == KClockType) {
		onSettingsChanged();
	}
}

/*!
 Creates CEnvironmentChangeNotifier object to listen system settings changes.
*/
void HsClockSettingsNotifier::createSystemChangeObserver() 
{
    if ( !mDateTimeNotifier ){
        mDateTimeNotifier = CEnvironmentChangeNotifier::NewL( 
            CActive::EPriorityLow,
            TCallBack( EnvironmentChanged, this ) );
        
        mDateTimeNotifier->Start();            
    }
}   

/*!
    Reads clock format from system locale settings
*/
QString HsClockSettingsNotifier::clockTypeString() const
{
	int clockType = -1;
	// Read the clocktype value from the cenrep
	// 0 is for Analogue type and 1 for Digital
	QVariant value = mSettingsManager->readItemValue(*mClockTypeSettingsKey);
	clockType = value.toInt();

    if ( clockType == 0 ) {
        return QString(ANALOG);
    } else {
        return QString(DIGITAL);
    }
}

/*!
    Reads time format from system locale settings
*/
QString HsClockSettingsNotifier::timeFormatString() const
{
    TLocale locale;
    TTimeFormat timeFormat = locale.TimeFormat();
    if ( timeFormat == ETime12 ) {
        return QString(TIME12);
    } else {
        return QString(TIME24);
    }
}

/*!
*/
void HsClockSettingsNotifier::onSettingsChanged()
{
    QString clockType = clockTypeString();
    QString timeFormat = timeFormatString();
    if ( clockType != mClockType || timeFormat != mTimeFormat ) {
        mClockType = clockType;
        mTimeFormat = timeFormat;
        emit settingsChanged(mClockType, mTimeFormat);
    }
}

