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

#include <QGraphicsLinearLayout>

#include <HbStackedLayout>
#include <HbIconItem>
#include <HbTextItem>

#include "hsshortcutwidget.h"
#include "hsshortcutservice.h"

#include "caservice.h"
#include "canotifier.h"

/*!
    \class HsShortcutWidget
    \ingroup group_hsshortcutwidgetprovider
    \brief Implementation for the homescreen shortcut widget.

    Shortcut can be defined to launch different applications or applications 
    with parameters, for example browser with certain url.

     \section how_to_use_shortcut_plugin How to use HsShortcutWidget

    At the moment code is compiled with homescreen core implementation.

*/

/*!

    Constructs shortcut widget object
*/
HsShortcutWidget::HsShortcutWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
  : HbWidget(parent, flags),
    mShortcutBackgroundItem(0),
    mShortcutIconItem(0),
    mShortcutTextItem(0),
    mMcsId(-1)    
{
    setPreferredSize(QSizeF(82,82));
    resize(82,82);    
}

/*!
    \fn HsShortcutWidget::~HsShortcutWidget()

    Destructor
*/
HsShortcutWidget::~HsShortcutWidget()
{
}

/*!
    Getter for menu content service id
*/
int HsShortcutWidget::mcsId() const
{
    return mMcsId;
}

/*!
    Sets menu content service id to \a mcsId
*/
void HsShortcutWidget::setMcsId(int mcsId)
{
    mMcsId = mcsId;
}

/*!
    \fn void HsShortcutWidget::onEntryChanged(const CaEntry &entry, ChangeType changeType)
    
    Invoked when \a entry has changed with a \a changeType event.
*/
void HsShortcutWidget::onEntryChanged(const CaEntry &entry, ChangeType changeType)
{

    switch(changeType) {
        case RemoveChangeType: {
            emit finished();
            break;
        }
        case UpdateChangeType: {
            hideOrShowWidget(entry.flags());
            mShortcutIconItem->setIcon(fetchIcon(mMcsId));

            QString text = fetchText(mMcsId);
            mShortcutTextItem->setText(text);            
            break;
        }
        default:
            break;
    }
}

/*!
    \fn void HsShortcutWidget::onInitialize()

    Initializes shortcut
*/
void HsShortcutWidget::onInitialize()
{
    constructUI();

    if (!HsShortcutService::instance() || mMcsId < 0) {
        emit finished();
    }

    createCaNotifier(mMcsId);
    
    mShortcutIconItem->setIcon(fetchIcon(mMcsId));

    QString text = fetchText(mMcsId);
    mShortcutTextItem->setText(text);

    setEnabled(true);
}

/*!
    \fn void HsClockWidget::show()

    Shows the widget
*/
void HsShortcutWidget::onShow()
{
    hideOrShowWidget(fetchEntryFlags(mMcsId));
}

/*!
    \fn void HsClockWidget::show()

    Hides the widget
*/
void HsShortcutWidget::onHide()
{
    hide();
}

/*!
    \fn void HsShortcutWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)

    Executes configured action
*/
void HsShortcutWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    
    CaEntry *entry = CaService::instance()->getEntry(mMcsId);
    if (!entry) {
        return;
    }

    if (entry->role() == ItemEntryRole) {
        CaService::instance()->executeCommand(mMcsId);
    } else {
        executeCollectionAction(mMcsId, entry->entryTypeName());
    }
}

/*!
    \fn void HsShortcutWidget::constructUI()

    Constructs and initializes ui parts
*/
void HsShortcutWidget::constructUI()
{
    mShortcutBackgroundItem = new HbIconItem("hs_shortcut_bg");
    mShortcutBackgroundItem->setAlignment(Qt::AlignCenter);
    mShortcutIconItem = new HbIconItem;
    mShortcutIconItem->setAlignment(Qt::AlignCenter);
    mShortcutTextItem = new HbTextItem;
    mShortcutTextItem->setAlignment(Qt::AlignCenter);

    HbStackedLayout *mainLayout = new HbStackedLayout;
    mainLayout->addItem(mShortcutBackgroundItem);
    
    QGraphicsLinearLayout *contentLayout = 
        new QGraphicsLinearLayout(Qt::Vertical);
    contentLayout->setContentsMargins(4, 4, 4, 4);
    contentLayout->addItem(mShortcutIconItem);
    contentLayout->addItem(mShortcutTextItem);
    mainLayout->addItem(contentLayout);

    setLayout(mainLayout);
}

/*!
    \internal
*/
HbIcon HsShortcutWidget::fetchIcon(int aShortcutId)
{
	CaEntry *entry = CaService::instance()->getEntry(aShortcutId);
    if (!entry) {
        return HbIcon();
    }
    return entry->makeIcon();    
}

/*!
    \internal
*/
QString HsShortcutWidget::fetchText(int aShortcutId)
{
	CaEntry *entry = CaService::instance()->getEntry(aShortcutId);
    if (!entry) {
        return QString();
    }
    return entry->text();
}

/*!
    \internal
*/
void HsShortcutWidget::executeCollectionAction(
        int shortcutId, const QString& collectionType)
{
    HsShortcutService::instance()->executeCollectionAction(
            shortcutId, collectionType);
}

/*!
    \internal
*/
void HsShortcutWidget::createCaNotifier(int aShortcutId)
{
    CaNotifierFilter filter;
    filter.setIds(QList<int>() << aShortcutId);
    
    CaNotifier *itemNotifier = CaService::instance()->createNotifier(filter);
    itemNotifier->setParent(this);

    connect(itemNotifier,
        SIGNAL(entryChanged(const CaEntry&, ChangeType)),
        SLOT(onEntryChanged(const CaEntry&, ChangeType)),Qt::QueuedConnection);
}

/*!
    \internal
*/
void HsShortcutWidget::hideOrShowWidget(EntryFlags aEntryFlags)
{
    if (aEntryFlags.testFlag(MissingEntryFlag)) {
        hide();
    } else {
        show();
    }
}

/*!
    \internal
*/
EntryFlags HsShortcutWidget::fetchEntryFlags(int aShortcutId)
{
    CaEntry *entry = CaService::instance()->getEntry(aShortcutId);
    EntryFlags entryFlags = 0;
    if (entry) {
        entryFlags = entry->flags();
    }
    return entryFlags;
}
