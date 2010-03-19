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
* Description:  Menu Application Library state.
 *
*/


#ifndef HSADDSHORTCUTSTATE_H
#define HSADDSHORTCUTSTATE_H

#include <hsmenubasestate.h>

#include "hsmenustates_global.h"
HS_STATES_TEST_CLASS(MenuStatesTest)



/**
 * @ingroup group_hsmenustateplugin
 * @brief Application Library State.
 *
 * Parent state for Application Library functionality (browsing applications and collections)
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsAddToHomeScreenState: public  HsMenuBaseState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    /**
     * Constructor.
     *
     * @since S60 ?S60_version.
     * @param parent Owner.
     */
    HsAddToHomeScreenState(QState *parent = 0);

    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    virtual ~HsAddToHomeScreenState();

private slots:

    /**
     * Inherited from HsMenuBaseState.
     *
     * @since S60 ?S60_version.
     */
    void onEntry(QEvent *event);

private:

    void showMessageWidgetCorrupted(int itemId);

    void addWidget(HsContentService &service, const QString &library,
                   const QString &uri, int entryId);

    void addShortcut(HsContentService &contentService, int entryId);

    void logActionResult(QString operationName, int entryId,
                         bool operationSucceded);


    // keep path in memory so it wont
    QString mLibraryPath;

};

#endif //HSADDSHORTCUTSTATE_H
