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


#include "hnmdmenuitem.h"
#include "hnglobals.h"
#include "hnconvutils.h"
#include "hnstringhandler.h"
#include "hnconditioninterface.h"
#include "hnmdeventmappingelement.h"
#include "hnmdevent.h"
#include "hnmenuitemmodel.h"
#include "hnconditionfactory.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdMenuItem::IsMenuItemElementName( const TDesC8& aNameToCompare )    
    {
    return !aNameToCompare.Compare( KMenuItemElementName8 )
        || !aNameToCompare.Compare( KMenuItemSpecificElementName8 )
        || !aNameToCompare.Compare( KMenuItemActionElementName8 );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdMenuItem::AppendChildItemL( CHnMdMenuItem* aMenuItem )
    {
    iChildren.AppendL( aMenuItem );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdMenuItem::EvaluateL( CHnMenuItemModel* aMenuModel,
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
    {
    for ( TInt i( 0 ); i < iChildren.Count(); i++ )
      {
      CHnMdMenuItem* mdMenu = iChildren[ i ];
      if ( mdMenu->IsValidL( aQueriesResultsList, aPos ) )
          {
          CHnMenuItemModel* menu = CHnMenuItemModel::NewL( mdMenu->NameL() );
          mdMenu->EvaluateL( menu, aQueriesResultsList, aPos );
          aMenuModel->AppendChildMenuL( menu );
          }
      }
    aMenuModel->SetCommand( iEvent );
    aMenuModel->SetPosition( iPosition );
    aMenuModel->SetMenuItemType( iType );
    }

// ---------------------------------------------------------------------------
// ConstructL takes as an argument an item xml element. It iterates through 
// item elements in order to find menuitems.
// ---------------------------------------------------------------------------
//
void CHnMdMenuItem::CreatePropertiesL( TXmlEngElement aElement,
                                 THnMdCommonPointers* /* aCmnPtrs */ )
    {
    iType = CHnMenuItemModel::EItemApplication;
    if ( !aElement.Name().Compare( KMenuItemSpecificElementName8 ) )
    	{
    	iType = CHnMenuItemModel::EItemSpecific;
    	}
    else if ( !aElement.Name().Compare( KMenuItemActionElementName8 ) )
    	{
    	iType = CHnMenuItemModel::EItemAction;
    	}
    
    // name
    HBufC* name = HnConvUtils::Str8ToStrLC(
            aElement.AttributeValueL( KName8 ) );
    iName.Assign(name);
    CleanupStack::Pop( name );

    // position
    TPtrC8 pos = aElement.AttributeValueL( KPositionAttrName8 );
    TLex8 lexPosition ( pos );
    User::LeaveIfError( lexPosition.Val( iPosition ) );

    // event
    if( aElement.AttributeValueL( KEventAttrName8 ).Length() && 
                                            !aElement.HasChildNodes() )
        { // there might not be the event attribute - submenues
        TPtrC8 event = aElement.AttributeValueL( KEventAttrName8 );
        TLex8 lex( event );
        User::LeaveIfError( lex.Val( iEvent ) );
        }
    
    // condition
    if( aElement.AttributeValueL( KMenuConditionAttrName8 ).Length() )
        {
        iCondition = HnConditionFactory::NewL(  
          aElement.AttributeNodeL( KMenuConditionAttrName8 ) );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdMenuItem::CreateChildrenL( TXmlEngElement aElement,
                                        THnMdCommonPointers* aCmnPtrs )
    { 
    RXmlEngNodeList< TXmlEngElement > children;
    CleanupClosePushL( children );
    aElement.GetChildElements( children );
    TInt count = children.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        TXmlEngElement child = children.Next();
        if ( IsMenuItemElementName( child.Name() ) )
            {
            AppendChildItemL( CHnMdMenuItem::NewL( child, aCmnPtrs ) );
            }
        }
    CleanupStack::PopAndDestroy( &children );
    }

// ---------------------------------------------------------------------------
// ConstructL takes as an argument an item xml element. It iterates through 
// item elements in order to find menuitems.
// ---------------------------------------------------------------------------
//
void CHnMdMenuItem::ConstructL( TXmlEngElement aElement,
                                THnMdCommonPointers* aCmnPtrs )
    {
    if ( IsMenuItemElementName( aElement.Name() ) )
        { // this is an ordinary menu item
        CreatePropertiesL( aElement, aCmnPtrs );
        CreateChildrenL( aElement, aCmnPtrs ); // in case there are subitems
        }
    else
        { // this is the root level
        CreateChildrenL( aElement, aCmnPtrs );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMenuItem* CHnMdMenuItem::NewL( TXmlEngElement aElement,
        THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdMenuItem* self = CHnMdMenuItem::NewLC( aElement, aCmnPtrs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMenuItem* CHnMdMenuItem::NewLC( TXmlEngElement aElement, 
        THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdMenuItem* self = new( ELeave ) CHnMdMenuItem;
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aCmnPtrs );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMenuItem::CHnMdMenuItem()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdMenuItem::~CHnMdMenuItem()
    {
    iName.Close();
    iChildren.ResetAndDestroy();
    iNameValueCache.Close();
    delete iCondition;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CHnMdMenuItem::NameL() 
    {
    iNameValueCache.Close();
    HnStringHandler::LocaliseL( iNameValueCache, iName, NULL /*common params*/);
    return iNameValueCache;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdMenuItem::IsValidL( const CLiwGenericParamList& aQueryResults, 
        TInt aPos )    
    {
    TBool ret( EFalse );
    if( !iCondition || iCondition->ResultL( aQueryResults, aPos ) )
        {
        ret = ETrue;
        }
    return ret;
    }

