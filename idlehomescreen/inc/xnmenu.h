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
* Description:  Xuikon Menu Plugin
*
*/



#ifndef _XNMENU_H
#define _XNMENU_H

//  INCLUDES
#include "xncomponent.h"
#include "mxncomponentinterface.h"

class TAknsItemID;

namespace XnMenuInterface
{
_LIT8(KType, "menu");

class MXnMenuObserver
    {
public:
    /**
     * Called when options menu is about to open. Observer can add 
     * its own menu item to the aMenuItems array.
     * @param aMenuItem menu item
     * @param aPlugin plugin node
     * @return ETrue if the item should be shown, EFalse otherwise
     */        
    virtual TBool DynInitMenuItemL( CXnNodeAppIf& aMenuItem, CXnNodeAppIf* aPlugin = NULL ) = 0;
    };

/**
*  @ingroup group_xnmenufactory
*  @lib xn3menufactory.dll
*  @since S60 3.1
*/
class MXnMenuInterface : public XnComponentInterface::MXnComponentInterface
    {
public:
    /**
    * Soft key position enum
    * These are same as in CEikButtonGroupContainer::TCommandPosition
    */
    enum TSoftKeyPosition
        {
        ELeft = 0,
        ERight = 2,    
        ECenter = 3  /* Not supported when setting icon */
        };        

public:
		
    static inline const TDesC8& Type()
        {
        return KType;
        }
		
public: // New functions

    /**
    * Sets softkey image. If there was no leave, menu takes  
    * bitmap and mask ownership.
    * @since Series 60 3.2
    * @param aBitmap Bitmap to draw
    * @param aMask Mask to use
    * @param aPos Softkey position
    * @param aPreserveAspectRatio ETrue if image's aspect ratio is preserved
    * @param aInvertMask ETrue if white mask is used, EFalse if black mask is used
    * @param aTransferOwnership ETrue if bitmap and mask ownerhsip is transferred to menuadapter
    */        
    virtual void SetSoftKeyImageL( 
        CFbsBitmap* aBitmap, 
        CFbsBitmap* aMask, 
        XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos,
        TBool aPreserveAspectRatio = EFalse, TBool aInvertMask = EFalse,
        TBool aTransferOwnership = ETrue ) = 0;   

    virtual void SetSoftKeyImageL( 
        CFbsBitmap* aBitmap, 
        CFbsBitmap* aMask, 
        XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos,
        CXnNodeAppIf* aNode,
        TBool aPreserveAspectRatio = EFalse, TBool aInvertMask = EFalse,
        TBool aTransferOwnership = ETrue ) = 0;   

    /**
    * Sets soft key image.
    * @param aId icon bitmap IID 
    * @param aBmpFile file path.
    * @param aBmp Bitmap ID.
    * @param aBmpM Bitmap mask ID.
    * @param aPos Softkey position.
    * @param aEnable ETrue for set icon, EFalse for use default icon.
    */
    virtual void SetSoftKeyImageL( const TAknsItemID& aId,
                                   const TDesC& aBmpFile,
                                   const TInt32 aBmp,
                                   const TInt32 aBmpM,
                                   XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos, 
                                   TBool aEnable ) = 0;

    /**
    * Sets softkey text. 
    * @since Series 60 3.2
    * @param aText Text to draw    
    * @param aPos Softkey position
    */                
    virtual void SetSoftKeyTextL( const TDesC& aText,
        XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos ) = 0;           

    /**
    * Sets softkey. 
    * @param aNode Softey node. 
    * @param aPos Softkey position.
    */       
    virtual void SetSoftKeyL( CXnNodePluginIf* aSoftkeyNode,
        XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos ) = 0;

    /**
    * Returns currently visible softkey node. 
    * @param aPos Softkey position.
    * @return Currently visible softkey. 
    */       
    virtual CXnNodePluginIf* SoftKeyL( XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos ) = 0;

   /**
    * Sets MXnMenuObserver observer
    * @since S60 5.1
    * @param aObserver observer to set
    */                
    virtual void SetObserver( XnMenuInterface::MXnMenuObserver& aObserver ) = 0;

   /**
    * Removes the observer
    * @since S60 5.1    
    */                   
    virtual void RemoveObserver() = 0;        

   /**
    * Tries to display menubar
    * @since S60 5.1    
    * @param aMenuNodeId the menu node id
    */                      
    virtual void TryDisplayingMenuBarL( const TDesC& aMenuNodeId ) = 0;
    };
}   
    
class CXnMenu : public CXnComponent, public XnMenuInterface::MXnMenuInterface
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXnMenu* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXnMenu();
    
    protected: // New functions

       /**
        * @see MXnMenuInterface::SetSoftKeyImage
        */
       void SetSoftKeyImageL(
        CFbsBitmap* aBitmap,
        CFbsBitmap* aMask,
        XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos,
        TBool aPreserveAspectRatio, TBool aInvertMask,
        TBool aTransferOwnership = ETrue );
       
       void SetSoftKeyImageL( 
        CFbsBitmap* aBitmap, 
        CFbsBitmap* aMask, 
        XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos,
        CXnNodeAppIf* aNode,
        TBool aPreserveAspectRatio, TBool aInvertMask,
        TBool aTransferOwnership = ETrue );

       /**
        * @see MXnMenuInterface::SetSoftKeyImage
        */
       void SetSoftKeyImageL( const TAknsItemID& aId,
                              const TDesC& aBmpFile,
                              const TInt32 aBmp,
                              const TInt32 aBmpM,
                              XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos, 
                              TBool aEnable );

       /**
        * @see MXnMenuInterface::SetSoftkeyTextL
        */        
       void SetSoftKeyTextL( const TDesC& aText,
        XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos );           

       /**
        * @see MXnMenuInterface::SetSoftKeyL
        */        
        void SetSoftKeyL( CXnNodePluginIf* aSoftkeyNode,
            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos );

       /**
        * @see MXnMenuInterface::SoftKeyL
        */        
        CXnNodePluginIf* SoftKeyL( XnMenuInterface::MXnMenuInterface::TSoftKeyPosition aPos );

       /**
        * @see MXnMenuInterface::SetObserver
        */                
       void SetObserver( XnMenuInterface::MXnMenuObserver& aObserver );

       /**
        * @see MXnMenuInterface::RemoveObserver
        */                
       void RemoveObserver();

       /**
        * @see MXnMenuInterface::TryDisplayingMenuBarL
        */                       
       void TryDisplayingMenuBarL( const TDesC& aMenuNodeId );
       
                              
      /**
       * @see MXnComponentInterface::MakeInterfaceL
       */
      XnComponentInterface::MXnComponentInterface* MakeInterfaceL( const TDesC8& aType );   
    private:
    
        /**
        * C++ default constructor.
        */
        CXnMenu();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
   };

#endif      // _XNMENU_H 
            
// End of File
