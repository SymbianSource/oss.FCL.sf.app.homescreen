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


#include <uri8.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <sacls.h>
#include <apgtask.h>
#include <centralrepository.h>
#include <AknUtils.h>

#include "hnengine.h"
#include "hnconvutils.h"
#include "hncontrollerinterface.h"
#include "hnfilter.h"
#include "hnsuitemodelcontainer.h"
#include "hnsuitemodel.h"
#include "hnserviceglobals.h"
#include "hnglobals.h"
#include "hnmdsuite.h"
#include "hnitemid.h"
#include "hnliwutils.h"
#include "hnrepositorymanager.h"
#include "menudebug.h"
#include "hnrepositorymanager.h"
#include "hnsuiteobserver.h"
#include "hnmdbasekey.h"
#include "hnitemsorder.h"

_LIT8(  KAnd8,                      "&" );
_LIT8(  KEquals8,                   "=" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnEngine::ConstructL()
    {
    iEEnv = CEikonEnv::Static();
    iSuiteContainer = CHnSuiteModelContainer::NewL( *this , iControllerInterface );
    iMetaDataModel = CHnMdModel::NewL( this, iSuiteContainer );
    iLight = CHWRMLight::NewL(this);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnEngine* CHnEngine::NewL( MHnControllerInterface& aController )
    {
    CHnEngine* self = CHnEngine::NewLC( aController );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnEngine* CHnEngine::NewLC( MHnControllerInterface& aController )
    {
    CHnEngine* self = new( ELeave ) CHnEngine( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnEngine::CHnEngine( MHnControllerInterface& aController ):
    iControllerInterface( aController )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnEngine::~CHnEngine()
    {
    delete iLight;
    if( iSuiteContainer )
        {
        iSuiteContainer->RemoveLiwObjects();
        }
    delete iMetaDataModel;
    delete iSuiteContainer;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::GetSuiteModelsCountL()
    {
    return iSuiteContainer->GetSuiteModelCount();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnEngine::SuiteModelLoadedL( TInt aId )
    {
    TBool ret( EFalse );
    TInt count( iSuiteContainer->GetSuiteModelCount() );
    for( int i=0; i<count; i++ )
        {
        CHnSuiteModel* suite = iSuiteContainer->GetSuiteModel( i );
        if( suite->GetItemsOrder()->GetSuiteId() == aId )
            {
            ret = ETrue;
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnSuiteModel* CHnEngine::GetLastSuiteModelL()
    {
    return iSuiteContainer->GetLastSuiteModel();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnSuiteModel* CHnEngine::GetParentSuiteModelL()
    {
    return iSuiteContainer->GetParentSuiteModel();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnEngine::TriggerHnEventL( const TInt aHnEventId,
        const TInt aRecipientId, CLiwGenericParamList* aEventParameters )
    {
    TInt ret = iSuiteContainer->OfferHnEventL( aHnEventId, aRecipientId,
            aEventParameters);
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnEngine::HighlightPlacementFromUriL( const TDesC8& aUriFragment )
    {
    TLex8 lexFrag( aUriFragment );
    TInt highLight( 0 );
    if( !( lexFrag.Val( highLight ) ) )
        {
        iSuiteContainer->GetLastSuiteModel()->SetSuiteHighlightL( highLight );
        iControllerInterface.HandleSuiteEventL( ESuiteHighlightChanged,
                iSuiteContainer->GetLastSuiteModel() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::LoadSuiteFromUriL( const TDesC8& aSuiteName,
        const TDesC8& aUriQuery, const TDesC8& aUriFragment )
    {
    TInt error( KErrNone );

    if( !( aSuiteName.Compare( KRoot8 ) ) )
        {
        CLiwGenericParamList* uriParams =
			UriQueryToLiwListLC( aUriQuery, aUriFragment, aSuiteName );
        CLiwGenericParamList* params = CLiwGenericParamList::NewLC();
        params->AppendL( iMetaDataModel->GetSuiteParameters( 0 ) );
        params->AppendL( *uriParams );
        ResetModelsL();
        iMetaDataModel->QueueForeground( CHnMdModel::EWhenAnySuiteIsEvaluated );
        error = InitializeL( *params );
        CleanupStack::PopAndDestroy( params );
        CleanupStack::PopAndDestroy( uriParams );
        }
    else
        {
        CLiwGenericParamList* params =
			UriQueryToLiwListLC( aUriQuery, aUriFragment, aSuiteName );

        HBufC* suiteName = HnConvUtils::Str8ToStrLC( aSuiteName );
        params->AppendL( TLiwGenericParam( KNewSuiteParamNameEn,
            TLiwVariant( *suiteName ) ) );

        error = HandleNewSuiteLoadedEventL( *params );

        CleanupStack::PopAndDestroy( suiteName );
        CleanupStack::PopAndDestroy( params );
        }

    return error;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnEngine::SuitesAreTheSameL( const TDesC& aLastSuiteName,
    const TDesC8& aFirstSuiteName )
    {
    RBuf8 lastSuiteName;
    CleanupClosePushL( lastSuiteName );
    lastSuiteName.CreateL( aLastSuiteName.Length() );
    lastSuiteName.Copy( aLastSuiteName );
    TBool suitesAreTheSame( !lastSuiteName.Compare( aFirstSuiteName ) );
    CleanupStack::PopAndDestroy( &lastSuiteName );
    return suitesAreTheSame;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnEngine::HandleTheSameSuitesL( TBool aNextExists, const TDesC8& aParams )
    {
    RBuf exitMode;
    CleanupClosePushL( exitMode );
    CLiwGenericParamList* params = UriQueryToLiwListLC( aParams, KNullDesC8, KNullDesC8 );
    HnLiwUtils::GetStringL( *params, KExitModeParams, exitMode );

    if ( !exitMode.Compare( KExitModeHide ) )
        {
        // set parameters
        iSuiteContainer->GetLastSuiteModel()->SetExitMode( EExitModeHide );
        }

    CleanupStack::PopAndDestroy( params );
    CleanupStack::PopAndDestroy( &exitMode );

    if ( !aNextExists )
        {
        // refresh if necessary
        iControllerInterface.NotifyUiRefreshL( ERefreshSuite );
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnEngine::LoadSuitesFromUriL( const TDesC8& aUri )
	{
	DEBUG(("_MM_:CHnEngine::LoadSuitesFromUriL IN"));
	DEBUG8(("_MM_:\tURI: %S",&aUri));

	TBool consumed(EFalse);

    if ( aUri.Find( KSetFocusWithPref ) != KErrNotFound )
	    {
	    LoadFromCrL( aUri );
	    consumed = ETrue;
	    }

    if ( !consumed )
    	{
    	consumed = HandleActionL( aUri );
    	}

    if ( !consumed )
    	{
    	LoadSuitesL( aUri );
    	}

    DEBUG(("_MM_:CHnEngine::LoadSuitesFromUriL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleModelEventL( const TDesC& aEventName,
                                CLiwGenericParamList& aParams )
    {
    TInt ret(KErrNotFound);

    if ( aEventName == KNewSuiteLoadedMdEvent )
        {
        // This handles new suite creation, putting its visual model
        // onto the stack eventually.
        ret = HandleNewSuiteLoadedEventL( aParams );
        }
    else if ( aEventName == KBackMdEvent )
        {
        const TDesC& suiteName = iSuiteContainer->GetLastSuiteModel()->SuiteName();
        ret = HandleBackEventL( suiteName );
        }
    else if (aEventName == KReevaluateMdEvent )
        {
        // This event triggers reevaluate on the model.
        // aParams holds liw packed filter.
        ret = HandleReevaluateEventL( aParams );
        }
    else if (aEventName == KSwitchWidgetMdEvent )
        {
        // Widget change is requested.
        ret = HandleWidgetChangeL( aParams );
        }
    else if (aEventName == KRefreshUiMdEvent )
        {
        // Refresh of the whole of the UI is refreshed
        iControllerInterface.NotifyUiRefreshL( ERefreshSuite );
        ret = KErrNone;
        }
    else if (aEventName == KRefreshToolbarMdEvent )
        {
        // Just the toolbar in UI is refreshed
        iControllerInterface.NotifyUiRefreshL( ERefreshToolbar );
        ret = KErrNone;
        }
    else if (aEventName == KStartEditModeMdEvent )
        {
        // Starting edit mode
        iControllerInterface.NotifyUiRefreshL( EStartEditMode );
        ret = KErrNone;
        }
    else if (aEventName == KStopEditModeMdEvent )
        {
        // Stopping edit mode
        iControllerInterface.NotifyUiRefreshL( EStopEditMode );
        ret = KErrNone;
        }
    else if (aEventName == KSetFocusEvent )
        {
        // SetFocus
        ret = HandleSetFocusEventL( aParams );
        }
    else if (aEventName == KAppGainForeground )
    	{
    	//force matrix gain foreground
        DEBUG(("_MM_:CHnEngine::HandleModelEventL EForegroundGain"));
        iControllerInterface.NotifyUiRefreshL( EForegroundGain );
        ret = KErrNone;
    	}
    else if (aEventName == KAppGainBackground )
    	{
    	//force matrix gain background
        DEBUG(("_MM_:CHnEngine::HandleModelEventL EBackgroundGain"));
        iControllerInterface.NotifyUiRefreshL( EBackgroundGain );
        ret = KErrNone;
    	}

    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleWidgetChangeL(
        CLiwGenericParamList& aParams )
    {
    THnSuiteWidgetType type = EChangeWidget;
    TInt pos( 0 );
    const TLiwGenericParam* param = NULL;
    param = aParams.FindFirst( pos,  KWidgetTypeAttrName8 );
    if ( pos != KErrNotFound )
        {
        TPtrC widgetType( KNullDesC );
        param->Value().Get( widgetType );
        if ( widgetType.Compare( KWidgetTypeList ) == 0 )
            type = EListWidget;
        else if ( widgetType.Compare( KWidgetTypeGrid ) == 0 )
            type = EGridWidget;
        else if ( widgetType.Compare( KWidgetTypeCoverFlow ) == 0 )
            type = ECoverFlowWidget;
        }

    CHnSuiteModel* model  = iSuiteContainer->GetLastSuiteModel();
    return HandleWidgetChangeL( model->SuiteName(), type );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleWidgetChangeL( const TDesC& aSuiteName,
        THnSuiteWidgetType aType )
    {
    DEBUG(("_MM_:CMatrixMenuAppUi::HandleWidgetChangeL IN"));
    TInt ret( KErrGeneral );

    CHnRepositoryManager* rep = CHnRepositoryManager::NewLC();
    THnSuiteWidgetType type( EUnspecified );
    TInt err( rep->ReadSuiteWidgetTypeL( aSuiteName, type ) );

    if( ( !err || err == KErrNotFound ) && type != EUnspecified )
        {
        ret = rep->StoreSuiteWidgetTypeL( aSuiteName, aType );
        }
    CleanupStack::PopAndDestroy( rep );
    DEBUG(("_MM_:CMatrixMenuAppUi::HandleWidgetChangeL OUT"));
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleReevaluateEventL(
        CLiwGenericParamList& aParams )
    {
    TInt ret(KErrNone);
    // Paths to retrieve filter values
    _LIT8( KGenreGlobalPath, "filter:genre");
    _LIT8( KIdGlobalPath, "filter:id");
    _LIT8( KGenrePath, "filter:[%d]/genre");
    _LIT8( KItemPath, "filter:[%d]/item");

    CHnFilter* filter = CHnFilter::NewLC();

    TLiwGenericParam param;
    param.PushL();
    aParams.AtL(0, param);
    ASSERT( !param.Name().Compare( KFilterElementItem8 ) );

    TLiwVariant suiteIdVariant;
    suiteIdVariant.PushL();
    TLiwVariant genreVariant;
    genreVariant.PushL();
    TInt idFound =
        HnLiwUtils::GetVariantL( aParams, KIdGlobalPath(), suiteIdVariant );
    TInt found =
        HnLiwUtils::GetVariantL(aParams, KGenreGlobalPath(), genreVariant );
    if (idFound != KErrNotFound)
        {
        filter->SetSuiteId( suiteIdVariant.AsTInt32() );
        filter->SetEvaluateSuiteL( ETrue );
        }
    else if (found != KErrNotFound && found != KErrBadDescriptor)
        {
        filter->SetSuiteNameL( genreVariant.AsDes() );
        filter->SetEvaluateSuiteL( ETrue );
        }
    else
        {
        filter->SetEvaluateSuiteL( EFalse );
        // For each entry in the fitler list....
        for ( int i=0; i< param.Value().AsList()->Count(); i++ )
            {
            RBuf8 path;
            CleanupClosePushL( path );
            path.CreateL( KGenrePath().Length() );
            path.Format( KGenrePath, i );
            TLiwVariant genreVariant;
            HnLiwUtils::GetVariantL(aParams, path, genreVariant );
            path.Close();
            path.CreateL( KItemPath().Length() );
            path.Format( KItemPath, i );
            TLiwVariant itemVariant;
            HnLiwUtils::GetVariantL(aParams, path, itemVariant );
            CleanupStack::PopAndDestroy( &path );

            TPtrC suiteName;
            TPtrC itemId;
            genreVariant.Get(suiteName);
            itemVariant.Get(itemId);

            CHnItemId* itemIdObject = CHnItemId::NewLC(suiteName, itemId);
            filter->AppendItemIdL( itemIdObject );
            CleanupStack::Pop( itemIdObject );

            genreVariant.Reset();
            itemVariant.Reset();
            }
        }
    CleanupStack::PopAndDestroy( &genreVariant );
    CleanupStack::PopAndDestroy( &suiteIdVariant );
    CleanupStack::PopAndDestroy( &param );
    iMetaDataModel->EvaluateL( *filter );
    CleanupStack::PopAndDestroy( filter );
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleBackEventL( const TDesC& aGenre, TInt aIterations )
    {
    return iMetaDataModel->HandleBackEventL(
            iSuiteContainer, aGenre, aIterations );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleNewSuiteLoadedEventL(
        CLiwGenericParamList& aParams )
    {
    DEBUG(("_MM_:CHnEngine::HandleNewSuiteLoadedEventL IN"));
    TInt err( KErrNone );
    const TLiwGenericParam* param =
        aParams.FindFirst( err, KNewSuiteParamNameEn );
    if( err != KErrNotFound )
        {
        TPtrC genre;
        param->Value().Get( genre );
        DEBUG16(("_MM_:\tSuite genre name: %S",&genre));
        // load and evaluate the suite
        if( genre.Compare( KFolderSuite) )
            {
            //only folder suite is supported
            User::Leave( KErrNotSupported );
            }
        err = iMetaDataModel->LoadSuiteL( genre, &aParams );
        if ( !err )
            {
            GetLastSuiteModelL()->RegisterSuiteObserverL( &iControllerInterface, EPriorityNull );
            CHnFilter* filter = CHnFilter::NewLC();
            filter->SetSuiteNameL( genre );
            filter->SetEvaluateSuiteL( ETrue );
            if ( iEditMode )
                {
                iMetaDataModel->SetModeL( EMdModeEdit );
                }
            else
                {
                iMetaDataModel->SetModeL( EMdModeNormal );
                }

            TRAP( err, iMetaDataModel->EvaluateL( *filter ) );

            CleanupStack::PopAndDestroy( filter );
            if ( err )
                {
                iMetaDataModel->DeleteLastSuite();
                iSuiteContainer->PopSuiteModelL( genre );
                }
            }
        else
            {
            // show error note to User
            HBufC* message( NULL );
            switch ( err )
                {
                case KErrPathNotFound :
                    {
                    message = KDebugNoteDirNotFound().AllocL();
                    }
                    break;
                case KErrNotFound :
                    {
                    message = KDebugNoteFileNotFound().AllocL();
                    }
                    break;
                default :
                    {
                    message = HBufC::NewL( KDebugNoteOtherError().Length() +
                                            KMaxLength );
                    message->Des().AppendFormat( KDebugNoteOtherError, err );
                    }
                    break;
                }
            iEEnv->AlertWin( *message );
            delete message;
            }
        }
    DEBUG(("_MM_:\tReturned error code: %d",err));
    DEBUG(("_MM_:CHnEngine::HandleNewSuiteLoadedEventL OUT"));

    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleSetFocusEventL( const CLiwGenericParamList& aParams  )
    {
    // Triggering event in hierarchy navigator, specifying
    // that suite given by the name genre has been
    DEBUG(("_MM_:CHnEngine::HandleSetFocusEventL IN"));

    TInt posSuite( 0 );
    TInt posItem( 0 );
	TInt64 suiteCustomId( KErrNotFound );
	TInt64 itemCustomId( KErrNotFound );

	// Get suite's and item's custom ids.
	const TLiwGenericParam* paramSuiteId = aParams.FindFirst( posSuite,  KSuiteCustomId8 );
	const TLiwGenericParam* paramItemId = aParams.FindFirst( posItem,  KItemCustomId8 );


	if ( posSuite >= 0 && posItem >= 0 )
    	{
    	suiteCustomId = paramSuiteId->Value().AsTInt64();
    	itemCustomId = paramItemId->Value().AsTInt64();

    	// Get matching suite.
    	CHnSuiteModel* suiteModel = iSuiteContainer->GetMatchingSuiteModel( suiteCustomId );

    	if ( suiteModel )
    		{
    		// If suite is not null, then find matching item model.
    		TInt index( KErrNotFound );
    		CHnItemModel* itemModel = suiteModel->GetMatchingItemModelL( itemCustomId, index );

    		if ( itemModel )
    			{
    			// If itemModel is not null then set highlight and set highligh
    			// on matching item.
    			suiteModel->SetSuiteHighlightL( index );
        		iControllerInterface.HandleSuiteEventL( ESuiteHighlightChanged, suiteModel );
    			}
    		else
    			{
    			suiteModel->QueueFocus( itemCustomId );
    			}
    		}
    	}

    DEBUG(("_MM_:CHnEngine::HandleSetFocusEventL OUT"));
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleMoveFocusBeforeDeleteEventL(
        const CLiwGenericParamList& /* aParams */  )
    {
    // Triggering event in hierarchy navigator, specifying
    // that suite given by the name genre has been
    DEBUG(("_MM_:CHnEngine::HandleMoveFocusBeforeDeleteEventL IN"));

    TInt currentHighlight( GetLastSuiteModelL()->GetSuiteHighlight() );
    TInt itemsCount( GetLastSuiteModelL()->GetItemModelsCount() );

    TInt shift( 0 );
    if( AknLayoutUtils::PenEnabled() && iEditMode )
        {
        // add empty item
        shift = 1;
        }
    if( currentHighlight == itemsCount - shift - 1 )
        {
        GetLastSuiteModelL()->SetSuiteHighlightL( currentHighlight - 1 );
        }
    else
        {
        GetLastSuiteModelL()->SetSuiteHighlightL( currentHighlight + 1 );
        }
    iControllerInterface.HandleSuiteEventL(
            ESuiteHighlightChanged, GetLastSuiteModelL() );

    DEBUG(("_MM_:CHnEngine::HandleMoveFocusBeforeDeleteEventL OUT"));
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleRefreshIconsEventL( const CLiwGenericParamList& /* aParams */ )
    {
    iMetaDataModel->EvaluateL();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnEngine::HandleDisableActionsForItemL( const CLiwGenericParamList& aParams )
    {
    TInt pos( 0 );
    aParams.FindFirst( pos, KItemCustomId8 );
    if ( pos != KErrNotFound )
        {
        TInt itemCustomId = aParams[pos].Value().AsTInt32();
        TInt itemId = 0;
        CHnItemModel* itemModel =
                iSuiteContainer->GetLastSuiteModel()->GetMatchingItemModelL(
                        itemCustomId, itemId );
        if ( itemModel )
            {
            itemModel->DeleteAllActionModels();
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
//void CHnEngine::InstallChangeL( TInt aStatus )
//    {
//    switch ( aStatus)
//        {
//        case EInstOpNone:
//            {
//            break;
//            }
//        case EInstOpInstall:
//        case EInstOpUninstall:
//            {
//            iMetaDataModel->ReleaseLocalization();
//            break;
//            }
//        }
//    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnEngine::InitializeL( CLiwGenericParamList& aParam )
    {
    DEBUG(("_MM_:CMatrixMenuAppUi::InitializeL IN"));
    User::LeaveIfError( HandleNewSuiteLoadedEventL( aParam ) );
    DEBUG(("_MM_:CMatrixMenuAppUi::InitializeL OUT"));
    // This is an interface method so some value has to be returned.
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnEngine::ParseSuiteUriPathL( const TDesC8& aUriPath,
        CDesC8ArrayFlat& aOutput )
    {
    DEBUG(("_MM_:CHnEngine::ParseSuiteUriPathL IN"));
    DEBUG(("_MM_:\tURI path: %S",&aUriPath));
    if ( aUriPath.Length() )
        {
        // copy the path so we can freely trim already parsed chunks
        RBuf8 buf( aUriPath.AllocL() );
        CleanupClosePushL( buf );

        // get rid of the initial slash
        if ( buf[0] == KSlash8()[0] )
            {
            buf.Delete( 0, 1 );
            DEBUG(("_MM_:\tDeleted leading slash"));
            }

        // get the first chunk
        TInt slashPos = buf.Find( KSlash8 );
        TPtrC8 chunkPtr;
        if ( slashPos != KErrNotFound )
            {
            chunkPtr.Set( buf.Left( slashPos ) );
            }
        else
            {
            // no slash - one chunk path
            chunkPtr.Set( buf );
            }

        while ( chunkPtr.Length() )
            {
            aOutput.AppendL( chunkPtr );
            DEBUG8(( "_MM_:\t\tParsed segment: %S", &chunkPtr ));

            // get rid of the parsed chunk and get a new one
            if ( slashPos != KErrNotFound )
                {
                // also remove the trailing slash
                buf.Delete( 0, chunkPtr.Length() + 1 );
                slashPos = buf.Find( KSlash8 );
                if ( slashPos != KErrNotFound )
                    {
                    chunkPtr.Set( buf.Left( slashPos ) );
                    }
                else
                    {
                    // last chunk
                    chunkPtr.Set( buf );
                    }
                }
            else
                {
                // no slashes found, this was the last one
                break;
                }
            }

        CleanupStack::PopAndDestroy( &buf );
        }
    DEBUG(("_MM_:CHnEngine::ParseSuiteUriPathL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnEngine::ResetModelsL()
    {
    MMPERF(("CHnEngine::ResetModelsL - START"));

    iControllerInterface.NotifyUiRefreshL( ERemoveLiwObjects );

    if( iSuiteContainer )
        {
        iSuiteContainer->RemoveLiwObjects();
        }
    delete iMetaDataModel;
    iMetaDataModel = NULL;
    delete iSuiteContainer;
    iSuiteContainer = NULL;

    iSuiteContainer = CHnSuiteModelContainer::NewL( *this , iControllerInterface );
    MMPERF(("CHnEngine::ResetModelsL - suite container ready"));

    iMetaDataModel = CHnMdModel::NewL( this, iSuiteContainer );

    MMPERF(("CHnEngine::ResetModelsL - DONE"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CLiwGenericParamList* CHnEngine::UriQueryToLiwListLC(
        const TDesC8& aUriQuery, const TDesC8& aUriFragment, const TDesC8& aSuiteName )
    {
    CLiwGenericParamList* list = CLiwGenericParamList::NewLC();

    CLiwDefaultMap* map = CLiwDefaultMap::NewL();
    map->PushL();

    RPointerArray< HBufC8 > params = SplitL( aUriQuery, KAnd8 );

    for( TInt i = 0; i < params.Count(); i++ )
        {
        RPointerArray< HBufC8 > paramValue = SplitL( *params[ i ], KEquals8 );
        CleanupResetAndDestroyPushL( paramValue );
        ASSERT( paramValue.Count() == 2 );
        map->InsertL( *paramValue[ 0 ], TLiwVariant( *paramValue[ 1 ] ) );
        CleanupStack::PopAndDestroy( &paramValue );
        }

    if ( aUriFragment.Length() > 0 && aSuiteName.Length() > 0 )
        {
        map->InsertL( KUriHighlight, TLiwVariant( aUriFragment ) );
        map->InsertL( KUriHighlightSuite, TLiwVariant( aSuiteName ) );
        }

    list->AppendL( TLiwGenericParam(  KParams8, TLiwVariant( map )));

    params.ResetAndDestroy();
    CleanupStack::PopAndDestroy( map );

    return list;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
RPointerArray< HBufC8 > CHnEngine::SplitL( const TDesC8& aSource,
        const TDesC8& aSeparator )
    {
    RPointerArray< HBufC8 > tokens;
    CleanupResetAndDestroyPushL( tokens );

    if ( aSource.Length() > 0 && aSeparator.Length() > 0 )
        {
        TPtrC8 tmpSource = aSource;

        TPtrC8 token( KNullDesC8 );

        while( ETrue )
            {
            TInt length = tmpSource.Find( aSeparator );

            if ( length >= 0 )
                {
                token.Set( tmpSource.Mid( 0, length ) );
                tokens.AppendL( token.AllocL() );
                tmpSource.Set( tmpSource.Mid( length + 1 ) );
                }
            else
                {
                token.Set( tmpSource );
                tokens.AppendL( token.AllocL() );
                break;
                }
            }
        }
    CleanupStack::Pop( &tokens );
    return tokens;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnEngine::SetEditModeL( TBool aEditMode )
    {
    iEditMode = aEditMode;
    if ( aEditMode )
        {
        iMetaDataModel->SetModeL( EMdModeEdit );
        }
    else
        {
        iMetaDataModel->SetModeL( EMdModeNormal );
        }
    //The suite on top of the stack is probably displayed.
    //Switching to edit mode has some dramatic ui impact,
    //thus when parts of the suite will get displayed
    //it may look awkward thus the suite on top of the stack
    //is marked as uninitialized, therefore ui will be able to
    //react only to the ready suite model.
    if (AknLayoutUtils::PenEnabled() || (!AknLayoutUtils::PenEnabled() && iEditMode))
        {
        GetLastSuiteModelL()->GetItemsOrder()->MarkSuiteUninitialized();
        iMetaDataModel->EvaluateL();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnEngine::IsOnlyRootConfLoaded()
    {
    return ( iSuiteContainer &&
            iSuiteContainer->GetSuiteModelCount() == KModelCountForRoot );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//

EXPORT_C void CHnEngine::HandleRequestL( const CLiwGenericParamList& aParam,
                                CLiwGenericParamList* /*aOutput*/ )
    {
    TInt pos( 0 );
    aParam.FindFirst( pos, KHnRequest );
    if (pos != KErrNotFound)
        {
        TPtrC8 command = aParam[pos].Value().AsData();
        if( command == KSetFocus )
            {
            HandleSetFocusEventL( aParam );
            }
        else if( command == KMoveFocusBeforeDelete )
            {
            HandleMoveFocusBeforeDeleteEventL( aParam );
            }
        else if( command == KRefreshIcons )
            {
            HandleRefreshIconsEventL( aParam );
            }
        else if (command == KEvaluateMdModel)
            {
            GetLastSuiteModelL()->GetItemsOrder()->MarkSuiteUninitialized();
            iMetaDataModel->EvaluateL();
            }
        else if ( command == KDisableActionsForItem )
            {
            HandleDisableActionsForItemL( aParam );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnEngine::LoadFromCrL( const TDesC8& aUri )
	{
	HBufC8* uriBuf = HBufC8::NewLC( aUri.Length() );
	uriBuf->Des().Copy( aUri );

	TUriParser8 parser;
	parser.Parse( *uriBuf );
	User::LeaveIfError( parser.IsSchemeValid() ? KErrNone : KErrPathNotFound );

	const TDesC8& query = parser.Extract( EUriQuery );
    RPointerArray< HBufC8 > params = SplitL( query, HnLogicalRelations::KLogicalAnd8);

    TBuf8<KApaMaxAppGroupName + KTimeStampBufferLength + KUidStringLength + 2> item;

    for( TInt i = 0; i < params.Count(); i++ )
        {
        RPointerArray< HBufC8 > paramValue = SplitL( *params[ i ], HnLogicalRelations::KLogicalEqual8 );
        CleanupResetAndDestroyPushL( paramValue );
        ASSERT( paramValue.Count() == 2 );

        if ( !paramValue[0]->Compare(KMcsAppGroupName8) )
        	{
        	TBuf8<KApaMaxAppGroupName> appgrname;
        	if( paramValue[1]->Length()<=KApaMaxAppGroupName )
        	    {
        	    appgrname.Append( *paramValue[1] );
        	    }

        	//get current time
        	TTime currentTime;
        	currentTime.HomeTime();
        	TDateTime date(currentTime.DateTime());
        	item.Format(KCRepTimeFormat, &appgrname , date.Year(), date.Month(),
        		date.Day(), date.Hour(), date.Minute(), date.Second(), date.MicroSecond());
        	}
        else if ( !paramValue[0]->Compare( KKeyTypeUid ) )
        	{
        	item.Append( KComma8 );
            if( paramValue[1]->Length()<=KUidStringLength )
                {
                item.Append( *paramValue[1] );
                }
        	}
        CleanupStack::PopAndDestroy( &paramValue );
        }

    TBuf<KApaMaxAppGroupName + KTimeStampBufferLength + KUidStringLength + 2> item1;
    item1.Copy(item);
	CRepository *cenRep = CRepository::NewLC( KCRUidMenu );
	cenRep->Set(KMenuShowFolder, item1);
	CleanupStack::PopAndDestroy( cenRep );

    params.ResetAndDestroy();
    CleanupStack::PopAndDestroy( uriBuf );

	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnEngine::LightStatusChanged(TInt aTarget,
        CHWRMLight::TLightStatus aStatus)
    {
        DEBUG(("_Mm_:CHnEngine::LightStatusChanged: target %d, status %d - IN", aTarget, aStatus));
    if (CHWRMLight::EPrimaryDisplay == aTarget
            || CHWRMLight::EPrimaryDisplayAndKeyboard == aTarget)
        {
        switch (aStatus)
            {
            case CHWRMLight::ELightOn:
                {
                TRAP_IGNORE( iControllerInterface.NotifyUiRefreshL(ELightOn) );
                break;
                }
            case CHWRMLight::ELightOff:
                {
                TRAP_IGNORE( iControllerInterface.NotifyUiRefreshL(ELightOff) );
                break;
                }
            }
        }
        DEBUG(("_Mm_:CMmAppUi::LightStatusChanged - OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnEngine::HandleActionL( const TDesC8& aUri )
	{
	TBool exitActionConsumed(EFalse);

	HBufC8* uriBuf = HBufC8::NewLC( aUri.Length() );
	uriBuf->Des().Copy( aUri );

	TUriParser8 parser;
    parser.Parse( *uriBuf );
    User::LeaveIfError( parser.IsSchemeValid() ? KErrNone : KErrPathNotFound );

    const TDesC8& host8 = parser.Extract( EUriHost );
    const TDesC8& query = parser.Extract( EUriQuery );

	RBuf action;
    CleanupClosePushL( action );
    CLiwGenericParamList* paramsUri = UriQueryToLiwListLC( query, KNullDesC8, KNullDesC8 );
    HnLiwUtils::GetStringL( *paramsUri, KActionParams, action );

    if ( !action.Compare( KActionExit ) )
    	{
    	RBuf host;
		host.CreateL( host8.Length() );
		CleanupClosePushL( host );
		host.Copy(host8);
    	CHnSuiteModel* suiteModel = iSuiteContainer->GetSuiteModel( host );
    	TBool exitHideHostNotFound( ETrue  );
    	if( suiteModel  && suiteModel->ExitMode() == EExitModeHide )
    	    {
    	    exitHideHostNotFound = EFalse;
    	    }

		if ( exitHideHostNotFound )
			{
			//send to foreground
			CLiwGenericParamList* pl = CLiwGenericParamList::NewL();
			CleanupStack::PushL( pl );
			HandleModelEventL( KAppGainForeground, *pl );
			CleanupStack::PopAndDestroy( pl );
			}
		else
			{
			//send to background
			CLiwGenericParamList* pl = CLiwGenericParamList::NewL();
			CleanupStack::PushL( pl );
			HandleModelEventL( KAppGainBackground, *pl );
			CleanupStack::PopAndDestroy( pl );
			}

    	if ( exitHideHostNotFound && iSuiteContainer->GetLastSuiteModel() == suiteModel  )
    		{
            HandleBackEventL( host,  1 );
    		}
    	else
    		{
        	//reset to root
            CLiwGenericParamList* params = CLiwGenericParamList::NewLC();
            params->AppendL( iMetaDataModel->GetSuiteParameters( 0 ) );
        	ResetModelsL();
            InitializeL( *params );
            CleanupStack::PopAndDestroy( params );
    		}

    	CleanupStack::PopAndDestroy( &host );
    	exitActionConsumed = ETrue;
    	}

    CleanupStack::PopAndDestroy( paramsUri );
    CleanupStack::PopAndDestroy( &action );
    CleanupStack::PopAndDestroy( uriBuf );
    return exitActionConsumed;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnEngine::LoadSuitesL( const TDesC8& aUri )
	{
	TBool sendToForeground(EFalse);
	HBufC8* uriBuf = HBufC8::NewLC( aUri.Length() );
	uriBuf->Des().Copy( aUri );

	TUriParser8 parser;
	parser.Parse( *uriBuf );
	User::LeaveIfError( parser.IsSchemeValid() ? KErrNone : KErrPathNotFound );

	CDesC8ArrayFlat* suiteNameArray = new( ELeave ) CDesC8ArrayFlat( KDefaultGranularity ) ;
	CleanupStack::PushL( suiteNameArray );

	const TDesC8& host = parser.Extract( EUriHost );
    RBuf8 host8;
    CleanupClosePushL( host8 );
    if (host.Compare( KNullDesC8 ))
        {
        host8.CreateL( host );
        host8.LowerCase();
        suiteNameArray->AppendL( host8 );
        DEBUG8(( "_MM_:\tURI host part: %S", &host8 ));
        }

	const TDesC8& path = parser.Extract( EUriPath );
	if (path.Compare( KNullDesC8 ))
		{
		HBufC8* path8 = HBufC8::NewLC( path.Length() );
		path8->Des().Copy( path );
		path8->Des().LowerCase();
		ParseSuiteUriPathL( *path8, *suiteNameArray );
		CleanupStack::PopAndDestroy( path8 );
		}

	TBool suiteExists( EFalse );
	iMetaDataModel->IgnoreEvaluations( ETrue );
	iMetaDataModel->QueueForeground( CHnMdModel::EWhenAnySuiteIsEvaluated );

    if( host8.Length() == 0 || ( host8.Length() > 0 &&
            !iMetaDataModel->SuiteModelExistsL( host8 ) ) )
        {
        suiteNameArray->Reset();
        suiteNameArray->AppendL( KRoot8 );
        }

	for( TInt i = 0; i < suiteNameArray->Count(); ++i )
		{
		suiteExists = iMetaDataModel->SuiteModelExistsL(
			( *suiteNameArray )[i] );
		TBool nextExists = i < suiteNameArray->Count() - 1 &&
			iMetaDataModel->SuiteModelExistsL( ( *suiteNameArray )[i + 1] );
		TBool isLast = ( i == ( suiteNameArray->Count() - 1 ) );

		TBool turnOnEvaluation = ( suiteExists && ( !nextExists || isLast ) );
		if( turnOnEvaluation || !suiteExists )
			{
			iMetaDataModel->IgnoreEvaluations( EFalse );
			}

		TInt err( KErrNone );

		// Ignore loading new suite if the last suite in model is the same
		// as first one in the uri.
		if ( i == 0 )
			{
			if ( SuitesAreTheSameL( iMetaDataModel->GetLastSuite()->SuiteName(), (*suiteNameArray)[i] ) )
				{
				HandleTheSameSuitesL( nextExists, parser.Extract( EUriQuery ) );
				sendToForeground = ETrue;
				continue;
				}
			}

		if( suiteExists )
			{
			if (!nextExists)
			    {
			    // Tricky: queue foreground so that if the suite is evaluated synchronously
			    // the foreground will be gained.
			    iMetaDataModel->QueueForeground( CHnMdModel::EWhenAnySuiteIsEvaluated );
			    }
			err = LoadSuiteFromUriL(
				( *suiteNameArray )[i], parser.Extract( EUriQuery ), parser.Extract( EUriFragment ) );
			if ( KErrNone == err && !nextExists && iMetaDataModel->IsForegroundQueued() )
			    {
			    // Tricky: if foreground is still queued it means that the suite is evaluated
			    // asynchronously. Override the previos foreground queue setting with one
			    // that will be effective only if the last loaded suite is evaluated.
			    iMetaDataModel->QueueForeground( CHnMdModel::EWhenCurrentTopSuiteIsEvaluated );
			    }
			if ( err != KErrNone )
				{
				sendToForeground = ETrue;
				}
			}

		if( !suiteExists || err != KErrNone || !nextExists )
			{
			CHnFilter* filter = CHnFilter::NewLC();

			filter->SetSuiteId(
				iSuiteContainer->GetLastSuiteModel()->CustomId() );

			filter->SetEvaluateSuiteL( ETrue );
			iMetaDataModel->IgnoreEvaluations( EFalse );

			TRAPD( err, iMetaDataModel->EvaluateL( *filter ) );

			CleanupStack::PopAndDestroy( filter );
			if( err )
				{
				iMetaDataModel->DeleteLastSuite();
				iSuiteContainer->PopSuiteModelL(
					 HnConvUtils::Str8ToStr( ( *suiteNameArray )[i] )->Des() );
				sendToForeground = ETrue;
				}
			break;
			}
		}

	// set highlight only when path is valid!
	if( suiteExists )
		{
		HighlightPlacementFromUriL( parser.Extract( EUriFragment ) );
		}

    CleanupStack::PopAndDestroy( &host8 );
	CleanupStack::PopAndDestroy( suiteNameArray );
	CleanupStack::PopAndDestroy( uriBuf );

//    send to foreground
	if ( sendToForeground )
		{
		CLiwGenericParamList* pl = CLiwGenericParamList::NewL();
		CleanupStack::PushL( pl );
		HandleModelEventL( KAppGainForeground, *pl );
		CleanupStack::PopAndDestroy( pl );
		iMetaDataModel->QueueForeground( CHnMdModel::ENever );
		}

	}

// End of file
