/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Model for handling indicator data.
*
*/

#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <settingsinternalcrkeys.h>


#include "snsrindicatormodel.h"
#include "snsrindicatorinfo.h"

/*!
    \class SnsrIndicatorModel
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Model for handling indicator data.
 */

// TODO: what is the type string of silent indicator? couldn't
// find it in wk22 -> workaround solution: assume that it contains
// substring "silent"
const char *gSilentIndicatorTypeString = "silent";
const char *gOfflineIndicatorTypeString = "offline";


/*!
    Constructs a new SnsrIndicatorModel
    \param parent Parent object.
 */
SnsrIndicatorModel::SnsrIndicatorModel(QObject *parent): QObject(parent),
    mOfflineStateOn(false)
{   
    initializeOfflineModeIndication();
}

/*!
    Destructs the class.
 */
SnsrIndicatorModel::~SnsrIndicatorModel()
{
     mNotificationIndicators.clear();
     mSettingIndicators.clear();
     mIndicatorsToShow.clear();
     delete mSettingsManager;
     delete mOfflineKey;
}

/*!
    If there are already some active indicators when screensaver is
    launched, then this method is called.
    Indicators that are gonna be shown by the screensaver are
    added to the model. 
    \param activeIndicators Active universal indicators in 
    chronological order according to their arrival time.
 */
void SnsrIndicatorModel::handleActiveIndicators(
    const QList<HbIndicatorInterface*> &activeIndicators)
{
    mNotificationIndicators.clear();
    mSettingIndicators.clear();
    
    bool addedAny(false);
    
    for (int i = 0; i < activeIndicators.size(); ++i) {
        HbIndicatorInterface* activatedIndicator = activeIndicators.at(i);
        SnsrIndicatorInfo indicatorInfo;
        if (activatedIndicator 
            && showIndicatorInScreensaver(*activatedIndicator,indicatorInfo)) {
            addIndicator(indicatorInfo);
            addedAny = true;
        }
    }
    
    // Send signal only once if there were any such indicators that
    // screensaver is supposed to show (=added to the model).
    if (addedAny) {
        emitChangeSignal();
    }
    
    // TODO: no we need to listen to update signals?
    // used for changing icon?
    /* //connect indicator's update signal
     QObject::connect(activatedIndicator, SIGNAL(dataChanged()),
                      this, SLOT(indicatorUpdated()));*/
}

/*!
    Called when some universal indicator gets activated.
    \param activatedIndicator Activated universal indicator
 */
void SnsrIndicatorModel::handleActivatedIndicator(
    HbIndicatorInterface *activatedIndicator)
{
    SnsrIndicatorInfo indicatorInfo;
    if (activatedIndicator
        && showIndicatorInScreensaver(*activatedIndicator,indicatorInfo)) {
        addIndicator(indicatorInfo);
        emitChangeSignal();
    }
}

/*!
    Called when some universal indicator gets deactivated.
    \param deactivatedIndicator Deactivated universal indicator
 */
void SnsrIndicatorModel::handleDeactivatedIndicator(
    HbIndicatorInterface *deactivatedIndicator)
{
    if (deactivatedIndicator) {
        SnsrIndicatorInfo indicatorInfo(
            deactivatedIndicator->indicatorType(), 
            deactivatedIndicator->indicatorData(HbIndicatorInterface::MonoDecorationNameRole).toString(),
            deactivatedIndicator->category());
        
        if (findAndRemoveIndicator(indicatorInfo)) {
            emitChangeSignal();
        }
    }
}

/*!
    Sends a signal with a list of all currently active indicators.
    No signal is sent if there are no active indicators currently.
 */
void SnsrIndicatorModel::initializeIndicatorWidget()
{
    getCurrentOfflineState(); // update status to be on the safe side
    
    if ( anyActiveIndicators() ) {
        emitChangeSignal();
    }
}

/*!
    listens airplane repository key. Not sure if the state can even be changed at the moment.
    TODO: do we really need this?
 */
void SnsrIndicatorModel::offlineValueChanged( const XQSettingsKey &key, const QVariant &value )
{
    bool previousState(mOfflineStateOn);
    switch ( key.key() ) {
        case KSettingsAirplaneMode:
             if( value.toInt() == 1)
                 {
                 mOfflineStateOn = true;                     
                 }
             else
                 {
                 mOfflineStateOn = false; 
                 }            
             if (previousState != mOfflineStateOn) {
                 emitChangeSignal();
             }
             break;
        default:
            break;

        }
}

/*!
    Add the indicator into this model. Handle here the order in which
    indicators are shown in screensaver: notification indicators should be
    shown in the same order as shown in statusbar, that is in reversed
    chronological order. Silent indicator should always be the right-most.
 */
void SnsrIndicatorModel::addIndicator(const SnsrIndicatorInfo &indicatorInfo)
{
    // info from pattern library - todo: remove
    //Indicators are displayed inside of each group in reversed chronological order 
    // according to the arrival time; the indicator that appeared most recently is placed on top of the group.

    //Status bar indicators follow the same order; the most recent notification indicator (4) is on the left 
    // and similarly the most recent settings indicator (5) is on the right.

    // use prepend to keep the list in reversed chronological order
    if (indicatorInfo.category == HbIndicatorInterface::NotificationCategory) {
        mNotificationIndicators.prepend(indicatorInfo);
    }
    else {
        mSettingIndicators.append(indicatorInfo);
    }
}

/*!
   Remove indicator if it can be found.
   /retval true if the indicator was removed
 */
bool SnsrIndicatorModel::findAndRemoveIndicator(const SnsrIndicatorInfo &indicatorInfo)
{
    bool removed(false);
    int index(-1);
    QList<SnsrIndicatorInfo> &indicatorList = 
        indicatorInfo.category == HbIndicatorInterface::NotificationCategory 
        ? mNotificationIndicators : mSettingIndicators;
    
    for (int i = 0; i < indicatorList.size(); ++i) {
        if (indicatorList.at(i).type == indicatorInfo.type) {
            index = i;
            break;
        }
    }
    if (index >= 0) {
        indicatorList.removeAt(index);
        removed = true;
    }
    return removed;
}

/*!
   Check whether to show the indicator in screensaver or not.
   Icon path is asked with HbIndicatorInterface::MonoDecorationNameRole 
   which refers to the indicator icons shown also in the statusbar:
   only 4 Notification indicators has premission to publish data with
   that role. As those 4 are the ones wanted also by screensaver, no
   extra checks are needed.
   From the Setting category one should show only Silent indicator.
   No indicators are shown from the Progress category.
   If indicator should be shown by screensaver, indicator info is
   packaged and returned within the parameter.
   
   /retval true if the indicator should be shown in screensaver
 */
bool SnsrIndicatorModel::showIndicatorInScreensaver(
        const HbIndicatorInterface &indicatorInterface, SnsrIndicatorInfo &indicatorInfo)
{
    bool show(false);
    // all indicators shown by screensaver must have valid status bar icon path
    QString iconPath = 
            indicatorInterface.indicatorData(HbIndicatorInterface::MonoDecorationNameRole).toString();
    QString typeString = indicatorInterface.indicatorType();
    HbIndicatorInterface::Category category = indicatorInterface.category();
    
    if (category == HbIndicatorInterface::NotificationCategory
        && !iconPath.isEmpty()) {
        // only 4 indicators of this category has permission to publish status bar
        // icon path->we should show all of them->no additional checks are neeeded.
        show = true;
    }
    else if (category == HbIndicatorInterface::SettingCategory
             && !iconPath.isEmpty()
             // TODO: what is the type string of silent indicator? couldn't
             // find it in wk22 -> workaround solution: assume that it contains
             // substring "silent"
             // && type == gSilentIndicatorTypeString ) { 
             && typeString.contains(gSilentIndicatorTypeString, Qt::CaseInsensitive)) {  
        show = true;
    }
    else { // e.g. no Progress category indicators are shown
        show = false;
    }
 
    // package indicator data into indicatorInfo parameter
    indicatorInfo.type = typeString;
    indicatorInfo.iconPath = iconPath;
    indicatorInfo.category = category;
        
    return show;
}

/*!
   Check whether there is any active indicator currently.
   /retval true if there is at least one indicator to show
 */
bool SnsrIndicatorModel::anyActiveIndicators() const
{
  bool any(false);
  if ( mNotificationIndicators.size() != 0 
      || mSettingIndicators.size() != 0 
      || mOfflineStateOn ) {
      any = true;
  }
  return any;
}

/*!
    Send appropriate signal so that UI widget can update itself.
 */
void SnsrIndicatorModel::emitChangeSignal()
{   
    if (anyActiveIndicators()) {   
        updateIndicatorsToShowList();
        emit indicatorsUpdated(mIndicatorsToShow);
    }
    else {
        mIndicatorsToShow.clear();
        emit allIndicatorsDeactivated();
    }
}

/*!
   Updates the list that contains all the indicators that
   should be shown in UI. List should contain active indicators
   in this order: 
   offline + notification indicators + settings indicators.
   This is the list that will be sent to the widget.
 */
void SnsrIndicatorModel::updateIndicatorsToShowList()
{
    mIndicatorsToShow.clear();
    
    if (mOfflineStateOn) {
        mIndicatorsToShow.append(mOfflineIndicatorInfo);
    }
    mIndicatorsToShow.append(mNotificationIndicators);
    mIndicatorsToShow.append(mSettingIndicators);
}

/*!
 * Reads the airplanemode repository value and starts monitoring it.
 * if the value is 1, offline indicator is shown.
 */
void SnsrIndicatorModel::initializeOfflineModeIndication()
{
    mOfflineIndicatorInfo = SnsrIndicatorInfo(
        gOfflineIndicatorTypeString,
        QLatin1String("qtg_mono_offline"),
        HbIndicatorInterface::NotificationCategory);
    
    mSettingsManager = new XQSettingsManager(this);
    //connect to offlineValueChanged slot so we get information if the value is changed while screensaver is on
    connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
            this, SLOT(offlineValueChanged( const XQSettingsKey, const QVariant)));
    mOfflineKey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidCommunicationSettings.iUid, KSettingsAirplaneMode);
    if(mSettingsManager->readItemValue(*mOfflineKey).toInt() == 1)
        {
        mOfflineStateOn = true;
        } 
    mSettingsManager->startMonitoring(*mOfflineKey);
}

/*!
    Get current offline status.
 */
void SnsrIndicatorModel::getCurrentOfflineState()
{
    if(mSettingsManager->readItemValue(*mOfflineKey).toInt() == 1) {
        mOfflineStateOn = true;
    }
    else {
       mOfflineStateOn = false;
    }
}

