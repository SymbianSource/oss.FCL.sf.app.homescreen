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

#ifndef HSCONFIGURATION_H
#define HSCONFIGURATION_H

#include <QObject>

#include <HbFeedback>
#include <HbDeviceProfile>

#include "hsdomainmodel_global.h"

class HsGeneralConfiguration;
class HsFeedbackConfiguration;
class HsSnapConfiguration;
class HsDatabase;
class HsConfiguration;

class HSDOMAINMODEL_EXPORT HsConfiguration
{
    
public:
    static void loadConfiguration();

    static inline int bounceEffect() { return mBounceEffect; };
    static inline qreal tapAndHoldDistance() { return mTapAndHoldDistance; };
    static inline int widgetTapAndHoldTimeout() { return mWidgetTapAndHoldTimeout; };
    static inline int sceneTapAndHoldTimeout() { return mSceneTapAndHoldTimeout; };
    static inline qreal pageChangeZoneWidth() { return mPageChangeZoneWidth; };
    static inline qreal pageIndicatorSpacing() { return mPageIndicatorSpacing; };
    static inline int pageChangeAnimationDuration() { return mPageChangeAnimationDuration; };
    static inline int pageChangeZoneAnimationDuration() { return mPageChangeZoneAnimationDuration; };
    static inline int pageChangeZoneReverseAnimationDuration() { return mPageChangeZoneReverseAnimationDuration; };
    static inline int pageRemovedAnimationDuration() { return mPageRemovedAnimationDuration; };
    static inline int newPageAddedAnimationDuration() { return mNewPageAddedAnimationDuration; };
    static inline int widgetDragEffectDuration() { return mWidgetDragEffectDuration; };
    static inline int widgetDropEffectDuration() { return mWidgetDropEffectDuration; };
    static inline int bounceFeedbackEffectDistance() { return mBounceFeedbackEffectDistance; };
    static inline int defaultPageId() { return mDefaultPageId; };
    static inline int maximumPageCount() { return mMaximumPageCount; };
    static inline qreal maximumWidgetHeight() { return mMaximumWidgetHeight; };
    static inline qreal maximumWidgetWidth() { return mMaximumWidgetWidth; };
    static inline qreal minimumWidgetHeight() { return mMinimumWidgetHeight; };
    static inline qreal minimumWidgetWidth() { return mMinimumWidgetWidth; };
    static inline bool shortcutLabelsVisible() { return mShortcutLabelsVisible; };
    static inline qreal pageChangePanDistance() { return mPageChangePanDistance; };
    static inline int pageChangePanDistanceInPixels() { return mPageChangePanDistanceInPixels; };

    static inline HbFeedback::InstantEffect pageChangeFeedbackType() { return mPageChangeFeedbackType; };
    static inline HbFeedback::InstantEffect widgetPickFeedbackType() { return mWidgetPickFeedbackType; };
    static inline HbFeedback::InstantEffect widgetDropFeedbackType() { return mWidgetDropFeedbackType; };
    static inline HbFeedback::InstantEffect widgetRepositionFeedbackType() { return mWidgetRepositionFeedbackType; };
    static inline HbFeedback::InstantEffect widgetOverTrashbinFeedbackType() { return mWidgetOverTrashbinFeedbackType; };
    static inline HbFeedback::InstantEffect widgetDropToTrashbinFeedbackType() { return mWidgetDropToTrashbinFeedbackType; };
    static inline HbFeedback::InstantEffect shortcutWidgetTapFeedbackType() { return mShortcutWidgetTapFeedbackType; };
    static inline HbFeedback::InstantEffect widgetMoveBlockedFeedbackType() { return mWidgetMoveBlockedFeedbackType; };
    static inline HbFeedback::InstantEffect clockWidgetTapFeedbackType() { return mClockWidgetTapFeedbackType; };
    static inline HbFeedback::InstantEffect widgetSnappingFeedbackType() { return mWidgetSnappingFeedbackType; };

    static inline bool snapEnabled() { return mSnapEnabled; };
    static inline qreal snapForce() { return mSnapForce; };
    static inline qreal snapGap() { return mSnapGap; };
    static inline qreal snapBorderGap() { return mSnapBorderGap; };
    static inline int snapTimeout() { return mSnapTimeout; };

    // setters should be removed before code is released to SF
    static void setBounceEffect(int bounceEffect) { mBounceEffect = bounceEffect; };
    static void setTapAndHoldDistance(qreal tapAndHoldDistance) { mTapAndHoldDistance = tapAndHoldDistance; };
    static void setWidgetTapAndHoldTimeout(int tapAndHoldTimeout) { mWidgetTapAndHoldTimeout = tapAndHoldTimeout; };
    static void setSceneTapAndHoldTimeout(int tapAndHoldTimeout) { mSceneTapAndHoldTimeout = tapAndHoldTimeout; };
    static void setPageChangeZoneWidth(qreal pageChangeZoneWidth) { mPageChangeZoneWidth = pageChangeZoneWidth; };
    static void setPageIndicatorSpacing(qreal pageIndicatorSpacing) { mPageIndicatorSpacing = pageIndicatorSpacing; };
    static void setPageChangeAnimationDuration(int pageChangeAnimationDuration) { mPageChangeAnimationDuration = pageChangeAnimationDuration; };
    static void setPageChangeZoneAnimationDuration(int pageChangeZoneAnimationDuration) { mPageChangeZoneAnimationDuration = pageChangeZoneAnimationDuration; };
    static void setPageChangeZoneReverseAnimationDuration(int pageChangeZoneReverseAnimationDuration) { mPageChangeZoneReverseAnimationDuration = pageChangeZoneReverseAnimationDuration; };
    static void setPageRemovedAnimationDuration(int pageRemovedAnimationDuration) { mPageRemovedAnimationDuration = pageRemovedAnimationDuration; };
    static void setNewPageAddedAnimationDuration(int newPageAddedAnimationDuration) { mNewPageAddedAnimationDuration = newPageAddedAnimationDuration; };
    static void setWidgetDragEffectDuration(int widgetDragEffectDuration) { mWidgetDragEffectDuration = widgetDragEffectDuration; };
    static void setWidgetDropEffectDuration(int widgetDropEffectDuration) { mWidgetDropEffectDuration = widgetDropEffectDuration; };
    static void setBounceFeedbackEffectDistance(int bounceFeedbackEffectDistance) { mBounceFeedbackEffectDistance = bounceFeedbackEffectDistance; };
    static void setDefaultPageId(int defaultPageId) { mDefaultPageId = defaultPageId;  };
    static void setMaximumPageCount(int maximumPageCount) { mMaximumPageCount = maximumPageCount; };
    static void setMaximumWidgetHeight(qreal maximumWidgetHeight) { mMaximumWidgetHeight = maximumWidgetHeight; };
    static void setMaximumWidgetWidth(qreal maximumWidgetWidth) { mMaximumWidgetWidth = maximumWidgetWidth; };
    static void setMinimumWidgetHeight(qreal minimumWidgetHeight) { mMinimumWidgetHeight = minimumWidgetHeight; };
    static void setMinimumWidgetWidth(qreal minimumWidgetWidth) { mMinimumWidgetWidth = minimumWidgetWidth; };
    static void setShortcutLabelsVisible(bool shortcutLabelsVisible) { mShortcutLabelsVisible = shortcutLabelsVisible; };
    static void setPageChangePanDistance(qreal pageChangePanDistance) { mPageChangePanDistance = pageChangePanDistance; 
                                                                        mPageChangePanDistanceInPixels = pageChangePanDistance * HbDeviceProfile::current().unitValue(); };

    static void setPageChangeFeedbackType(HbFeedback::InstantEffect pageChangeFeedbackType ) { mPageChangeFeedbackType = pageChangeFeedbackType; };
    static void setWidgetPickFeedbackType(HbFeedback::InstantEffect widgetPickFeedbackType ) { mWidgetPickFeedbackType = widgetPickFeedbackType; };
    static void setWidgetDropFeedbackType(HbFeedback::InstantEffect widgetDropFeedbackType ) { mWidgetDropFeedbackType = widgetDropFeedbackType; };
    static void setWidgetRepositionFeedbackType(HbFeedback::InstantEffect widgetRepositionFeedbackType ) { mWidgetRepositionFeedbackType = widgetRepositionFeedbackType; };
    static void setWidgetOverTrashbinFeedbackType(HbFeedback::InstantEffect widgetOverTrashbinFeedbackType ) { mWidgetOverTrashbinFeedbackType = widgetOverTrashbinFeedbackType; };
    static void setWidgetDropToTrashbinFeedbackType(HbFeedback::InstantEffect widgetDropToTrashbinFeedbackType ) { mWidgetDropToTrashbinFeedbackType = widgetDropToTrashbinFeedbackType; };
    static void setShortcutWidgetTapFeedbackType(HbFeedback::InstantEffect shortcutWidgetTapFeedbackType ) { mShortcutWidgetTapFeedbackType = shortcutWidgetTapFeedbackType; };
    static void setWidgetMoveBlockedFeedbackType(HbFeedback::InstantEffect widgetMoveBlockedFeedbackType ) { mWidgetMoveBlockedFeedbackType = widgetMoveBlockedFeedbackType; };
    static void setClockWidgetTapFeedbackType(HbFeedback::InstantEffect clockWidgetTapFeedbackType ) { mClockWidgetTapFeedbackType = clockWidgetTapFeedbackType; };
    static void setWidgetSnappingFeedbackType(HbFeedback::InstantEffect widgetSnappingFeedbackType ) { mWidgetSnappingFeedbackType = widgetSnappingFeedbackType; };

    static void setSnapEnabled(bool snapEnabled) { mSnapEnabled = snapEnabled; };
    static void setSnapForce(qreal snapForce) { mSnapForce = snapForce; };
    static void setSnapGap(qreal snapGap) { mSnapGap = snapGap; };
    static void setSnapBorderGap(qreal snapBorderGap) { mSnapBorderGap = snapBorderGap; };
    static void setSnapTimeout(int snapTimeout) { mSnapTimeout = snapTimeout; };

private:
    static int mBounceEffect;		
	static qreal mTapAndHoldDistance;
	static int mWidgetTapAndHoldTimeout;
	static int mSceneTapAndHoldTimeout;
    static qreal mPageChangeZoneWidth;
	static qreal mPageIndicatorSpacing;
    static int mPageChangeAnimationDuration;
	static int mPageChangeZoneAnimationDuration;
    static int mPageChangeZoneReverseAnimationDuration;
	static int mPageRemovedAnimationDuration;
	static int mNewPageAddedAnimationDuration;
	static int mWidgetDragEffectDuration;
	static int mWidgetDropEffectDuration;
	static int mBounceFeedbackEffectDistance;
    static int mDefaultPageId;
    static int mMaximumPageCount;
    static qreal mMaximumWidgetHeight;
    static qreal mMaximumWidgetWidth;
    static qreal mMinimumWidgetHeight;
    static qreal mMinimumWidgetWidth;
    static bool mShortcutLabelsVisible;
    static qreal mPageChangePanDistance;
    static int mPageChangePanDistanceInPixels;

    static HbFeedback::InstantEffect mPageChangeFeedbackType;
	static HbFeedback::InstantEffect mWidgetPickFeedbackType;
	static HbFeedback::InstantEffect mWidgetDropFeedbackType;
	static HbFeedback::InstantEffect mWidgetRepositionFeedbackType;
	static HbFeedback::InstantEffect mWidgetOverTrashbinFeedbackType;
	static HbFeedback::InstantEffect mWidgetDropToTrashbinFeedbackType;
	static HbFeedback::InstantEffect mShortcutWidgetTapFeedbackType;
	static HbFeedback::InstantEffect mWidgetMoveBlockedFeedbackType;
	static HbFeedback::InstantEffect mClockWidgetTapFeedbackType;
	static HbFeedback::InstantEffect mWidgetSnappingFeedbackType;

    static bool mSnapEnabled;
    static qreal mSnapForce;
    static qreal mSnapGap;
    static qreal mSnapBorderGap;
    static int mSnapTimeout;
};

#endif // HSCONFIGURATION_H
