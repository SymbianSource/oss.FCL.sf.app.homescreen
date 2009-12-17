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
* Description:   
*
*/



#ifndef C_HNMDLOCALIZATIONELEMENT_H
#define C_HNMDLOCALIZATIONELEMENT_H


#include <e32base.h>
#include <e32hashtab.h> //RHashMap
#include <xmlengelement.h> 
#include <xmlengnodelist.h>


/**
 * Localization Element.
 * 
 * This is the localization element which is wrapped by the localization
 * class. 
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdLocalizationElement) : public CBase
    {
    /**
     * State of element - used for caching purpose 
     */
    enum TLocalizationElementStates {
        EInitialized = 1,
        EWholeFileCached = 2,
        EUsedItemsCached = 4
    };
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully constructed element.
     */
    static CHnMdLocalizationElement* NewL( TXmlEngElement aElement );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aNamespace namespace
     * @param aSource source
     * @return Fully constructed element.
     */
    static CHnMdLocalizationElement* NewL( const TDesC& aNamespace, 
                                           const TDesC& aSource);

    /**
     * Standard C++ virtal destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnMdLocalizationElement();
    
    /**
     * Gets namespace.
     *
     * @since S60 5.0
     * @return Namespace.
     */
    const TDesC& Namespace() const;
    
    /**
     * Gets source.
     *
     * @since S60 5.0
     * @return Source.
     */
    const TDesC& Source() const;
    
    /**
     * Gets source path.
     *
     * @since S60 5.0
     * @return Source.
     */
    HBufC* SourcePath() const;
    
    /**
     * Returns true if path to source exists.
     *
     * @since S60 5.0
     * @return Does path to source exist.
     */
    TBool SourceExists() const;
    
    /**
     * Reloads the resource file path.
     *
     * @since S60 5.0
     */
    void LocateLanguageFileL();

    /**
     * Gets resource id from corresponsing rsg if exist
     * Inside it implemented caching mechanism
     * If file is small (<KWholeFileReadLimit) cache all entries, 
     * otherwise it caches only used resources.
     * 
     * @since S60 5.0
     * @param aResourceName Resource name.
     * @return Resource id if exist.
     */
    const TInt& FindResourceIdL( HBufC8* aResourceName );
    
private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdLocalizationElement();

    /**
     * Standard symbian 2nd pahse constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     */
    void ConstructL( TXmlEngElement aElement );

    /**
     * Standard symbian 2nd pahse constructor.
     *
     * @since S60 5.0
     * @param aNamespace namespace
     * @param aSource source
     */
    void ConstructL( const TDesC& aNamespace, const TDesC& aSource);
    
    /**
     * Sets namespace
     *
     * @since S60 5.0
     * @param aNamespace Namespace.
     */
    void SetNamespaceL( TXmlEngAttr aNamespace );
    
    /**
     * Sets source.
     *
     * @since S60 5.0
     * @param aSource Xml attribute.
     */
    void SetSourceL( TXmlEngAttr aSource );
        
    /**
     * Read corresponding rsg file return content.
     * 
     * @since S60 5.0 
     * @return Descriptor with file content.
     */     
    HBufC8* ReadRsgContentLC( );
    
    /**
     * Parse content of corresponding rsg file and 
     * fill in maping to internal map for all resource listed in content.
     *  
     * @since S60 5.0 
     * @param aContent Descriptor with content.
     */    
    void ParseRsgContentL( const TDesC8& aContent );

    /**
     * Parse content of corresponding rsg file and 
     * fill in maping to internal map only requested resource 
     *
     * @since S60 5.0  
     * @param aContent Descriptor with content.
     * @param aResourceName Requested resource name.
     */        
    void ParseRsgContentL( const TDesC8& aContent, const TDesC8& aResourceName );    
    
private: // data

    /**
     * Namespace.
     */
    RBuf iNamespace;
    
    /**
     * Source.
     */
    RBuf iSource;
    
    /**
     * Source path.
     */
    HBufC* iSourcePath;
    
    /**
     * Mapping between resource name and resource id.
     */
    RHashMap< HBufC8*, TInt> iResourceIDs;
    
    /**
     * State of element. 
     */
    TInt iState;
    };

#endif // C_HNMMLOCALIZATIONELEMENT_H
