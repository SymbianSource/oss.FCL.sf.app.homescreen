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


#include <liwservicehandler.h>
#include "hnmdquery.h"
#include "hnmdqueries.h"
#include "hnconditioninterface.h"
#include "hnmdmenuitem.h"
#include "hnbuttonmodel.h"
#include "hnmdbutton.h"
#include "hnmdeventmapping.h"
#include "hnmduimapping.h"
#include "hnitemid.h"
#include "hnmdlocalization.h"
#include "hnmdsuite.h"
#include "hnxmlmodelprovider.h"
#include "hnmdaction.h"
#include "hnfilter.h"
#include "hnmdtoolbar.h"
#include "hnmdnotifyrequests.h"
#include "hnmditem.h"
#include "hninterface.h"
#include "hnconditionfactory.h"
#include "hnattributetext.h"
#include "hnsuitemodelcontainer.h"
#include "hnmenuitemmodel.h"
#include "hnitemsorder.h"
#include "hnactionmodel.h"
#include "hnitemmodel.h"
#include "hntoolbarmodel.h"
#include "hnconvutils.h"
#include "hnutils.h"
#include "hnmdbasekey.h"
#include "hnmdservicecommand.h"
#include "hnliwutils.h"
#include "hnstringhandler.h"
#include "hnglobals.h"
#include "menudebug.h"

using namespace LIW;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::ConstructL(TXmlEngElement aElement,
                                THnMdCommonPointers* aCmnPtrs)
    {
    ASSERT( !aElement.Name().Compare( KItemElementName8 ) 
            || !aElement.Name().Compare( KEditModeItem8 ) );

    iCmnPtrs = aCmnPtrs;
    
    // set count
    SetCountL( aElement );

    // set template
    SetTemplateL( aElement.AttributeNodeL( KTemplateAttrName8 ) );

    // set condition
    if( aElement.AttributeValueL( KMenuConditionAttrName8 ).Length() )
        {
        iCondition = HnConditionFactory::NewL( 
                aElement.AttributeNodeL( KMenuConditionAttrName8 ) );
        }

    //set item id
    SetItemIdL( aElement );

    // sets the alternative layout for edit mode
    SetEditModeItemL( aElement );
    
    // sets move_locked and delete_locked attributes
    SetAttributesL( aElement );
    
    // sets type of the item
    SetTypeL( aElement );
    
    // sets uid of the item
    SetUidL( aElement );
    
    // sets custom id
    SetCustomIdL( aElement );
    
    // sets msk
    SetMiddleSoftKeyL( aElement, aCmnPtrs );
    
    //set queries
    iQueries = CHnMdQueries::NewL( aElement );

    //set notify requests
    iNotifyRequests = CHnMdNotifyRequests::NewL( aElement, *iItemId, 
            iCmnPtrs );
    
    //set event mapping
    iEventMapping = CHnMdEventMapping::NewL( aElement, iCmnPtrs );

    //set ui mapping
    iUiMapping = CHnMdUiMapping::NewL( aElement, iCmnPtrs );

    //set menu items
    iMenu = CHnMdMenuItem::NewL( aElement, iCmnPtrs );

    //set toolabr element
    iToolbar = CHnMdToolbar::NewL( aElement, iCmnPtrs );
         
    iQueriesResultsList = CLiwGenericParamList::NewL();
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetAlternativeModeItem(CHnMdItem* aAlternativeModeItem)
    {
    iAlternativeModeItem = aAlternativeModeItem;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdItem* CHnMdItem::TakeAlternativeModeItem()
    {
    CHnMdItem * ret = iAlternativeModeItem;
    iAlternativeModeItem = NULL;
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnMdItem::HasAlternativeModeItem() const
    {
    return (iAlternativeModeItem != NULL) ? ETrue : EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetEditModeItemL( TXmlEngElement aElement )
    {
    RXmlEngNodeList< TXmlEngElement> children;
    CleanupClosePushL ( children );
    aElement.GetChildElements ( children );

    TInt count = children.Count();
    for (TInt j = 0; j < count ; j++ )
        {
        TXmlEngElement item = children.Next();
             
        if ( !item.Name().Compare( KEditModeItem8 ) )
            {
            iAlternativeModeItem = CHnMdItem::NewL( item, iCmnPtrs );
            iAlternativeModeItem->iItemId->UpdateL( *iItemId );
            break;
            }
        }
    CleanupStack::PopAndDestroy ( &children );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdItem* CHnMdItem::NewL(TXmlEngElement aElement,
                                        THnMdCommonPointers* aCmnPtrs)
    {
    CHnMdItem* self = CHnMdItem::NewLC(aElement, aCmnPtrs);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdItem* CHnMdItem::NewLC(TXmlEngElement aElement,
                                         THnMdCommonPointers* aCmnPtrs)
    {
    CHnMdItem* self = new( ELeave ) CHnMdItem;
    CleanupStack::PushL( self );
    self->ConstructL(aElement, aCmnPtrs);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdItem::CHnMdItem() : iAlternativeModeItem(NULL), iAllowableTemplate(NULL)
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdItem::~CHnMdItem()
    {
    if ( iNotifyRequests )
        {
        CLiwGenericParamList* empty = NULL;
        TRAP_IGNORE( iNotifyRequests->SendAllRequestsL( *empty, KLiwOptCancel ) );
        }

    delete iNotifyRequests;
    delete iAlternativeModeItem;
    delete iAllowableTemplate;
    iTemplate.Close();
    delete iMenu;
    delete iQueries;
    delete iEventMapping;
    delete iUiMapping;
    delete iItemId;
    delete iToolbar;
    iRemoveLocked8.Close();
    iRunning8.Close();
    iDrmProtection8.Close();
    iMoveLocked8.Close();
    iType8.Close();
    iUid8.Close();
    iCustomId8.Close();
    iCount8.Close();
    delete iQueriesResultsList;
    delete iQc;
    delete iCondition;
    delete iMsk;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::EvaluateItemItselfL(CHnSuiteModel & aSuiteModel,
        CLiwGenericParamList& aParams, TInt aItemPosition )
    {
    
    DEBUG16(("_MM_: CHnMdItem::EvaluateItemL IN"));
    iSuiteModel = &aSuiteModel;
   
    if ( !iNotifyRequests->Sent() )
    	{
    	iNotifyRequests->SendAllRequestsL( aParams );
    	}
    
    iItemPosition = aItemPosition;
    
    //allocate memory for new results
    iQueriesResultsList->Reset();
    iQueriesResultsList->AppendL( aParams );

    //delete old query collector and create new one
    //currently running evaluations will be cancelled
    delete iQc;
    iQc = NULL;
    iQc = CHnQueryResultCollector::NewL( *iQueries , aParams );
    iQc->StartAsynchL( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnMdItem::AddItemL( CHnSuiteModel& aSuiteModel, TInt aItemId, 
        const CLiwGenericParamList& aQueriesResultsList, TInt aItemModelNumber, 
        TInt aItemPos )
	{
	TInt err( KErrNone );
	CHnItemModel* itemModel = CHnItemModel::NewL();
	CleanupStack::PushL( itemModel );
	
	TBool result = FillItemModelL( itemModel, aQueriesResultsList, aItemPos );
	
	if ( result )
		{
		if ( !itemModel->GetTemplate().Compare( KTemplateEmpty8 ) )
			{
			CHnItemModel* empty = aSuiteModel.GetItemModel( aSuiteModel.IdByIndex( -1 ) );
			// empty has been already added
			if ( empty )
				{
				err = KErrAlreadyExists;
				}
			}
		}
	else
		{
		err = KErrGeneral;
		}
	
	if( KErrNone == err )
		{
		aSuiteModel.AddItemL( aItemId, itemModel, aItemModelNumber, aItemPos );
		CleanupStack::Pop( itemModel );
	    }
	else
		{
		CleanupStack::PopAndDestroy( itemModel );
		itemModel = NULL;
		}
	return err;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnMdItem::UpdateItemL( CHnSuiteModel & aSuiteModel, TInt aItemId,
		const CLiwGenericParamList& aQueriesResultsList, TInt aItemPos )
	{
	TBool result = ETrue;
	CHnItemModel* itemModel = aSuiteModel.GetItemModel( aItemId );
	itemModel->ClearAttributesL();
	result = FillItemModelL( itemModel, aQueriesResultsList, aItemPos );
	if( result )
		{
		aSuiteModel.UpdateItemTemplateL( aItemId );
		}  
	return result ? KErrNone : KErrGeneral;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::RemoveItemL( CHnSuiteModel & aSuiteModel, TInt aId )
	{
	//TInt id = iIds[ aPosition ];
	//iIds.Remove( aPosition );
	aSuiteModel.RemoveItemL( aId );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::EvaluateItemL( CHnSuiteModel & aSuiteModel,
                               CLiwGenericParamList& aParamList, 
                               TInt aItemPosition )
    {
    if( iEvaluationNeeded  )
        {
        EvaluateItemItselfL( aSuiteModel, aParamList, aItemPosition );
        iEvaluationNeeded = EFalse;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::MarkEvaluationL( CHnFilter& aFilter )
    {
    if( aFilter.HasItemIds() )
        {
        aFilter.ResetIterator();
        while( aFilter.HasNextItemId() && !iEvaluationNeeded )
            {
            const CHnItemId* item = aFilter.GetNextItemId();
            if( iItemId->Compare( *item ) )
                {
                iEvaluationNeeded = ETrue;
                }
            }
        }
     else
        {
        iEvaluationNeeded = ETrue;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnMdItem::EvaluateItemL( CHnSuiteModel & aSuiteModel,
        const CHnItemId* aItemId, CLiwGenericParamList& aParamList, 
        TInt aItemPosition )
    {
    DEBUG(("_MM_: CHnMdItem::EvaluateItemL IN")); //TO CHANGE LATER (id : %S)", iItemId->Id()));
    TInt result( KErrNotFound ); 
    if( iItemId->Compare( *aItemId ) )
        {
        EvaluateItemItselfL( aSuiteModel, aParamList, aItemPosition );
        result = KErrNone;
        }
    DEBUG(("_MM_: CHnMdItem::EvaluateItemL OUT"));
    return result;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnMdItem::GetIndexValuesFromOutputsL( RPointerArray < HBufC8 > &aValues,
        TXmlEngElement aElement )
	{
	RXmlEngNodeList < TXmlEngElement > children;
	CleanupClosePushL( children );
	aElement.GetChildElements( children );
	TInt amount = children.Count();
	
	TPtrC8 n = aElement.Name();

	for( TInt j( 0 ); j < amount; j++ )
		{
		TXmlEngElement item = children.Next();
		
		if ( !item.Name().Compare( KOutputElementItem8 ) )
			{
			if ( item.HasAttributeL( KValueAttrName8 ) &&
				 item.AttributeValueL( KValueAttrName8 ).Find( KIndex8 ) >= 0 &&
				 item.HasAttributeL( KAliasAttrName8 ) &&
				!item.AttributeValueL( KAliasAttrName8 ).Compare( KTitle8 ) )
				{
				TInt pos = item.AttributeValueL( KValueAttrName8 ).Find( KIndexWithBrackets8 ) - 1;
				TInt siz = KCount().Length();
				TInt len = pos + siz;
				TInt len0 = item.AttributeValueL( KValueAttrName8 ).Length() + KCount().Length();
				
				HBufC8* buf = HBufC8::NewL( len0 );
				buf->Des().Copy( item.AttributeValueL( KValueAttrName8 ) );
				buf->Des().Replace( pos, siz, KCount() );
				buf->Des().SetLength( len );
				aValues.Append( buf );
				}
			}
		}
	
	CleanupStack::PopAndDestroy( &children );
	return aValues.Count();
	}
        
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetCountL( TXmlEngElement aElement )
    {
    TXmlEngAttr aAttr = aElement.AttributeNodeL( KCountAttrName8 );
    iCount8.Close();
    iCount8.CreateL( aAttr.Value() );
    
    // replace "*"
    if ( !iCount8.Compare( KStar8 ) )
    	{
    	RPointerArray<HBufC8> values;
    	CleanupClosePushL( values );
    	if( GetIndexValuesFromOutputsL( values, aElement ) > 0 )
    	    {
            for( TInt i = 0; i < values.Count(); i++ )
                {
                HBufC8* buf = values[i];
                iCount8.Close();
                iCount8.CreateL( buf->Length() );
                iCount8.Copy( *buf );
                }
    	    }
    	else
    	    {
            iCount8.Close();
            iCount8.CreateL( KDefaultCount8().Length() );
            iCount8.Copy( KDefaultCount8 );
    	    }
    	values.ResetAndDestroy();
    	CleanupStack::PopAndDestroy( &values );
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetTemplateL( TXmlEngAttr aAttr )
    {
    iAllowableTemplate = new ( ELeave ) CDesC8ArrayFlat( 3 );
       
    for (int i = 0; i < HnUtils::GetColumnTextColumnCount(aAttr.Value()); i++)
        {
        HBufC8* templateBuf = HnUtils::GetColumnTextLC( i, aAttr.Value() );
        templateBuf->Des().Trim();
        iAllowableTemplate->AppendL( *templateBuf );
        CleanupStack::PopAndDestroy(templateBuf);
        }
      
    if (iAllowableTemplate->Count() > 0 )
        {
        const TDesC8 & val = (*iAllowableTemplate)[0];
        iTemplate.Close();
        iTemplate.CreateL( val.Length() + 1 );
        iTemplate.Copy( val );
        }
    else
        iTemplate.CreateL( KNullDesC8() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetTemplateL( CHnItemModel* aItemModel )
    {
    ASSERT( aItemModel );
    CHnAttributeText* templateAttr = CHnAttributeText::NewL();
    templateAttr->SetNameL( KTemplate8 );
    templateAttr->SetValueL( iTemplate );
    aItemModel->SetAttributeL( templateAttr );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetAlternativeTemplateL( TInt aOption )
    {
    if ( !iTemplate.Compare( KTemplateEmpty8 ) )
        {
        return;
        }
    iTemplate.Close();
    if (aOption >=0 && aOption < iAllowableTemplate->Count())
        {
        const TDesC8 & val = (*iAllowableTemplate)[aOption];
        iTemplate.CreateL( val.Length() + 1 );
        iTemplate.Copy( val );
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
void CHnMdItem::SetItemIdL( TXmlEngElement aItemElement )
    {
    HBufC* suiteName = HnConvUtils::Str8ToStrLC(
        aItemElement.AttributeNodeL( KSuiteElementName8 ).Value() );
    HBufC* itemId = HnConvUtils::Str8ToStrLC(
        aItemElement.AttributeNodeL( KItemIdAttrName8 ).Value() );

    iItemId = CHnItemId::NewL( *suiteName, *itemId );

    CleanupStack::PopAndDestroy( itemId );
    CleanupStack::PopAndDestroy( suiteName );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetDeleteMoveAttributesL( CHnItemModel* aItemModel, 
		const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
	{
	aItemModel->ClearFlags();
	TLiwVariant moveLocked;
	moveLocked.PushL();
	HnLiwUtils::GetVariantL( aQueriesResultsList, iMoveLocked8, aPos, moveLocked );
    if ( moveLocked == TLiwVariant( (TBool) ETrue ) ) 
    	{
    	aItemModel->SetFlag( EItemFlagMoveLocked );
    	}
    CleanupStack::PopAndDestroy( &moveLocked );
    
    TLiwVariant deleteLocked;
    deleteLocked.PushL();
    HnLiwUtils::GetVariantL( aQueriesResultsList, iRemoveLocked8, aPos, deleteLocked );
    if ( deleteLocked == TLiwVariant( (TBool) ETrue ) )
    	{
    	aItemModel->SetFlag( EItemFlagRemoveLocked );
    	}
    CleanupStack::PopAndDestroy( &deleteLocked );
    
    TLiwVariant running;
    running.PushL();
    HnLiwUtils::GetVariantL( aQueriesResultsList, iRunning8, aPos, running );
    if ( running == TLiwVariant( (TBool) ETrue ) )
    	{
    	aItemModel->SetFlag( EItemFlagRunning );
    	}
    CleanupStack::PopAndDestroy( &running );
    
    TLiwVariant drm;
    drm.PushL();
    HnLiwUtils::GetVariantL( aQueriesResultsList, iDrmProtection8, aPos, drm );
    TPtrC drmRights;
    drm.Get( drmRights );
    if ( !drmRights.Compare( KMenuDrmRightsExpired ) )
        {
        aItemModel->SetFlag( EItemFlagDrmExpired );
        }
    CleanupStack::PopAndDestroy( &drm );
	}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetTypeL( CHnItemModel* aItemModel, 
		const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
	{
	RBuf8 type;
    CleanupClosePushL( type );
    HnLiwUtils::GetStringL( aQueriesResultsList, iType8, aPos, type );
    aItemModel->SetType( type );
    CleanupStack::PopAndDestroy( &type );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetUidL( CHnItemModel* aItemModel, 
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
    {
    RBuf8 uid;
    CleanupClosePushL( uid );
    HnLiwUtils::GetStringL( aQueriesResultsList, iUid8, aPos, uid );
    aItemModel->SetItemUidL( uid );
    CleanupStack::PopAndDestroy( &uid );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetCustomIdL( CHnItemModel* aItemModel, 
		const CLiwGenericParamList& aParams, TInt aPos )
	{
	TInt64 customId( KErrNotFound );
	if ( KErrNone == HnLiwUtils::GetInt64L(
			aParams, iCustomId8, aPos, customId ) )
		{
		aItemModel->SetCustomId( customId );
		}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CHnMdItem::FillItemModelL( CHnItemModel* aItemModel, 
		const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
	{
	TBool result( EFalse );
	
	// set template for item
	SetTemplateL( aItemModel );
    
    // set delete/move locked attributes
	SetDeleteMoveAttributesL( aItemModel, aQueriesResultsList, aPos );

    // set type of an item    
	SetTypeL( aItemModel, aQueriesResultsList, aPos );
	
    // set uid of an item    
    SetUidL( aItemModel, aQueriesResultsList, aPos );
	
	// set custom id
    SetCustomIdL( aItemModel, aQueriesResultsList, aPos );
    
    // fills mul item	
	TRAP_IGNORE( result = iUiMapping->FillGraphicalItemL( 
	        aItemModel, aQueriesResultsList, aPos ) );
	
	if ( result )
		{
		// fills item model
		FillItemModelWithMenuItemsL( aItemModel, aQueriesResultsList, aPos );
		FillItemModelWithToolbarL( aItemModel, aQueriesResultsList, aPos );
		FillItemModelWithMiddleSoftKeyL( aItemModel, aQueriesResultsList, 
		        aPos );
					
		aItemModel->DeleteAllActionModels();
		iEventMapping->FillActionsL( aItemModel->GetActions(), 
		        aQueriesResultsList, aPos );
		}
	return result;
    } 

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdItem::FillItemModelWithMenuItemsL( CHnItemModel* aItemModel, 
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
    {
    CHnMenuItemModel* menu = CHnMenuItemModel::NewL( KMenuItemName() );
    CleanupStack::PushL( menu );
    iMenu->EvaluateL( menu, aQueriesResultsList, aPos );
    aItemModel->SetMenuItemModel( menu );
    CleanupStack::Pop( menu );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdItem::FillItemModelWithToolbarL( CHnItemModel* aItemModel, 
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
    {
    CHnToolbarModel* toolbar = CHnToolbarModel::NewLC();
    iToolbar->EvaluateL( toolbar, aQueriesResultsList, aPos );
    aItemModel->SetToolbarModel( toolbar );
    CleanupStack::Pop( toolbar );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdItem::FillItemModelWithMiddleSoftKeyL( CHnItemModel* aItemModel, 
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
    {
    if ( iMsk )
        {
        CHnButtonModel* msk = CHnButtonModel::NewLC( iMsk->GetIndex() );
    	const TDesC& helpText = iMsk->EvaluateButtonTextL( 
                aQueriesResultsList, aPos );
        msk->SetHelpTextL( helpText );
        aItemModel->SetMiddleSoftKey( msk );
        CleanupStack::Pop( msk );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdItem::IsValidL( const CLiwGenericParamList& aQueryResults, 
                  TInt aPos )    
    {
    TBool ret( EFalse );
    if( !iCondition || iCondition->ResultL( aQueryResults, aPos ) )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//

TBool CHnMdItem::CompareItems( CHnItemId* aSearchCriteria )
    {
    return iItemId->Compare( *aSearchCriteria );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetAttributesL( TXmlEngElement aElement )
    {
    iMoveLocked8.Close();
    iRemoveLocked8.Close();
    iRunning8.Close();
    iDrmProtection8.Close();
    
    if ( aElement.HasAttributeL( KMoveLocked8 ) )
    	{
    	TPtrC8 av = aElement.AttributeValueL( KMoveLocked8 );
    	iMoveLocked8.CreateL( av );
    	}
    else
    	{
    	iMoveLocked8.CreateL( KStringFalse8 );
    	}
    
    if ( aElement.HasAttributeL( KRemoveLocked8 ) )
    	{
    	TPtrC8 av = aElement.AttributeValueL( KRemoveLocked8 );
    	iRemoveLocked8.CreateL( av );
    	}
    else
    	{
    	iRemoveLocked8.CreateL( KStringFalse8 );
    	}
    
    if ( aElement.HasAttributeL( KRunning8 ) )
    	{
    	TPtrC8 av = aElement.AttributeValueL( KRunning8 );
    	iRunning8.CreateL( av );
    	}
    else
    	{
    	iRunning8.CreateL( KStringFalse8 );
    	}
    
    if ( aElement.HasAttributeL( KDrmProtection8 ) )
        {
        TPtrC8 av = aElement.AttributeValueL( KDrmProtection8 );
        iDrmProtection8.CreateL( av );
        }
    else
        {
        iDrmProtection8.CreateL( KMenuDrmUnknown8 );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetTypeL( TXmlEngElement aElement )
    {
    iType8.Close();
    
    if ( aElement.HasAttributeL( KItemType8 ) )
    	{
    	TPtrC8 av = aElement.AttributeValueL( KItemType8 );
    	iType8.CreateL( av );
    	}
    else
    	{
    	iType8.CreateL( KItemTypeUnknown8 );
    	}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetUidL( TXmlEngElement aElement )
    {
    iUid8.Close();
    
    if ( aElement.HasAttributeL( KItemUid8 ) )
        {
        TPtrC8 av = aElement.AttributeValueL( KItemUid8 );
        iUid8.CreateL( av );
        }
    else
        {
        iUid8.CreateL( TUid::Null().Name().Length() );
        iUid8.Append( TUid::Null().Name() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::SetCustomIdL( TXmlEngElement aElement )
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
void CHnMdItem::SetMiddleSoftKeyL( TXmlEngElement aElement, 
        THnMdCommonPointers* aCmnPtrs )
    {
    RXmlEngNodeList< TXmlEngElement> children;
    CleanupClosePushL ( children );
    aElement.GetChildElements ( children );
    TInt count = children.Count();
    for (TInt i = 0; i < count; i++ )
        {
        TXmlEngElement item = children.Next();
             
        if ( !item.Name().Compare( KMiddleSoftKey8 ) )
            {
            iMsk = CHnMdButton::NewLC( item, aCmnPtrs );
            CleanupStack::Pop( iMsk );
            break;
            }
        }
    CleanupStack::PopAndDestroy( &children );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CHnMdItem::Template()
	{
	return iTemplate;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CHnMdItem::Type()
	{
	return iType8;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CHnMdItem::Uid()
    {
    return iUid8;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnMdItem::GetItemsMaxCountL()
    {
    TInt ret( KErrNotFound );
    
    // check if iCount8 equals "*"
    if ( !iCount8.Compare( KStar8() ) )
    	{
    	ret = KErrUnknown;
    	}
    // check if iCount8 is a number
    else if ( HnConvUtils::Str8ToInt( iCount8, ret ) >= 0 ) 
    	{
    	; // ret contains a number
    	}
    // try to retrieve number of items from given paths
    else
		{
		TInt columnsCount( HnUtils::GetColumnTextColumnCount( iCount8 ) );
		for (int i = 0; i < columnsCount; i++)
			{
			HBufC8* tmpCountBuf = HnUtils::GetColumnTextLC( i, iCount8 );
			TInt pos = tmpCountBuf->Des().Find( KCount() );
			
			if ( pos >= 0 )
				{
				tmpCountBuf->Des().Replace( pos, KCount().Length(), KNullDesC8 );
				TLiwVariant variant;
				variant.PushL();
				HnLiwUtils::GetVariantL( *iQueriesResultsList, *tmpCountBuf, variant );

				const CLiwList* list = variant.AsList();
				CLiwIterable* iterable = variant.AsIterable();
				TInt count = KErrNotFound;
				if ( list )
				    {
				    count = list->Count();
				    }
				else if ( iterable )
				    {
				    count = HnLiwUtils::GetIterableItemCountL( *iterable );
				    }

				if ( count != KErrNotFound )
					{
					if ( ( i == 0 ) || ( ( i > 0 ) && ( count < ret )  ) )
						{
						ret = count;
						}
					}
				CleanupStack::PopAndDestroy( &variant );
				}
	
			CleanupStack::PopAndDestroy( tmpCountBuf );
			}
		}
   
    return ret; 
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdItem::ResultsCollectedL( CLiwGenericParamList* aResults )
    {
    iQueriesResultsList->AppendL( *aResults );

    RArray< TInt > ids;
    CleanupClosePushL( ids );
    iSuiteModel->GetItemsOrder()->GetIdsForL( iItemPosition, ids );
    TInt startCount = ids.Count();
    TInt visibleItemsBeforeEvaluation = iSuiteModel->GetItemsOrder()->Count();
    TInt id( 0 );

    TBool result = ETrue;
    TInt itemCounter = 0;
    
    TInt maxCount( GetItemsMaxCountL() );
    
    for ( TInt i(0); result; i++, itemCounter++)
        {
        result = ( maxCount != KErrUnknown ) ? i < maxCount && result : result;
        
        if ( !result )
            {
            continue;
            }
       
        id = (ids.Count() > 0 && i < ids.Count()) ? ids[itemCounter]
				: KErrNotFound;
        
        if( id == KErrNotFound )
        	{
        	id = iCmnPtrs->iIdGenerator->GetNextId();
        	TInt err = AddItemL( *iSuiteModel, id, *iQueriesResultsList,
        	        iItemPosition, itemCounter );
        	if ( KErrAlreadyExists == err )
        		{
        		// update empty item which is identified by "-1"
        		TInt err = UpdateItemL( *iSuiteModel, iSuiteModel->IdByIndex(-1), *iQueriesResultsList, i );
        		result = ( KErrNone == err ) ? ETrue : EFalse;
        		}
        	else if ( KErrNone != err )
        		{
        		result = EFalse;
        		}
        	}
        else
        	{
           	TInt err = UpdateItemL( *iSuiteModel, id, *iQueriesResultsList, i );
           	result = ( KErrNone == err ) ? ETrue : EFalse;
        	}
        
        if ( !IsValidL( *iQueriesResultsList, i ) )
            {
            RemoveItemL( *iSuiteModel, id );
            itemCounter--;
            continue;
            }
        }
    
    for( TInt it(startCount - 1); it >= itemCounter -1 && it >= 0; it-- )
    	{
    	RemoveItemL( *iSuiteModel, ids[it] );
    	}
    
    CleanupStack::PopAndDestroy( &ids );    
    iSuiteModel->RefreshMulModelL( visibleItemsBeforeEvaluation );
    TBool observersNotified = 
		iSuiteModel->ItemEvaluationFinishedL( iItemPosition );
    if ( observersNotified )
    	{
    	iCmnPtrs->iModel->SuiteModelReadyToShowL( iSuiteModel );
    	}
    
    DEBUG(("_MM_: CHnMdItem::ResultsCollectedL OUT"));
    }

// End of file
