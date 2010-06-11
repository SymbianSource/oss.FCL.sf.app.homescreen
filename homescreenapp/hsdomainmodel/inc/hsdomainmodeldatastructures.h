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

#ifndef HSDOMAINMODELDATASTRUCTURES_H
#define HSDOMAINMODELDATASTRUCTURES_H

#include <QRectF>
#include "hsdomainmodel_global.h"
#include <HbFeedback>
#include <QMetaEnum>

class HSDOMAINMODEL_EXPORT HsSceneData
{
public:
    HsSceneData() 
      : id(-1)
    {}
    
    int     id;
    QString portraitWallpaper;
    QString landscapeWallpaper;

};

class HSDOMAINMODEL_EXPORT HsPageData
{
public:
    HsPageData() 
      : id(-1), indexPosition(-1)
    {}
    
    int id;    
    int indexPosition;
};

class HSDOMAINMODEL_EXPORT HsWidgetData
{
public:
    HsWidgetData()
      : id(-1), 
        pageId(-1)
    {}

    int     id;
    QString uri;
    int     pageId;
};

class HSDOMAINMODEL_EXPORT HsWidgetPresentationData
{
public:
    HsWidgetPresentationData()
        : orientation(Qt::Vertical), x(0), y(0), zValue(0), 
          widgetId(-1)
    {}

    void setPos(const QPointF &pos)
    {
        x = pos.x();
        y = pos.y();
    }

    QPointF pos() const
    {
        return QPointF(x, y);
    }

    Qt::Orientation orientation;
    qreal   x;
    qreal   y;
    qreal   zValue;
    int     widgetId;
};

class HSDOMAINMODEL_EXPORT HsGeneralConfiguration
{	
public:
	int bounceEffect; // pixels, bounce effect reservation, half in left side and half in right side of wallpaper 		
	qreal tapAndHoldDistance; // pixels
    int widgetTapAndHoldTimeout; // ms, duration of long press before action is triggered on widget
	int sceneTapAndHoldTimeout; // ms, duration of long press before action is triggered on background
	qreal pageChangeZoneWidth; // pixels
	qreal pageIndicatorSpacing; // pixels
    int pageChangeAnimationDuration; // ms, this is how long page change animation takes
	int pageChangeZoneAnimationDuration; // ms, this is how long crawling is done before page is actually changed
    int pageChangeZoneReverseAnimationDuration; // ms, animation duration back to starting position when crawling ends before page is changed
	int pageRemovedAnimationDuration; // ms
	int newPageAddedAnimationDuration; // ms
	int widgetDragEffectDuration; // ms
	int widgetDropEffectDuration; // ms
	int boundaryFeedbackEffectDistance; // pixels, when widget is moved within n pixels from first page right border or 
                                        // n pixels from last page's left border, feedback effect is played
    int defaultPageId; // id of default page (which cannot be removed)
    int maximumPageCount; // maximum number of pages
    qreal maximumWidgetHeight; // px
    qreal maximumWidgetWidth; // px
    qreal minimumWidgetHeight; // px
    qreal minimumWidgetWidth; // px
    bool shortcutLabelsVisible; // display labels under shortcut widgets
    qreal pageChangePanDistance; // un, amount of pan needed to change page
};

class HSDOMAINMODEL_EXPORT HsFeedbackConfiguration
{
public: 
    HbFeedback::InstantEffect feedbackFromString(const QString &feedback)
    {
    int index = HbFeedback::staticMetaObject.indexOfEnumerator("InstantEffect");
    QMetaEnum metaEnum = HbFeedback::staticMetaObject.enumerator(index);

    int intFeedback = metaEnum.keysToValue(feedback.toAscii());

    if (intFeedback < HbFeedback::None || intFeedback > HbFeedback::InstantMaxUser) {
        return HbFeedback::None;
    } 

    return static_cast<HbFeedback::InstantEffect>(intFeedback);
    }

public:
	HbFeedback::InstantEffect pageChangeFeedbackType;
	HbFeedback::InstantEffect widgetPickFeedbackType;
	HbFeedback::InstantEffect widgetDropFeedbackType;
	HbFeedback::InstantEffect widgetRepositionFeedbackType;
	HbFeedback::InstantEffect widgetOverTrashbinFeedbackType;
	HbFeedback::InstantEffect widgetDropToTrashbinFeedbackType;
	HbFeedback::InstantEffect shortcutWidgetTapFeedbackType;
	HbFeedback::InstantEffect widgetMoveBlockedFeedbackType;
	HbFeedback::InstantEffect clockWidgetTapFeedbackType;
	HbFeedback::InstantEffect widgetSnappingFeedbackType;
};

class HSDOMAINMODEL_EXPORT HsSnapConfiguration
{	
public:
	bool snappingEnabled; // variable to tell if snapping is enabled or not
	qreal snapForce; // this is the distance at which the widget starts being attracted to the line
	qreal snapGap; // the gap at which the objects settle into from each other when they snap
	qreal borderGap; // (the gap from the border edges when the widget settles on the edges of the page
	int timeout; // time in milliseconds after which the snapping line will be shown
};

#endif // HSDOMAINMODELDATASTRUCTURES_H
