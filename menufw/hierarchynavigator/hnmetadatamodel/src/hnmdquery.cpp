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
* Description:  
*
*/


#include <xmlengdom.h>
// for CleanupResetAndDestroyPushL
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "hnmdkeyfactory.h"
#include "hnmdbasekey.h"
#include "hnmdquery.h"
#include "hnconvutils.h"
#include "hnmdservicecommand.h"
#include "hnmdservice.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdQuery::ConstructL( TXmlEngElement aElement )
    {
    ASSERT( aElement.Name() == KQueryElementItem8 );

    SetNameSpaceL( aElement.AttributeNodeL( KNameSpaceAttrName8 ) );
    iService = CHnMdService::NewL( aElement );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdQuery* CHnMdQuery::NewL( TXmlEngElement aElement )
    {
    CHnMdQuery* self = CHnMdQuery::NewLC( aElement );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdQuery* CHnMdQuery::NewLC( TXmlEngElement aElement )
    {
    CHnMdQuery* self = new( ELeave ) CHnMdQuery();
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdQuery::CHnMdQuery()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdQuery::~CHnMdQuery()
    {
    iNameSpace.Close();
    delete iService;
    }
  
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CHnMdQuery::NameSpace() const
    {
    return iNameSpace;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdQuery::SetNameSpaceL( const TDesC& aNameSpace )
    {
    iNameSpace.Close();
    iNameSpace.CreateL(aNameSpace);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdQuery::SetNameSpaceL( TXmlEngAttr aAttr )
    {
    HBufC* nspace = HnConvUtils::Str8ToStrLC( aAttr.Value() );
    SetNameSpaceL( *nspace );
    
    // clean up
    CleanupStack::PopAndDestroy( nspace );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdService& CHnMdQuery::GetService() const
    {
    return *iService;
    }

