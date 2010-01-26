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


#include "tsfswidgetlist.h"
#include <widgetregistrydata.h>

// --------------------------------------------------------------------------
// CTsFsWidgetList::NewL
// --------------------------------------------------------------------------
//
CTsFsWidgetList* CTsFsWidgetList::NewL()
    {
    return new (ELeave) CTsFsWidgetList;
    }

// --------------------------------------------------------------------------
// CTsFsWidgetList::CTsFsWidgetList
// --------------------------------------------------------------------------
//   
CTsFsWidgetList::CTsFsWidgetList()
    {
    }

// --------------------------------------------------------------------------
// CTsFsWidgetList::~CTsFsWidgetList
// --------------------------------------------------------------------------
//
CTsFsWidgetList::~CTsFsWidgetList()
    {
    ResetArrayOfWidgetInfo( iRunningWidgets );        
    iRunningWidgets.Reset();
    }

// --------------------------------------------------------------------------
// CTsFsWidgetList::CleanupConnect
// --------------------------------------------------------------------------
//
void CTsFsWidgetList::CleanupConnect( TAny* aThis )
    {
    CTsFsWidgetList* self = static_cast<CTsFsWidgetList*>( aThis );
    self->iWidgetRegistryClientSession.Disconnect();
    }

// --------------------------------------------------------------------------
// CTsFsWidgetList::InitializeWidgetListL
// --------------------------------------------------------------------------
//
void CTsFsWidgetList::InitializeWidgetListL()
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
// CTsFsWidgetList::ResetArrayOfWidgetInfo
// --------------------------------------------------------------------------
//
void CTsFsWidgetList::ResetArrayOfWidgetInfo( 
        RWidgetInfoArray& aWidgetInfoArr )
    {
    for ( TInt i = 0; i < aWidgetInfoArr.Count(); i++ )
        {
        CWidgetInfo *item = aWidgetInfoArr[i];
        delete item;
        }
    }


// end of file
