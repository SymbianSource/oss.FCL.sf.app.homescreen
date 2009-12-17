/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  event handler class
*
*/


#include <liwservicehandler.h>
#include "hneventhandler.h"
#include "hnmdmodel.h"
#include "hnglobals.h"
#include "hnconvutils.h"
#include "hnliwutils.h"
#include "hnmdbasekey.h"
#include "hncontrollerinterface.h"
#include "hnactionmodel.h"
#include "hnservicehandler.h"
#include "menudebug.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// Safe constructor. Initializes refrerece to the Meta Data Model in which 
// the event-to-action mappings are stored.
// ---------------------------------------------------------------------------
//
CHnEventHandler::CHnEventHandler( MHnMdModelEventObserver& aModelObserver,
        MHnControllerInterface& aController ):
        iEventObserver( aModelObserver), iControllerInterface( aController )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnEventHandler::~CHnEventHandler()
    {
    delete iServiceHandler;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnEventHandler* CHnEventHandler::NewLC( 
                                      MHnMdModelEventObserver& aModelObserver,
                                      MHnControllerInterface& aController )
    {
    CHnEventHandler* self =
        new (ELeave)CHnEventHandler( aModelObserver, aController );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnEventHandler* CHnEventHandler::NewL( 
                                    MHnMdModelEventObserver& aModelObserver,
                                    MHnControllerInterface& aController )
    {
    CHnEventHandler* self=CHnEventHandler::NewLC( aModelObserver, 
                                                  aController );
    CleanupStack::Pop(self); 
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnEventHandler::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

TInt CHnEventHandler::ExecuteStandardActionL( CHnActionModel* aActionModel )
    {
    delete iServiceHandler;
    iServiceHandler = NULL;
    iServiceHandler = CHnServiceHandler::NewL(
            aActionModel->Service(),
            aActionModel->Interface(),
            aActionModel->CommandName(),
            aActionModel->ServiceMode(),
            aActionModel->ConstructorLC(), 
            aActionModel->CommandLC() );

    CleanupStack::Pop( 2 );
    TInt res = iServiceHandler->ExecuteL( this, 0 ); 
    return res;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnEventHandler::ExtractUidFromActionL( const TDesC8& aInterface, 
        TUid& aUid )
    {
    DEBUG16(("_MM_: CHnEventHandler::ExtractUidFromActionL UIext - uid %S",
                &aInterface ));
  
    TLex8 lex( aInterface );
    lex.Inc( 2 );
    return lex.Val( (TUint32 &) aUid.iUid, EHex );
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnEventHandler::ExecuteInternalActionL( CHnActionModel* aActionModel )
    {
    TInt ret(KErrNotFound);
    CLiwGenericParamList* params = aActionModel->CommandLC();
    if ( aActionModel->CommandName() == KServiceOpenSuite )
        {
        ret = iEventObserver.HandleModelEventL( KNewSuiteLoadedMdEvent(), 
                *params );
        }
    else if ( aActionModel->CommandName() == KServiceSwitchWidget )
        {
        ret = iEventObserver.HandleModelEventL( KSwitchWidgetMdEvent(), 
                *params );
        }
    else if ( aActionModel->CommandName() == KServiceStartEditMode )
        {
        ret = iEventObserver.HandleModelEventL( KStartEditModeMdEvent(), 
                *params );
        }
    else if ( aActionModel->CommandName() == KServiceStopEditMode )
        {
        ret = iEventObserver.HandleModelEventL( KStopEditModeMdEvent(), 
                *params );
        }
    else if ( aActionModel->CommandName() == KServiceBack )
        {
        ret = iEventObserver.HandleModelEventL( KBackMdEvent(), 
                *params );
        }
    else if ( aActionModel->CommandName() == KSetFocus )
        {
        ret = iEventObserver.HandleModelEventL( KSetFocusEvent(), 
                *params );
        }

    CleanupStack::PopAndDestroy( params );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnEventHandler::ExecuteExtensionManagerActionL(
        CHnActionModel* aActionModel )
    {
    TUid uid;
    TInt err = ExtractUidFromActionL( aActionModel->Interface(), uid );
    
    if ( KErrNone == err )
        {
        HBufC* cmd = HnConvUtils::Str8ToStrFastLC( aActionModel->CommandName() );
        CLiwGenericParamList* command = aActionModel->CommandLC();
        err = iControllerInterface.ExecuteExtensionActionL( uid, *cmd, command );        
        CleanupStack::PopAndDestroy( command );
        CleanupStack::PopAndDestroy( cmd );
        }
    
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnEventHandler::ExecuteActionL( CHnActionModel* aActionModel )
    {
    ASSERT( aActionModel );
    TInt err( KErrNone );
    
    if( aActionModel->Service() == KServiceMultimediaMenu )
        {
        if ( aActionModel->Interface().Length() == 0 )
            {
            err = ExecuteInternalActionL( aActionModel );
            }
        else
            {
            err = ExecuteExtensionManagerActionL( aActionModel );
            }
        }
    else
        {
        err = ExecuteStandardActionL( aActionModel );
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CHnEventHandler::HandleNotifyL( TInt /*aCmdId*/, TInt /*aEventId*/,
        CLiwGenericParamList& /*aEventParamList*/,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
    return KErrNone;
    }
