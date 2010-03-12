/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  List query dialog adapter and data interface
*
*/

#ifndef __XNLISTQUERYDIALOGADAPTER_H__
#define __XNLISTQUERYDIALOGADAPTER_H__

//  INCLUDES
#include "xncontroladapter.h"
#include <e32base.h>

#include "xncomponent.h"
#include "xnnode.h"
#include "mxncomponentinterface.h"

// FORWARD DECLARATIONS
class CXnNodePluginIf;
class CXnUiEngine;
class CAknListQueryDialog;

// CLASS DECLARATION

namespace XnListQueryDialogInterface
{
_LIT8(KType, "listquerydialog");
class MXnListQueryDialogInterface : public XnComponentInterface::MXnComponentInterface
   {

public:
        
    static inline const TDesC8& Type()
        {
        return KType;
        }
        
public: // New functions

    /**
     * Replaces or appends new item into list
     * @param aText The new text line
     * @param aIndex index
     */
    virtual void ReplaceItemL(const TDesC& aText, TInt aIndex ) = 0;
    
    /**
     * Inserts or appends new item into list
     * @param aText The new text line
     * @param aIndex index
     */
    virtual void InsertItemL(const TDesC& aText, TInt aIndex ) = 0;
    
    /**
     * Deletes the item from the given index
     * @param aIndex list index
     */
    virtual void DeleteItem( TInt aIndex ) = 0;
   };
 }   

/**
 *  List query dialog data interface 
 *  @ingroup group_xnlayoutengine
 *  @lib xnlayoutengine.lib
 *  @since 9.2
 */
class CXnListQueryDialog : public CXnComponent, public XnListQueryDialogInterface::MXnListQueryDialogInterface
   {

public:


    /**
     * 2 phase construction.
     */
    static CXnListQueryDialog* NewL();


    /**
     * Destructor.
     */
    virtual ~CXnListQueryDialog();

public: // From MXnListQueryDialogInterface

    /**
     * Replaces or appends new item into list
     * @param aText The new text line
     * @param aIndex index
     */
    void ReplaceItemL(const TDesC& aText, TInt aIndex );

    /**
     * Inserts or appends new item into list
     * @param aText The new text line
     * @param aIndex index
     */
    void InsertItemL(const TDesC& aText, TInt aIndex );
    
    /**
     * Deletes the item from the given index
     * @param aIndex list index
     */
    void DeleteItem( TInt aIndex );
    

public: // from MXnComponentInterface

    /**
      * Create a component interface according to the given type.
      * @param aType Type of the interface to create
      * @return Created interface or NULL if the provided type is not supported.
      */ 
    XnComponentInterface::MXnComponentInterface* MakeInterfaceL(const TDesC8& aType);
    
private:

    CXnListQueryDialog();
    void ConstructL();

   };

// CLASS DECLARATION

/**
*  @ingroup group_xnlayoutengine
*  List query dialog adapter interface class
*  @lib xnlayoutengine.lib
*  @since Series 60 5.0
*/
NONSHARABLE_CLASS( CXnListQueryDialogAdapter ) : public CXnControlAdapter
	{
	public: // Constructors and destructor
		/**
        * Two-phased constructor.
        */
		static CXnListQueryDialogAdapter* NewL( CXnNodePluginIf& aNode );
		
        /**
        * Destructor.
        */
		~CXnListQueryDialogAdapter();
		
		/**
         * Replaces or appends new item into list
         * @param aText The new text line
         * @param aIndex index
         */
	    void ReplaceItemL(const TDesC& aText, TInt aIndex );
	    
	    /**
         * Inserts or appends new item into list
         * @param aText The new text line
         * @param aIndex index
         */
	    void InsertItemL(const TDesC& aText, TInt aIndex );
	    
	    /**
         * Deletes the item from the given index
         * @param aIndex list index
         */
	    void DeleteItem( TInt aIndex );
	    
	    /**
	     * Displays the dialog
	     */
		void TryDisplayingDialogL();      

    private: // Constructors
		/**
        * C++ default constructor.
        */
        CXnListQueryDialogAdapter( CXnNodePluginIf& aNode );

	   	/**
        * Two-phased constructor.        
        */ 
	    void ConstructL();	   
	    
    private: // New functions
        /**
        * Sets the selected item active, which runs associated event.
        */ 
        void ActivateItemL( TInt aIndex );
        
        /**
        * Inserts the index into event.
        */ 
        void ModifyDynamicEventL( TInt aIndex );
            
    private:    // Data	
        
        // Dialog
        CAknListQueryDialog* iDialog;
        
        // Node 
        CXnNodePluginIf& iNode;
        /**
         * List items
         * own
         */
        CDesCArrayFlat* iItemArray;
        
        /**
         * Array of static item nodes
         * Not own
         */
        RPointerArray< CXnNodePluginIf > iStaticItems;
        
        /**
         * Dynamic item node
         * Not own
         */
        CXnNodePluginIf* iDynamicItem;
        
	};

#endif // __XNLISTQUERYDIALOGADAPTER_H__

// End of File
