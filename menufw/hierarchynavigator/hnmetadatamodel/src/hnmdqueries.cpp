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
#include "hnmdqueries.h"
#include "hnmdquery.h"
#include "hnglobals.h"
#include "hnmdmodel.h"
#include "hnconvutils.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdQueries::ConstructL( TXmlEngElement aElement )
    {
    // construction...
    RXmlEngNodeList< TXmlEngElement > entries;
    CleanupClosePushL( entries );
    aElement.GetChildElements( entries );
    
    TInt entriesAmount = entries.Count();
    
    for (TInt j = 0; j < entriesAmount; j++ )
        {
        TXmlEngElement item = entries.Next();
        
        if ( !item.Name().Compare( KQueryElementItem8 ) )
            {
            CHnMdQuery *query = CHnMdQuery::NewL( item );
            AddQuery( query );
            }
        }
    CleanupStack::PopAndDestroy( &entries );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TInt CHnMdQueries::AddQuery( CHnMdQuery* aQuery )
    {
    return iQueries.Append( aQuery );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CHnMdQuery& CHnMdQueries::Query( TInt aPosition ) const
    {
    ASSERT( aPosition >=0 && aPosition < iQueries.Count() );
    return *iQueries[aPosition];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TInt CHnMdQueries::Count() const
    {
    return iQueries.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdQueries* CHnMdQueries::NewL( TXmlEngElement aElement )
    {
    CHnMdQueries* self = CHnMdQueries::NewLC( aElement );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdQueries* CHnMdQueries::NewLC( TXmlEngElement aElement )
    {
    CHnMdQueries* self = new( ELeave ) CHnMdQueries();
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdQueries::CHnMdQueries()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdQueries::~CHnMdQueries()
    {
    iQueries.ResetAndDestroy();
    }


    
