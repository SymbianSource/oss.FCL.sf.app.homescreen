/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for wrapper for AknVolumeControl
*
*/


#include <e32base.h>
#include <coecntrl.h>
#include "xncomponent.h"
#include "xnvolumecontrol.h"
#include "xnvolumecontroladapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnText::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnVolumeControl* CXnVolumeControl::NewL()
    {
	CXnVolumeControl* self = new( ELeave ) CXnVolumeControl;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnText::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnVolumeControl::ConstructL()
    {
    CXnComponent::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// CXnText::ConstructL
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnVolumeControl::CXnVolumeControl()
    {
    }

// -----------------------------------------------------------------------------
// CXnText::ConstructL
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnVolumeControl::~CXnVolumeControl()
    {
    }
    
// -----------------------------------------------------------------------------
// CXnVolumeControl::SetValue
// -----------------------------------------------------------------------------
//    
void CXnVolumeControl::SetValue( TInt aValue )
	{
	TRAP_IGNORE( (static_cast<CXnVolumeControlAdapter*>( ControlAdapter() ) )->
		SetValueL( aValue ) );
	}
	
// -----------------------------------------------------------------------------
// CXnVolumeControl::Value
// -----------------------------------------------------------------------------
//    	
TInt CXnVolumeControl::Value()
	{
	TInt value = 0;
	TRAPD( err, value = (static_cast<CXnVolumeControlAdapter*>( ControlAdapter() ) )->ValueL() );
	if( !err )
		{
		return value;
		}
	else return KErrNotFound;
	}
     
// -----------------------------------------------------------------------------
// CXnVolumeControl::SetRange
// -----------------------------------------------------------------------------
//         
void CXnVolumeControl::SetRange( TInt aMinimumValue, TInt aMaximumValue )
	{
	TRAP_IGNORE( (static_cast<CXnVolumeControlAdapter*>( ControlAdapter() ) )->
		SetRangeL( aMinimumValue, aMaximumValue ) );
	}

// -----------------------------------------------------------------------------
// CXnVolumeControl::GetRange
// -----------------------------------------------------------------------------
//    
void CXnVolumeControl::GetRange( TInt& aMinimumValue, TInt& aMaximumValue )
	{
	TRAP_IGNORE( (static_cast<CXnVolumeControlAdapter*>( ControlAdapter() ) )->
		GetRangeL( aMinimumValue, aMaximumValue ) );
	}
	
// -----------------------------------------------------------------------------
// CXnVolumeControl::SetObserver
// -----------------------------------------------------------------------------
//  	
void CXnVolumeControl::SetObserver( MXnVolumeControlObserver* aObserver )
    {
    (static_cast<CXnVolumeControlAdapter*>( ControlAdapter() ) )->SetObserver( aObserver );
    }

// ---------------------------------------------------------
// CXnVolumeControl::MakeInterfaceL
// ---------------------------------------------------------
XnComponentInterface::MXnComponentInterface* CXnVolumeControl::MakeInterfaceL(const TDesC8& aType)
    {
    if (aType != XnVolumeControlInterface::KType)
        {
        return NULL;
        }
    XnVolumeControlInterface::MXnVolumeControlInterface* volumeIF = static_cast<XnVolumeControlInterface::MXnVolumeControlInterface*>(this);
    return volumeIF;
    }        
