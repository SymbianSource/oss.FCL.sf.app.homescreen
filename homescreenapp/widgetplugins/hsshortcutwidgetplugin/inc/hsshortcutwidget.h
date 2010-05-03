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
* Description: Home screen shortcut widget.
*
*/

#ifndef HSSHORTCUTWIDGET_H
#define HSSHORTCUTWIDGET_H

#include <HbWidget>

#include "cadefs.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestShortcutWidget)

class HbFrameItem;    
class HbIconItem;
class HbTextItem;
class HbTouchArea;
class CaEntry;

class HsShortcutWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(int mcsId READ mcsId WRITE setMcsId)
    
public:
    HsShortcutWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~HsShortcutWidget();

    void setMcsId(int mcsId);
    int mcsId() const;

    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void finished();

public slots:
    void onInitialize();
    void onShow();
    void onHide();

    void onEntryChanged(const CaEntry &entry, ChangeType changeType);

private:
    Q_DISABLE_COPY(HsShortcutWidget)

    void handleMousePressEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void updatePrimitives();
    void createCaNotifier();
    void updateVisibility();

private:
    HbFrameItem *mBackground;
    HbIconItem *mIcon;
    HbTextItem *mText;
    HbTouchArea *mTouchArea;
    
    bool mIsPressed;

    int mMcsId;
    QSharedPointer<CaEntry> mCaEntry;

    HOMESCREEN_TEST_FRIEND_CLASS(TestShortcutWidget)
};

#endif // HSSHORTCUTWIDGET_H
