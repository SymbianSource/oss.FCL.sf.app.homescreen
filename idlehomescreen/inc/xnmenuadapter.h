/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _XNMENUADAPTER_H
#define _XNMENUADAPTER_H

//  INCLUDES
#include "xncontroladapter.h"
#include <eikmobs.h>
#include <eikmenup.h>
#include "xnmenu.h"

// FORWARD DECLARATIONS
class TXnUiEnginePluginIf;
class CXnNodePluginIf;
    
// CONSTANTS

_LIT8(KXnMenuBar, "menubar");
_LIT8(KXnMenuItem, "menuitem");
_LIT8(KXnDynMenuItem, "dynmenuitem");
_LIT8(KXnWidgetMenuItem, "widgetmenuitem");
_LIT8(KXnWidgetSoftkey, "widgetsoftkey");
_LIT8(KXnMenu, "menu");
_LIT8(KXnLabel, "label");
_LIT8(KXnImage, "image");
_LIT8(KXnImagePath, "path");
_LIT8(KXnMenuExtension, "menuextension");
_LIT8(KXnMenuItemSettings, "hs_widget_menuitem_settings");
_LIT8(KXnSource, "source");
_LIT8(KXnTarget, "target");
    
// CLASS DECLARATION
class TXnMenuItem
    {
    public :
        CXnNodePluginIf* iNode;
        TBool iAutomaticSP;
        TInt iParentIndex;
        CEikMenuPaneItem::SData iData;
        TBool iIsCascade;
    };

// CLASS DECLARATION

class CXnSoftkeyItem;
/**
*  @ingroup group_xnmenufactory
*  @lib xn3menufactory.dll
*  @since S60 3.1
*/
class CXnMenuAdapter : public CXnControlAdapter, public MEikMenuObserver
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         */
        static CXnMenuAdapter* NewL( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );
        
        /**
         * Destructor.
         */
        virtual ~CXnMenuAdapter();

    public: // new functions
        virtual void SetContainerL(CEikButtonGroupContainer& aContainer);
        virtual TBool IsMenuFocused();

        /**
         * Closes the menu, if it is open.  
         * @since Series 60 3.2
        */
        virtual void StopDisplayingMenu();

        /**
        * Sets soft key image. If there was no leave, menu takes  
        * bitmap and mask ownership.
        * @since Series 60 3.2
        * @param aBitmap Bitmap to draw
        * @param aMask Mask to use
        * @param aPos Softkey position
        * @param aPreserveAspectRatio ETrue if image's aspect ratio is preserved
        * @param aInvertMask ETrue if white mask is used, EFalse if black mask is used
        * @param aTransferOwnership ETrue if bitmap and mask ownerhsip is transferred to menuadapter
        */        
        void SetSoftKeyImageL(
            CFbsBitmap* aBitmap,
            CFbsBitmap* aMask,
            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos,
            CXnNodeAppIf* aNode,
            TBool aPreserveAspectRatio, TBool aInvertMask,TBool aTransferOwnership);

        /**
        * Sets soft key image.
        * @param aId icon bitmap IID 
        * @param aBmpFile file path.
        * @param aBmp Bitmap ID.
        * @param aBmpM Bitmap mask ID.
        * @param aPos Softkey position.
        * @param aEnable ETrue for set icon, EFalse for use default icon.
        */
        void SetSoftKeyImageL( const TAknsItemID& aId,
                               const TDesC& aBmpFile,
                               const TInt32 aBmp,
                               const TInt32 aBmpM,
                               XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos, 
                               TBool aEnable );

        /**
        * Sets softkey text. 
        * @since Series 60 3.2
        * @param aText Text to draw    
        * @param aPos Softkey position
        */                       
        void SetSoftKeyTextL( const TDesC& aText,
            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos );           

        /**
        * Sets softkey. 
        * @param aNode Softey node. 
        * @param aPos Softkey position.
        */                       
        void SetSoftKeyL( CXnNodePluginIf* aSoftkeyNode,
            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos );           

        /**
        * Gets currently visible softkey. 
        * @param aPos Softkey position.
        * @return Softey node. 
        */                       
        CXnNodePluginIf* SoftKeyL( XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos );

        /**
         * Finds softkey node by given position
         * @since Series 60 5.0         
         */
        virtual TBool FindSoftKeyNodeByPosition( const TPoint& aPosition, CXnNodePluginIf*& aNode );

        /**
         * Tries to display menubar        
         * @since Series 60 5.1    
         * @param aMenuNodeId the menu node id
         */        
        void TryDisplayingMenuL( const TDesC& aMenuNodeId );
       
    public: // from CCoeControl

        /**
         * See CCoeControl documentation
         */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
        
        /**
         * See CCoeControl documentation
         */
        virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        
        /**
         * See CCoeControl documentation
         */
        virtual void HandleResourceChange(TInt aType);
        
        /**
        * From CCoeAppUiBase. Informs the components about the screen layout switch.
	    */
        virtual void HandleScreenDeviceChangedL();
    
    public: // from MEikMenuObserver
    

        /**
         * Will panic if we don't override it, because we don't have any resource for it.
         * Default implementation will read resources 
         */
        virtual void RestoreMenuL(CCoeControl* aMenuControl,TInt aResourceId,TMenuType aType);
        
        /**
         * Initialize menu pane dynamically
         */
        virtual void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
        
        /**
         * See CCoeControl documentation
         */
        virtual void SetEmphasis(CCoeControl* aMenuControl,TBool aEmphasis);
    
        /**
         * See CCoeControl documentation
         */
	    virtual void ProcessCommandL(TInt aCommandId);
	    
	    /**
        * This is done when application goes to background. 
        */ 
	    void DoEnterPowerSaveModeL(TModeEvent aEvent);
	    
	    /**
        * This is done when application goes to foreground. 
        */ 
        void DoExitPowerSaveModeL(TModeEvent aEvent);
        
		/*
		 * Handles menu property change
		 */
		virtual void HandleMenuPropertyChangeL(CXnNodePluginIf* aNode, CXnProperty* aProperty);
    
		/*
		 * Sets a dynamic menuitem initialisation observer
		 */		
		void SetObserver( XnMenuInterface::MXnMenuObserver& aObserver );

		/*
		 * Removes the dynamic menuitem initialisation observer
		 */		
		void RemoveObserver();

    private:

        /**
        * C++ default constructor.
        */
        CXnMenuAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CXnControlAdapter* aParent, CXnNodePluginIf& aNode );
        
        /**
         * Populate menu from nodes
         */
        void PopulateMenuL();
        
        /**
         * Create menu items. Recursive function
         */
        TInt AddMenuItemL( TInt aParentIndex, CXnNodePluginIf* aNode,
                           RPointerArray< const TDesC8 >& aGroups );
                
        /**
         * Finds softkey item by position
         */                                			        			 
        CXnNodePluginIf* FindSoftkeyItemByPosition( const TPoint& aPosition );

        /**
         * Prepare image for softkey
         */                                			        			 
		CEikImage* PrepareSoftkeyImageL( const CXnSoftkeyItem& aItem, const TSize& aSize );
		
        /**
         * Sets appearance to softkey, either text or image
         */                                			        			 		
		void SetSoftkeyAppearanceL( CXnSoftkeyItem& aItem );

        /**
         * Initializes softkey items array
         */                                			        			 				
		void InitSoftkeyItemsL();

        /**
         * Initializes single softkey item image
         */                                			        			 						
		void InitSoftkeyItemImageL( CXnSoftkeyItem& aItem );

        /**
         * Check wheter msk is available in this layout
         */                                			        			 								
		TBool IsMskAvailable();
		
		/*
		 * Resolves the softkey node which is in this pointer event location
		 */
		CXnNodePluginIf* ResolvePointerEventReceiver(  const TPoint aPosition ); 
		
        /**
         * Tries to display menubar        
         */        		
		void TryDisplayingMenuL();
		
		/**
         * Get cba button rect from laf        
         */
		TRect ButtonTouchableRectByPosition( TInt aPosition );
		/**
		 * Finds softkey from node tree
		 */
		CXnNodePluginIf* FindSoftkeyNodeL(const TDesC8& aNodeType);
		/**
		 * Creates menu bar
		 */
		void CreateMenuBarL();
		/**
		 * Updates softkeys if their "need update" state is on
		 */
		void UpdateSoftkeyAppearancesL();
		/**
		 * Creates image with right size and set it to left or right softkey
		 */
		void ChangeSoftkeyImageL(const CXnSoftkeyItem& aSoftkey);
		/**
		 * Inserts menu items to menu pane
		 */
		void AddItemsToMenuPaneL(CEikMenuPane* aMenuPane, TInt aResourceId);
		
		/**
         * Creates and initialized softkey item (TXnSoftkeyItem holds data for softkey)
         * if not created yet and appends it to the array
         */
		CXnSoftkeyItem* AppendSoftkeyItemL(CXnNodePluginIf* aNode);

		/**
		 * Dig type attribute from softkey node. 
		 * Return KNullDesC if not found
		 */
		const TDesC8& SoftkeyPlaceL(CXnNodePluginIf& aNode);

		/**
	     * Finds softkey item by position.
	     */                                			        			 
    	CXnNodePluginIf* DoFindSoftkeyItemByPositionL( const TPoint& aPosition );		

    	/**
    	 * Get visible softkey item based on position.
    	 */
    	CXnSoftkeyItem*  SoftkeyItemL(TInt aPosition);

    	/**
    	 * Get softkey item based on given node
    	 */
    	CXnSoftkeyItem* SoftkeyItemL(CXnNodePluginIf* aNode);

    	/**
    	 * Get softkey item based on given node
    	 */
    	CXnSoftkeyItem* SoftkeyItemL(CXnNodeAppIf* aNode);

    	/**
    	 * Set member flag true based on item.
    	 */
    	void SetUpdateAppearanceL(CXnSoftkeyItem* aItem);
    	                
        /**
         * Finds menu item from focused widget.
         */		
        CXnNodePluginIf* FindWidgetMenuItemL( CXnNodePluginIf* aNode );

        /**
         * Finds a <widgetsoftkey> node from view which 'source' arrtibure matches with the 'target' attribute 
         * of given node and returns its label.
         */     
        HBufC* FindSoftkeyLabelL( CXnNodePluginIf* aNode );
		
    private:    // Data
        TXnUiEnginePluginIf*                iUiEngine;
        CXnNodePluginIf*                    iRootNode;
        CXnNodePluginIf*                    iMenuBarNode;
        CXnNodePluginIf*                    iPointerEventNode;
        CXnNodePluginIf*                    iKeyEventNode;
        CArrayFixFlat< TXnMenuItem >        iMenuItems;
        TInt                                iIdCounter;
        CEikButtonGroupContainer*           iContainer;        
        CEikMenuBar*                        iMenuBar;  
        XnMenuInterface::MXnMenuObserver*   iObserver;        
        TBool                               iMenuShown; 
        RPointerArray<CXnSoftkeyItem>       iSoftkeyItems;
        TInt                                iPreviousMenuPaneId;
        TBool                               iUpdateLskAppearance;
        TBool                               iUpdateMskAppearance;
        TBool                               iUpdateRskAppearance;
        CXnSoftkeyItem*                     iVisibleLSK;
        CXnSoftkeyItem*                     iVisibleRSK;
        CXnSoftkeyItem*                     iVisibleMSK;
   };



#endif      // _XNMENUADAPTER_H   
            
// End of File
