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

#include "hnmdsuite.h"
#include "hnmditem.h"
#include "hnmdmodel.h"
#include "hnstringhandler.h"
#include "hnsuitemodelcontainer.h"
#include "hnitemsorder.h"
#include "hnconvutils.h"
#include "hnliwutils.h"
#include "hnutils.h"
#include "hnfilter.h"
#include "menudebug.h"
#include "hnactionmodel.h"
#include "hnmdaction.h"
#include "hnmdservicecommand.h"
#include "hnmdlocalization.h"
#include "hnmdqueries.h"
#include "hnmdquery.h"
#include "hnrepositorymanager.h"
#include "hnmdbasekey.h"
#include "hninterface.h"
#include "hnservicehandler.h"
#include "hnmdeventmapping.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
EXPORT_C CLiwGenericParamList& CHnMdSuite::GetSuiteParameters()
    {
    return *iSuiteParams;
    }
 
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMdSuite::SetSuiteParametersL(CLiwGenericParamList& aParams)
    {
    iSuiteParams->Reset();
    iSuiteParams->AppendL( aParams );               
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::ConstructL( TXmlEngElement aElement )
    {
    DEBUG(("_MM_: CHnMdSuite::ConstructL IN"));

    // append localization elements
    iCmnPtrs->iLocalization->AppendLocalizationsL( aElement );
    
    RXmlEngNodeList<TXmlEngElement> elements;
    CleanupClosePushL( elements );
    aElement.GetChildElements( elements );
    
    // suite element was found
    TBool suiteFound( EFalse );
    
    // one suite per file
    TXmlEngElement suiteElement;
    while ( elements.HasNext() )
        {
        suiteElement = elements.Next();
        if ( !suiteElement.Name().Compare( KSuiteElementName8 ) )
            {
            suiteFound = ETrue;
            break;
            }
        }
    
    if ( suiteFound )
        {
        // set event mapping
        iEventMapping = CHnMdEventMapping::NewL( suiteElement, iCmnPtrs );
   
        // set genre
        SetGenreL( suiteElement.AttributeNodeL( KGenreAttrName8 ) );
        
        // set widget type
        // widget needs to be set before template will be set
        SetWidgetTypeL( suiteElement.AttributeNodeL( KWidgetTypeAttrName8 ) );
        
        // set template
        SetTemplateL( suiteElement.AttributeNodeL( KTemplateAttrName8 ) );
        
        // set name
        SetTitleL( suiteElement.AttributeNodeL( KTitleAttrName8 ) );
        
        // set empty text
        SetEmptyTextL( suiteElement.AttributeNodeL( KEmptyTextAttrName8 ) );
        
        // set custom identifier
        SetCustomIdL( suiteElement );
            
        // set entries
        CreateItemsL( suiteElement, iCmnPtrs );
        
        // set queries
        iQueries = CHnMdQueries::NewL( suiteElement );
        
        // set cached parameters
        iSuiteParams = CLiwGenericParamList::NewL();
        }
    else
        {
        // definiotion of the suite was not found!
        DEBUG(("_MM_: CHnMdSuite::ConstructL Suite definition not found!"));
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( &elements );
    DEBUG(("_MM_: CHnMdSuite::ConstructL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdSuite* CHnMdSuite::NewL(TXmlEngElement aElement,
                                            THnMdCommonPointers* aCmnPtrs)
    {
    CHnMdSuite* self = CHnMdSuite::NewLC( aElement, aCmnPtrs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdSuite* CHnMdSuite::NewLC(TXmlEngElement aElement,
                                            THnMdCommonPointers* aCmnPtrs)
    {
    CHnMdSuite* self = new( ELeave ) CHnMdSuite( aCmnPtrs );
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdSuite::CHnMdSuite( THnMdCommonPointers* aCmnPtrs )
    : iCmnPtrs( aCmnPtrs ), iAllowableTemplate(NULL)
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdSuite::~CHnMdSuite()
    {
    iAllowableWidgetType.Close();
    delete iAllowableTemplate;
    delete iEventMapping;
    iSuiteName.Close();
    iItems.ResetAndDestroy();
    iTemplate.Close();
    iTitle.Close();
    iEmptyText.Close();
    iCustomId8.Close();
    delete iQueries;
    delete iSuiteParams;
    iAllowedTypes.Close();
    delete iQc;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnMdSuite::EvaluateL( CHnSuiteModel & aSuiteModel )
    {
    MMPERF(("CHnMdSuite::EvaluateL - START"));
    aSuiteModel.GetItemsOrder()->DefineModelItemCountL( iItems.Count() );
    if( iEvaluationNeeded )
        {
        EvaluateSuiteItselfL( aSuiteModel );
        MMPERF(("CHnMdSuite::EvaluateL - suite evaluated"));
        iEvaluationNeeded = EFalse;
        }
    for( TInt i( 0 ); i < iItems.Count(); i++ )
    	{
    	CHnMdItem * item = iItems[i];
    	item->SetAlternativeTemplateL( GetAlternativeWidgetNumber() );
    	item->EvaluateItemL( aSuiteModel, GetSuiteParameters(), i );
    	}
    
    MMPERF(("CHnMdSuite::EvaluateL - items evaluated - DONE"));
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::MarkEvaluationL( CHnFilter& aFilter, CHnSuiteModel& aSuiteModel )
    {  
    if (aFilter.HasSuiteId())
        {
        if (aSuiteModel.CustomId() == aFilter.SuiteId() )
            {
            if ( aFilter.IsEvaluateSuite() )
                {
                iEvaluationNeeded = ETrue;
                }
            for (TInt item(0); item < iItems.Count(); item++)
               {
               iItems[item]->MarkEvaluationL( aFilter );
               }
            }
        else
            {
            for (TInt itemIter(0); itemIter < iItems.Count(); itemIter++)
                {
                RArray< TInt > ids;
                CleanupClosePushL( ids );
                aSuiteModel.GetItemsOrder()->GetIdsForL( itemIter, ids);
                for (TInt idsIter(0); idsIter < ids.Count(); idsIter++)
                    {
                    if (aFilter.SuiteId() == ids[idsIter])
                        {
                        iItems[itemIter]->MarkEvaluationL( aFilter );
                        if (aFilter.IsEvaluateSuite())
                            {
                            iEvaluationNeeded = ETrue;
                            }
                        }
                    }                        
                CleanupStack::PopAndDestroy( &ids );
                }
            }
        }
    else if( aFilter.HasSuiteName() )
        {
        if( !SuiteName().Compare( aFilter.SuiteName()) )
            {
            if ( aFilter.IsEvaluateSuite() )
                {
                iEvaluationNeeded = ETrue;
                }    
            for( TInt i( 0 ); i < iItems.Count(); i++ )
                {
                iItems[i]->MarkEvaluationL( aFilter );
                } 
            } 
        }
    else
        {
        if( aFilter.IsEvaluateSuite() )
            {
            iEvaluationNeeded = ETrue;
            }
        for( TInt i( 0 ); i < iItems.Count(); i++ )
           {
           iItems[i]->MarkEvaluationL( aFilter );
           } 
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::CreateItemsL( TXmlEngElement aElement,
                        THnMdCommonPointers* aCmnPtrs )
    {
    ASSERT( aElement.Name() == KSuiteElementName8 );

    // construction...
    RXmlEngNodeList< TXmlEngElement > elements;
    CleanupClosePushL( elements );
    aElement.GetChildElements( elements );

    TInt elementsCount = elements.Count();

    for (TInt j = 0; j < elementsCount; j++ )
        {
        TXmlEngElement element = elements.Next();
        if ( !element.Name().Compare( KItemElementName8 ) )
            {
            CHnMdItem *item = CHnMdItem::NewL( element, aCmnPtrs );
            CleanupStack::PushL( item );
            iItems.AppendL( item );
            CleanupStack::Pop( item );
            }
        }
    CleanupStack::PopAndDestroy( &elements );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetGenreL(const TDesC& aGenre)
    {
    iSuiteName.Close();
    iSuiteName.CreateL( aGenre );
    iSuiteName.LowerCase();
    DEBUG16(("\t_MM_: suite attribute (genre): %S", &aGenre ));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetGenreL(TXmlEngAttr aGenre)
    {
    HBufC* genre = HnConvUtils::Str8ToStrLC( aGenre.Value() );
    SetGenreL(*genre);
    // clean up
    CleanupStack::PopAndDestroy(genre);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetTemplateL( TXmlEngAttr aTemplate )
    {
    SetTemplateL( aTemplate.Value() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetTemplateL( const TDesC8& aTemplate )
    {
    delete iAllowableTemplate;
    iAllowableTemplate = NULL;
    iAllowableTemplate = new ( ELeave ) CDesC8ArrayFlat( 3 );
    
    for (int i = 0; i < HnUtils::GetColumnTextColumnCount( aTemplate ); i++)
        {
        HBufC8* templateBuf = HnUtils::GetColumnTextLC( i, aTemplate );
        templateBuf->Des().Trim();
        iAllowableTemplate->AppendL( *templateBuf );
        CleanupStack::PopAndDestroy(templateBuf);
        }
    SetAlternativeTemplateL( GetAlternativeWidgetNumber() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdSuite::AddInternalStateL( CLiwGenericParamList& aParamList )
    {
    TLiwGenericParam param;
    param.PushL();
    
    CLiwDefaultMap* suiteMap = CLiwDefaultMap::NewL();
    suiteMap->PushL();

    TLiwVariant suiteWidgetTypeVariant;
    suiteWidgetTypeVariant.PushL();
    
    CLiwDefaultMap* widgetTypeMap = CLiwDefaultMap::NewL();
    widgetTypeMap->PushL();

    if ( iWidgetType == EGridWidget )
        {
        widgetTypeMap->InsertL( KWidgetType8(), KWidgetTypeGrid8() );
        }
    else if ( iWidgetType == EListWidget )
        {
        widgetTypeMap->InsertL( KWidgetType8(), KWidgetTypeList8() );
        }
    else if ( iWidgetType == ECoverFlowWidget )
        {
        widgetTypeMap->InsertL( KWidgetType8(), KWidgetTypeCoverFlow8() );
        }
    
    widgetTypeMap->InsertL( KAllowedTypes8(), iAllowedTypes );
    
    suiteWidgetTypeVariant.Set( widgetTypeMap );
    
    // add suite widget type
    suiteMap->InsertL( KSuite8(), suiteWidgetTypeVariant );
    aParamList.Remove( KMm8 );
    
    // set internal state
    param.SetNameAndValueL( KMm8, TLiwVariant( suiteMap ) );
    aParamList.AppendL( param );

    CleanupStack::PopAndDestroy( widgetTypeMap );
    CleanupStack::PopAndDestroy( &suiteWidgetTypeVariant );
    CleanupStack::PopAndDestroy( suiteMap );
    CleanupStack::PopAndDestroy( &param );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdSuite::DoSetWidgetTypeL( TDesC8 & aConfDes )
    {
    if ( !aConfDes.Compare( KWidgetTypeGrid8 ))
        {
        iAllowableWidgetType.AppendL( EGridWidget );
        }
    else if ( !aConfDes.Compare( KWidgetTypeList8 ) )
        {
        iAllowableWidgetType.AppendL( EListWidget );        
        }
    else if ( !aConfDes.Compare( KWidgetTypeCoverFlow8 ) )
        {
        iAllowableWidgetType.AppendL( ECoverFlowWidget );                
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnMdSuite::GetAlternativeWidgetNumber()
    {
    return iAllowableWidgetType.Find( iWidgetType );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetAlternativeTemplateL( TInt aOption )
    {
    iTemplate.Close();
    if (aOption >= 0 && aOption < iAllowableTemplate->Count())
        {
        const TDesC8 & val = (*iAllowableTemplate)[aOption];
        iTemplate.CreateL(val.Length());
        iTemplate.Copy(val);
        iTemplate.Trim();
        }
    else
        {
        iTemplate.CreateL( KNullDesC8() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetAlternativeTemplateL( CLiwGenericParamList& aParams )
    {
    RBuf8 logicalTemplate;
    logicalTemplate.CleanupClosePushL();
    if ( KErrNone == HnLiwUtils::GetStringL( 
            aParams, KTemplatePath8, logicalTemplate ) &&
            KErrNone != logicalTemplate.Compare( KTemplatePath8 ) &&
            logicalTemplate.Length() )
        {
        SetTemplateL( logicalTemplate );
        }
    else
    	{
    	SetAlternativeTemplateL( GetAlternativeWidgetNumber() );
    	}
    CleanupStack::PopAndDestroy( &logicalTemplate );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnMdSuite::ReadWidgetTypeFromServiceL( CLiwGenericParamList& aParams, 
        RBuf8& aWidgetType )
    {
    return 
        HnLiwUtils::GetStringL( aParams, KWidgetTypePath8, aWidgetType ) == KErrNone 
        && aWidgetType.Compare( KWidgetTypePath8 ) !=  KErrNone 
        && aWidgetType.Length();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetAlternativeWidgetL( CLiwGenericParamList& aParams )
    {
    RBuf8 widget;
    widget.CleanupClosePushL();
    if ( ReadWidgetTypeFromServiceL( aParams, widget )  )
        {
        iAllowableWidgetType.Reset();
        SetWidgetTypeL( widget );
        }
    else
        {
        ReadWidgetTypeL();
        }
    CleanupStack::PopAndDestroy( &widget );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetWidgetTypeL( TXmlEngAttr aWidgetType )
    {
    TPtrC8 widgetType = aWidgetType.Value();
    DEBUG8(("\t_MM_: suite attribute (widget type): %S", &widgetType));

    SaveWidgetTypeL( widgetType );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetWidgetTypeL(TDesC8& aWidgetType)
    {
    iAllowedTypes.Close();
    iAllowedTypes.CreateL(aWidgetType);
    const TInt columnCount(HnUtils::GetColumnTextColumnCount(aWidgetType));
    for (TInt i = 0; i < columnCount; i++)
        {
        HBufC8* type = HnUtils::GetColumnTextLC(i, aWidgetType);
        type->Des().Trim();
        DoSetWidgetTypeL(*type);
        CleanupStack::PopAndDestroy(type);
        }
    if (iAllowableWidgetType.Count() <= 0)
        {
        User::Leave(KErrCorrupt);
        }
    iWidgetType = iAllowableWidgetType[0];
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SaveWidgetTypeL( TDesC8& aWidgetType )
    {
    SetWidgetTypeL( aWidgetType );
    CHnRepositoryManager* repMan = CHnRepositoryManager::NewLC();
        
    if ( iAllowableWidgetType.Count() > 0 )
        {
        THnSuiteWidgetType crWidgetType( EUnspecified );
        TInt err( repMan->ReadSuiteWidgetTypeL( iSuiteName, crWidgetType ) );
        if( !err || err == KErrNotFound )
            {
            TInt found = iAllowableWidgetType.Find( crWidgetType );
            if ( crWidgetType != EUnspecified && found != KErrNotFound )
                {
                iWidgetType = crWidgetType;
                }
            else
                {
                err = repMan->StoreSuiteWidgetTypeL( iSuiteName, iWidgetType );
                }
            }
        }
            
    CleanupStack::PopAndDestroy( repMan );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetModeL( TMdMode aMode )
    {
    if ( iMode != aMode )
    	{
	    for( TInt i( 0 ); i < iItems.Count() ; i++ )
	        {
	        if( iItems[i]->HasAlternativeModeItem() )
	            {
	            CHnMdItem * currentItem = iItems[i];
	            CHnMdItem * newItem = currentItem->TakeAlternativeModeItem();
	            newItem->SetAlternativeModeItem( currentItem );
	            iItems.InsertL( newItem, i );
	            iItems.Remove( i+1 );
	            }
	        }
	    iMode = aMode;
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TMdMode CHnMdSuite::Mode()
	{
	return iMode;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetTitleL( TXmlEngAttr aTitle )
    {
    iTitle.Close();
    iTitle.CreateL( aTitle.Value().Length() );
    iTitle.Copy( aTitle.Value() );
    DEBUG8(("\t_MM_: suite attribute (name): %S", &iTitle));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetEmptyTextL( TXmlEngAttr aEmptyText )
    {
    iEmptyText.Close();
    iEmptyText.CreateL( aEmptyText.Value().Length() );
    iEmptyText.Copy( aEmptyText.Value() );
    DEBUG8(("\t_MM_: suite attribute (name): %S", &iEmptyText));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
//
void CHnMdSuite::EvaluateSuiteItselfL( CHnSuiteModel & aSuiteModel )
    {
    
    DEBUG(("_MM_: CHnMdSuite::EvaluateSuiteItselfL IN"));
    iSuiteModel = &aSuiteModel;
    //delete old query collector and create new one
    //currently running evaluations will be cancelled
    delete iQc;
    iQc = NULL;
    iQc = CHnQueryResultCollector::NewL( *iQueries, GetSuiteParameters() );
    iQc->StartAsynchL( this );
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHnMdSuite::SuiteName()
    {
    return iSuiteName;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnMdSuite::SaveWidgetTypeL() const
	{
	CHnRepositoryManager *repManager = CHnRepositoryManager::NewLC();
	DEBUG8(("\t_MM_: Saving '%S' widget type to CR: %d", &iSuiteName, iWidgetType));
	TInt ret( repManager->StoreSuiteWidgetTypeL( iSuiteName, iWidgetType ) );
	CleanupStack::PopAndDestroy(repManager);
	return ret;
	}
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnMdSuite::ReadWidgetTypeL()
	{
	TInt ret = KErrNone;
	
	if (iAllowableWidgetType.Count() <= 1)
	    {
	    DEBUG8(("\t_MM_: Suite '%S' is allowed to have just one widget: %d ", &iSuiteName, iWidgetType));
	    return ret;
	    }
	    
	CHnRepositoryManager* repManager = CHnRepositoryManager::NewLC();
	THnSuiteWidgetType widgetType( EUnspecified );
	TInt err( repManager->ReadSuiteWidgetTypeL( iSuiteName, widgetType ) );
	if( ( !err || err == KErrNotFound ) && widgetType != EUnspecified )
		{
		if ( widgetType == EChangeWidget )
		    {
		    ASSERT( iAllowableWidgetType.Find( iWidgetType ) >= 0 );
		    TInt index = (iAllowableWidgetType.Find( iWidgetType ) + 1) 
		        % iAllowableWidgetType.Count();
		    iWidgetType = iAllowableWidgetType[index];
		    DEBUG8(("\t_MM_: Suite '%S' widget change invoked: %d", &iSuiteName, widgetType));
		    SaveWidgetTypeL();
		    }
		else if ( iAllowableWidgetType.Find( widgetType ) != KErrNotFound )
		    {
		    DEBUG8(("\t_MM_: Suite '%S' sets widget: %d from CR", &iSuiteName, widgetType));
	        iWidgetType = widgetType;
		    }
		}
	CleanupStack::PopAndDestroy( repManager );
	return ret;
	}
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdSuite::ResultsCollectedL( CLiwGenericParamList* aResults )
    {
    CLiwGenericParamList* queriesResultsList = CLiwGenericParamList::NewLC();
    queriesResultsList->AppendL( *aResults );
    queriesResultsList->AppendL( GetSuiteParameters() );

    RBuf title;
    CleanupClosePushL( title );
    HnLiwUtils::GetStringL( *queriesResultsList, iTitle, title );
    
    RBuf suiteTitle;
    CleanupClosePushL( suiteTitle );
    HnStringHandler::LocaliseL( suiteTitle, title, iCmnPtrs );
    
    DEBUG16( ( "\t_MM_: SetTitleL: %S", &suiteTitle ) ); 
    iSuiteModel->SetTitleL( suiteTitle );
    
    CleanupStack::PopAndDestroy( &suiteTitle );
    CleanupStack::PopAndDestroy( &title );
    
    RBuf emptyText;
    CleanupClosePushL( emptyText );
    HnLiwUtils::GetStringL( *queriesResultsList, iEmptyText, emptyText );
    
    RBuf emptyTextLoc;
    CleanupClosePushL( emptyTextLoc );
    HnStringHandler::LocaliseL( emptyTextLoc, emptyText, iCmnPtrs );
    
    DEBUG16(("\t_MM_: SetEmptyTestL: %S", &emptyTextLoc));
    iSuiteModel->SetEmptyTextL( emptyTextLoc );
    
    CleanupStack::PopAndDestroy( &emptyTextLoc );
    CleanupStack::PopAndDestroy( &emptyText );
    
    // set widget type
    SetAlternativeWidgetL( *queriesResultsList );
    iSuiteModel->SetWidgetType( iWidgetType );
    
    // set custom id
    SetCustomIdL( iSuiteModel, *queriesResultsList, 0 );
    
    // set exit mode
    SetExitModeL();
        
    // set template 
    SetAlternativeTemplateL( *queriesResultsList );
    iSuiteModel->SetTemplateL( iTemplate );
    
    // add widget type to aParams
    AddInternalStateL( GetSuiteParameters() );
    DEBUG(("\t_MM_: WidgetType: %d", iSuiteModel->WidgetType()));
            
    // evaluate actions
    iEventMapping->FillActionsL( iSuiteModel->GetActions(), 
            *queriesResultsList );
    
    CleanupStack::PopAndDestroy( queriesResultsList );

    iSuiteModel->RefreshMulModelL( iSuiteModel->GetItemsOrder()->Count() );
    TBool observersNotified = iSuiteModel->SuiteEvaluationFinishedL();
    if ( observersNotified )
        {
        iCmnPtrs->iModel->SuiteModelReadyToShowL( iSuiteModel );
        }
    
    // set uri based highlight
    SetUriBasedHighlightL();

    //DEBUG16(("\t\t_MM_: SetTemplate: "));
    DEBUG(("_MM_: CHnMdSuite::ResultsCollectedL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetUriBasedHighlightL()
    {
    RBuf tmp;
    CleanupClosePushL( tmp );
    HnLiwUtils::GetStringL( *iSuiteParams, KParamsUriHighlightSuite, tmp );
    
    if ( !tmp.Compare( this->iSuiteName ) )
        {
        tmp.Close();
        HnLiwUtils::GetStringL( *iSuiteParams, KParamsUriHighlight, tmp );
        
        TLex lexFrag( tmp );
        TInt highLight( 0 );
        if( !( lexFrag.Val( highLight ) ) )
            {
            iSuiteModel->SetSuiteHighlightL( highLight ); 
            }
        }

    CleanupStack::PopAndDestroy( &tmp );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetCustomIdL( TXmlEngElement aElement )
    {
    iCustomId8.Close();
    
    if ( aElement.HasAttributeL( KCustomId8 ) )
    	{
    	TPtrC8 av = aElement.AttributeValueL( KCustomId8 );
    	iCustomId8.CreateL( av );
    	}
    else
    	{
    	iCustomId8.CreateL( KItemTypeUnknown8 );
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetCustomIdL( CHnSuiteModel* aSuiteModel, 
		const CLiwGenericParamList& aParams, TInt aPos )
	{
	TInt64 customId( KErrNotFound );
	if ( KErrNone == HnLiwUtils::GetInt64L(
			aParams, iCustomId8, aPos, customId ) )
		{
		aSuiteModel->SetCustomId( customId );
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdSuite::SetExitModeL()
    {
    RBuf exitMode;
    CleanupClosePushL( exitMode );
    HnLiwUtils::GetStringL( *iSuiteParams, KExitModeParams, exitMode );
    
    if ( !exitMode.Compare( KExitModeHide ) )
        {
        iSuiteModel->SetExitMode( EExitModeHide );
        }
    else
        {
        iSuiteModel->SetExitMode( EExitModeNormal );
        }
    
    CleanupStack::PopAndDestroy( &exitMode );
    }


// End of file
