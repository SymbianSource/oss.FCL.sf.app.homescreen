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
* Description:  Shortcut widget
*
*/

#ifndef HSSHORTCUTWIDGET_H
#define HSSHORTCUTWIDGET_H

#include <QVariant>
#include <QMap>

#include <HbWidget>

#include "hstest_global.h"
#include "cadefs.h"

HOMESCREEN_TEST_CLASS(TestShortcutWidget)

class HbIconItem;
class HbTextItem;
class CaEntry;

class HsShortcutWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(int mcsId READ mcsId WRITE setMcsId)
    
public:
    HsShortcutWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~HsShortcutWidget();

    int mcsId() const;
    void setMcsId(int mcsId);

signals:
    void finished();

public slots:
    void onEntryChanged(const CaEntry &entry, ChangeType changeType);

    void onInitialize();
    void onShow();
    void onHide();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) { Q_UNUSED(event) }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void constructUI();

    HbIcon fetchIcon(int aShortcutId);
    QString fetchText(int aShortcutId);

    void executeCollectionAction(int shortcutId, const QString& collectionType);
    void createCaNotifier(int aShortcutId);
    
    void hideOrShowWidget(EntryFlags aEntryFlags);
    EntryFlags fetchEntryFlags(int aShortcutId);

private:    
    HbIconItem *mShortcutBackgroundItem;
    HbIconItem *mShortcutIconItem;
    HbTextItem *mShortcutTextItem;

    int mMcsId;

    HOMESCREEN_TEST_FRIEND_CLASS(TestShortcutWidget)
};

#endif // HSSHORTCUTWIDGET_H
