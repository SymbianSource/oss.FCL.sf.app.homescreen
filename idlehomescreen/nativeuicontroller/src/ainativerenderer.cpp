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
* Description:  
*
*/


#include <aipropertyextension.h>
#include "ainativerenderer.h"
#include "ainativeuiplugins.h"

using namespace AiNativeUiController;


CAiNativeRenderer::~CAiNativeRenderer()
    {
    }

    
TInt CAiNativeRenderer::Publish( MAiPropertyExtension& aPlugin, TInt aContent, 
									TInt aResource, TInt aIndex )
    {
    TRAPD( err, DoMimeTypeCheckL( aPlugin, aContent ) );
    
    if( err != KErrNone )
        {
        return err;
        }
        
    TRAP( err, DoPublishL( aPlugin, aContent, aResource, aIndex ) );
    return err;
    }
    
    
TInt CAiNativeRenderer::Publish( MAiPropertyExtension& aPlugin, TInt aContent,
									const TDesC16& aText, TInt aIndex )
    {    
    TRAPD( err, DoMimeTypeCheckL( aPlugin, aContent ) );
    
    if( err != KErrNone )
        {
        return err;
        }
        
    TRAP( err, DoPublishL( aPlugin, aContent, aText, aIndex ) );
    return err;
    }
    
    
TInt CAiNativeRenderer::Publish( MAiPropertyExtension& aPlugin, TInt aContent, 
									const TDesC8& aBuf, TInt aIndex )
    {    
    TRAPD( err, DoMimeTypeCheckL( aPlugin, aContent ) );
    
    if( err != KErrNone )
        {
        return err;
        }
        
    TRAP( err, DoPublishL( aPlugin, aContent, aBuf, aIndex ) );
    return err;
    }
    
    
TInt CAiNativeRenderer::Publish( MAiPropertyExtension& aPlugin, TInt aContent, 
									RFile& aFile, TInt aIndex )
    {    
    TRAPD( err, DoMimeTypeCheckL( aPlugin, aContent ) );
    
    if( err != KErrNone )
        {
        return err;
        }
        
    TRAP( err, DoPublishL( aPlugin, aContent, aFile, aIndex ) );
    return err;
    }
    
    
TInt CAiNativeRenderer::Clean( MAiPropertyExtension& aPlugin, TInt aContent )
    {    
    TRAPD( err, DoMimeTypeCheckL( aPlugin, aContent ) );
    
    if( err != KErrNone )
        {
        return err;
        }
        
    TRAP( err, DoCleanL( aPlugin, aContent ) );
    return err;
    }
    
    
void CAiNativeRenderer::DoPublishL
        ( MAiPropertyExtension& /*aPlugin*/, TInt /*aContent*/, TInt /*aResource*/, TInt /*aIndex*/ )
    { 
	// Default implementation, overridden in subclass if needed.
    User::Leave( KErrNotFound );   
    }
    
    
void CAiNativeRenderer::DoPublishL
        ( MAiPropertyExtension& /*aPlugin*/, TInt /*aContent*/, const TDesC16& /*aText*/, TInt /*aIndex*/ )
    {    
    // Default implementation, overridden in subclass if needed. 
    User::Leave( KErrNotFound );   
    }
    
    
void CAiNativeRenderer::DoPublishL
        ( MAiPropertyExtension& /*aPlugin*/, TInt /*aContent*/, const TDesC8& /*aBuf*/, TInt /*aIndex*/ )
    {    
    // Default implementation, overridden in subclass if needed. 
    User::Leave( KErrNotFound );   
    }
    
    
void CAiNativeRenderer::DoPublishL
        ( MAiPropertyExtension& /*aPlugin*/, TInt /*aContent*/, RFile& /*aFile*/, TInt /*aIndex*/ )
    {    
    // Default implementation, overridden in subclass if needed. 
    User::Leave( KErrNotFound );   
    }
    
    
void CAiNativeRenderer::DoCleanL
        ( MAiPropertyExtension& /*aPlugin*/, TInt /*aContent*/ )
    {    
    // Default implementation, overridden in subclass if needed. 
    User::Leave( KErrNotFound );   
    }


TInt CAiNativeRenderer::GetIntContentId( const TDesC& aCid ) const
    {
    for( TInt i( 0 ); i < KAiDeviceStatusContentCount; i++ )
        {
        const TAiContentItem& item = KAiDeviceStatusContent[i];
        if( ContentCid(item) == aCid )
            {
            //match
            return item.id;            
            }
        }

    for( TInt i( 0 ); i < KAiScutContentCount; i++ )
        {
        const TAiContentItem& item = KAiScutContent[i];
        if( ContentCid(item) == aCid )
            {
            //match
            return item.id;            
            }
        }

    return KErrNotFound;
    }
    

TBool CAiNativeRenderer::MatchMimeType( MAiPropertyExtension& aPlugin, 
                                        TInt aContentId, 
                                        const TDesC8& aMimeType ) const
    {      
    TUid pluginUid;
    TRAPD( err, pluginUid = aPlugin.PublisherInfoL()->iUid );
    if ( err != KErrNone )
        {
        return EFalse;
        }
    // loop through Device Status content items
    if ( pluginUid == KDeviceStatusPluginUid )
        {
        for( TInt i( 0 ); i < KAiDeviceStatusContentCount; i++ )
            {
            const TAiContentItem& item = KAiDeviceStatusContent[i];
            
            if( item.id != aContentId )
                {
                continue;
                }            
            
            TBuf8<KAiContentTypeMaxLength> mimeType((TText8*) item.type);
                
            return ( mimeType.Match( aMimeType ) != KErrNotFound );
            }        
        }
    else if ( pluginUid == KShortcutPluginUid )
    { 
    // if match not found, loop through Shortcut content items
    for( TInt i( 0 ); i < KAiScutContentCount; i++ )
        {
        const TAiContentItem& item = KAiScutContent[i];
        
        if( item.id != aContentId )
            {
            continue;
            }            
        
        TBuf8<KAiContentTypeMaxLength> mimeType((TText8*) item.type);
            
        return ( mimeType.Match( aMimeType ) != KErrNotFound );
        }        
    }
    else if ( pluginUid == KProfilePluginUid )
    { 
    // if match not found, loop through Profile content items
    for( TInt i( 0 ); i < KAiProfileContentCount; i++ )
        {
        const TAiContentItem& item = KAiProfileContent[i];
        
        if( item.id != aContentId )
            {
            continue;
            }            
        
        TBuf8<KAiContentTypeMaxLength> mimeType((TText8*) item.type);
            
        return ( mimeType.Match( aMimeType ) != KErrNotFound );
        }        
    }
    
    return EFalse;    
    }


void CAiNativeRenderer::DoMimeTypeCheckL( MAiPropertyExtension& aPlugin, TInt aContentId )
    {
    MAiContentItemIterator* iter = 
        (MAiContentItemIterator*)aPlugin.GetPropertyL( EAiPublisherContent );
    const TAiContentItem& item = iter->ItemL( aContentId );

    aContentId = GetIntContentId( ContentCid(item) );

    if( !MatchMimeType( aPlugin, aContentId, ContentType(item) ) )
        {
        User::Leave( KErrNotSupported );
        }

    }
    
