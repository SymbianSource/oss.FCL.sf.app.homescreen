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


#ifndef C_HNMDVALUEBASE_H
#define C_HNMDVALUEBASE_H
 
#include <e32base.h>

class CLiwGenericParamList;
struct THnMdCommonPointers;
class TXmlEngElement;
class CHnAttributeBase;

enum THnMdValueType
    {
    EHnTextType, 
    EHnImageType
    };

/**
 * Value Base
 * 
 * Based class used for output transformation.
 * Specific classes based on information from XML make correct transformation
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdValueBase) : public CBase
    {
public:
    /**
     * Create specific class based on parameter
     * 
     * @param aElement XML element.
     * @param aCmnPtrs Pointer to common pointer structure.
     */
    static CHnMdValueBase* CreateL( const TXmlEngElement& aElement, 
                                THnMdCommonPointers* aCmnPtrs );

    /**
     * Get value from spefic class
     * 
     * @param aQueriesResultsList query results use for dynnamic data
     * @param aPos position if data are in list format.
     * @return A pointer to the CHnAttributeBase object.
     */
    
    virtual CHnAttributeBase* CreateAttributeL( 
            const CLiwGenericParamList* aQueriesResultsList, TInt aPos ) = 0;
            
    };

#endif /*C_HNMDVALUEBASE_H*/
