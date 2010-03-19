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
* Description: Trash bin widget.
*
*/

#include <QGraphicsLinearLayout>
#include <HbIconItem>

#include "hstrashbinwidget.h"

const char* KTrashBinNormalItemImageName = "qtg_graf_hs_delete_normal";
const char* KTrashBinHighlightItemImageName = "qtg_graf_hs_delete_highlight";

/*!
    \internal
    \class HsTrashBinWidget
    \ingroup group_hsutils
    \brief Trash bin widget.
*/

/*!
    \internal
    Constructor.
    
    \a parent Owner.
*/
HsTrashBinWidget::HsTrashBinWidget(QGraphicsItem *parent)
    : HbWidget(parent),
      mIconItem(0),
      mNormalIcon(0),
      mHighlightIcon(0),
      mNormalIconVisible(false)
{
    mIconItem = new HbIconItem(this);

    mNormalIcon = new HbIcon(KTrashBinNormalItemImageName);
    mHighlightIcon = new HbIcon(KTrashBinHighlightItemImageName);

    mIconItem->setIcon(*mNormalIcon);
    setPreferredSize(80,80); // workaround for icon updating problem when changing theme
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
    layout->addItem(mIconItem);
    setLayout(layout);
}

/*!
    \internal
    Destructor.    
*/
HsTrashBinWidget::~HsTrashBinWidget()
{

}

/*!
    \internal
*/
void HsTrashBinWidget::activate()
{
    if (mNormalIconVisible) {
        mIconItem->setIcon(*mHighlightIcon);
        mNormalIconVisible = false;
    }
}

/*!
    \internal
*/
void HsTrashBinWidget::deactivate()
{
    if (!mNormalIconVisible) {
        mIconItem->setIcon(*mNormalIcon);
        mNormalIconVisible = true;
    }
}
