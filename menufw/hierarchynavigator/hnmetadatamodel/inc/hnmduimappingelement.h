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



#ifndef C_HNMDUIAPPINGELEMENT_H
#define C_HNMDUIAPPINGELEMENT_H

#include <e32base.h>
#include <xmlengdom.h>
#include <e32hashtab.h>

class CHnMdValueBase;
class CHnItemModel;
class CLiwGenericParamList;
class CHnConditionInterface;
struct THnMdCommonPointers;

/**
 * UI Mapping Element.
 * 
 * This class is designed to support output attributes defined in the
 * xml configuration. Most common case is when these attributes
 * define the content of the items, e.g.: text, icons.
 * 
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdUiMappingElement) : public CBase
    {
public:
    
    /**
     * Two-phase constructor.
     *
     * @param aElement Element.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdUiMappingElement* NewL( TXmlEngElement aElement,
                                        THnMdCommonPointers* aCmnPtrs );

    /**
     * Two-phase constructor.
     *
     * @param aElement Element.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdUiMappingElement* NewLC( TXmlEngElement aElement,
                                         THnMdCommonPointers* aCmnPtrs );

    /**
     * Standard C++ virtual destructor.
     */
    virtual ~CHnMdUiMappingElement();
    
    /**
     * Gets alias.
     *
     * @return Alias.
     */
    const TDesC8& Alias() const;
    
    /**
     * Fills graphical item.
     * 
     * @param aItemModel Item's model.
     * @param aQueriesResultsList Parameters used when filling.
     * @param aPos Position of a record in a parameters list.
     * @return True if filling was correctly finished.
     */
    TBool FillGraphicalItemL( CHnItemModel* aItemModel ,
            const CLiwGenericParamList& aQueriesResultsList, TInt aPos );
    
    /**
     * Checks if the uimappingelement is valid.
     *
     * @since S60 5.0
     * @param aQueryResults Parameters used when validating.
     * @param aPos Position of a record in a parameters list.
     * @return True if condition was satisfied.
     */
    TBool IsValidL( const CLiwGenericParamList& aQueryResults, TInt aPos );
           
private:

    /**
     * Standard C++ constructor.
     */
    CHnMdUiMappingElement();

    /**
     * Standard symbian 2nd phase constructor.
     *
     * @param aElement Element.
     * @param aCmnPtrs Common pointers.
     */
    void ConstructL( const TXmlEngElement& aElement , 
                     THnMdCommonPointers* aCmnPtrs);
    
    /**
     * Sets string handler.
     *
     * @param aName Name.
     */
    void SetValueL( const TXmlEngAttr& aName);
    
    /**
     * Sets type.
     *
     * @param aType Type.
     */
    void SetTypeL( const TXmlEngAttr& aType );
    
    /**
     * Set alias.
     *
     * @param aName Name.
     */
    void SetAliasL( const TXmlEngAttr& aName );
    
    /**
     * Set value.
     *
     * @param aElement Element.
     * @param aCmnPtrs Common parameters.
     */    
    void SetValueL( const TXmlEngElement& aElement,
            THnMdCommonPointers* aCmnPtrs );
    

private: // data

    /**
     *
     * Alias.
     */
    RBuf8 iAlias;
    
    /**
     * ValueL.
     */
    CHnMdValueBase* iValue;
    
    /**
     * Not own - common pointers structure.
     */
    THnMdCommonPointers* iCmnPtrs;
    
    /**
     * Own - condition.
     */
    CHnConditionInterface* iCondition;
    };

#endif // C_HNMUIAPPINGELEMENT_H
