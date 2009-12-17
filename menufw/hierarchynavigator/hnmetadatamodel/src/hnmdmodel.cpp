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

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <AvkonInternalCRKeys.h>

#include "hnmdmodel.h"
#include "hnmdsuite.h"
#include "hnrepositoryobserver.h"
#include "hnfilter.h"
#include "hnglobals.h"
#include "hneventhandler.h"
#include "hnmdlocalization.h"
#include "hnxmlmodelprovider.h"
#include "hnsuitemodelcontainer.h"
#include "hnitemsorder.h"
#include "hninterface.h"
#include "menudebug.h"
#include "hnconvutils.h"
#include "hnbitmapidcache.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdModel::ConstructL( MHnMdModelEventObserver *aModelObserver,
        CHnSuiteModelContainer * aSuiteModelContainer )
    {
    MMPERF(("CHnMetaDataModel::ConstructL - START"));
    MMPERF(("CHnMetaDataModel::ConstructL - query handler ready"));
    iLocalization = CHnMdLocalization::NewL();
    MMPERF(("CHnMetaDataModel::ConstructL - localization ready"));
    iXmlModelProvider = CHnXmlModelProvider::NewL();
    MMPERF(("CHnMetaDataModel::ConstructL - bitmap and mask ids cache ready"));
    iBitmapIdCache = CHnBitmapIdCache::NewL();
    MMPERF(("CHnMetaDataModel::ConstructL - model privider ready"));
    iXmlModelProvider->ReloadModelL();
    MMPERF(("CHnMetaDataModel::ConstructL - model reloaded"));
            
    iCmnPtrs.iLocalization = iLocalization;
    iCmnPtrs.iIdGenerator = &iIdGenerator;
    iCmnPtrs.iModel = this;
    iCmnPtrs.iXmlModelProvider = iXmlModelProvider;
    iCmnPtrs.iContainer = aSuiteModelContainer;
    iCmnPtrs.iModelEventObserver = aModelObserver;
    iCmnPtrs.iBitmapIdCache = iBitmapIdCache;
    
    THnMdCommonPointers::SetStatic(&iCmnPtrs);
    
    iMode = EMdModeNormal;
    iRepositoryWidgetTypeObserver = CHnRepositoryWidgetTypeObserver::NewL( &iCmnPtrs, KMatrixRepositoryUid );
    iRepositoryShowFolderObserver = CHnRepositoryShowFolderObserver::NewL( &iCmnPtrs, KCRUidMenu, KMenuShowFolder );
    iRepositoryZoomObserver = CHnRepositoryZoomObserver::NewL( &iCmnPtrs, KCRUidAvkon, KAknGlobalUiZoom );
    MMPERF(("CHnMetaDataModel::ConstructL - rep. observer ready"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMdModel* CHnMdModel::NewL( 
        MHnMdModelEventObserver* aModelObserver,
        CHnSuiteModelContainer * aSuiteModelContainer  )
    {
    CHnMdModel* self = CHnMdModel::NewLC( aModelObserver,
            aSuiteModelContainer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMdModel* CHnMdModel::NewLC( 
        MHnMdModelEventObserver* aModelObserver,
        CHnSuiteModelContainer * aSuiteModelContainer )
    {
    CHnMdModel* self = new( ELeave ) CHnMdModel();
    CleanupStack::PushL( self );
    self->ConstructL( aModelObserver, aSuiteModelContainer );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdModel::CHnMdModel()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdModel::~CHnMdModel()
    {
    delete iRepositoryWidgetTypeObserver;
    delete iRepositoryShowFolderObserver;
    delete iRepositoryZoomObserver;
    iLoadedSuites.ResetAndDestroy();
    delete iXmlModelProvider;
    delete iLocalization;
    delete iBitmapIdCache;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnMdModel::HandleBackEventL(
        CHnSuiteModelContainer* aMulContainer,
        const TDesC& aSuiteName,
        TInt aIterations )
    {
    TInt ret( KErrNotFound );
#ifdef _DEBUG
    // check if genres match
    CHnMdSuite* lastMdModel = GetLastSuite();
    ASSERT( !lastMdModel->SuiteName().Compare( aSuiteName ) );
#endif
    
    TInt countDown( aIterations );
    CHnSuiteModel* current = NULL;
    while( ( current = aMulContainer->GetParentSuiteModel() ) != NULL &&
            countDown > 0 )
        {
        current = aMulContainer->GetLastSuiteModel();
        // pop the suite model
        aMulContainer->PopSuiteModelL( current->SuiteName() );
        
        // remove the suite that we are leaving
        DeleteLastSuite();
        countDown--;
        }
    
    current = aMulContainer->GetLastSuiteModel();
    current->GetItemsOrder()->MarkSuiteUninitialized();
    CHnFilter* filter = CHnFilter::NewLC();
    filter->SetSuiteNameL( current->SuiteName() );
    filter->SetSuiteId( current->CustomId() );
    GetLastSuite()->MarkEvaluationL( *filter, *current);
    CleanupStack::PopAndDestroy( filter );
    //Evaluate suite at the top (if needed)
    EvaluateTopSuiteL();
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::HandleSisInstallationEventL(
        CHnSuiteModelContainer* /*aModelContainer*/ )
    {
    iXmlModelProvider->ReloadModelL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdModel::ReloadStackSuitesL( CHnSuiteModelContainer* aModelContainer )
	{
    RPointerArray< CLiwGenericParamList > paramsArray;
    CleanupResetAndDestroyPushL( paramsArray );

    TBool rootDisplayed( iLoadedSuites.Count() == 1 );
    while( iLoadedSuites.Count() > 0 )
        {
        // break loop to leave root suite on the stack
        if( ( iLoadedSuites.Count() == 1 && paramsArray.Count() > 0 )
                || rootDisplayed )
            {
            break;
            }
        
        CHnMdSuite* suite = GetLastSuite();
        if( iXmlModelProvider->SuiteExistsL( suite->SuiteName() ) )
            {
            CLiwGenericParamList* suiteParams = CLiwGenericParamList::NewL();
            CleanupStack::PushL( suiteParams );
            suiteParams->AppendL( suite->GetSuiteParameters() );
            paramsArray.Append( suiteParams );
            CleanupStack::Pop( suiteParams );
            }
        aModelContainer->PopSuiteModelL( suite->SuiteName() );
        DeleteLastSuite();
        }
    
    TInt err( KErrNone );
    CHnFilter* filter = CHnFilter::NewLC();
    filter->SetEvaluateSuiteL( ETrue );

    SetModeL( iMode );
    
    for( TInt i( paramsArray.Count() - 1 ); i >= 0 && !err; i-- )
        {
        CLiwGenericParamList* suiteParams = CLiwGenericParamList::NewL();
        CleanupStack::PushL(suiteParams);
        suiteParams->AppendL( *(paramsArray[ i ]));
        TInt pos( 0 );
        const TLiwGenericParam* param = suiteParams->FindFirst( pos, KSuiteName8 );
        if ( param && pos >= 0)
            {
            TPtrC suiteName;
            param->Value().Get( suiteName );
            err = LoadSuiteL( suiteName, suiteParams );
            }
        CleanupStack::PopAndDestroy( suiteParams );

        GetLastSuite()->SetModeL( iMode );
        GetLastSuite()->MarkEvaluationL(
                *filter, *iCmnPtrs.iContainer->GetLastSuiteModel() );
        GetLastSuite()->EvaluateL( *iCmnPtrs.iContainer->GetLastSuiteModel() );
        }
    
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( &paramsArray );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::DeleteLastSuite()
    {
    CHnMdSuite* tmpPtr = GetLastSuite();
    iLoadedSuites.Remove( iLoadedSuites.Count() - 1 );
    delete tmpPtr;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::ReleaseLocalization()
    {
    iLocalization->ReleaseResourceFiles();
    iXmlModelProvider->ResetCache();
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::ReloadLocalizationL()
    {
    iLocalization->ReloadResourceFilesL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::EvaluateL()
    {
    CHnFilter* empty = CHnFilter::NewLC();
    EvaluateL( *empty );
    CleanupStack::PopAndDestroy( empty );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::SetModeL( TMdMode aMode )
    {
    iMode = aMode;
    for ( TInt i( 0 ); i < iLoadedSuites.Count(); i++ )
        {
        CHnMdSuite* suite = iLoadedSuites[i];
        ASSERT( suite != NULL );
        suite->SetModeL( aMode );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMdSuite* CHnMdModel::GetLastSuite()
    {
    return (iLoadedSuites.Count()>0) 
            ? iLoadedSuites[ iLoadedSuites.Count()-1 ]
            : NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::IgnoreEvaluations( TBool aIgnore )
    {
    iIgnoreEvaluations = aIgnore;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::QueueForeground(
        TForegroundGainTriggeringEvent aWhen )
    {
    switch ( aWhen )
        {
        case EWhenAnySuiteIsEvaluated:
            iForegroundQueued = ETrue;
            iForegroundTriggeringSuite = NULL;
            break;
        case EWhenCurrentTopSuiteIsEvaluated:
            iForegroundQueued = ETrue;
            iForegroundTriggeringSuite = iCmnPtrs.iContainer->GetLastSuiteModel();
            break;
        case ENever:
            iForegroundQueued = EFalse;
            iForegroundTriggeringSuite = NULL;
            break;
        default:
            ASSERT( 0 );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnMdModel::IsForegroundQueued() const
	{
	return iForegroundQueued;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdModel::SuiteModelReadyToShowL( CHnSuiteModel* aJustEvaluatedSuite )
    {
    if ( !aJustEvaluatedSuite )
        {
        User::Leave( KErrArgument );
        }
    
    if ( IsForegroundQueued() && ( !iForegroundTriggeringSuite ||
            iForegroundTriggeringSuite == aJustEvaluatedSuite ) )
    	{
    	CLiwGenericParamList* pl = CLiwGenericParamList::NewL();
    	CleanupStack::PushL( pl );
    	iCmnPtrs.iModelEventObserver->HandleModelEventL( KAppGainForeground, *pl );
    	CleanupStack::PopAndDestroy( pl );
    	QueueForeground( ENever );
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdSuite* CHnMdModel::GetSuite( TInt aPosition )
    {
    return ( (aPosition >= 0) && (aPosition < iLoadedSuites.Count()) ) 
            ? iLoadedSuites[ aPosition ]
            : NULL;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CLiwGenericParamList& CHnMdModel::GetSuiteParameters( TInt aPosition )
    {
    return GetSuite( aPosition )->GetSuiteParameters();
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::EvaluateL( CHnFilter& aFilter )
    {
    for ( TInt i = iLoadedSuites.Count() - 1; i >= 0; i-- )
        {
        iLoadedSuites[i]->MarkEvaluationL( aFilter, *iCmnPtrs.iContainer->GetSuiteModel(i) );
        if( i == iLoadedSuites.Count() - 1 && !iIgnoreEvaluations )
            {
            iLoadedSuites[i]->EvaluateL( *iCmnPtrs.iContainer->GetSuiteModel(i) );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnMdModel::LoadSuiteL( const TDesC& aGenre, 
        CLiwGenericParamList* aSuiteParams )
    {
    TInt err( KErrNone );
      
	RXmlEngDocument xmlDoc;
	// Xml model provider takes ownership of xmlDoc.
	TRAP( err, iXmlModelProvider->GetModelL( aGenre, xmlDoc ) );

	TXmlEngElement element;
    if ( !err )
        {
        element = xmlDoc.DocumentElement().AsElement();
        if ( element.Name() != KContentElementName8 )
            {
            err = KErrCorrupt;
            }
        }
    
    if ( !err )
        {
        CHnMdSuite* newSuite = CHnMdSuite::NewLC( element, &iCmnPtrs );
        
        if (aSuiteParams)
            {
            newSuite->SetSuiteParametersL( *aSuiteParams );
            }
        
        TInt pos( 0 );
        newSuite->GetSuiteParameters().FindFirst( pos, KSuiteName8);
        if ( pos == KErrNotFound )
        	{
        	newSuite->GetSuiteParameters().AppendL( 
        			TLiwGenericParam(KSuiteName8, TLiwVariant( aGenre ) ) );
        	}

        iCmnPtrs.iContainer->PushNewSuiteModelL( newSuite->SuiteName() );
        iCmnPtrs.iContainer->GetLastSuiteModel()->GetItemsOrder()->
            SetSuiteId( iCmnPtrs.iIdGenerator->GetNextId() );
        
        CleanupStack::Pop( newSuite );
        iLoadedSuites.AppendL( newSuite );
        }
    else
    	{
    	MMPERF(("CHnMdModel::LoadSuiteL - Error TRAPPED!"));
    	}
    
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnMdModel::SuiteModelExistsL( const TDesC8& aSuiteModel )
    {
    TBool res( EFalse );
    HBufC* suiteName = HnConvUtils::Str8ToStrLC( aSuiteModel );
    res = iXmlModelProvider->SuiteExistsL( *suiteName ) ||
            aSuiteModel.Compare( KRoot8 ) == KErrNone ;
    CleanupStack::PopAndDestroy( suiteName );
    return res;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdModel::GetCurrentUriL( TDes& aUri )
    {
    iXmlModelProvider->ReloadModelL();
    
    aUri.Append( KPrefMm );
    for( TInt i(0); i < iLoadedSuites.Count(); i++ )
        {
        aUri.Append( iLoadedSuites[ i ]->SuiteName() );
        aUri.Append( KSlash );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdModel::EvaluateTopSuiteL()
    {
    if ( !iIgnoreEvaluations )
        {
        TInt topItem = iLoadedSuites.Count() - 1;
        iLoadedSuites[topItem]->EvaluateL( *iCmnPtrs.iContainer->GetSuiteModel(topItem) );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnMdModel::LoadedSuitesCount()
	{
	return iLoadedSuites.Count();
	}


