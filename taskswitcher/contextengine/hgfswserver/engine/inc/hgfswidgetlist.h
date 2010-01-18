/*
 * ============================================================================
 *  Name        : hgfswidgetlist.h
 *  Part of     : Hg Teleport
 *  Description : web widget list
 *  Version     : %version: sa1spcx1#5 %
 *
 *  Copyright 2008 Nokia.  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 */

#ifndef HGFSWIDGETLIST_H_
#define HGFSWIDGETLIST_H_

#include <e32base.h>
#include <widgetregistryclient.h>

const TUint32 KWidgetAppUidValue = 0x10282822;

/**
 * class handling the list of running wrt widgets
 */
NONSHARABLE_CLASS(CHgFsWidgetList) : public CBase
    {
public:
    static CHgFsWidgetList* NewL();

    /** Destructor */
    ~CHgFsWidgetList();

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
    CHgFsWidgetList();
    void ResetArrayOfWidgetInfo( RWidgetInfoArray& aWidgetInfoArr );
    
    static void CleanupConnect( TAny* aThis );

private:
    /** Contains list of widgets that are currently running */
    RWidgetInfoArray iRunningWidgets;
    RWidgetRegistryClientSession iWidgetRegistryClientSession;
    };

#endif
