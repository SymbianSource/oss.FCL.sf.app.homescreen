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

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <QDir>

#include <HbMainWindow>
#include <HbVkbHost>

#include "hsidlewidget.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswallpaper.h"
#include "hstrashbinwidget.h"
#include "hspageindicator.h"
#include "hsdocumentloader.h"
#include "hshomescreenstatecommon.h"

namespace
{
    const char gControlLayerDocmlName[] = "controllayer.docml";
    const char gControlLayerName[]      = "controlLayer";
    const char gTrashBinName[]          = "trashBin";
    const char gPageIndicatorName[]     = "pageIndicator";
}

/*!
    \class HsIdleWidget
    \ingroup group_hshomescreenstateplugin
    \brief View part of the home screen idle state.

    Maintains the idle view ui layers and takes care of 
    receiving user input and communicating it to the idle 
    state for further processing.
*/

/*!
    Constructs a new idle widget with the given \a parent.
*/
HsIdleWidget::HsIdleWidget(QGraphicsItem *parent)
  : HbWidget(parent),
    mControlLayer(0), mPageLayer(0), mSceneLayer(0),
    mDelayedPressEvent(0),
    mTrashBin(0), mPageIndicator(0)
{
    setFlag(ItemHasNoContents);
    
    loadControlLayer();

    QGraphicsLinearLayout *linearLayout = 0;

    linearLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    linearLayout->setContentsMargins(0, 0, 0, 0);
    linearLayout->setSpacing(0);
    mPageLayer = new HbWidget(this);
    mPageLayer->setLayout(linearLayout);
    mPageLayer->setZValue(1);
    
    linearLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    linearLayout->setContentsMargins(0, 0, 0, 0);
    linearLayout->setSpacing(0);
    mSceneLayer = new HbWidget(this);
    mSceneLayer->setLayout(linearLayout);
    mSceneLayer->setZValue(0);
}

/*!
    Destroys this idle widget.
*/
HsIdleWidget::~HsIdleWidget()
{
    clearDelayedPress();

    QList<HsPage *> pages = HsScene::instance()->pages();
    foreach (HsPage *page, pages) {
        page->setParentItem(0);
        if (page->scene()) {
            page->scene()->removeItem(page);
        }
    }

    HsWallpaper *wallpaper = HsScene::instance()->wallpaper();
    wallpaper->setParentItem(0);
    if (wallpaper->scene()) {
        wallpaper->scene()->removeItem(wallpaper);
    }
}

/*!
    Layouts the ui layers according to the given \a rect.
*/
void HsIdleWidget::setGeometry(const QRectF &rect)
{
    int n = HsScene::instance()->pages().count();
    mControlLayer->resize(rect.size());
    mPageLayer->resize(n * rect.width(), rect.height());
    mSceneLayer->resize(2 * rect.width() + HSBOUNDARYEFFECT, rect.height());
    HbWidget::setGeometry(rect);
}

/*!
    Stores the given mouse press \a event.
*/
void HsIdleWidget::captureDelayedPress(QGraphicsSceneMouseEvent *event)
{
    if (event) {
        mDelayedPressEvent = new QMouseEvent(QEvent::MouseButtonPress,
            event->scenePos().toPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        mDelayedPressEvent->setAccepted(false);
    }
}

/*!
    Sends the stored mouse press event.
*/
void HsIdleWidget::sendDelayedPress()
{
    if (mDelayedPressEvent) {
        QApplication::sendEvent(HsScene::mainWindow()->viewport(), mDelayedPressEvent);
        clearDelayedPress();     
    }
}

/*!
    Deletes the stored mouse press event.
*/
void HsIdleWidget::clearDelayedPress()
{    
    if (mDelayedPressEvent) {
        delete mDelayedPressEvent;
        mDelayedPressEvent = 0;
    }
}

/*!
    Sets the active page \a index to the page 
    indicator.
*/
void HsIdleWidget::setActivePage(int index)
{
    mPageIndicator->setActiveItemIndex(index);
}

/*!
    Inserts the given \a page at index position
    \a index in the page layer.
*/
void HsIdleWidget::insertPage(int index, HsPage *page)
{
    QGraphicsLinearLayout *layout = 
        static_cast<QGraphicsLinearLayout *>(mPageLayer->layout());
    layout->insertItem(index, page);
    mPageLayer->resize(
        layout->count() * size().width(), size().height());
}

/*!
    Removes the page at index position
    \a index in the page layer.
*/
void HsIdleWidget::removePage(int index)
{
    QGraphicsLinearLayout *layout = 
        static_cast<QGraphicsLinearLayout *>(mPageLayer->layout());
    layout->removeAt(index);
    mPageLayer->resize(
        layout->count() * size().width(), size().height());
}

/*!
    \fn HsIdleWidget::controlLayer() const

    Returns the control layer.
*/

/*!
    \fn HsIdleWidget::pageLayer() const

    Returns the page layer.
*/

/*!
    \fn HsIdleWidget::sceneLayer() const

    Returns the scene layer.
*/

/*!
    \fn HsIdleWidget::trashBin() const

    Returns the trashbin widget.
*/

/*!
    \fn HsIdleWidget::pageIndicator() const

    Returns the page indicator widget.
*/

/*!
    \fn HsIdleWidget::mousePressed(QGraphicsItem *, QGraphicsSceneMouseEvent *, bool &)

    The idle state connects to this signal for handling mouse
    press events. It filters events for the item \a watched. 
    \a event is the filtered event. Sets the \a filtered true 
    if the event was filtered by this handler.
*/

/*!
    \fn HsIdleWidget::mouseMoved(QGraphicsItem *, QGraphicsSceneMouseEvent *, bool &)

    The idle state connects to this signal for handling mouse
    move events. It filters events for the item \a watched. 
    \a event is the filtered event. Sets the \a filtered true 
    if the event was filtered by this handler.
*/

/*!
    \fn HsIdleWidget::mouseReleased(QGraphicsItem *, QGraphicsSceneMouseEvent *, bool &)

    The idle state connects to this signal for handling mouse
    release events. It filters events for the item \a watched. 
    \a event is the filtered event. Sets the \a filtered true 
    if the event was filtered by this handler.
*/

/*!
    Sets the trashbin visible and hides the page indicator.
*/
void HsIdleWidget::showTrashBin()
{
    mPageIndicator->hide();
    mTrashBin->show();
}
 
/*!
    Sets the page indicator visible and hides the trashbin.
*/
void HsIdleWidget::showPageIndicator()
{
    mTrashBin->hide();
    mTrashBin->deactivate();
    mPageIndicator->setVisible(1 < mPageIndicator->itemCount());
}

/*!
    Filters the main window's graphics scene (\a object) \a event.
*/
bool HsIdleWidget::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)

    if (HbVkbHost::activeVkbHost()) {
        setFiltersChildEvents(false);
        return false;
    }

    bool filtered = false;

    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:
        if (mDelayedPressEvent &&
            scene()->mouseGrabberItem()) {
            scene()->mouseGrabberItem()->ungrabMouse();
        }
        setFiltersChildEvents(!mDelayedPressEvent);
        break;
    case QEvent::GraphicsSceneMouseMove:
        emit mouseMoved(0, static_cast<QGraphicsSceneMouseEvent *>(event), filtered);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        setItemsFocusable();
        emit mouseReleased(0, static_cast<QGraphicsSceneMouseEvent *>(event), filtered);
        if (filtered && scene()->mouseGrabberItem()) {
            scene()->mouseGrabberItem()->ungrabMouse();
        }        
        break;
    default:
        break;
    }

    return filtered;
}

/*!
    Filters events for the item \a watched. \a event is the filtered event.
*/
bool HsIdleWidget::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    bool filtered = false;

    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:
        emit mousePressed(watched, static_cast<QGraphicsSceneMouseEvent *>(event), filtered);
        if (filtered) {
            setItemsUnfocusable(static_cast<QGraphicsSceneMouseEvent *>(event));
        }
        break;    
    default:
        break;
    }

    return filtered;
}

/*!
    Reimplements QGraphicsWidget::polishEvent().
*/
void HsIdleWidget::polishEvent()
{
    HsScene *scene = HsScene::instance();
    Q_ASSERT(scene);

    QGraphicsLinearLayout *layout = 
        static_cast<QGraphicsLinearLayout *>(mPageLayer->layout());
    QList<HsPage *> pages = scene->pages();
    foreach (HsPage *page, pages) {
        layout->addItem(page);
    }

    layout = static_cast<QGraphicsLinearLayout *>(mSceneLayer->layout());
    HsWallpaper *wallpaper = HsScene::instance()->wallpaper();
    layout->addItem(wallpaper);
           
    mPageIndicator->initialize(pages.count(), scene->activePageIndex());    
    showPageIndicator();

    HsScene::mainWindow()->scene()->installEventFilter(this);
}

/*!
    Loads the control layer declared in a docml file.
*/
void HsIdleWidget::loadControlLayer()
{
    HsDocumentLoader loader;
    bool loaded = false;

#ifndef Q_OS_SYMBIAN
    QString path = QDir::currentPath();
#else
    QString path = "c:";
#endif

    QString file = path + "/hsresources/" + gControlLayerDocmlName;
    QString fallbackPath = QString(":/") + gControlLayerDocmlName;

    if (QFile::exists(file)) {
        loader.load(file, &loaded);
        if (!loaded) {
            loader.load(fallbackPath, &loaded);
        }
    } else {
        loader.load(fallbackPath, &loaded);       
    }

    if (loaded) {
        mControlLayer = qobject_cast<HbWidget *>(loader.findWidget(gControlLayerName));
        mControlLayer->setZValue(2);
        mControlLayer->setParentItem(this);

        mTrashBin = qobject_cast<HsTrashBinWidget *>(loader.findWidget(gTrashBinName));
        mTrashBin->setZValue(1e6);

        mPageIndicator = qobject_cast<HsPageIndicator *>(loader.findWidget(gPageIndicatorName));
        mPageIndicator->setZValue(1e6);
    } else {
        // TODO: Handle error.
    }
}

/*!
    Sets the items under the given mouse \a event scene position 
    unfocusable and stores the items.
*/
void HsIdleWidget::setItemsUnfocusable(QGraphicsSceneMouseEvent *event)
{
    mFocusableItems.clear();

    if (!scene()) {
        return;
    }

    QList<QGraphicsItem *> items = scene()->items(event->scenePos());
    if (items.isEmpty()) {
        return;
    }

    int i = 0;
    for (; i < items.count(); ++i) {
        if (mPageLayer->isAncestorOf(items[i])) {
            ++i;
            break;
        }
    }
 
    HsPage *page = HsScene::instance()->activePage();
    QList<HsWidgetHost *> widgets = page->widgets();
    
    for (; i < items.count(); ++i) {
        QGraphicsItem *item = items.at(i);
        if (page->isAncestorOf(item)) {
            foreach (HsWidgetHost *widget, widgets) {
                if ((item == widget || widget->isAncestorOf(item))&& item->isEnabled() && (item->flags() & QGraphicsItem::ItemIsFocusable)) {
                    if (!item->isWidget() || static_cast<QGraphicsWidget *>(item)->focusPolicy() & Qt::ClickFocus) {
                        item->setFlag(QGraphicsItem::ItemIsFocusable, false);
                        mFocusableItems.append(item);
                    }
                }
            }
        }
    }
}
 
/*!
    Sets the stored items focusable.
*/
void HsIdleWidget::setItemsFocusable()
{
    foreach (QGraphicsItem *item, mFocusableItems) {
        item->setFlag(QGraphicsItem::ItemIsFocusable);
    }
    mFocusableItems.clear();
}
