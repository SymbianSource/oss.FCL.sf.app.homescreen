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
* Description:  Builder class for XML UI node identifiers.
*
*/

// System includes

// User includes
#include <hscontentpublisher.h>
#include <hspublisherinfo.h>
#include "aicontentmodel.h"
#include "aixmluiutils.h"
#include "aixmluiconstants.h"
#include "aistrcnv.h"

#include "xmlnodeidgenerator.h"

using namespace AiXmlUiController;

// ----------------------------------------------------------------------------
// CXmlNodeIdGenerator::CXmlNodeIdGenerator
// 
// ----------------------------------------------------------------------------
//
CXmlNodeIdGenerator::CXmlNodeIdGenerator()
    {
    }

// ----------------------------------------------------------------------------
// CXmlNodeIdGenerator::NewL
// 
// ----------------------------------------------------------------------------
//
CXmlNodeIdGenerator* CXmlNodeIdGenerator::NewL()
    {
    return new ( ELeave ) CXmlNodeIdGenerator;
    }

// ----------------------------------------------------------------------------
// CXmlNodeIdGenerator::CXmlNodeIdGenerator
// 
// ----------------------------------------------------------------------------
//
CXmlNodeIdGenerator::~CXmlNodeIdGenerator()
    {
    delete iContentNodeIdBuf;
    }

// ----------------------------------------------------------------------------
// CXmlNodeIdGenerator::SettingsNodeIdL
// 
// ----------------------------------------------------------------------------
//
TPtrC CXmlNodeIdGenerator::SettingsNodeIdL( 
    const THsPublisherInfo& aPublisherInfo )
    {
    const TInt idLength( aPublisherInfo.Name().Length() +
      AiUiDef::xml::id::KSettingsIdSeparator().Length() +
      AiUiDef::xml::propertyClass::KSettings().Length() );
    
    TPtr bufPtr( 
        AiUtility::EnsureBufMaxLengthL( iContentNodeIdBuf, idLength ) );
    
    bufPtr.Copy( AiUiDef::xml::propertyClass::KSettings );
    bufPtr.Append( AiUiDef::xml::id::KSettingsIdSeparator );
    bufPtr.Append( aPublisherInfo.Name() );
    
    return bufPtr;
    }

// ----------------------------------------------------------------------------
// CXmlNodeIdGenerator::CXmlNodeIdGenerator
// 
// ----------------------------------------------------------------------------
//
TPtrC CXmlNodeIdGenerator::ContentNodeIdL( CHsContentPublisher& aPlugin, 
    const TAiContentItem& aContentItem )
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
                  
    return ContentNodeIdL( info, aContentItem );
    }
  
// ----------------------------------------------------------------------------
// CXmlNodeIdGenerator::CXmlNodeIdGenerator
// 
// ----------------------------------------------------------------------------
//
TPtrC CXmlNodeIdGenerator::ContentNodeIdL(
    const THsPublisherInfo& aPublisherInfo, 
    const TAiContentItem& aContentItem )
    {
    const TDesC& contentCid( ContentCid( aContentItem ) );
    
    // Calculate buffer size and ensure buffer has enough room
    const TInt idLength( aPublisherInfo.Name().Length() + 
      AiUiDef::xml::id::KContentIdSeparator().Length() + contentCid.Length() );
          
    TPtr bufPtr( 
        AiUtility::EnsureBufMaxLengthL( iContentNodeIdBuf, idLength ) );
    
    // Copy publisher info name to the buffer
    bufPtr.Copy( aPublisherInfo.Name() );
    
    // Append separator
    bufPtr.Append( AiUiDef::xml::id::KContentIdSeparator );
    
    // Append content item id
    bufPtr.Append( contentCid );
    
    return bufPtr;
    }

// ----------------------------------------------------------------------------
// CXmlNodeIdGenerator::CXmlNodeIdGenerator
// 
// ----------------------------------------------------------------------------
//
TPtrC CXmlNodeIdGenerator::ResourceNodeIdL(
    CHsContentPublisher& aPlugin, const TAiContentItem& aContentItem )
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
           
    return ResourceNodeIdL( info, aContentItem );
    }

// ----------------------------------------------------------------------------
// CXmlNodeIdGenerator::CXmlNodeIdGenerator
// 
// ----------------------------------------------------------------------------
//
TPtrC CXmlNodeIdGenerator::ResourceNodeIdL(
    const THsPublisherInfo& aPublisherInfo, 
    const TAiContentItem& aContentItem )
    {
    // Resources node id is similar to content node id
    return ContentNodeIdL( aPublisherInfo, aContentItem );
    }

// End of file
