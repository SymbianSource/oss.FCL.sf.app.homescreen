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

#include <QGraphicsSceneMouseEvent>

#include <HbStyleLoader>
#include <HbFrameItem>
#include <HbFrameDrawer>
#include <HbIconItem>
#include <HbTextItem>
#include <HbTouchArea>

#include "hsshortcutwidget.h"
#include "hsshortcutservice.h"

#include "caservice.h"
#include "canotifier.h"

/*!
    \class HsShortcutWidget
    \ingroup group_hsshortcutwidgetplugin
    \brief Implementation for the homescreen shortcut widget.

    Shortcut can be defined to launch different applications or applications 
    with parameters, for example browser with certain url.
*/

/*!
    Constructor.
*/
HsShortcutWidget::HsShortcutWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
  : HbWidget(parent, flags),
    mBackground(0), mIcon(0), mText(0), mTouchArea(0),
    mIsPressed(false),
    mMcsId(-1), mCaEntry() 
{
    HbStyleLoader::registerFilePath(":/hsshortcutwidget.widgetml");
    HbStyleLoader::registerFilePath(":/hsshortcutwidget.css");
}

/*!
    Destructor.
*/
HsShortcutWidget::~HsShortcutWidget()
{
    HbStyleLoader::unregisterFilePath(":/hsshortcutwidget.widgetml");
    HbStyleLoader::unregisterFilePath(":/hsshortcutwidget.css");
}

/*!
    Sets the menu content service id.
*/
void HsShortcutWidget::setMcsId(int mcsId)
{
    mMcsId = mcsId;
}

/*!
    Returns the menu content service id.
*/
int HsShortcutWidget::mcsId() const
{
    return mMcsId;
}

/*!
    Filters touch area events.
*/
bool HsShortcutWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    switch (event->type()) {
        case QEvent::GraphicsSceneMousePress:
            handleMousePressEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
            break;

        case QEvent::GraphicsSceneMouseRelease:
            handleMouseReleaseEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
            break;

        default:            
            break;
    }

    return true;
}

/*!
    Initializes this widget.
*/
void HsShortcutWidget::onInitialize()
{
    mCaEntry = CaService::instance()->getEntry(mMcsId);
    if (!mCaEntry.isNull()) {
        createCaNotifier(); 
        updatePrimitives();
    } else {
        emit finished();
    }
}

/*!
    Wakes up this widget.
*/
void HsShortcutWidget::onShow()
{
    updateVisibility();
}

/*!
    Puts this widget in quiescent state.
*/
void HsShortcutWidget::onHide()
{
}

/*!
    Invoked when \a entry has changed with a \a changeType.
*/
void HsShortcutWidget::onEntryChanged(const CaEntry &entry, ChangeType changeType)
{
    Q_UNUSED(entry)

    mCaEntry = CaService::instance()->getEntry(mMcsId);

    switch(changeType) {
        case RemoveChangeType:
            emit finished();
            break;
        case UpdateChangeType:
            updateVisibility();
            updatePrimitives();
            break;
        default:
            break;
    }
}

/*!
    Mouse press handler.
*/
void HsShortcutWidget::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{   
    Q_UNUSED(event)

    mIsPressed = true;
    updatePrimitives();
}

/*!
    Mouse release handler.
*/
void HsShortcutWidget::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    mIsPressed = false;
    updatePrimitives();

    if (!contains(event->pos())) {
        return;
    }

    if (mCaEntry->role() == ItemEntryRole) {
        CaService::instance()->executeCommand(mMcsId);
    } else {
        HsShortcutService::instance()->executeCollectionAction(mMcsId, mCaEntry->entryTypeName());
    }
}

/*!
    \internal
*/
void HsShortcutWidget::updatePrimitives()
{   
    // Background
    if (!mBackground) {
        HbFrameDrawer *drawer = new HbFrameDrawer(
            QLatin1String("qtg_fr_hsshortcut_normal"), HbFrameDrawer::NinePieces);
        mBackground = new HbFrameItem(drawer, this);
        HbStyle::setItemName(mBackground, QLatin1String("background"));
    }
    if (mIsPressed) {        
        mBackground->frameDrawer().setFrameGraphicsName(QLatin1String("qtg_fr_hsitems_pressed"));
    } else {
        mBackground->frameDrawer().setFrameGraphicsName(QLatin1String("qtg_fr_hsshortcut_normal"));
    }

    // Icon
    if (!mIcon) {
        mIcon = new HbIconItem(this);
        HbStyle::setItemName(mIcon, QLatin1String("icon"));
    }
    mIcon->setIcon(mCaEntry->makeIcon());
    
    // Text
    QString text = mCaEntry->text();
    if (!text.isNull()) {
        if (!mText) {
            mText = new HbTextItem(this);
            HbStyle::setItemName(mText, QLatin1String("text"));
        }
        mText->setText(text);
    } else {
        if (mText) {
            delete mText;
            mText = 0;
        }
    }

    // Touch Area
    if (!mTouchArea) {
        mTouchArea = new HbTouchArea(this);
        mTouchArea->installEventFilter(this);
        HbStyle::setItemName(mTouchArea, QLatin1String("toucharea"));
    }
}

/*!
    \internal
*/
void HsShortcutWidget::createCaNotifier()
{
    CaNotifierFilter filter;
    filter.setIds(QList<int>() << mMcsId);
    
    CaNotifier *notifier = CaService::instance()->createNotifier(filter);
    notifier->setParent(this);

    connect(notifier,
        SIGNAL(entryChanged(CaEntry,ChangeType)),
        SLOT(onEntryChanged(CaEntry,ChangeType)), 
        Qt::QueuedConnection);
}

/*!
    \internal
*/
void HsShortcutWidget::updateVisibility()
{
    setVisible(!mCaEntry->flags().testFlag(MissingEntryFlag));
}
