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
* Description:  web widget list
 *
*/


#ifndef TSFSWIDGETLIST_H_
#define TSFSWIDGETLIST_H_

#include <e32base.h>
#include <widgetregistryclient.h>

_LIT(KWidgetAppName, "widgetlauncher.exe");

const TUint32 KWidgetAppUidValue = 0x10282822;

const TInt KCloseWidgetCmd(2);

/**
 * class handling the list of running wrt widgets
 */
NONSHARABLE_CLASS(CTsFsWidgetList) : public CBase
    {
public:
    static CTsFsWidgetList* NewL();

    /** Destructor */
    ~CTsFsWidgetList();

    /**
     * Initializes the list that stores widgets.
     * @return   -
     */        
    void InitializeWidgetListL();

    /**
     * Returns a ref to the list of running widgets.
     */
    const RWidgetInfoArray& RunningWidgets() const { return iRunningWidgets; }

private:
    /** Constructor */
    CTsFsWidgetList();
    void ResetArrayOfWidgetInfo( RWidgetInfoArray& aWidgetInfoArr );
    
    static void CleanupConnect( TAny* aThis );

private:
    /** Contains list of widgets that are currently running */
    RWidgetInfoArray iRunningWidgets;
    RWidgetRegistryClientSession iWidgetRegistryClientSession;
    };

#endif
