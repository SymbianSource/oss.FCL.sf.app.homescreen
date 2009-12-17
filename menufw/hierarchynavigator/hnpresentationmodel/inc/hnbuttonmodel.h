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
* Description:   toolbar button model
*
*/


#ifndef C_HNBUTTONMODEL_H
#define C_HNBUTTONMODEL_H

#include <e32base.h>

class CGulIcon;
class CHnAttributeBase;

/**
 *  Button model class.
 * 
 *  Class represent a toolbar button. It contains all of the
 *  information essential to displaying a button in the 
 *  presentationlayer.
 *
 *  @lib hnpresentationmodel
 *  @since S60 5.0
 *  @ingroup group_hnpresentationmodel
 */
NONSHARABLE_CLASS( CHnButtonModel ) : public CBase
    {
public:

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aIndex Button index in the toolbar.
     */
    IMPORT_C static CHnButtonModel* NewLC( TInt aIndex );
    
    /**
     * Virtual Destructor.
     * 
     * @since S60 5.0
     */
    IMPORT_C ~CHnButtonModel();
    
    /**
     * Gets icon model.
     * 
     * @since S60 5.0
     * @return Icon for button, NULL if not exist
     */
    IMPORT_C CGulIcon* GetIcon() const;
    
    /**
     * Sets icon for button model.
     * 
     * @since S60 5.0
     * @param aIcon Icon for button, transfers ownership
     */
    IMPORT_C void SetIcon( CHnAttributeBase* aAttribute );
    
    /**
     * Gets tooltip for button model.
     * 
     * @since S60 5.0
     * @return Help text corresponding to button mdoel
     */
    IMPORT_C const TDesC& GetButtonText() const;
    
    /**
     * Sets help text for button model.
     * 
     * @since S60 5.0
     * @param aToolTip Text to display
     */
    IMPORT_C void SetHelpTextL( const TDesC& aToolTip );
    
    /**
     * Gets dimmed.
     * 
     * @since S60 5.0
     * @return iDimmed
     */
    IMPORT_C TBool GetDimmed() const;
    
    /**
     * Sets dimmed
     * 
     * @since S60 5.0
     * @param aDimmed Sets button state to dimmed
     */
    IMPORT_C void SetDimmed( TBool aDimmed );
    
    /**
     * Gets index of button model.
     * 
     * @since S60 5.0
     * @return Index
     */
    IMPORT_C TInt GetIndex() const;
    
    /**
     * Gets command for button model.
     * 
     * @since S60 5.0
     * @return Event id
     */
    IMPORT_C TInt GetEventId() const;
    
    /**
     * Sets command for button model.
     * 
     * @since S60 5.0
     * @param aEventId Id of event to invoke when button pressed
     */
    IMPORT_C void SetEventId( TInt aEventId );
    
    /**
     * Removes all  LIW objects owned by this object.
     * 
     * LIW objects owned by non-LIW objects that are owned by
     * this object are also removed.
     * @since S60 5.0
     */
    void RemoveLiwObjects();
    
private:

    /**
     * Default constructor.
     * 
     * @since S60 5.0
     * @param aIndex Index of the button, being created.
     */
    CHnButtonModel( TInt aIndex );
    
    /**
     * Second phase constructor.
     * 
     * @since S60 5.0
     */
    void ConstructL();
    
private: // data
    
    /**
     * Index of button, indicates the position on the toolbar.
     * Values: 0, 1, 2 applicable. 
     */
    TInt iIndex;
        
    /**
     * Event Id. 
     */
    TInt iEventId;
    
    /**
     * Text to be displayed as help for button.
     * Own
     */
    RBuf iToolTip;
    
    /**
     * Indicates if button should be dimmed.
     */
    TBool iDimmed;
    
    /**
     * Icon displayed on the button.
     * Own
     */    
    //CGulIcon* iIcon;
    
    /**
     * Own - icon attribute;
     */
    CHnAttributeBase* iIconAttribute;
    
    };
    
#endif // C_HNBUTTONMODEL_H
