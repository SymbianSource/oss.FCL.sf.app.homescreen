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
* Description:  Presents object map as name, path and mediatype data
*
*/



#include "objectmap.h"

namespace hspswrapper{


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CObjectMap::CObjectMap()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CObjectMap::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CObjectMap* CObjectMap::NewL()
    {
    CObjectMap* self = CObjectMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CObjectMap* CObjectMap::NewLC()
    {
    CObjectMap* self = new( ELeave ) CObjectMap;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CObjectMap::~CObjectMap()
    {
    delete iName;
    delete iPath;
    delete iMediaType;
    delete iTag;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CObjectMap& CObjectMap::SetNameL( const TDesC8& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CObjectMap::NameL()const
    {
    if(iName)
       {
       return *iName;
       }
   else
       {
       return KNullDesC8;
       }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CObjectMap& CObjectMap::SetPathL( const TDesC8& aPath )
    {
    delete iPath;
    iPath = NULL;
    iPath = aPath.AllocL();
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CObjectMap::Path()const
    {
    if(iPath)
      {
      return *iPath;
      }
    else
      {
      return KNullDesC8;
      }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CObjectMap& CObjectMap::SetMediaTypeL( const TDesC8& aMediaType )
    {
    delete iMediaType;
    iMediaType = NULL;
    iMediaType = aMediaType.AllocL();
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CObjectMap::MediaType()const
    {
    if(iMediaType)
      {
      return *iMediaType;
      }
    else
      {
      return KNullDesC8;
      }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CObjectMap& CObjectMap::SetTagL( const TDesC8& aTag )
    {
    delete iTag;
    iTag = NULL;
    iTag = aTag.AllocL();
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CObjectMap::Tag()const
    {
    if(iTag)
      {
      return *iTag;
      }
    else
      {
      return KNullDesC8;
      }
    }
    
}


