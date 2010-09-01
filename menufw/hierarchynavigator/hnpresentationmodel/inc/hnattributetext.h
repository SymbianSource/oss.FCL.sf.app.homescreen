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
* Description:   text attribute model
*
*/


#ifndef C_HNATTRIBUTETEXT_H
#define C_HNATTRIBUTETEXT_H

#include "hnattributebase.h"

/**
 *  Image attribute.
 * 
 *  Class defining behaviour of the CHnAttributeBase interface
 *  to support text. The model is responsible for creation 
 *  of the text, appropriately, depending on the definition in the
 *  xml.
 * 
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttributeText ) : public CHnAttributeBase
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttributeText* NewL();

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @return Fully constructed object.
     */
    IMPORT_C static CHnAttributeText* NewLC();

    
    /**
     * Standard C++ virtual destructor.
     */
    ~CHnAttributeText();
    
    /**
     * Sets the attribute value.
     * 
     * @since S60 5.0
     * @param aValue A reference to a descriptor.
     */
    IMPORT_C void SetValueL ( const TDesC8& aValue );

// From base class CHnAttributeBase
public:
    
    /**
     * Returns the value of the attribute.
     *
     * @since S60 5.0
     * @return Value of the attribute.
     */
    const TDesC8& Value( );

private:

    /**
      * Standard C++ constructor.
      * 
      * @since S60 5.0
      */
     CHnAttributeText();

     /**
      * Standard symbian 2nd phase constructor.
      * 
      * @since S60 5.0
      */
     void ConstructL();
    
private:

    /**
     * Attribute value.
     */
    RBuf8 iValue;
    
    };

#endif // C_HNATTRIBUTETEXT_H

