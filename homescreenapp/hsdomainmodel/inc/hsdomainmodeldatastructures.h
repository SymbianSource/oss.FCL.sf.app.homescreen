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

class HSDOMAINMODEL_EXPORT HsSceneData
{
public:
    HsSceneData() 
      : id(-1), defaultPageId(-1), maximumPageCount(-1),
        maximumWidgetHeight(-1.0), maximumWidgetWidth(-1.0),
        minimumWidgetHeight(-1.0), minimumWidgetWidth(-1.0)
    {}
    
    int     id;
    QString portraitWallpaper;
    QString landscapeWallpaper;
    int     defaultPageId;
    int     maximumPageCount;
    qreal   maximumWidgetHeight;
    qreal   maximumWidgetWidth;
    qreal   minimumWidgetHeight;
    qreal   minimumWidgetWidth;
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
        : x(0), y(0), zValue(0), 
          widgetId(-1)
    {}

    void setPos(const QPointF &pos)
    {
        x = pos.x();
        y = pos.y();
    }

    QString key;
    qreal   x;
    qreal   y;
    qreal   zValue;
    int     widgetId;
};

class HSDOMAINMODEL_EXPORT HsGeneralConfiguration
{	
public:
	int bounceEffect; // pixels, bounce effect reservation, half in left side and half in right side of wallpaper 		
	int tapAndHoldDistance; // pixels
    int widgetTapAndHoldTimeout; // ms, duration of long press before action is triggered on widget
	int sceneTapAndHoldTimeout; // ms, duration of long press before action is triggered on background
	int pageChangeZoneWidth; // pixels
	int pageIndicatorSpacing; // pixels
    int pageChangeAnimationDuration; // ms, this is how long page change animation takes
	int pageChangeZoneAnimationDuration; // ms, this is how long crawling is done before page is actually changed
    int pageChangeZoneReverseAnimationDuration; // ms, animation duration back to starting position when crawling ends before page is changed
	int pageRemovedAnimationDuration; // ms
	int newPageAddedAnimationDuration; // ms
	int widgetDragEffectDuration; // ms
	int widgetDropEffectDuration; // ms
	int boundaryFeedbackEffectDistance; // pixels, when widget is moved within n pixels from first page right border or 
                                        // n pixels from last page's left border, feedback effect is played
};

class HSDOMAINMODEL_EXPORT HsFeedbackConfiguration
{
public: 
    HbFeedback::InstantEffect feedbackFromString(QString feedback)
    {
        if (feedback == "Basic") {
            return HbFeedback::Basic;
        }
        else if (feedback == "Sensitive") {
            return HbFeedback::Sensitive;
        }
        else if (feedback == "BasicButton") {
            return HbFeedback::BasicButton;
        }
        else if (feedback == "SensitiveButton") {
            return HbFeedback::SensitiveButton;
        }
        else if (feedback == "BasicKeypad") {
            return HbFeedback::BasicKeypad;
        }
        else if (feedback == "SensitiveKeypad") {
            return HbFeedback::SensitiveKeypad;
        }
        else if (feedback == "BasicSlider") {
            return HbFeedback::BasicSlider;
        }
        else if (feedback == "SensitiveSlider") {
            return HbFeedback::SensitiveSlider;
        }
        else if (feedback == "BasicKeypad") {
            return HbFeedback::BasicKeypad;
        }
        else if (feedback == "BasicSlider") {
            return HbFeedback::BasicSlider;
        }
        else if (feedback == "SensitiveSlider") {
            return HbFeedback::SensitiveSlider;
        }
        else if (feedback == "BasicItem") {
            return HbFeedback::BasicItem;
        }
        else if (feedback == "SensitiveItem") {
            return HbFeedback::SensitiveItem;
        }
        else if (feedback == " ItemScroll") {
            return HbFeedback::ItemScroll;
        }
        else if (feedback == "ItemPick") {
            return HbFeedback::ItemPick;
        }
        else if (feedback == "ItemDrop") {
            return HbFeedback::ItemDrop;
        }
        else if (feedback == "ItemMoveOver") {
            return HbFeedback::ItemMoveOver;
        }
        else if (feedback == "BounceEffect") {
            return HbFeedback::BounceEffect;
        }
        else if (feedback == "Checkbox") {
            return HbFeedback::Checkbox;
        }
        else if (feedback == "MultipleCheckbox") {
            return HbFeedback::MultipleCheckbox;
        }
        else if (feedback == "Editor") {
            return HbFeedback::Editor;
        }
        else if (feedback == "TextSelection") {
            return HbFeedback::TextSelection;
        }
        else if (feedback == "BlankSelection") {
            return HbFeedback::BlankSelection;
        }
        else if (feedback == "LineSelection") {
            return HbFeedback::LineSelection;
        }
        else if (feedback == "EmptyLineSelection") {
            return HbFeedback::EmptyLineSelection;
        }
        else if (feedback == "PopUp") {
            return HbFeedback::PopUp;
        }
        else if (feedback == "PopupOpen") {
            return HbFeedback::PopupOpen;
        }
        else if (feedback == "PopupClose") {
            return HbFeedback::PopupClose;
        }
        else if (feedback == "Flick") {
            return HbFeedback::Flick;
        }
        else if (feedback == "StopFlick") {
            return HbFeedback::StopFlick;
        }
        else if (feedback == "MultitouchActivate") {
            return HbFeedback::MultitouchActivate;
        }
        else if (feedback == "RotateStep") {
            return HbFeedback::RotateStep;
        }
        else if (feedback == "PositiveTacticon") {
            return HbFeedback::PositiveTacticon;
        }
        else if (feedback == "NeutralTacticon") {
            return HbFeedback::NeutralTacticon;
        }
        else if (feedback == "NegativeTacticon") {
            return HbFeedback::NegativeTacticon;
        }
        else if (feedback == "NumberOfInstantFeedbacks") {
            return HbFeedback::NumberOfInstantFeedbacks;
        }
        else if (feedback == "InstantUser") {
            return HbFeedback::InstantUser;
        }
        else if (feedback == "InstantMaxUser") {
            return HbFeedback::InstantMaxUser;
        }
        return HbFeedback::None;
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
