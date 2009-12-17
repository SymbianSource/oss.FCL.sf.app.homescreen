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
* Description:  toolbar button model
*
*/


#include "hnbuttonmodel.h"
#include "gulicon.h"
#include "hnattributeimage.h"
#include "hnattributebase.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnButtonModel* CHnButtonModel::NewLC( TInt aIndex )
    {
    CHnButtonModel* self = new( ELeave ) CHnButtonModel( aIndex );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnButtonModel::~CHnButtonModel()
    {
    iToolTip.Close();
    //delete iIcon;
    delete iIconAttribute;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CHnButtonModel::GetIcon() const
    {
    CGulIcon* icon = NULL;
    
    if ( iIconAttribute )
        {
        icon = iIconAttribute->GetIcon();
        }
    
    return icon;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnButtonModel::SetIcon( CHnAttributeBase* aAttribute )
    {
    delete iIconAttribute;
    iIconAttribute = NULL;
    iIconAttribute = aAttribute;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHnButtonModel::GetButtonText() const
    {
    return iToolTip; 
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnButtonModel::SetHelpTextL( const TDesC& aToolTip )
    {
    iToolTip.Close();
    iToolTip.CreateL( aToolTip );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnButtonModel::GetDimmed() const
    {
    return iDimmed;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnButtonModel::SetDimmed( TBool aDimmed )
    {
    iDimmed = aDimmed;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnButtonModel::GetIndex() const
    {
    return iIndex;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHnButtonModel::GetEventId() const
    {
    return iEventId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnButtonModel::SetEventId( TInt aEventId )
    {
    iEventId = aEventId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnButtonModel::RemoveLiwObjects()
    {
    iIconAttribute->RemoveLiwObjects();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnButtonModel::CHnButtonModel( TInt aIndex ) 
    {
    iIndex = aIndex;
    iDimmed = EFalse;
    iEventId = KErrNotFound;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnButtonModel::ConstructL()
    {
    }

