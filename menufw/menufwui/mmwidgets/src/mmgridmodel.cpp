/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "mmgridmodel.h"
#include "mmlistboxmodel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmGridModel::CMmGridModel()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmGridModel::~CMmGridModel()
    {
    delete iMmListBoxModel;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmGridModel* CMmGridModel::NewLC()
    {
    CMmGridModel* self = new (ELeave)CMmGridModel();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmGridModel* CMmGridModel::NewL()
    {
    CMmGridModel* self = CMmGridModel::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//      
TInt CMmGridModel::NumberOfItems() const
    {
    return CAknGridM::NumberOfItems();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmGridModel::ConstructL()
    {
    iMmListBoxModel = CMmListBoxModel::NewL();
    delete iItemTextArray;
    iItemTextArray = iMmListBoxModel->ItemTextArray();
    SetOwnershipType(ELbmDoesNotOwnItemArray);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CMmGridModel::ConstructL(MDesCArray* /*aItemTextArray*/, 
		TListBoxModelItemArrayOwnership /*aOwnershipType*/)
    {
    //do not delete this is needed to cheat AVKON
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CMmListBoxModel* CMmGridModel::MmListBoxModel()
    {
    return iMmListBoxModel;
    }

// End of file
