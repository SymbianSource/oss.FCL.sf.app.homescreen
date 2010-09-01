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
* Description:  represents the notification request
*
*/


#include <e32base.h>
#include <xmlengdom.h> 
#include <hnglobals.h>

#include "hnmdnotifyrequest.h"
#include "hnmdnotifyrequests.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdNotifyRequests::ConstructL( TXmlEngElement aElement,
        const CHnItemId & aItemId,
        THnMdCommonPointers* aCmnPtrs )
    {
    iSent = EFalse;
    RXmlEngNodeList< TXmlEngElement > entries;
    CleanupClosePushL( entries );
    aElement.GetChildElements( entries );
    
    TInt entriesAmount = entries.Count();
    
    for (TInt j = 0; j < entriesAmount; j++ )
        {
        TXmlEngElement item = entries.Next();
        
        if ( !item.Name().Compare( KNotifyRequestElementName8 ) )
            {
            CHnMdNotifyRequest *request = CHnMdNotifyRequest::NewL( 
                    item, aItemId, aCmnPtrs );
            AddNotifyRequest( request );
            }
        }
        
    CleanupStack::PopAndDestroy( &entries );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CHnMdNotifyRequests::SendAllRequestsL( CLiwGenericParamList& aParams,
        TInt aCmdOptions )
    {
    for (int i=0; i<Count(); i++)
        {
        NotifyRequest(i).RequestNotifyL( aParams, aCmdOptions );
        }
    iSent = ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TInt CHnMdNotifyRequests::AddNotifyRequest( CHnMdNotifyRequest* aNotifyRequest )
    {
    return iNotifyRequests.Append( aNotifyRequest );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CHnMdNotifyRequest& CHnMdNotifyRequests::NotifyRequest( 
           TInt aPosition ) const
    {
    ASSERT( aPosition >=0 && aPosition < iNotifyRequests.Count() );
    return *iNotifyRequests[aPosition];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TInt CHnMdNotifyRequests::Count() const
    {
    return iNotifyRequests.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdNotifyRequests* CHnMdNotifyRequests::NewL( 
         TXmlEngElement aElement, const CHnItemId & aItemId, 
         THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdNotifyRequests* self = CHnMdNotifyRequests::NewLC(
             aElement, aItemId, aCmnPtrs);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdNotifyRequests* CHnMdNotifyRequests::NewLC(
        TXmlEngElement aElement, const CHnItemId & aItemId, 
        THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdNotifyRequests* self = new( ELeave ) CHnMdNotifyRequests();
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aItemId, aCmnPtrs );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdNotifyRequests::CHnMdNotifyRequests()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdNotifyRequests::~CHnMdNotifyRequests()
    {
    iNotifyRequests.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdNotifyRequests::Sent()
    {
    return iSent;
    }

