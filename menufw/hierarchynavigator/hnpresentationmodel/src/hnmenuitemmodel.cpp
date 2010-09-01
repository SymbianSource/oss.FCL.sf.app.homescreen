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
* Description:  menu item presentation model
*
*/


#include "hnmenuitemmodel.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMenuItemModel* CHnMenuItemModel:: NewL( const TDesC& aName )
    {
    CHnMenuItemModel* self = new( ELeave ) CHnMenuItemModel();
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
CHnMenuItemModel::~CHnMenuItemModel()
    {
    iName.Close();
    iChildren.ResetAndDestroy();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnMenuItemModel::HasNext()
    {
    TBool ret = ETrue;
    if( iNextMenu >= iChildren.Count() )
        {
        ret = EFalse;
        Reset();
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMenuItemModel* CHnMenuItemModel::GetNext()
    {
    ASSERT( iNextMenu < iChildren.Count() );
    CHnMenuItemModel* ret = iChildren[ iNextMenu ];
    iNextMenu++;
    return ret;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnMenuItemModel::HasNextSpecific()
    {
    TBool hasNext = EFalse;
    if ( iNextMenu >= iChildren.Count() )
    	{
    	Reset();
    	}
    else if ( iChildren[iNextMenu]->MenuItemType() == EItemSpecific )
    	{
    	hasNext = ETrue;
    	}
    else
    	{
    	iNextMenu++;
    	hasNext = HasNextSpecific();
    	}
    return hasNext;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMenuItemModel* CHnMenuItemModel::GetNextSpecific()
    {
    ASSERT( iNextMenu < iChildren.Count() );
    ASSERT( iChildren[iNextMenu]->MenuItemType() == EItemSpecific );
    CHnMenuItemModel* nextSpecific = iChildren[ iNextMenu ];
    iNextMenu++;
    return nextSpecific;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMenuItemModel::Reset()
    {
    iNextMenu = 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMenuItemModel::AppendChildMenuL( CHnMenuItemModel* aMenuModel )
    {
    ASSERT( aMenuModel );
    iChildren.AppendL( aMenuModel );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMenuItemModel::CHnMenuItemModel()
    {
    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMenuItemModel::ConstructL( const TDesC& aName )
    {
    ASSERT( aName.Length() );
    iMenuItemType = EItemApplication;
    iName.CreateL( aName );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHnMenuItemModel::NameL()
    {
    return iName;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnMenuItemModel::Command()
    {
    return iCommand;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMenuItemModel::SetCommand( TInt aCommandId )
    {
    iCommand = aCommandId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnMenuItemModel::Position()
    {
    return iPosition;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMenuItemModel::SetPosition( TInt aPosition )
    {
    iPosition = aPosition;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnMenuItemModel::SetMenuItemType( THnMenuItemType aMenuItemType )
    {
    iMenuItemType = aMenuItemType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnMenuItemModel::THnMenuItemType CHnMenuItemModel::MenuItemType()
    {
    return iMenuItemType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
EXPORT_C MHnMenuItemModelIterator* CHnMenuItemModel::GetMenuStructure()
    {
    return this;
    }


