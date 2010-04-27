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
* Description:  Base class for renderers in native ui controller.
*
*/


#ifndef M_AINATIVERENDERER_H
#define M_AINATIVERENDERER_H

// System includes
#include <e32base.h>

// User includes
#include <aidevicestatuscontentmodel.h>
#include <aiprofileplugincontentmodel.h>

// Forward declarations
class CHsContentPublisher;
class RFile;

namespace AiNativeUiController
{

/**
 *  Base class for renderers in native ui controller.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiNativeRenderer ) : public CBase
    {
public:

    ~CAiNativeRenderer();

public:
    // new methods
    
    /**
     * Publish resource.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aResource is id of the resource.
     * @param aIndex is index of the content.
     * @return KErrNone if publish is successful.
     */
    virtual TInt Publish( CHsContentPublisher& aPlugin, 
                            TInt aContent, 
                            TInt aResource,
                            TInt aIndex );

    /**
     * Publish unicode text.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aText is published text.
     * @param aIndex is index of the content.
     * @return KErrNone if publish is successful.
     */
    virtual TInt Publish( CHsContentPublisher& aPlugin, 
                            TInt aContent, 
                            const TDesC16& aText,
                            TInt aIndex );

    /**
     * Publish data.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aBuf is published data.
     * @param aIndex is index of the content.
     * @return KErrNone if publish is successful.
     */
    virtual TInt Publish( CHsContentPublisher& aPlugin, 
                            TInt aContent, 
                            const TDesC8& aBuf,
                            TInt aIndex );

    /**
     * Publish content of the file.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aFile is reference to opened file.
     * @param aIndex is index of the content.
     * @return KErrNone if publish is successful.
     */
    virtual TInt Publish( CHsContentPublisher& aPlugin, 
                            TInt aContent, 
                            RFile& aFile,
                            TInt aIndex );

    /**
     * Clean content.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aIndex is index of the content.
     * @return KErrNone if cleaning works.
     */
    virtual TInt Clean( CHsContentPublisher& aPlugin, TInt aContent );

    /**
     * Renderer must implement this method if it needs to support resource publishing.
     *
     * Default implementation leaves with KErrNotFound.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aResource is id of the resource.
     * @param aIndex is index of the content.
     */
    virtual void DoPublishL( CHsContentPublisher& aPlugin, 
                                TInt aContent, 
                                TInt aResource,
                                TInt aIndex );

    /**
     * Renderer must implement this method if it needs to support unicode text publishing.
     *
     * Default implementation leaves with KErrNotFound.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aText is published text.
     * @param aIndex is index of the content.
     */
    virtual void DoPublishL( CHsContentPublisher& aPlugin, 
                                TInt aContent, 
                                const TDesC16& aText,
                                TInt aIndex );

    /**
     * Renderer must implement this method if it needs to support data publishing.
     *
     * Default implementation leaves with KErrNotFound.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aBuf is published data.
     * @param aIndex is index of the content.
     */
    virtual void DoPublishL( CHsContentPublisher& aPlugin, 
                                TInt aContent, 
                                const TDesC8& aBuf,
                                TInt aIndex );

    /**
     * Renderer must implement this method if it needs to support file publishing.
     *
     * Default implementation leaves with KErrNotFound.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     * @param aFile is reference to file client.
     * @param aIndex is index of the content.
     */
    virtual void DoPublishL( CHsContentPublisher& aPlugin, 
                                TInt aContent, 
                                RFile& aFile,
                                TInt aIndex );

    /**
     * Renderer must implement this method if it needs to support content cleaning.
     *
     * Default implementation leaves with KErrNotFound.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContent is id of the content.
     */
    virtual void DoCleanL( CHsContentPublisher& aPlugin, TInt aContent );
    
    /**
     * Called when UI switches to foreground.
     */
    virtual void FocusObtainedL() { };
    
    /**
     * Called when UI switches to background.
     */
    virtual void FocusLostL() { };
    
    /**
     * Called when transaction is committed.
     */
    virtual void TransactionCommittedL() { };
    
    /**
     * Called when keylock state is enabled.
     */
    virtual void KeylockEnabledL() { };
    
    /**
     * Called when keylock state is disabled.
     */
    virtual void KeylockDisabledL() { };
    
    /**
     * LoadUIDefinitionL
     */
    virtual void LoadUIDefinitionL() { };       
    
protected:

    /**
     * Get integer content id using textual id.
     *
     * @since S60 3.2
     * @param aCid is textual content id.
     * @return content id.
     */
    TInt GetIntContentId( const TDesC& aCid ) const;

    /**
     * Does mime type check.
     *
     * @since S60 3.2
     * @param aContentId is content id.
     * @param aMimeType is mime type.
     * @return ETrue if match is successful.
     */
    TBool MatchMimeType( CHsContentPublisher& aPlugin, TInt aContentId, const TDesC8& aMimeType ) const;


    /**
     * Does mime type check.
     *
     * @since S60 3.2
     * @param aPlugin is publishing plugin.
     * @param aContentId is id of the content.
     */
    void DoMimeTypeCheckL( CHsContentPublisher& aPlugin, TInt aContentId );
    
    };

} // namespace AiNativeUiController

#endif // M_AINATIVERENDERER_H
