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
* Description:  toolbar presentation model
*
*/


#include "hntoolbarmodel.h"
#include "hnbuttonmodel.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnToolbarModel* CHnToolbarModel::NewLC()
    {
    CHnToolbarModel* self = new( ELeave ) CHnToolbarModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnToolbarModel::~CHnToolbarModel()
    {
    iButtons.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnToolbarModel::AddButtonL( CHnButtonModel* aButton )
    {
    if( iButtons.Count() <= KMaxButtonCount )
        {
        iButtons.AppendL( aButton );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const CHnButtonModel* CHnToolbarModel::GetButton( TInt aIndex ) const
    {
    CHnButtonModel* button = NULL;
    for( TInt i( 0 ); i < iButtons.Count(); i++ )
        {
        if( iButtons[ i ]->GetIndex() == aIndex )
            {
            button = iButtons[ i ];
            }
        }
    return button;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHnToolbarModel::HasButtons() const
    {
    TBool hasButtons = EFalse;
    if( iButtons.Count() != 0 )
        {
        hasButtons = ETrue;
        }
    return hasButtons;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnToolbarModel::RemoveLiwObjects()
    {
    for( TInt i( 0 ); i < iButtons.Count(); i++ )
        {
        iButtons[i]->RemoveLiwObjects();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnToolbarModel::CHnToolbarModel( TInt aMaxCount ) : 
                                        KMaxButtonCount( aMaxCount )
    {
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnToolbarModel::ConstructL()
    {
    }

