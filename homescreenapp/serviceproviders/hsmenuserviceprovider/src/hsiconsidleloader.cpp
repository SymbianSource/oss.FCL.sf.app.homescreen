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
 * Description: Menu All Applications state.
 *
 */

#include <QSize>
#include <QTimer>
#include "hsmenuitemmodel.h"
#include "hsiconsidleloader.h"

/*!
 Constructor
 \param model with defined icons
 \param pointer to parent object
 */
HsIconsIdleLoader::HsIconsIdleLoader(HsMenuItemModel *model, QObject *parent):
    QObject(parent),
    mModel(model),
    mTimer(NULL)
{
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(processWhenIdle()));
    mTimer->start(0); // NOTE: zero for idle
}

/*!
 Destructor
 */
HsIconsIdleLoader::~HsIconsIdleLoader()
{
    mTimer->stop();
}

/*!
 Preloading icons if idle
 \retval void
 */
void HsIconsIdleLoader::processWhenIdle()
{
    const QSize iconSize(mModel->getIconSize());
    for (int i=0; i<mModel->rowCount(); i++) {
        QModelIndex idx = mModel->index(i);
        mModel->entry(idx)->makeIcon(iconSize);
    }
    mTimer->stop(); // No more timing
}

