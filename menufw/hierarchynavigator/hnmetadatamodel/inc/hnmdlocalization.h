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



#ifndef C_HNMDLOCALIZATION_H
#define C_HNMDLOCALIZATION_H


#include <e32base.h>
#include <xmlengelement.h> 

class CHnMdLocalizationElement;
class CCoeEnv;

/**
 * Localization.
 * 
 * This class is designed to support localization.
 *
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdLocalization) : public CBase
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @return Fully constructed object.
     */
    static CHnMdLocalization* NewL();

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @return Fully constructed object.
     */
    static CHnMdLocalization* NewLC();

    /**
     * Standard C++ virtual destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnMdLocalization();
    
    /**
     * Appends localizations.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     */
    void AppendLocalizationsL( TXmlEngElement aElement );

    /**
     * Gets element by name.
     *
     * @since S60 5.0
     * @param aNamespace Namespace.
     * @return Localization element.
     */
    const CHnMdLocalizationElement* ElementByNamespace(
            const TDesC& aNamespace ) const;
            
    /**
     * Loads resource.
     * Resources can be defined in formats 
     * @code <namespace>:<id> @endcode
     * Namespaces can be defined by localization elements,
     * but can also point to rsg file.
     * Id can be rsg constants, but it can be also numbers.
     *
     * @since S60 5.0
     * @param aResourceName Resource name.
     * @param aDesParams descriptor parameters to format %U, %0U.
     * @param aIntParams integer parameters to format %N, %0N.* 
     * @return Descriptor.
     */
     HBufC* LoadL( const TDesC& aResourceName, 
                   const CDesC16Array* aDesParams, 
                   const CArrayFix<TInt>* aIntParams );
private:

    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdLocalization();

    /**
     * Standard symbian 2nd pahse constructor.
     *
     * @since S60 5.0
     */
    void ConstructL();
    
    /**
     * Appends element the list of localization elements, it also checks for duplicates
     * if it is RSC file it load the file in CoeEnv
     * 
     * @param aElement element to add, ownership is taken
     *      if element with the same namespace exists element is ignored
     *
     * @since S60 5.0
     */
    void AppendElementL( CHnMdLocalizationElement*  aElement );

    /**
     * Check extension of file and decide it is a resource file
     * 
     * @param aFilename file name to check 
     * @return ETrue if it is rsc file, else EFalse
     */
    TBool IsResourceFile( const TDesC&  aFilename );
        
    /**
     * Formats text.
     * 
     * @since S60 5.0
     * @param aTextToFormat A text to format.
     * @param aDesParams Parameters of the text to be formated.
     * @param aIntParams Integer parameters of the text to be formated.
     */
    HBufC* FormatTextL( const TDesC& aTextToFormat,
                        const CDesC16Array* aDesParams, 
                        const CArrayFix<TInt>* aIntParams );
    
    /**
     * Checks whether the localization element identified by a namespace is
     * duplicated.
     *
     * @param aNamespace Namespace.
     */
    TBool IsDuplicateL( TDesC8& aNamespace );
    
    /**
     * Checks whether the localization element identified by a namespace is
     * duplicated.
     *
     * @param aNamespace Namespace.
     */
    TBool IsDuplicateL( TDesC& aNamespace );
    
    /**
     * Checks whether the localization element identified by a namespace is
     * internal.
     *
     * @param aName Name.
     */
    TBool IsInternalL( const TDesC& aName );
    
private: // data

    /**
     * CoeEnv.
     * Not own.
     */
    CCoeEnv* iCoeEnv;

    /**
     * Own - Internal offset.
     */
    RArray< TInt > iInternalOffset;
    
    /**
     * Own - Internal localization.
     */
    RPointerArray<CHnMdLocalizationElement> iInternalLocalization;

    };

#endif // C_HNMMLOCALIZATION_H
