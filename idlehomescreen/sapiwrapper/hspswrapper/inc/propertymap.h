/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates name and value
*
*/


#ifndef C_CPROPERTYMAP_H
#define C_CPROPERTYMAP_H


#include <e32base.h>


namespace hspswrapper{



/**
 *  Presents property as name and value pair
 *
 *  @code
 *  @endcode
 *
 *  @lib xnlayoutengine.lib
 *  @since S60 v5.0
 */
class CPropertyMap : public CBase
    {
public:
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CPropertyMap* NewL();
    IMPORT_C static CPropertyMap* NewLC();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CPropertyMap();

    /**
     * Set property name
     *
     * @param aName Property nane
     * @return Reference to this object
     */
    IMPORT_C CPropertyMap& SetNameL( const TDesC8& aName );
    
    /**
     * Get property name
     *
    * @return Reference to name or KNullDesC8
     */
    IMPORT_C const TDesC8& Name()const;
    
    /**
     * Set property value
     *
     * @param aValue Property value
     * @return Reference to this object
     */
    IMPORT_C CPropertyMap& SetValueL( const TDesC8& aValue );
    
    /**
     * Get property value
     *
    * @return Reference to value or KNullDesC8
     */
    IMPORT_C const TDesC8& Value()const;

private:

    CPropertyMap();

    void ConstructL();


private: // data

    /**
     * Own. Property name
     */
    HBufC8* iName;
    /**
     * Own. Property value
     */
    HBufC8* iValue;
    };

}

#endif // C_CPROPERTYMAP_H
