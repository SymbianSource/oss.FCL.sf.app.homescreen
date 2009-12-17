/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This module contains the implementation of
* 					CDynamicThemeModifier class member functions.
*
*/



// INCLUDE FILES

#include "dynamicthememodifier.h"
#include <aisystemuids.hrh>
#include "xnodt.h"
#include "xndomdocument.h"
#include "xndomattribute.h"
#include "xndomnode.h"
#include "xndomlist.h"
#include "xndomdepthiterator.h"
#include <activeidle2domaincrkeys.h>

// CONSTANTS
#define NEWS_TICKER_GENERAL_SETTINGS_UID 0x10207196
const TUid KCRUidNewsTicker = {NEWS_TICKER_GENERAL_SETTINGS_UID};

/**
 * KGSNewsTickerStatus
 * CenRep key for "NewsTicker activated"
 * Off = 0
 * On = 1
 */
const TUint32 KGSNewsTickerStatus = 0x00000004;

/**
 * KAINTInTheme
 * CenRep key for "NewsTicker implemented in AI2 theme"
 * No = 0
 * Yes = 1
 */
const TUint32 KAiNTInTheme = 0x00000800;
const TInt KThemeArrayGranularity = 4;

_LIT8( KProperty, "property" );
_LIT8( KControl, "control" );
_LIT8( KName, "name" );
_LIT8( KValue, "value" );
_LIT8( KBlock, "block" );
_LIT8( KNone, "none" );

using namespace AiXmlUiController;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDynamicThemeModifier::CDynamicThemeModifier
// -----------------------------------------------------------------------------
//
CDynamicThemeModifier::CDynamicThemeModifier()
    {
    }

// -----------------------------------------------------------------------------
// CDynamicThemeModifier::ConstructL
// -----------------------------------------------------------------------------
//
void CDynamicThemeModifier::ConstructL()
    {
    // NT is not shown in theme by default. 
    // This is overridden later if it really is implemented (some other theme 
    // Basic activated)
    iAiCenRep = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
    User::LeaveIfError( iAiCenRep->Set( KAiNTInTheme, 0 ) );

	// Check if NT settings are visible in GS
    iNTCenRep = CRepository::NewL( KCRUidNewsTicker );
    User::LeaveIfError( iNTCenRep->Get( KGSNewsTickerStatus, iEnabledInCenRep ) );
    
    iNTCenRepObserver = CCenRepNotifyHandler::NewL( *this, 
                                                *iNTCenRep, 
                                                CCenRepNotifyHandler::EIntKey, 
                                                KGSNewsTickerStatus );    
    iNTCenRepObserver->StartListeningL();
    
    LoadThemesL();    
    }

// -----------------------------------------------------------------------------
// CDynamicThemeModifier::NewL
// -----------------------------------------------------------------------------
//
CDynamicThemeModifier* CDynamicThemeModifier::NewL()
    {
    CDynamicThemeModifier* self = new( ELeave ) CDynamicThemeModifier;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CDynamicThemeModifier::~CDynamicThemeModifier()
    {
    if( iThemeArray )
	    {
	    iThemeArray->ResetAndDestroy();
	    delete iThemeArray;    	
	    }
    delete iOdt;
    delete iDomDocument;
    delete iXnResult;
    delete iXnRequestClient;
    delete iXnContentAccessClient;
    delete iXnClient;
    if( iNTCenRepObserver )
	    {
	    iNTCenRepObserver->StopListening();	
	    delete iNTCenRepObserver;
	    }
    delete iNTCenRep;
    delete iAiCenRep;
    }

// ---------------------------------------------------------------------------
// CDynamicThemeModifier::LoadThemesL
// ---------------------------------------------------------------------------
//
void CDynamicThemeModifier::LoadThemesL()
    {
    const TUid KUidAI2 = TUid::Uid( AI_UID3_AIFW_EXE );
    
    iXnClient = CXnClient::NewL( *this );
    // we're only interested in AI2 themes
    iXnContentAccessClient = CXnContentAccessClient::NewL( *this,
    														KUidAI2.iUid );
    iXnRequestClient = CXnRequestClient::NewL( *this, KUidAI2.iUid );    														
    iXnResult = CXnResult::NewL();

    CXnODT* odt = CXnODT::NewL();
    CleanupStack::PushL( odt );

    odt->SetAppUid( KUidAI2.iUid );

    iThemeArray = new( ELeave ) CArrayPtrFlat<CXnODT>( KThemeArrayGranularity );

    // theme array gets filled up by XnClient after this
    // HandleXnClientMessage() is a callback method that we're interested in
    TXnServiceCompletedMessage ret =
    	iXnClient->XnGetListHeaders( *odt, *iThemeArray );

    if( ret == EXnGetListHeadersSuccess || ret == EXnGetListHeadersEmpty )
        {
		iXnClient->GetXnResult( *iXnResult );
		iNumberOfHeaders = iXnResult->iIntValue1;
        iXnClient->XnGetNextHeader();
        }

    CleanupStack::PopAndDestroy( odt );
    }



// -----------------------------------------------------------------------------
// CDynamicThemeModifier::HandleXnClientMessage
// -----------------------------------------------------------------------------
//
void CDynamicThemeModifier::HandleXnClientMessage(
									TXnServiceCompletedMessage aMessage )
    {
    // wait until all themes are fetched before loading settings
    if( ( aMessage == EXnGetListHeadersUpdate ) &&
        ( iThemeArray->Count() == iNumberOfHeaders ) )
        {
        TRAP_IGNORE( LoadSettingsFromThemeL() );
        }

    else if( aMessage == EXnGetListHeadersFailed )
        {
        if (iXnClient)
        	{
            iXnClient->XnCancelGetListHeaders();
        	}

        if ( iThemeArray )
    		{
    		iThemeArray->ResetAndDestroy();
    		delete iThemeArray;
    		iThemeArray = NULL;
    		}
        }

    }


// -----------------------------------------------------------------------------
// CDynamicThemeModifier::LoadSettingsFromThemeL
// -----------------------------------------------------------------------------
//
void CDynamicThemeModifier::LoadSettingsFromThemeL()
    {    
    for( TInt i = 0; i < iThemeArray->Count(); i++ )
	    {
   	   	CXnODT* odt = iThemeArray->At( i );

   	    // activated theme found -> load settings
   	   	if( odt->Flags() & EXnThemeStatusActive )
   	   	    {
   	   	    delete iDomDocument;
   	   	    iDomDocument = NULL;
            iDomDocument = CXnDomDocument::NewL();

            TXnServiceCompletedMessage ret = 
                iXnContentAccessClient->XnLoadSettings( *odt, *iDomDocument );
        	if( ret == EXnSettingsLoaded )
        	    {
        	    delete iOdt;
        	    iOdt = NULL;
			    iOdt = odt->CloneL();
	            odt = NULL;
	            // update theme to match CenRep value	            
			    EnableNewsTickerL( TBool( iEnabledInCenRep ) );	
	            break;	            
                }
   	   	    }
	    }
    }
    

// -----------------------------------------------------------------------------
// CDynamicThemeModifier::EnableNewsTickerL
// -----------------------------------------------------------------------------
//
void CDynamicThemeModifier::EnableNewsTickerL( TBool aEnabled )
    {
    TInt found = 0;
	CXnDomDepthIterator* iterator =
		CXnDomDepthIterator::NewL( *iDomDocument->RootNode() );
	CleanupStack::PushL( iterator );

	CXnDomNode* node = iterator->First();

	if( !node )
	    {
	    CleanupStack::PopAndDestroy( iterator );
	    return;
	    }

	// find <control> tag
	while( node )
	    {
		if( !node->Name().Compare( KControl ) )
		    {
			break;
		    }
		node = iterator->NextL();
	    }

	if( !node->Name().Compare( KControl ) )
	    {
		node = iterator->NextL();

		while( node )
    		{
			// look for <property> tag...
            if( !node->Name().Compare( KProperty ) )
    		    {
				// ...with attribute "value"...
    			if( !node->AttributeValue( KName ).Compare( KValue ) )
    			    {
    			    found = 1;
    				CXnDomAttribute* attr = static_cast<CXnDomAttribute*>
    					( node->AttributeList().FindByName( KValue) );

					// ...update it accordingly...
    				if( aEnabled )
    				    {
    				    attr->SetValueL( KBlock );
    				    }
                    else
    				    {
    				    attr->SetValueL( KNone );
    				    }
    		        }
    		    }
    	    node = iterator->NextL();
    		}

		// ...and communicate the changes via Content Access API.		
        for( TInt i = 0; i < iThemeArray->Count(); i++ )
    	    {
       	   	CXnODT* odt = iThemeArray->At( i );
    		
			// This might return an error if settings were not found from theme.
			// However, we don't need to take any actions since appropriate 
			// theme is left untouched in that case anyway
            iXnContentAccessClient->XnUpdateSettings( *odt, *iDomDocument );
    	    }

		// if found == 1, then NT is implemented in theme 
		// -> NT settings can be shown in GS
		User::LeaveIfError( iAiCenRep->Set( KAiNTInTheme, found ) );
		CleanupStack::PopAndDestroy( iterator );
	    }
    }

// -----------------------------------------------------------------------------
//  CDynamicThemeModifier::HandleNotifyInt
//  Callback method for observing CenRep changes
// -----------------------------------------------------------------------------
//
void CDynamicThemeModifier::HandleNotifyInt( TUint32 /*aId*/, TInt aNewValue )
    {
    iEnabledInCenRep = TBool( aNewValue );
    TRAP_IGNORE( EnableNewsTickerL( iEnabledInCenRep ) );
    ;
    }

//  End of File
