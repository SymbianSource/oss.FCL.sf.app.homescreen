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


#include <menudebug.h>

#include "hnrepositorymanager.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnRepositoryManager * CHnRepositoryManager::NewL()
    {
    CHnRepositoryManager * self = CHnRepositoryManager::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnRepositoryManager * CHnRepositoryManager::NewLC()
    {
    CHnRepositoryManager * self = new (ELeave) CHnRepositoryManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnRepositoryManager::StoreSuiteWidgetTypeL( 
                    const TDesC& aSuiteName, THnSuiteWidgetType aWidgetType )
    {
    DEBUG(("_MM_:CHnRepositoryManager::StoreSuiteWidgetTypeL IN"));
    DEBUG16(("_MM_:\taSuiteName: %S; aWidgetType: %d",&aSuiteName,aWidgetType));
    
    TInt ret( KErrGeneral );
    
    RArray< TUint32 > foundIds;
    CleanupClosePushL( foundIds );
    
    ret = iRepository->FindEqL( 0, 0, aSuiteName, foundIds );

    // If there is more than one entry of suite genre in central repository
    // it certainly means that it's a mistake.
    ASSERT( foundIds.Count() <= 1 );

    TInt keyId;
    if ( foundIds.Count() == 1 )
        {
        keyId = foundIds[ foundIds.Count() - 1 ];
        }
    else
        {
        foundIds.Reset();
        ret = iRepository->FindL(0, 0, foundIds);
        //Finding a free position to write
        keyId = ( foundIds.Count() > 0 ) ?
        		foundIds[ foundIds.Count() - 1 ] + 1 : 0;
        }
        
    DEBUG(("_MM_:\t\tkey ID: %d",keyId));
                
    TInt stateType(aWidgetType);
    if ( ( ret = iRepository->Set( keyId, aSuiteName ) ) == KErrNone )
    	{
        ret = iRepository->Set(keyId + KWidgetTypePosition, stateType);
    	}
    
    User::LeaveIfError( ret );

    DEBUG(("_MM_:\treturn value: %d",ret));
    DEBUG(("_MM_:CHnRepositoryManager::StoreSuiteWidgetTypeL OUT"));
    
    CleanupStack::PopAndDestroy( &foundIds );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnRepositoryManager::DeleteSuiteRelatedL( const TDesC & aSuiteName )
    {
    TInt ret( KErrNone );
    RArray< TUint32 > foundIds;
    CleanupClosePushL( foundIds );
    
    ret = iRepository->FindEqL( 0, 0, aSuiteName, foundIds );
    for (int i=0; i<foundIds.Count(); i++)
        {
        if( iRepository->Delete( foundIds[i] ) < 0 )
        	{
        	ret = KErrGeneral;
        	}
        if( iRepository->Delete( foundIds[i] + KWidgetTypePosition ) < 0 )
        	{
        	ret = KErrGeneral;
        	}
        }
    
    CleanupStack::PopAndDestroy( &foundIds );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnRepositoryManager::ReadSuiteWidgetTypeL(
		const TDesC& aSuiteName, THnSuiteWidgetType& aSuiteWidget )
    {
    TInt ret( KErrNone );
    RArray< TUint32 > foundIds;
    CleanupClosePushL( foundIds );
    ret = iRepository->FindEqL( 0, 0, aSuiteName, foundIds );
    
    // If there is more than one entry of suite genre in central repository
    // it certainly means that it's a mistake.
    ASSERT( foundIds.Count() <= 1 );
    if ( foundIds.Count() == 1 )
         {
         THnSuiteWidgetType suiteState( EUnspecified );
         ret = iRepository->Get( foundIds[0] + KWidgetTypePosition,
        		 (int &) suiteState);
         if( !ret )
             {
             switch( suiteState )
                {
                case EGridWidget:
                case EListWidget:
                case ECoverFlowWidget: 
                case EChangeWidget:
                case EUnspecified:
                    aSuiteWidget = suiteState;
                    break;
                default: 
                    ASSERT(false);                
                }
             }
        }

    CleanupStack::PopAndDestroy( &foundIds );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnRepositoryManager::~CHnRepositoryManager()
    {
    delete iRepository;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnRepositoryManager::ConstructL()
    {
    iRepository = CRepository::NewL( KMatrixRepositoryUid );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnRepositoryManager::CHnRepositoryManager() 
    : KWidgetTypePosition(1), iRepository(NULL)
    {
    }

