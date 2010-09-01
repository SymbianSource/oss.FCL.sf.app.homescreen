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
* Description:   item presentation model
*
*/


#include <liwservicehandler.h>
#include "hnitemmodel.h"
#include "hnmenuitemmodel.h"
#include "hnactionmodel.h"
#include "hntoolbarmodel.h"
#include "hnattributebase.h"
#include "hnattributetext.h"
#include "hnbuttonmodel.h"
#include "hnglobals.h"
#include "hnconvutils.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnItemModel* CHnItemModel::NewL()
    {
    CHnItemModel* self = new( ELeave ) CHnItemModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CHnItemModel::~CHnItemModel()
    {
    iTemplate.Close();
    delete iMenuModel;
    delete iToolbarModel;
    delete iMskModel;
    DeleteAllActionModels();
    iAttrs.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemModel::DeleteAllActionModels()
    {
    THashMapIter< TInt, CArrayPtr<CHnActionModel>* > iterator( iActions );
    while( iterator.NextKey() )
        {
        CArrayPtr<CHnActionModel>** actions = iterator.CurrentValue();
        (*actions)->ResetAndDestroy();
        delete *actions;
        iterator.RemoveCurrent();
        }
    iActions.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnItemModel::RemoveLiwObjects()
    {
    if ( iToolbarModel )
        {
        iToolbarModel->RemoveLiwObjects();
        }

    for( TInt i = 0; i < iAttrs.Count(); ++i )
        {
        iAttrs[i]->RemoveLiwObjects();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnItemModel::OfferHnEventL( CHnEventHandler & aEventHandler, 
        const TInt aEventId, CLiwGenericParamList* aEventParameters  )
    {
    TInt err = KErrNotFound;
    CArrayPtr<CHnActionModel>* actions = GetActions( aEventId );
    if ( actions ) 
        {
        TInt actionsCount = actions->Count();
        for( TInt i = 0; i < actionsCount; i++ )
            {
            ASSERT( actions->At( i ) );
            err = actions->At( i )->ExecuteL( aEventHandler,  aEventParameters );
            if ( err != KErrNone )
                {
                break;
                }
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
CHnItemModel::CHnItemModel() :
                iActions( &DefaultHash::Integer, &DefaultIdentity::Integer ),
                iEditable( EFalse ),
                iCustomId( KErrNotFound ),
                iMskModel( NULL )
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CHnItemModel::ConstructL()
    {
    iUid = TUid::Null();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CHnItemModel::SetMenuItemModel(  CHnMenuItemModel* aMenu )
    {
    ASSERT( aMenu );
    delete iMenuModel;
    iMenuModel = aMenu;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
MHnMenuItemModelIterator* CHnItemModel::GetMenuStructure()
    {
    if( iMenuModel )
        return iMenuModel->GetMenuStructure();
    else 
        return 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnItemModel::SetMiddleSoftKey( CHnButtonModel* aMsk )
    {
    delete iMskModel;
    iMskModel = aMsk;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CHnButtonModel* CHnItemModel::GetMiddleSoftKey()
    {
    return iMskModel;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C RHashMap< TInt, CArrayPtr<CHnActionModel>* >& 
        CHnItemModel::GetActions()
    {
    return iActions;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C CArrayPtr<CHnActionModel>* CHnItemModel::GetActions( TInt aId )
    {
    CArrayPtr<CHnActionModel>** ret = iActions.Find( aId );
    if( ret )
        {
        return *ret;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C const CHnToolbarModel* CHnItemModel::GetToolbarModel() const
    {
    return iToolbarModel;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CHnItemModel::SetToolbarModel( CHnToolbarModel* aToolbar )
    {
    delete iToolbarModel;
    iToolbarModel = NULL;
    iToolbarModel = aToolbar;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHnItemModel::GetTemplate()
    {
    CHnAttributeBase* templateAttr = GetAttribute( KTemplate8() );
    if ( templateAttr )
        {
        return templateAttr->Value();
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnItemModel::SetTemplateL( const TDesC8& aTemplate )
    {
    CHnAttributeText* templateAttr = CHnAttributeText::NewL();
    templateAttr->SetNameL( KTemplate8 );
    templateAttr->SetValueL( aTemplate );
    SetAttributeL( templateAttr );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnItemModel::SetAttributeL( CHnAttributeBase* aAttr )
    {
    for( TInt i( 0 ); i < iAttrs.Count(); i++ )
        {
        if( iAttrs[i]->Name() == aAttr->Name() )
            {
            delete iAttrs[i];
            iAttrs.Remove(i);
            break;
            }
        }
    iAttrs.AppendL( aAttr );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnItemModel::ClearAttributesL()
    {
    iAttrs.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C CHnAttributeBase* CHnItemModel::GetAttribute( const TDesC8 & aMulAttribute )
    {
    CHnAttributeBase* attr = NULL;
    for( TInt i( 0 ); i < iAttrs.Count(); i++ )
        {
        if( ! iAttrs[i]->Name().Compare( aMulAttribute ) )
            {
            attr = iAttrs[i];
            break;
            }
        }
    return attr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CHnItemModel::IsDeleteLocked()
    {
    TBool res = EFalse;
    if ( iFlags & EItemFlagRemoveLocked )
        {
        res = ETrue;
        }
    return res;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CHnItemModel::IsMoveLocked()
    {
    TBool res = EFalse;
    if ( iFlags & EItemFlagMoveLocked )
        {
        res = ETrue;
        }
    return res;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CHnItemModel::IsDrmExpired()
    {
    TBool res = EFalse;
    if ( iFlags & EItemFlagDrmExpired )
        {
        res = ETrue;
        }
    return res;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TBool CHnItemModel::IsRunning()
    {
    TBool res = EFalse;
    if ( iFlags & EItemFlagRunning )
        {
        res = ETrue;
        }
    return res;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CHnItemModel::SetFlag( TInt aFlag )
    {
    if ( ( iFlags & aFlag ) == 0 )
        {
        iFlags |= aFlag;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CHnItemModel::ClearFlags()
    {
    iFlags = 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CHnItemModel::SetType( const TDesC8& aType )
    {
    if ( !aType.Compare( KMenuFolder8 ) )
        {
        iType = EItemTypeFolder;
        }
    else if ( !aType.Compare( KMenuSuite8 ) )
        {
        iType = EItemTypeSuite;
        }
    else if ( !aType.Compare( KMenuApplication8 ) )
        {
        iType = EItemTypeApplication;
        }
    else if ( !aType.Compare( KMenuParentFolder8 ) )
        {
        iType = EItemTypeParentFolder;
        }
    else
        {
        iType = EItemTypeUnknown;        
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C void CHnItemModel::SetCustomId( TInt64 aCustomId )
    {
    iCustomId = aCustomId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt64 CHnItemModel::CustomId()
    {
    return iCustomId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TMcsItemType CHnItemModel::GetItemType()
    {
    return iType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TUid CHnItemModel::GetItemUid()
    {
    return iUid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnItemModel::SetItemUidL( const TDesC8& aUid )
    {
    TUint uint( KErrNone );
    HBufC* str = HnConvUtils::Str8ToStr( aUid );
    CleanupStack::PushL( str );
    
    TInt position( str->Find( KHexPrefix16 ) );
    TPtrC string( *str );
    TRadix radix( EDecimal );
    if ( position == 0 )
        {
        radix = EHex;
        string.Set( str->Mid( KHexPrefix16().Length() ) );
        }
    
    TLex( string ).Val( uint, radix );
    CleanupStack::PopAndDestroy( str );

    iUid = TUid::Uid( uint );
    }

