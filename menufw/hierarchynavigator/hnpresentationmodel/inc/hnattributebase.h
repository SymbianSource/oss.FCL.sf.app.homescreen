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
* Description:   abstract base class for attribute models
*
*/


#ifndef C_HNATTRIBUTEBASE_H
#define C_HNATTRIBUTEBASE_H

#include <e32base.h>
#include "hniconholder.h"

class CGulIcon;

/** 
 * Attribute type. Indicates the text or image attribute. 
 */
enum TAttributeType 
    {
    ETextAttribute,
    EImageAttribute
    };

/**
 * Attribute base (abstract) class.
 * 
 * Base class for attributes providing common functionality 
 * and defining required abstract methods for the interface.
 * Attributes are used to logically mediate visual properties, 
 * such as icons, texts, templates, etc., from the xml to the 
 * presentation layer. Atributes are used eg., to retrieve content 
 * of such visual items in the widget library (mmwidgets), 
 * for display purposes. 
 *
 * @lib hnpresentationmodel
 * @since S60 5.0
 * @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnAttributeBase ) : public CBase
     {
public:
    
    /**
     * Standard destructor.
     * 
     * @since S60 5.0
     */
    ~CHnAttributeBase();
    
    /**
     * Sets the attribute name.
     *
     * @since S60 5.0
     * @param aName Name of the attribute
     */
    IMPORT_C void SetNameL( const TDesC8& aName );

    /**
     * Gets the name of the attribute.
     *
     * @since S60 5.0
     * @return Attribute name.
     */
    IMPORT_C const TDesC8& Name() const;

    /**
     * Gets the value of the attribute.
     *
     * @since S60 5.0
     * @return Attribute's value.
     */
    IMPORT_C virtual const TDesC8& Value();
    
    /**
     * Gets icon, packed in CGulIcon.
     * 
     * @see CGulIcon
     * @since S60 5.0
     * @return Icon as a poitner to CGulIcon.
     */
    IMPORT_C virtual CGulIcon* GetIcon( TSize* aDesiredIconSize 
            = NULL );
    
    /**
     * Gets icon holder, which owns the same icon that GetIcon returns.
     * 
     * Icon holder should be used when the icon from model needs to
     * be stored. Please see @c CHnIconHolder description for more
     * details on how to store icons.
     * 
     * @param aDesiredIconSize Desired size of the icon contained in
     *                         the returned icon holder.
     * @return An icon holder. You can call GetGulIcon on it to get the
     *         actual icon (may return NULL).
     */
    virtual CHnIconHolder* GetIconHolder( TSize* aDesiredIconSize );
    
    /**
     * Gets attributes's type.
     * 
     * @since S60 5.0
     * @return Attribute's type.
     */    
    IMPORT_C TAttributeType Type();
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    virtual void RemoveLiwObjects();

protected: // data
    
    /**
     * Attribute type.
     */
    TAttributeType iAttributeType;
    
private: // data

    /**
     * Attribute name. 
     */
    RBuf8 iName;
    
    };

#endif // C_HNATTRIBUTEBASE_H

