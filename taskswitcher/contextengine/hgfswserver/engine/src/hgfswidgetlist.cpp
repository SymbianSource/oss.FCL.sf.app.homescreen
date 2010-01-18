/*
 * ============================================================================
 *  Name        : hgfswidgetlist.cpp
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

#include "hgfswidgetlist.h"
#include <widgetregistrydata.h>

// --------------------------------------------------------------------------
// CHgFsWidgetList::NewL
// --------------------------------------------------------------------------
//
CHgFsWidgetList* CHgFsWidgetList::NewL()
    {
    return new (ELeave) CHgFsWidgetList;
    }

// --------------------------------------------------------------------------
// CHgFsWidgetList::CHgFsWidgetList
// --------------------------------------------------------------------------
//   
CHgFsWidgetList::CHgFsWidgetList()
    {
    }

// --------------------------------------------------------------------------
// CHgFsWidgetList::~CHgFsWidgetList
// --------------------------------------------------------------------------
//
CHgFsWidgetList::~CHgFsWidgetList()
    {
    ResetArrayOfWidgetInfo( iRunningWidgets );        
    iRunningWidgets.Reset();
    }

// --------------------------------------------------------------------------
// CHgFsWidgetList::CleanupConnect
// --------------------------------------------------------------------------
//
void CHgFsWidgetList::CleanupConnect( TAny* aThis )
    {
    CHgFsWidgetList* self = static_cast<CHgFsWidgetList*>( aThis );
    self->iWidgetRegistryClientSession.Disconnect();
    }

// --------------------------------------------------------------------------
// CHgFsWidgetList::InitializeWidgetListL
// --------------------------------------------------------------------------
//
void CHgFsWidgetList::InitializeWidgetListL()
    {
    ResetArrayOfWidgetInfo( iRunningWidgets );
    iRunningWidgets.Reset();
    User::LeaveIfError( iWidgetRegistryClientSession.Connect() );
    CleanupStack::PushL( TCleanupItem( CleanupConnect, this) );
    iWidgetRegistryClientSession.RunningWidgetsL(iRunningWidgets);
    CleanupStack::Pop(); // clean WidgetRegistryClientSession item
    iWidgetRegistryClientSession.Disconnect();
    }

// --------------------------------------------------------------------------
// CHgFsWidgetList::ResetArrayOfWidgetInfo
// --------------------------------------------------------------------------
//
void CHgFsWidgetList::ResetArrayOfWidgetInfo( 
        RWidgetInfoArray& aWidgetInfoArr )
    {
    for ( TInt i = 0; i < aWidgetInfoArr.Count(); i++ )
        {
        CWidgetInfo *item = aWidgetInfoArr[i];
        delete item;
        }
    }


// end of file
