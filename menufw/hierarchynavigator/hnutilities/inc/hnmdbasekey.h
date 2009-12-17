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


#ifndef C_HNMDBASEKEY_H
#define C_HNMDBASEKEY_H

#include <e32base.h>
#include <e32hashtab.h>
#include <xmlengdom.h>

class CLiwDefaultMap;
class CLiwGenericParamList;
class TLiwVariant;
class CHnMdBaseKey;

/**
 *  Abstract Class representing keys used in XML configuration.
 *  These keys can become parameters for LIW requests for example.
 *  Deriving classes defines particular key types like integers or strings.
 *
 *  @lib hierarchynavigatorengine
 *  @since S60 v5.0
 *  @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( CHnMdBaseKey ) : public CBase
    {
    friend class HnMdKeyFactory; 
    
public:

    /**
     * Standard destructor.
     */
    virtual ~CHnMdBaseKey();
    
    /**
     * Factory method.
     *
     * since S60 v5.0
     * @return Fully constructed object.
     */
    virtual CHnMdBaseKey* CopyLC() = 0;
    
    /**
     * Getter - key name.
     * since S60 v5.0
     * 
     * @return Key name.
     */     
    const TDesC8& KeyName() const;
    
    /**
     * Getter - key content.
     * since S60 v5.0
     * 
     * @return Key content.
     */ 
    virtual const TDesC& KeyContent() const;
    
    /**
     * Getter - sub keys.
     * 
     * @since S60 v5.0
     * @return Sub keys array.
     */
    const RPointerArray<CHnMdBaseKey>& SubKeys() const;
    
    /**
     * Getter - key content.
     *
     * @since S60 v5.0
     * @param aContent Output content.
     */
    virtual void GetKeyContent( TInt& aContent ) const;
    
    /**
     * Getter - key content.
     *
     * @since S60 v5.0
     * @param aContent Output content.
     */
    virtual void GetKeyContent( TInt64& aContent ) const;
     
    /**
     * Evaluates key - sets params.
     * 
     * @since S60 v5.0
     * @param aParamList List of parameters.
     * @param aPos Position of the item in the items group.
     * 
     */
    IMPORT_C CHnMdBaseKey* EvaluateLC(
        const CLiwGenericParamList& aParamList,
        TInt aPos = 0 );
  
    /**
     * Change to variant.
     * 
     * @since S60 v5.0
     * @param aRet Output variant.
     * 
     */
    IMPORT_C virtual void ToVariantL( TLiwVariant& aRet ) const;
     
    /**
     * Change to generic param list.
     * 
     * @since S60 v5.0
     * @param aRet Output generic param list.
     * 
     */
    IMPORT_C virtual void ToGenericParamListL( CLiwGenericParamList & aRet ) const;
        
    /**
     * Adds a sub key.
     * 
     * @since S60 v5.0
     * @param aKey New key.
     */
    IMPORT_C void AddSubKeyL( CHnMdBaseKey* aKey );
    
    
protected:

    /**
     * Standard constructor.
     * 
     * @since S60 v5.0
     */
    CHnMdBaseKey();
       
    /**
     * Symbian constructor for performing 2nd stage construction.
     * 
     * @since S60 v5.0
     * @param aElement XML DOM Element.
     */
    void BaseConstructL( TXmlEngElement aElement );
    
    /**
     * Symbian constructor for performing 2nd stage construction.
     * 
     * @since S60 v5.0
     * @param aKey Key.
     */
    void BaseConstructL( const CHnMdBaseKey* aKey );
        
    /**
     * Sets key name.
     * 
     * @since S60 v5.0
     * @param aName Name of new key.
     */
    void SetKeyNameL( const TXmlEngAttr& aName );
    
    /**
     * Sets key content.
     *
     * @since S60 v5.0
     * @param aContent XML Attribute.
     */
    void SetKeyContentL( const TXmlEngAttr& aContent );

    /**
     * Sets sub keys.
     *
     * @since S60 v5.0
     * @param aElement XML DOM Element.
     */
    void SetSubKeysL( const TXmlEngElement& aElement );
    
    /**
      * Sets sub keys.
      *
      * @since S60 v5.0
      * @param aKey New key.
      */
    void SetSubKeysL( const CHnMdBaseKey* aKey );
    
    /**
     * Sets value.
     *
     * @since S60 v5.0
     * @param aElement XML DOM Element.
     */
    void SetValueL( const TXmlEngElement& aElement );
    
    /**
     * Sets value.
     *
     * @since S60 v5.0
     * @param aValue New value.
     */
    void SetValueL( const TDesC8& aValue );

	    /**
     * Sets value.
     *
     * @since S60 v5.0
     * @param aValue New value.
     */
    void SetValueL( const TDesC& aValue );
	
    /**
     * Sets key or name.
     *
     * @since S60 v5.0
     * @param aElement XML DOM Element.
     */
    void SetKeyOrNameL( const TXmlEngElement& aElement );
    
    /**
     * Sets key name.
     * 
     * @since S60 v5.0
     * @param aName New key name.
     */
    void SetKeyNameL( const TDesC8& aName );
    
    /**
     * Evaluates key - sets params.
     * 
     * @since S60 v5.0
     * @param aParamList List of parameters.
     * @param aPos Position in the param list. Default is 0.
     */
    void DoEvaluateL( const CLiwGenericParamList& aParamList,
                        TInt aPos = 0 );
    
    /**
     * Evaluates the key value.
     * 
     * @since S60 v5.0
     * @param aParamList List of parameters.
     * @param aPos Position in the param list. Default is 0.
     */
    virtual void EvaluateKeyValueL( const CLiwGenericParamList& aParamList,
                        TInt aPos = 0 );
    
protected: // data
    
    /**
     * Key name (8-bit descriptor).
     * Own. 
     */
    RBuf8 iKeyName;

    /**
     * Key content.
     * Own. 
     */
    RBuf iContent;
       
   /**
    * Subkeys (i.e. to handle with actions).
    * Own. 
    */
    RPointerArray<CHnMdBaseKey> iSubKeys;

    };

#endif // C_HNMDBASEKEY_H
