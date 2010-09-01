/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Global query active object
*
*/

#include "globalqueryhandler.h"
#include "xmluicontroller.h"
#include "appui.h"

#include <avkon.hrh> //EAknSoftkeyOk, EAknSoftkeyCancel, EAknSoftkeyDone

using namespace AiXmlUiController;

// ============================ LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CGlobalQueryHandler* CGlobalQueryHandler::NewL(
    CXmlUiController& aUiController )
    {
    CGlobalQueryHandler* self =  
            new ( ELeave )CGlobalQueryHandler( aUiController );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
 
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CGlobalQueryHandler::CGlobalQueryHandler( CXmlUiController& aUiController ) 
    : CActive( EPriorityStandard ), iUiController( aUiController )                 
    {
    CActiveScheduler::Add( this );
    }
        
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CGlobalQueryHandler::ConstructL()
    {    
    }
      
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CGlobalQueryHandler::~CGlobalQueryHandler() 
    { 
    Cancel(); 
    }
 
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CGlobalQueryHandler::DoCancel()
    {
    }
 
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CGlobalQueryHandler::RunL()
    {
    if( iStatus == EAknSoftkeyOk || iStatus == EAknSoftkeyYes )        
        {
        iUiController.AppUi()->SetOnlineStateL( iSetOnline );
        }
    else
        {
         iUiController.AppUi()->SetOnlineStateL(!iSetOnline );
        }
        
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CGlobalQueryHandler::SetOnlineParamAndActivate( TBool aOnline )
    { 
    iSetOnline = aOnline;
    
    SetActive();
    }

// END OF FILE

