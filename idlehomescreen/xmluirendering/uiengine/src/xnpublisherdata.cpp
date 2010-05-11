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
* Description:  Publisher data
*
*/

// System includes

// User includes
#include <aifwpublisherinfo.h>
#include "xnappuiadapter.h"
#include "xnplugindata.h"

#include "xnpublisherdata.h"

// Constants
enum TXnPublisherDataFlags
    {
    EIsVirgin,
    EIsStarted,
    EIsLoading,
    EIsFailed    
    };

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CXnPublisherData::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnPublisherData* CXnPublisherData::NewL( CXnPluginData& aOwner, 
    CXnNode& aNode )         
    {
    CXnPublisherData* self = CXnPublisherData::NewLC( aOwner, aNode );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnPublisherData* CXnPublisherData::NewLC( CXnPluginData& aOwner,
    CXnNode& aNode )
    {
    CXnPublisherData* self = new ( ELeave ) CXnPublisherData( aOwner, aNode );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::~CXnPublisherData()
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CXnPublisherData::~CXnPublisherData()
    {
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::CXnPublisherData()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXnPublisherData::CXnPublisherData( CXnPluginData& aOwner, CXnNode& aNode )
    : iOwner( aOwner ), iNode( aNode )
    {
    iAppUi = static_cast< CXnAppUiAdapter* >( iAvkonAppUi );
    
    iFlags.Set( EIsVirgin );
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::ConstructL()
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CXnPublisherData::ConstructL()
    {    
    User::LeaveIfError( iAppUi->PublisherInfo( iNode, iInfo ) );            
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::operator==()
// 
// -----------------------------------------------------------------------------
//
TBool CXnPublisherData::operator==( CXnNode& aNode ) const
    {
    THsPublisherInfo info;
    
    TRAPD( err, iAppUi->PublisherInfo( aNode, info ) );
    
    if ( !err )
        {
        return iInfo == info;
        }
    
    return EFalse;             
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::operator==()
// 
// -----------------------------------------------------------------------------
//
TBool CXnPublisherData::operator==( const THsPublisherInfo& aInfo ) const
    {
    return iInfo == aInfo;
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::Info()
// 
// -----------------------------------------------------------------------------
//
const THsPublisherInfo& CXnPublisherData::Info() const
    {
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::ContentSource()
// 
// -----------------------------------------------------------------------------
//
CXnNode* CXnPublisherData::ContentSource() const
    {
    return &iNode;
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::Load()
// 
// -----------------------------------------------------------------------------
//
void CXnPublisherData::Load( TInt aReason ) 
    {
    if ( iFlags.IsSet( EIsFailed ) )
        {
        return;
        }
    
    if ( aReason != EAiFwPluginStartup && iFlags.IsSet( EIsVirgin ) )
        {               
        aReason = EAiFwSystemStartup;                          
        }
    
    iFlags.Set( EIsLoading );
    
    TAiFwCallback cb( NotifyLoaded, this );
    TAiFwPublisherInfo publisher( iInfo, cb, aReason );
           
    iAppUi->LoadPublisher( publisher );
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::Load()
// 
// -----------------------------------------------------------------------------
//
void CXnPublisherData::Destroy( TInt aReason ) 
    {
    if ( iFlags.IsSet( EIsFailed ) )
        {
        return;
        }
        
    iFlags.Clear( EIsLoading );
        
    TAiFwCallback cb( NotifyDestroyed, this );
    TAiFwPublisherInfo publisher( iInfo, cb, aReason );
        
    iAppUi->DestroyPublisher( publisher );
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::IsFailed()
// 
// -----------------------------------------------------------------------------
//
TBool CXnPublisherData::IsFailed() const
    {
    return iFlags.IsSet( EIsFailed );
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::IsLoading()
// 
// -----------------------------------------------------------------------------
//
TBool CXnPublisherData::IsLoading() const
    {
    return iFlags.IsSet( EIsLoading );
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::Owner()
// 
// -----------------------------------------------------------------------------
//
CXnPluginData* CXnPublisherData::Owner() const
    {
    return &iOwner;
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::NotifyLoaded()
// 
// -----------------------------------------------------------------------------
//
/* static */ void CXnPublisherData::NotifyLoaded( TAny* aAny, TInt aResult )
    {
    CXnPublisherData* self = static_cast< CXnPublisherData* >( aAny );

    self->iFlags.Clear( EIsLoading );
    
    if ( aResult == KErrCancel )
        {
        // Loading canceled
        }
    else
        {
        // Loading complete 
        if ( aResult == KErrNone || aResult == KErrAlreadyExists )
            {
            self->iFlags.Set( EIsStarted );
            self->iFlags.Clear( EIsVirgin );
            }
        else
            {
            self->iFlags.Set( EIsFailed );
            }               
        }    
    
    TRAP_IGNORE( self->iOwner.NotifyPublisherReadyL() );
    }

// -----------------------------------------------------------------------------
// CXnPublisherData::NotifyDestroyed()
// 
// -----------------------------------------------------------------------------
//
/* static */ void CXnPublisherData::NotifyDestroyed( TAny* /*aAny*/, 
    TInt /*aResult*/ )
    {          
    }

// End of file
