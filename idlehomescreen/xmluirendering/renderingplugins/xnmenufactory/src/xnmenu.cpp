/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for wrapper for a menu
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include <barsread.h>

#include "xnmenu.h"
#include "xnmenuadapter.h"
// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CXnMenu::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnMenu* CXnMenu::NewL()
    {
    CXnMenu* self = new( ELeave ) CXnMenu;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnMenu::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnMenu::ConstructL()
    {
    CXnComponent::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CXnMenu::ConstructL
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnMenu::CXnMenu()
    {
    }

// -----------------------------------------------------------------------------
// CXnMenu::ConstructL
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnMenu::~CXnMenu()
    {
    }
    

// -----------------------------------------------------------------------------
// CXnMenu::SetSoftKeyImageL
// 
// -----------------------------------------------------------------------------
//
void CXnMenu::SetSoftKeyImageL( CFbsBitmap* aBitmap, CFbsBitmap* aMask,        
    TSoftKeyPosition aPos, TBool aPreserveAspectRatio, TBool aInvertMask,
    TBool aTransferOwnership )
    {
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
    adapter->SetSoftKeyImageL( aBitmap, aMask, aPos, NULL, aPreserveAspectRatio, aInvertMask, aTransferOwnership );    
    }

// -----------------------------------------------------------------------------
// CXnMenu::SetSoftKeyImageL
// 
// -----------------------------------------------------------------------------
//
void CXnMenu::SetSoftKeyImageL( CFbsBitmap* aBitmap, CFbsBitmap* aMask,        
    TSoftKeyPosition aPos, CXnNodeAppIf* aNode, TBool aPreserveAspectRatio, TBool aInvertMask,
    TBool aTransferOwnership )
    {
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
    adapter->SetSoftKeyImageL( aBitmap, aMask, aPos, aNode, aPreserveAspectRatio, aInvertMask, aTransferOwnership );    
    }

// -----------------------------------------------------------------------------
// CXnMenu::SetSoftKeyImageL
// 
// -----------------------------------------------------------------------------
//
void CXnMenu::SetSoftKeyImageL( const TAknsItemID& aId,
    const TDesC& aBmpFile,
    const TInt32 aBmp, const TInt32 aBmpM,
    XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos, 
    TBool aEnable )
    {
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
    adapter->SetSoftKeyImageL( aId, aBmpFile, aBmp, aBmpM, aPos, aEnable );    
    }

// -----------------------------------------------------------------------------
// CXnMenu::SetSoftKeyTextL
// 
// -----------------------------------------------------------------------------
//
void CXnMenu::SetSoftKeyTextL( const TDesC& aText, TSoftKeyPosition aPos )
    {
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
    adapter->SetSoftKeyTextL( aText, aPos );       
    }

// -----------------------------------------------------------------------------
// CXnMenu::SetSoftKeyL
// 
// -----------------------------------------------------------------------------
//
void CXnMenu::SetSoftKeyL( CXnNodePluginIf* aSoftkeyNode, TSoftKeyPosition aPos )
    {
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
    adapter->SetSoftKeyL( aSoftkeyNode, aPos );       
    }

// -----------------------------------------------------------------------------
// CXnMenu::SoftKeyL
// 
// -----------------------------------------------------------------------------
//
CXnNodePluginIf* CXnMenu::SoftKeyL( TSoftKeyPosition aPos )
    {
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
    return adapter->SoftKeyL( aPos );       
    }

// -----------------------------------------------------------------------------
// CXnMenu::SetObserver
// 
// -----------------------------------------------------------------------------
//
void CXnMenu::SetObserver( XnMenuInterface::MXnMenuObserver& aObserver )
    {
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
    adapter->SetObserver( aObserver );           
    }        

// -----------------------------------------------------------------------------
// CXnMenu::RemoveObserver
// 
// -----------------------------------------------------------------------------
//
void CXnMenu::RemoveObserver()
    {
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
    adapter->RemoveObserver();           
    }        

// -----------------------------------------------------------------------------
// CXnMenu::TryDisplayingMenuBarL
// 
// -----------------------------------------------------------------------------
//        
void CXnMenu::TryDisplayingMenuBarL( const TDesC& aMenuNodeId )
    {    
    CXnMenuAdapter* adapter = static_cast< CXnMenuAdapter* >( ControlAdapter() );
           
    adapter->TryDisplayingMenuL( aMenuNodeId );               
    }
    
// -----------------------------------------------------------------------------
// CXnMenu::MakeInterfaceL
// 
// -----------------------------------------------------------------------------
//
XnComponentInterface::MXnComponentInterface* CXnMenu::MakeInterfaceL(
    const TDesC8& aType)  
    {
    if( aType != XnMenuInterface::KType )
        {
        return NULL;
        }
        
    return static_cast<XnMenuInterface::MXnMenuInterface*>( this );
    }
