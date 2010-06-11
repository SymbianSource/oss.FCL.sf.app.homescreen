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
* Description:
*
*/
#include "hsconfiguration.h"
#include "hsdomainmodeldatastructures.h"
#include "hsdatabase.h"

#include <HbInstantFeedback>

/*!
    Loads configuration data from database.
*/
void HsConfiguration::loadConfiguration()
{
    HsDatabase *db = HsDatabase::instance();

    HsGeneralConfiguration generalConfiguration;
    if (db->generalConfiguration(generalConfiguration)) {
        mBounceEffect = generalConfiguration.bounceEffect;
        mTapAndHoldDistance = generalConfiguration.tapAndHoldDistance;
        mWidgetTapAndHoldTimeout = generalConfiguration.widgetTapAndHoldTimeout;
        mSceneTapAndHoldTimeout = generalConfiguration.sceneTapAndHoldTimeout;
        mPageChangeZoneWidth = generalConfiguration.pageChangeZoneWidth;
        mPageIndicatorSpacing = generalConfiguration.pageIndicatorSpacing;
        mPageChangeAnimationDuration = generalConfiguration.pageChangeAnimationDuration;
        mPageChangeZoneAnimationDuration = generalConfiguration.pageChangeZoneAnimationDuration;
        mPageChangeZoneReverseAnimationDuration = generalConfiguration.pageChangeZoneReverseAnimationDuration;
        mPageRemovedAnimationDuration = generalConfiguration.pageRemovedAnimationDuration;
        mNewPageAddedAnimationDuration = generalConfiguration.newPageAddedAnimationDuration; 
        mWidgetDragEffectDuration = generalConfiguration.widgetDragEffectDuration;
        mWidgetDropEffectDuration = generalConfiguration.widgetDropEffectDuration;
        mBounceFeedbackEffectDistance = generalConfiguration.boundaryFeedbackEffectDistance;
        mDefaultPageId = generalConfiguration.defaultPageId;
        mMaximumPageCount = generalConfiguration.maximumPageCount;
        mMaximumWidgetHeight = generalConfiguration.maximumWidgetHeight;
        mMaximumWidgetWidth = generalConfiguration.maximumWidgetWidth;
        mMinimumWidgetHeight = generalConfiguration.minimumWidgetHeight;
        mMinimumWidgetWidth = generalConfiguration.minimumWidgetWidth;
        mShortcutLabelsVisible = generalConfiguration.shortcutLabelsVisible;
        mPageChangePanDistance = generalConfiguration.pageChangePanDistance;
        mPageChangePanDistanceInPixels = mPageChangePanDistance * HbDeviceProfile::current().unitValue();
    } else {
        Q_ASSERT_X(1, "Configuration", "Homescreen configuration could not be loaded from database");
    }

    HsFeedbackConfiguration feedbackConfiguration;
    if (db->feedbackConfiguration(feedbackConfiguration)) {
        mPageChangeFeedbackType = feedbackConfiguration.pageChangeFeedbackType;
	    mWidgetPickFeedbackType = feedbackConfiguration.widgetPickFeedbackType;
	    mWidgetDropFeedbackType = feedbackConfiguration.widgetDropFeedbackType;
	    mWidgetRepositionFeedbackType = feedbackConfiguration.widgetRepositionFeedbackType;
	    mWidgetOverTrashbinFeedbackType = feedbackConfiguration.widgetOverTrashbinFeedbackType;
	    mWidgetDropToTrashbinFeedbackType = feedbackConfiguration.widgetDropToTrashbinFeedbackType;
	    mShortcutWidgetTapFeedbackType = feedbackConfiguration.shortcutWidgetTapFeedbackType;
	    mWidgetMoveBlockedFeedbackType = feedbackConfiguration.widgetMoveBlockedFeedbackType;
	    mClockWidgetTapFeedbackType = feedbackConfiguration.clockWidgetTapFeedbackType;
	    mWidgetSnappingFeedbackType = feedbackConfiguration.widgetSnappingFeedbackType;    
    } else {
        Q_ASSERT_X(1, "Configuration", "Homescreen feedback cnfiguration could not be loaded from database");
    }

    HsSnapConfiguration snapConfiguration;
    if (db->snapConfiguration(snapConfiguration)) {
        mSnapEnabled = snapConfiguration.snappingEnabled;
        mSnapForce = snapConfiguration.snapForce;
        mSnapGap = snapConfiguration.snapGap;
        mSnapBorderGap = snapConfiguration.borderGap;
        mSnapTimeout = snapConfiguration.timeout;
    } else {
        Q_ASSERT_X(1, "Configuration", "Homescreen snap configuration could not be loaded from database");
    }
}

// static member variables initilized here, these values will be used if fetching from database fails
int HsConfiguration::mBounceEffect = 20;
qreal HsConfiguration::mTapAndHoldDistance = 16.0;
int HsConfiguration::mWidgetTapAndHoldTimeout = 500;
int HsConfiguration::mSceneTapAndHoldTimeout = 500;
qreal HsConfiguration::mPageChangeZoneWidth = 60.0;
qreal HsConfiguration::mPageIndicatorSpacing = 8.0;
int HsConfiguration::mPageChangeAnimationDuration = 200;
int HsConfiguration::mPageChangeZoneAnimationDuration = 800;
int HsConfiguration::mPageChangeZoneReverseAnimationDuration = 200;
int HsConfiguration::mPageRemovedAnimationDuration = 200;
int HsConfiguration::mNewPageAddedAnimationDuration = 700;
int HsConfiguration::mWidgetDragEffectDuration = 200;
int HsConfiguration::mWidgetDropEffectDuration = 200;
int HsConfiguration::mBounceFeedbackEffectDistance = 3;
int HsConfiguration::mDefaultPageId = 1;
int HsConfiguration::mMaximumPageCount = 8;
qreal HsConfiguration::mMaximumWidgetHeight = 39;
qreal HsConfiguration::mMaximumWidgetWidth = 48;
qreal HsConfiguration::mMinimumWidgetHeight = 8.75;
qreal HsConfiguration::mMinimumWidgetWidth = 8.75;
bool HsConfiguration::mShortcutLabelsVisible = false;
qreal HsConfiguration::mPageChangePanDistance = 17.91;
int HsConfiguration::mPageChangePanDistanceInPixels = 120;

HbFeedback::InstantEffect HsConfiguration::mPageChangeFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mWidgetPickFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mWidgetDropFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mWidgetRepositionFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mWidgetOverTrashbinFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mWidgetDropToTrashbinFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mShortcutWidgetTapFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mWidgetMoveBlockedFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mClockWidgetTapFeedbackType = HbFeedback::None;
HbFeedback::InstantEffect HsConfiguration::mWidgetSnappingFeedbackType = HbFeedback::None;

bool HsConfiguration::mSnapEnabled = false;
qreal HsConfiguration::mSnapForce = 0;
qreal HsConfiguration::mSnapGap = 0;
qreal HsConfiguration::mSnapBorderGap = 0;
int HsConfiguration::mSnapTimeout = 0;

