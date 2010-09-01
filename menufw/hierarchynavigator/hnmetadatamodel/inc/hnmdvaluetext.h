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


#ifndef CHNMDTEXT_H
#define CHNMDTEXT_H

#include <e32base.h>

#include "hnmdvaluebase.h"
#include "hnglobals.h"

class TXmlEngElement;
class CLiwGenericParamList;
struct THnMdCommonPointers;
class CDesC16Array;
class CDesC8Array;

// CLASS DECLARATION

/**
 * Value Text
 * 
 * Specific class inherigint from CHnMdValue Base. This
 * class is responsible for texts: implements text handling in UI mapping phase
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdValueText) : public CHnMdValueBase
    {
public:

    /**
     * Two-phased constructor.
     * 
     * @param aElement XML element.
     * @param aCmnPtrs Common pointers.
     */
    static CHnMdValueText* NewL( const TXmlEngElement& aElement,
            THnMdCommonPointers* aCmnPtrs );
        
    /**
     * Destructor.
     */
    ~CHnMdValueText();

    /**
     * From CHnMdValueBase.
     * 
     * Retruns localized formatted text.
     * 
     * @param aQueriesResultsList Parameters used when getting localized formatted text. 
     * @param aPos Position of the correspoing record in the parameters list. 
     * @param aValue Localized formatted text.
     * @return Error code.
     */
    TInt GetL( const CLiwGenericParamList* aQueriesResultsList,
            TInt aPos, TPtrC& aValue );
    
    /**
     * Creates attribute.
     * 
     * @param aQueriesResultsList Parameters used when creating an attribute. 
     * @param aPos Position of the correspoing record in the parameters list. 
     * @return Attribute object.
     */
    CHnAttributeBase* CreateAttributeL( const CLiwGenericParamList* aQueriesResultsList,
                TInt aPos );

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CHnMdValueText();
    
    /**
     * EPOC default constructor for performing 2nd stage construction.
     * 
     * @param aElement XML element.
     * @param aCmnPtrs Common pointers.
     */
    void ConstructL( const TXmlEngElement& aElement,
                     THnMdCommonPointers* aCmnPtrs );

    /**
     * Fills list of parameters for text formating.
     * 
     * @param aQueryList query results use for dynamic data.
     * @param aPos position if data are in list format.
     * @param aDesParams array of descriptor parameters to be filled .
     * @param aNumericParams array of numeric parameters to be filled  .
     */
    void FillParamsL( const CLiwGenericParamList* aQueryList, TInt aPos,
                      CDesC16Array& aDesParams, CArrayFix<TInt>& aNumericParams);
    
    
    /**
     * Gets value from resource file
     *
     * @param aQueriesResultsList Query results use for dynamic data
     * @param aPos Position if data are in list format
     * @param aValue Return value
     * @return Error code      
     */
    TInt GetFromResourceFileL( const CLiwGenericParamList* aQueriesResultsList,
            TInt aPos, RBuf& aValue );
    
    
    /**
     * Gets simple value.
     *
     * @param aQueriesResultsList Query results use for dynamic data.
     * @param aPos Position if data are in list format.
     * @param aValue Return value.
     * @return Error code.
     */
    TInt GetSimpleValueL( const CLiwGenericParamList* aQueriesResultsList,
            TInt aPos, RBuf& aValue );
    
private: // data
    /**
     * Container of common pointers used in Hierarchy navigator
     * Not own.
     */
    THnMdCommonPointers* iCmnPtrs;
    
    /**
     * Simple value, it can be unlocalized text,
     * or text with localization namespace
     * or path to query.
     */
    RBuf8 iSimpleValue;
    
    /**
     * Resource file name or path for query.
     * Own.
     */
    RBuf8 iResourceFile;
    
    /**
     * Resource id value or path for query.
     * Own.
     */
    RBuf8 iResourceId;
    
    /**
     * It is cached value that is returned to client of this class.
     * Own.
     */
    RBuf iCachedValue;
    
    /**
     * Parameters list defined in XML, it should be path to query.
     * Own.
     */
    CDesC8Array* iParameters;
    
    /**
     * Cached list of descriptor parameters used for formating.
     * Own.
     */
    CDesC16Array* iCachedDesParameters;
    
    /**
     * Cached list of numeric parameters used for formating.
     * Own.
     */
    CArrayFix<TInt>* iCachedIntParameters;
    
    };

#endif // CHNMDTEXT_H
