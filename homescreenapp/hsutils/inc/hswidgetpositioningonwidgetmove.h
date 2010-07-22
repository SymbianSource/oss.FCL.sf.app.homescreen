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

#ifndef HSWIDGETPOSITIONINGONWIDGETMOVE_H
#define HSWIDGETPOSITIONINGONWIDGETMOVE_H

#include <QVariantHash>
#include <QList>
#include <QRectF>
#include <QPointF>
#include <QLineF>

#include "hsutils_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(t_hsUtils)

class HSUTILS_EXPORT HsWidgetPositioningOnWidgetMove
{
public:
    class Result
    {
    public:
        Result() : hasHorizontalSnap(false), hasVerticalSnap(false),
                   horizontalSnapPosition(0.0), verticalSnapPosition(0.0)
        { }
        bool isNull() const 
        { return !hasHorizontalSnap && !hasVerticalSnap; }

        bool hasHorizontalSnap;
        bool hasVerticalSnap;
        qreal horizontalSnapPosition;
        qreal verticalSnapPosition;
        QLineF verticalSnapLine;
        QLineF horizontalSnapLine;
    };

    virtual ~HsWidgetPositioningOnWidgetMove() {}
    
    virtual void setConfiguration(const QVariantHash &configuration) = 0;

    virtual void setPagePresentation(const QRectF &containerRect, 
                                     const QList<QRectF> &inactiveRects,
                                     const QRectF &activeRect) = 0;

    virtual Result run(const QRectF &movingRect) = 0;

    static void setInstance(HsWidgetPositioningOnWidgetMove *instance);
    static HsWidgetPositioningOnWidgetMove *instance();

private:
    static HsWidgetPositioningOnWidgetMove *mInstance;
};

class HsSnapRectangle
{
public:
    HsSnapRectangle() : rectangle(QRectF()) 
    { }
    HsSnapRectangle(QRectF rect) : rectangle(rect), isLeftSnapableForAbove(true), isLeftSnapableForBelow(true),
                                   isRightSnapableForAbove(true), isRightSnapableForBelow(true),
                                   isTopSnapableForLeft(true), isTopSnapableForRight(true),
                                   isBottomSnapableForLeft(true), isBottomSnapableForRight(true)
    { }

    QRectF rectangle;
    bool isLeftSnapableForAbove;
    bool isLeftSnapableForBelow;
    bool isRightSnapableForAbove;
    bool isRightSnapableForBelow;
    bool isTopSnapableForLeft;
    bool isTopSnapableForRight;
    bool isBottomSnapableForLeft;
    bool isBottomSnapableForRight;
};

class HSUTILS_EXPORT HsSnapToLines : public HsWidgetPositioningOnWidgetMove
{
public:
    HsSnapToLines();

    void setConfiguration(const QVariantHash &configuration);

    void setPagePresentation(const QRectF &containerRect, 
                             const QList<QRectF> &inactiveRects,
                             const QRectF &activeRect);

    Result run(const QRectF &movingRect);
private:

    void createSnappableRectangles(const QList<QRectF> &inactiveRects);
    void checkForCenterSnapping();
    void checkInactiveRectLieAboveOrBelowOfMovingRect();
    void compareLeftSideOfMovingRectForSnapping();
    void compareRightSideOfMovingRectForSnapping();
    void checkInactiveRectLieLeftOrRightOfMovingRect();
    void compareTopOfMovingRectForSnapping();
    void compareBottomOfMovingRectForSnapping();
    void extendVerticalLine();
    void extendHorizontalLine();
    void checkInactiveRectPositionToVerticalLine();
    void extendVerticalLineToIncludeInactiveRect();
    void checkInactiveRectPositionToHorizontalLine();
    void extendHorizontalLineToIncludeInactiveRect();

private: //data
    QRectF mContainerRect;

    qreal mActiveRectWidth;
    qreal mActiveRectHeight;

    QList<HsSnapRectangle> mInactiveSnapRects;

    QRectF mInactiveRectToCompare;
    HsSnapRectangle mInactiveSnapRectToCompare;
    QRectF mMovingRect;
    qreal mHorizontalSnapPosition;
    qreal mVerticalSnapPosition;

    bool mHorizontalSnapFound;
    QLineF mVerticalLine;

    bool mVerticalSnapFound;
    QLineF mHorizontalLine;

    bool mRectLieAbove;
    bool mLeftInRange;
    bool mRightInRange;
    qreal mMinVerticalEdgesDistance;
    qreal mVerticalDistance;
    qreal mVerticalDistanceFromSelectedRect;
    qreal mContainerVerticalEdgeDistance;

    bool mRectLieLeft;
    bool mTopInRange;
    bool mBottomInRange;
    qreal mMinHorizontalEdgesDistance;
    qreal mHorizontalDistance;
    qreal mHorizontalDistanceFromSelectedRect;
    qreal mContainerHorizontalEdgeDistance;

    bool mSnapEnabled;
    qreal mSnapForce;
    qreal mSnapGap;
    qreal mSnapBorderGap;

    bool mRectVerticalEdgeLiesInLineWithVerticalLine;
    bool mRectLieAboveVerticalLine;
    bool mRectLieBelowVerticalLine;

    bool mRectHorizontalEdgeLiesInLineWithHorizontalLine;
    bool mRectLieLeftOfHorizontalLine;
    bool mRectLiesRightOfHorizontalLine;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // HSWIDGETPOSITIONINGONWIDGETMOVE_H
