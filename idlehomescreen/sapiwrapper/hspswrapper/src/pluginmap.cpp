/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  contains id and uid
*
*/


#include "pluginmap.h"

namespace hspswrapper{

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPluginMap::CPluginMap()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPluginMap::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginMap* CPluginMap::NewL()
    {
    CPluginMap* self = CPluginMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginMap* CPluginMap::NewLC()
    {
    CPluginMap* self = new( ELeave ) CPluginMap;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginMap::~CPluginMap()
    {
    delete iPluginId;
    delete iPluginUid;
    delete iActivationState;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginMap& CPluginMap::SetPluginIdL(const TDesC8& aPluginId)
    {
    delete iPluginId;
    iPluginId = NULL;
    iPluginId = aPluginId.AllocL();
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPluginMap::PluginId()const
    {
    if(iPluginId)
       {
       return *iPluginId;
       }
    else
       {
       return KNullDesC8;
       }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginMap& CPluginMap::SetPluginUidL(const TDesC8& aPluginUid)
    {
    delete iPluginUid;
    iPluginUid = NULL;
    iPluginUid = aPluginUid.AllocL();
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPluginMap::PluginUid()const
    {
    if(iPluginUid)
       {
       return *iPluginUid;
       }
    else
       {
       return KNullDesC8;
       }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginMap& CPluginMap::SetActivationStateL(const TDesC8& aState)
    {
    delete iActivationState;
    iActivationState = NULL;
    iActivationState = aState.AllocL();
    return *this;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CPluginMap::ActivationState()const
    {
    if( iActivationState )
        {               
        return ( *iActivationState == _L8("1") ) ? ETrue : EFalse; 
        }
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginMap& CPluginMap::SetLockingStatusL( const TDesC8& aLockingStatus )
    {
    delete iLockingStatus;
    iLockingStatus = NULL;
    iLockingStatus = aLockingStatus.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPluginMap::LockingStatus() const
    {
    if ( iLockingStatus )
        {
        return *iLockingStatus;
        }
    else
        {
        return KNullDesC8;
        }
    }

}
