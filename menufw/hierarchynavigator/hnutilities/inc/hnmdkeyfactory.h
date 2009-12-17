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


#ifndef HNMDKEYFACTORY_H
#define HNMDKEYFACTORY_H

#include <e32base.h>
#include <xmlengdom.h>

class CHnMdBaseKey;

/**
 * Provides static methods for creating particular key classes.
 *
 * @lib hnutilities.lib
 * @since S60 v3.0
 * @ingroup group_hnutilities
 */
NONSHARABLE_CLASS( HnMdKeyFactory )
    {  
public:

    /**
     * Creates particular key type depending on XML element.
     * Sets name and value of the key read also from XML. 
     * 
     * since S60 v5.0
     * @param aElement XML DOM Element.
     * @return Pointer to created key.
     */
    IMPORT_C static CHnMdBaseKey* CreateLC( TXmlEngElement aElement );
    
    /**
     * Creates particular key type depending on XML element.
     * Sets name and value of the key read also from XML. 
     *
     * since S60 v5.0
     * @param aElement XML DOM Element.
     * @return Pointer to created key.
     */
    IMPORT_C static CHnMdBaseKey* CreateL( TXmlEngElement aElement );
    
    /**
     * Creates particular key type depending on passed argument.
     * Sets name and value of the key. 
     * 
     * since S60 v5.0
     * @param aName Key name.
     * @param aType Key type.
     * @param aValue Key value.
     * @return Pointer to created key.
     */
    IMPORT_C static CHnMdBaseKey* CreateL(
        const TDesC8& aName,
        const TDesC8& aType,
        const TDesC8& aValue );

    /**
    * Creates particular key type depending on passed argument.
    * Sets name and value of the key. 
    * 
    * since S60 v5.0
    * @param aName Key name.
    * @param aType Key type.
    * @param aValue Key value.
    * @return Pointer to created key.
    */
   IMPORT_C static CHnMdBaseKey* CreateL(
       const TDesC8& aName,
       const TDesC8& aType,
       const TDesC& aValue );    
    
 private:
 
    /**
     * Creates particular key type depending on passed argument.
     *
     * since S60 v5.0
     * @param aKeyType Key type.
     * @return Pointer to created key.
     */
    static CHnMdBaseKey* CreateL( const TDesC8& aKeyType );
    };

#endif // HNMDKEYFACTORY_H
