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
* Description:
*
*/

#ifndef HSCONFIGURATION_H
#define HSCONFIGURATION_H

#include <QObject>
#include <QVariant>

#include <HbFeedback>

#include "hsdomainmodel_global.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestHsDomainModel)

#define HSCONFIGURATION_GET(getter) \
    HsConfiguration::instance()->getter()
#define HSCONFIGURATION_SET(setter, value) \
    HsConfiguration::instance()->setter(value)

class HSDOMAINMODEL_EXPORT HsConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int bounceEffect READ bounceEffect WRITE setBounceEffect)
    Q_PROPERTY(int widgetTapAndHoldTimeout READ widgetTapAndHoldTimeout WRITE setWidgetTapAndHoldTimeout)
    Q_PROPERTY(int sceneTapAndHoldTimeout READ sceneTapAndHoldTimeout WRITE setSceneTapAndHoldTimeout)
    Q_PROPERTY(int pageChangeAnimationDuration READ pageChangeAnimationDuration WRITE setPageChangeAnimationDuration)
    Q_PROPERTY(int pageChangeZoneAnimationDuration READ pageChangeZoneAnimationDuration WRITE setPageChangeZoneAnimationDuration)
    Q_PROPERTY(int pageChangeZoneReverseAnimationDuration READ pageChangeZoneReverseAnimationDuration WRITE setPageChangeZoneReverseAnimationDuration)
    Q_PROPERTY(int pageRemovedAnimationDuration READ pageRemovedAnimationDuration WRITE setPageRemovedAnimationDuration)
    Q_PROPERTY(int newPageAddedAnimationDuration READ newPageAddedAnimationDuration WRITE setNewPageAddedAnimationDuration)
    Q_PROPERTY(int widgetDragEffectDuration READ widgetDragEffectDuration WRITE setWidgetDragEffectDuration)
    Q_PROPERTY(int widgetDropEffectDuration READ widgetDropEffectDuration WRITE setWidgetDropEffectDuration)
    Q_PROPERTY(int defaultPageId READ defaultPageId WRITE setDefaultPageId)
    Q_PROPERTY(int maximumPageCount READ maximumPageCount WRITE setMaximumPageCount)    
    Q_PROPERTY(bool isShortcutLabelVisible READ isShortcutLabelVisible WRITE setShortcutLabelVisible)
    Q_PROPERTY(qreal bounceFeedbackEffectDistance READ bounceFeedbackEffectDistance WRITE setBounceFeedbackEffectDistance)
    Q_PROPERTY(qreal pageChangePanDistanceInPixels READ pageChangePanDistanceInPixels WRITE setPageChangePanDistanceInPixels)
    Q_PROPERTY(qreal tapAndHoldDistance READ tapAndHoldDistance WRITE setTapAndHoldDistance)
    Q_PROPERTY(qreal pageChangeZoneWidth READ pageChangeZoneWidth WRITE setPageChangeZoneWidth)
    Q_PROPERTY(qreal pageIndicatorSpacing READ pageIndicatorSpacing WRITE setPageIndicatorSpacing)
    Q_PROPERTY(qreal maximumWidgetHeight READ maximumWidgetHeight WRITE setMaximumWidgetHeight)
    Q_PROPERTY(qreal maximumWidgetWidth READ maximumWidgetWidth WRITE setMaximumWidgetWidth)
    Q_PROPERTY(qreal minimumWidgetHeight READ minimumWidgetHeight WRITE setMinimumWidgetHeight)
    Q_PROPERTY(qreal minimumWidgetWidth READ minimumWidgetWidth WRITE setMinimumWidgetWidth)
    Q_PROPERTY(qreal pageChangePanDistance READ pageChangePanDistance WRITE setPageChangePanDistance)
    Q_PROPERTY(HbFeedback::InstantEffect pageChangeFeedbackEffect READ pageChangeFeedbackEffect WRITE setPageChangeFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetPickFeedbackEffect READ widgetPickFeedbackEffect WRITE setWidgetPickFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetDropFeedbackEffect READ widgetDropFeedbackEffect WRITE setWidgetDropFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetRepositionFeedbackEffect READ widgetRepositionFeedbackEffect WRITE setWidgetRepositionFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetOverTrashbinFeedbackEffect READ widgetOverTrashbinFeedbackEffect WRITE setWidgetOverTrashbinFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetDropToTrashbinFeedbackEffect READ widgetDropToTrashbinFeedbackEffect WRITE setWidgetDropToTrashbinFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect shortcutWidgetTapFeedbackEffect READ shortcutWidgetTapFeedbackEffect WRITE setShortcutWidgetTapFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetMoveBlockedFeedbackEffect READ widgetMoveBlockedFeedbackEffect WRITE setWidgetMoveBlockedFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect clockWidgetTapFeedbackEffect READ clockWidgetTapFeedbackEffect WRITE setClockWidgetTapFeedbackEffect)
    Q_PROPERTY(HbFeedback::InstantEffect widgetSnappingFeedbackEffect READ widgetSnappingFeedbackEffect WRITE setWidgetSnappingFeedbackEffect)
    Q_PROPERTY(bool isSnapEnabled READ isSnapEnabled WRITE setSnapEnabled)
    Q_PROPERTY(int snapTimeout READ snapTimeout WRITE setSnapTimeout)
    Q_PROPERTY(qreal snapForce READ snapForce WRITE setSnapForce)
    Q_PROPERTY(qreal snapGap READ snapGap WRITE setSnapGap)
    Q_PROPERTY(qreal snapBorderGap READ snapBorderGap WRITE setSnapBorderGap)
    Q_PROPERTY(int snapLineFadeInDuration READ snapLineFadeInDuration WRITE setSnapLineFadeInDuration)
    Q_PROPERTY(int snapLineFadeOutDuration READ snapLineFadeOutDuration WRITE setSnapLineFadeOutDuration)
    Q_PROPERTY(bool isSnapEffectsEnabled READ isSnapEffectsEnabled WRITE setSnapEffectsEnabled)
    Q_PROPERTY(SceneType sceneType READ sceneType WRITE setSceneType)

    Q_ENUMS(SceneType)

public:
    HsConfiguration(QObject *parent = 0);
    ~HsConfiguration();

    enum SceneType
    {
        PageWallpapers,
        SceneWallpaper
    };

    int bounceEffect() const { return mBounceEffect; }
    void setBounceEffect(int effect) { mBounceEffect = effect; }
    int widgetTapAndHoldTimeout() const { return mWidgetTapAndHoldTimeout; }
    void setWidgetTapAndHoldTimeout(int timeout) { mWidgetTapAndHoldTimeout = timeout; }
    int sceneTapAndHoldTimeout() const { return mSceneTapAndHoldTimeout; }
    void setSceneTapAndHoldTimeout(int timeout) { mSceneTapAndHoldTimeout = timeout; }
    int pageChangeAnimationDuration() const { return mPageChangeAnimationDuration; }
    void setPageChangeAnimationDuration(int duration) { mPageChangeAnimationDuration = duration; }    
    int pageChangeZoneAnimationDuration() const { return mPageChangeZoneAnimationDuration; }
    void setPageChangeZoneAnimationDuration(int duration) { mPageChangeZoneAnimationDuration = duration; }    
    int pageChangeZoneReverseAnimationDuration() const { return mPageChangeZoneReverseAnimationDuration; }
    void setPageChangeZoneReverseAnimationDuration(int duration) { mPageChangeZoneReverseAnimationDuration = duration; }
    int pageRemovedAnimationDuration() const { return mPageRemovedAnimationDuration; }
    void setPageRemovedAnimationDuration(int duration) { mPageRemovedAnimationDuration = duration; }    
    int newPageAddedAnimationDuration() const { return mNewPageAddedAnimationDuration; }
    void setNewPageAddedAnimationDuration(int duration) { mNewPageAddedAnimationDuration = duration; }  
    int widgetDragEffectDuration() const { return mWidgetDragEffectDuration; }
    void setWidgetDragEffectDuration(int duration) { mWidgetDragEffectDuration = duration; }  
    int widgetDropEffectDuration() const { return mWidgetDropEffectDuration; }
    void setWidgetDropEffectDuration(int duration) { mWidgetDropEffectDuration = duration; }  
    int defaultPageId() const { return mDefaultPageId; }
    void setDefaultPageId(int id) { mDefaultPageId = id; }
    int maximumPageCount() const { return mMaximumPageCount; }
    void setMaximumPageCount(int count) { mMaximumPageCount = count; }  
    bool isShortcutLabelVisible() const { return mIsShortcutLabelVisible; }
    void setShortcutLabelVisible(bool visible) { mIsShortcutLabelVisible = visible; }  
    qreal bounceFeedbackEffectDistance() const { return mBounceFeedbackEffectDistance; }
    void setBounceFeedbackEffectDistance(qreal distance) { mBounceFeedbackEffectDistance = distance; }  
    qreal pageChangePanDistanceInPixels() const { return mPageChangePanDistanceInPixels; }
    void setPageChangePanDistanceInPixels(qreal distance) { mPageChangePanDistanceInPixels = distance; }  
    qreal tapAndHoldDistance() const { return mTapAndHoldDistance; }
    void setTapAndHoldDistance(qreal distance) { mTapAndHoldDistance = distance; }  
    qreal pageChangeZoneWidth() const { return mPageChangeZoneWidth; }
    void setPageChangeZoneWidth(qreal width) { mPageChangeZoneWidth = width; }  
    qreal pageIndicatorSpacing() const { return mPageIndicatorSpacing; }
    void setPageIndicatorSpacing(qreal spacing) { mPageIndicatorSpacing = spacing; }
    qreal maximumWidgetHeight() const { return mMaximumWidgetHeight; }
    void setMaximumWidgetHeight(qreal height) { mMaximumWidgetHeight = height; }  
    qreal maximumWidgetWidth() const { return mMaximumWidgetWidth; }
    void setMaximumWidgetWidth(qreal width) { mMaximumWidgetWidth = width; }  
    qreal minimumWidgetHeight() const { return mMinimumWidgetHeight; }
    void setMinimumWidgetHeight(qreal height) { mMinimumWidgetHeight = height; }
    qreal minimumWidgetWidth() const { return mMinimumWidgetWidth; }
    void setMinimumWidgetWidth(qreal width) { mMinimumWidgetWidth = width; }  
    qreal pageChangePanDistance() const { return mPageChangePanDistance; }
    void setPageChangePanDistance(qreal distance) { mPageChangePanDistance = distance; }  
    HbFeedback::InstantEffect pageChangeFeedbackEffect() const { return mPageChangeFeedbackEffect; }
    void setPageChangeFeedbackEffect(HbFeedback::InstantEffect effect) { mPageChangeFeedbackEffect = effect; }   
    HbFeedback::InstantEffect widgetPickFeedbackEffect() const { return mWidgetPickFeedbackEffect; }
    void setWidgetPickFeedbackEffect(HbFeedback::InstantEffect effect) { mWidgetPickFeedbackEffect = effect; }  
    HbFeedback::InstantEffect widgetDropFeedbackEffect() const { return mWidgetDropFeedbackEffect; }
    void setWidgetDropFeedbackEffect(HbFeedback::InstantEffect effect) { mWidgetDropFeedbackEffect = effect; }  
    HbFeedback::InstantEffect widgetRepositionFeedbackEffect() const { return mWidgetRepositionFeedbackEffect; }
    void setWidgetRepositionFeedbackEffect(HbFeedback::InstantEffect effect) { mWidgetRepositionFeedbackEffect = effect; } 
    HbFeedback::InstantEffect widgetOverTrashbinFeedbackEffect() const { return mWidgetOverTrashbinFeedbackEffect; }
    void setWidgetOverTrashbinFeedbackEffect(HbFeedback::InstantEffect effect) { mWidgetOverTrashbinFeedbackEffect = effect; } 
    HbFeedback::InstantEffect widgetDropToTrashbinFeedbackEffect() const { return mWidgetDropToTrashbinFeedbackEffect; }
    void setWidgetDropToTrashbinFeedbackEffect(HbFeedback::InstantEffect effect) { mWidgetDropToTrashbinFeedbackEffect = effect; } 
    HbFeedback::InstantEffect shortcutWidgetTapFeedbackEffect() const { return mShortcutWidgetTapFeedbackEffect; }
    void setShortcutWidgetTapFeedbackEffect(HbFeedback::InstantEffect effect) { mShortcutWidgetTapFeedbackEffect = effect; } 
    HbFeedback::InstantEffect widgetMoveBlockedFeedbackEffect() const { return mWidgetMoveBlockedFeedbackEffect; }
    void setWidgetMoveBlockedFeedbackEffect(HbFeedback::InstantEffect effect) { mWidgetMoveBlockedFeedbackEffect = effect; } 
    HbFeedback::InstantEffect clockWidgetTapFeedbackEffect() const { return mClockWidgetTapFeedbackEffect; }
    void setClockWidgetTapFeedbackEffect(HbFeedback::InstantEffect effect) { mClockWidgetTapFeedbackEffect = effect; } 
    HbFeedback::InstantEffect widgetSnappingFeedbackEffect() const { return mWidgetSnappingFeedbackEffect; }
    void setWidgetSnappingFeedbackEffect(HbFeedback::InstantEffect effect) { mWidgetSnappingFeedbackEffect = effect; }
    bool isSnapEnabled() const { return mIsSnapEnabled; }
    void setSnapEnabled(bool enabled) { mIsSnapEnabled = enabled; }
    int snapTimeout() const { return mSnapTimeout; }
    void setSnapTimeout(int timeout) { mSnapTimeout = timeout; }
    qreal snapForce() const { return mSnapForce; }
    void setSnapForce(qreal force) { mSnapForce = force; }  
    qreal snapGap() const { return mSnapGap; }
    void setSnapGap(qreal gap) { mSnapGap = gap; }  
    qreal snapBorderGap() const { return mSnapBorderGap; }
    void setSnapBorderGap(qreal gap) { mSnapBorderGap = gap; }
    int snapLineFadeInDuration() const { return mSnapLineFadeInDuration; }
    void setSnapLineFadeInDuration(int duration) { mSnapLineFadeInDuration = duration; }
    int snapLineFadeOutDuration() const { return mSnapLineFadeOutDuration; }
    void setSnapLineFadeOutDuration(int duration) { mSnapLineFadeOutDuration = duration; }
    bool isSnapEffectsEnabled() const { return mIsSnapEffectsEnabled; }
    void setSnapEffectsEnabled(bool enabled) { mIsSnapEffectsEnabled = enabled; }
    SceneType sceneType() const { return mSceneType; }
    void setSceneType(SceneType type) { mSceneType = type; }
     
    QSizeF minimumWidgetSizeInUnits() const;
    QSizeF maximumWidgetSizeInUnits() const;
    QSizeF minimumWidgetSizeInPixels() const;
    QSizeF maximumWidgetSizeInPixels() const;
    
    bool load();
    bool importFromFile(const QString &path);
    bool exportToFile(const QString &path);
        
    static HsConfiguration *instance();
    static HsConfiguration *takeInstance();
    static void setInstance(HsConfiguration *instance);

private:
    Q_DISABLE_COPY(HsConfiguration)
    
private:
    int mBounceEffect;
    int mWidgetTapAndHoldTimeout;
    int mSceneTapAndHoldTimeout;
    int mPageChangeAnimationDuration;
    int mPageChangeZoneAnimationDuration;
    int mPageChangeZoneReverseAnimationDuration;
    int mPageRemovedAnimationDuration;
    int mNewPageAddedAnimationDuration;
    int mWidgetDragEffectDuration;
    int mWidgetDropEffectDuration;
    int mDefaultPageId;
    int mMaximumPageCount;
    bool mIsShortcutLabelVisible;
    qreal mBounceFeedbackEffectDistance;
    qreal mPageChangePanDistanceInPixels;
    qreal mTapAndHoldDistance;
    qreal mPageChangeZoneWidth;
    qreal mPageIndicatorSpacing;
    qreal mMaximumWidgetHeight;
    qreal mMaximumWidgetWidth;
    qreal mMinimumWidgetHeight;
    qreal mMinimumWidgetWidth;
    qreal mPageChangePanDistance;
    HbFeedback::InstantEffect mPageChangeFeedbackEffect;
    HbFeedback::InstantEffect mWidgetPickFeedbackEffect;
    HbFeedback::InstantEffect mWidgetDropFeedbackEffect;
    HbFeedback::InstantEffect mWidgetRepositionFeedbackEffect;
    HbFeedback::InstantEffect mWidgetOverTrashbinFeedbackEffect;
    HbFeedback::InstantEffect mWidgetDropToTrashbinFeedbackEffect;
    HbFeedback::InstantEffect mShortcutWidgetTapFeedbackEffect;
    HbFeedback::InstantEffect mWidgetMoveBlockedFeedbackEffect;
    HbFeedback::InstantEffect mClockWidgetTapFeedbackEffect;
    HbFeedback::InstantEffect mWidgetSnappingFeedbackEffect;
    bool mIsSnapEnabled;
    int mSnapTimeout;
    qreal mSnapForce;
    qreal mSnapGap;
    qreal mSnapBorderGap;
    int mSnapLineFadeInDuration;
    int mSnapLineFadeOutDuration;
    bool mIsSnapEffectsEnabled;
    SceneType mSceneType;

    static HsConfiguration *mInstance;

    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSCONFIGURATION_H
