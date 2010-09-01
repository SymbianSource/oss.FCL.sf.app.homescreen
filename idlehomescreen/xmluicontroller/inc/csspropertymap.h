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
* Description:  Utility for caching CSS property values
*
*/


#ifndef C_CSSPROPERTYMAP_H
#define C_CSSPROPERTYMAP_H


#include <e32base.h>
#include <e32hashtab.h>

class CXnProperty;
class CXnDomPropertyValue;

namespace AiXmlUiController
{

typedef RPtrHashMap< TDesC8, CXnDomPropertyValue > RDomPropertyValueMap;
typedef RPtrHashMap< TDesC8, RDomPropertyValueMap > RElementMap;


/*
 *  @ingroup group_xmluicontroller
 * 
 *  Utility for storing CSS property values. 
 *
 *  @lib AiXmlUiMain
 */
class CCssPropertyMap : public CBase
    {

public:

    static CCssPropertyMap* NewL();

    virtual ~CCssPropertyMap();

    /**
     * Stores property value.
     *
     * @param aElementId Ui element id to associate property value with
     * @param aProperty property to store
     */

    void StorePropertyValueL( const TDesC8& aElementId, CXnProperty& aProperty );
    
    /**
     * Finds property value.
     *
     * @param aElementId Ui element Id to associate property value
     * @param aName property name for lookup
     * @return property value or NULL.
     */
    CXnDomPropertyValue* FindPropertyValue( const TDesC8& aElementId, const TDesC8& aName );

    void Reset();

private:

    CCssPropertyMap();

private: // data

    /**
     * Hashmap of element specific property value maps. Own.
     */
    RElementMap iElementMap;
    
    };

} // namespace AiXmlUiController

#endif // C_CSSPROPERTYMAP_H
