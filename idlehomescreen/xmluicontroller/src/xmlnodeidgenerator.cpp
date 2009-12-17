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


#include "xmlnodeidgenerator.h"
#include "aipropertyextension.h"
#include "aicontentmodel.h"
#include "aixmluiutils.h"
#include "aixmluiconstants.h"
#include "aistrcnv.h"

using namespace AiXmlUiController;

CXmlNodeIdGenerator::CXmlNodeIdGenerator()
    {
    }
    
CXmlNodeIdGenerator* CXmlNodeIdGenerator::NewL()
    {
    return new(ELeave) CXmlNodeIdGenerator;
    }
    
CXmlNodeIdGenerator::~CXmlNodeIdGenerator()
    {
    delete iContentNodeIdBuf;
    }

TPtrC CXmlNodeIdGenerator::SettingsNodeIdL( const TAiPublisherInfo& aPubInfo )
    {
    const TInt idLength = aPubInfo.iName.Length()
                          + AiUiDef::xml::id::KSettingsIdSeparator().Length()
                          + AiUiDef::xml::propertyClass::KSettings().Length();
    TPtr bufPtr( AiUtility::EnsureBufMaxLengthL( iContentNodeIdBuf, idLength ) );
    
    bufPtr.Copy( AiUiDef::xml::propertyClass::KSettings );
    bufPtr.Append( AiUiDef::xml::id::KSettingsIdSeparator );
    bufPtr.Append( aPubInfo.iName );
    
    return bufPtr;
    }

TPtrC CXmlNodeIdGenerator::ContentNodeIdL(MAiPropertyExtension& aPlugin, const TAiContentItem& aContentItem)
    {
    TAiPublisherInfo* info = static_cast<TAiPublisherInfo*>( aPlugin.GetPropertyL( EAiPublisherInfo ) );
    
    LeaveIfNull( info, KErrNotFound );
    
    return ContentNodeIdL(*info, aContentItem);
    }
    
TPtrC CXmlNodeIdGenerator::ContentNodeIdL
        ( const TAiPublisherInfo& aPubInfo, const TAiContentItem& aContentItem )
    {
    const TDesC& contentCid = ContentCid( aContentItem );
    
    // Calculate buffer size and ensure buffer has enough room
    const TInt idLength = aPubInfo.iName.Length()
                          + AiUiDef::xml::id::KContentIdSeparator().Length()
                          + contentCid.Length();
    TPtr bufPtr( AiUtility::EnsureBufMaxLengthL( iContentNodeIdBuf, idLength ) );
    
    // Copy publisher info name to the buffer
    bufPtr.Copy( aPubInfo.iName );
    
    // Append separator
    bufPtr.Append( AiUiDef::xml::id::KContentIdSeparator );
    
    // Append content item id
    bufPtr.Append( contentCid );
    
    return bufPtr;
    }

TPtrC CXmlNodeIdGenerator::ResourceNodeIdL
        ( MAiPropertyExtension& aPlugin, const TAiContentItem& aContentItem )
    {
    TAiPublisherInfo* info = static_cast<TAiPublisherInfo*>( aPlugin.GetPropertyL( EAiPublisherInfo ) );
    
    LeaveIfNull( info, KErrNotFound );
    
    return ResourceNodeIdL(*info, aContentItem);
    }

TPtrC CXmlNodeIdGenerator::ResourceNodeIdL
        ( const TAiPublisherInfo& aPubInfo, const TAiContentItem& aContentItem )
    {
    // Resources node id is similar to content node id
    return ContentNodeIdL( aPubInfo, aContentItem );
    }
