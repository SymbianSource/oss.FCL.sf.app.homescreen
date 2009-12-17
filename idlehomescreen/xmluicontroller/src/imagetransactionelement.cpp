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
* Description:  Image transaction element
*
*/


#include "xnbitmap.h"
#include "xnnewsticker.h"
#include "xntype.h"
#include "xnmenu.h"
#include "xnmenuadapter.h"
#include "xnuiengineappif.h"

#include <imageconversion.h>
#include <gulicon.h>

#include "imagetransactionelement.h"
#include "xmluicontrollerpanic.h"
#include "aixmluiutils.h"
#include "xnuiengineappif.h"

using namespace AiXmlUiController;
using namespace XnImageInterface;
using namespace ContentAccess;

namespace AiXmlUiController
    {

TBool IsNodeVisible( CXnNodeAppIf& aNode )
    {
    CXnProperty* propVisibility = aNode.GetPropertyL( XnPropertyNames::style::common::KVisibility );;

    if( propVisibility )
        {
        const TDesC8& visibility = propVisibility->StringValue();

        if( visibility == XnPropertyNames::style::common::visibility::KHidden )
            {
            return EFalse;
            }    
        }

    CXnProperty* propDisplay = aNode.GetPropertyL( XnPropertyNames::style::common::KDisplay );

    if( propDisplay )
        {
        const TDesC8& display = propDisplay->StringValue();

        if( display == XnPropertyNames::style::common::display::KNone )
            {
            return EFalse;
            }
        }

    CXnProperty* propDisabled = aNode.GetPropertyL( XnPropertyNames::common::KDisabled );

    if( propDisabled )
        {
        const TDesC8& disabled = propDisabled->StringValue();

        if( disabled == XnPropertyNames::KTrue )
            {
            return EFalse;
            }
        }
    return ETrue;
    }
/**
 * Uses CImageDecoder to decode images asynchronously.
 * When decoding is done, it tries to find target and set
 * image into element. Destroys itself at end of RunL method.
 */
class CKamikazeImageDecoder: public CActive
    {
public:
    static CKamikazeImageDecoder* NewL(
            RFile& aFile, const TDesC8& aNodeId, 
            const TDesC8& aNodeNs, TXnUiEngineAppIf& aUiEngine);
    ~CKamikazeImageDecoder();
public:
    /**
     * Start decoding and forget instance. 
     * Destroyes itself.
     */
    void DecodeLD();
private:
    // from CActive
    void DoCancel();
    void RunL();
private:    
    void ConstructL(RFile& aFile, const TDesC8& aNodeId,const TDesC8& aNodeNs);
    CKamikazeImageDecoder(TXnUiEngineAppIf& aUiEngine);
    void SetImageL();
private:
    /**
     * Image decoder. Own.
     */
    CImageDecoder* iDecoder;
    /**
     * Target node id
     */
    HBufC8* iNodeId;
    /**
     * Target node namespace
     */
    HBufC8* iNodeNs;
    /**
     * Provides services to find target
     */
    TXnUiEngineAppIf& iUiEngine;
    /**
     * Icon. Own.
     */
    CGulIcon* iIcon;
    };

CKamikazeImageDecoder* CKamikazeImageDecoder::NewL(
    RFile& aFile, 
    const TDesC8& aNodeId,
    const TDesC8& aNodeNs,
    TXnUiEngineAppIf& aUiEngine)
    {
    CKamikazeImageDecoder* p = new (ELeave)CKamikazeImageDecoder(aUiEngine);
    CleanupStack::PushL(p);
    p->ConstructL(aFile,aNodeId,aNodeNs);
    CleanupStack::Pop(p);
    return p;
    }

CKamikazeImageDecoder::CKamikazeImageDecoder(TXnUiEngineAppIf& aUiEngine):
    CActive(CActive::EPriorityStandard ), iUiEngine(aUiEngine)
    {
    CActiveScheduler::Add( this );
    }

CKamikazeImageDecoder::~CKamikazeImageDecoder()
    {
    Cancel();
    delete iDecoder;
    delete iNodeId;
    delete iNodeNs;
    delete iIcon;
    }

void CKamikazeImageDecoder::ConstructL(
    RFile& aFile, 
    const TDesC8& aNodeId, 
    const TDesC8& aNodeNs)
    {
    // Create new decoder for file
    iDecoder = CImageDecoder::FileNewL( aFile, EView );
    iNodeId = aNodeId.AllocL();
    iNodeNs = aNodeNs.AllocL();
    iIcon = CGulIcon::NewL();
    }

void CKamikazeImageDecoder::DecodeLD()
    {
    TFrameInfo frameInfo = iDecoder->FrameInfo(0);
    CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap;
    iIcon->SetBitmap( bitmap );
    
    User::LeaveIfError( bitmap->Create( frameInfo.iOverallSizeInPixels,
                        frameInfo.iFrameDisplayMode ) );
    
    CFbsBitmap* mask(0);
    if ( frameInfo.iFlags & TFrameInfo::ETransparencyPossible )
        {
        mask = new( ELeave ) CFbsBitmap;
        iIcon->SetMask( mask );
        User::LeaveIfError( mask->Create( frameInfo.iOverallSizeInPixels,
                                          ( ( frameInfo.iFlags & TFrameInfo::EAlphaChannel )
                                          ? EGray256 : EGray2 ) ) );
        }
    
    if(iIcon->Mask())
        {
        iDecoder->Convert( &iStatus, *bitmap, *mask );
        }
    else
        {
        iDecoder->Convert( &iStatus, *bitmap );
        }
    SetActive();
    }

void CKamikazeImageDecoder::DoCancel()
    {
    iDecoder->Cancel();
    }

void CKamikazeImageDecoder::SetImageL()
    {
     
    // Find target
    CXnNodeAppIf* node = iUiEngine.FindNodeByIdL(*iNodeId,*iNodeNs);
    if(!node)
        {
        return;
        }
        
    const TDesC8& type = node->Type()->Type();
    
    if ( type == XnImageInterface::MXnImageInterface::Type() )
        {
        MXnImageInterface* imageIntr = NULL;
        if ( !XnComponentInterface::MakeInterfaceL( imageIntr, *node ) )
            {
            return;
            }
        // Set new bitmaps. Ownership is transferred to MXnImageInterface
        iIcon->SetBitmapsOwnedExternally( ETrue );
        imageIntr->SetContentBitmaps( iIcon->Bitmap(), iIcon->Mask() );
      
        }
    else if ( type == XnNewstickerInterface::MXnNewstickerInterface::Type() )
        {
        XnNewstickerInterface::MXnNewstickerInterface* newsTicker = NULL;
        XnComponentInterface::MakeInterfaceL( newsTicker, *node );
        LeaveIfNull( newsTicker, KErrNotSupported );
        
        User::LeaveIfError( newsTicker->SetSeparatorImageL( iIcon ) );
        // Ownership of icon transferred to newsticker
        iIcon = NULL;
        }
    // Menu softkey icons
    else if ( ( ( type == KXnMenuItem || type == KXnMenu ) &&
                IsNodeVisible( *node ) ) ||
              type == XnPropertyNames::softkey::KNodeName )
        {
        CXnNodeAppIf* menuBar = node->ParentL();
        
        XnMenuInterface::MXnMenuInterface::TSoftKeyPosition softkeyPosition = 
            XnMenuInterface::MXnMenuInterface::ELeft;
                    
        CXnProperty* property = 
            node->GetPropertyL( XnPropertyNames::softkey::KTypeAttribute );     
        if ( property && property->StringValue() == XnPropertyNames::softkey::type::KRight )
            {
            softkeyPosition = XnMenuInterface::MXnMenuInterface::ERight;
            }
        else if ( property && property->StringValue() == XnPropertyNames::softkey::type::KLeft )
            {
            softkeyPosition = XnMenuInterface::MXnMenuInterface::ELeft;
            }
        else  // Image can be published only to RSK or LSK
            {
            return;
            }
            
        XnMenuInterface::MXnMenuInterface* menuIf = NULL;
        
        XnComponentInterface::MakeInterfaceL( menuIf, *menuBar );
        if ( menuIf )
            {
            // Use black mask and preserver aspect ratio
           
            menuIf->SetSoftKeyImageL( iIcon->Bitmap(), 
                                          iIcon->Mask(), 
                                          softkeyPosition, 
                                          node,
                                          ETrue, 
                                          ETrue ); 
            iIcon->SetBitmapsOwnedExternally( ETrue );
            }
        if( menuBar )
            {
            iUiEngine.RefreshMenuL();
            }
        }
        
    }

void CKamikazeImageDecoder::RunL()
    {
    CleanupStack::PushL(this);
    if(iStatus == KErrNone)
        {
        SetImageL();
        iUiEngine.RenderUIL();
        }
    CleanupStack::PopAndDestroy(this);
    }
     
// ======== MEMBER FUNCTIONS ========

CImageTransactionElement::CImageTransactionElement(
                                AiUtility::CContentPriorityMap& aContentPriorityMap)
    : CTransactionElement(aContentPriorityMap)
    {
    }

CImageTransactionElement* CImageTransactionElement::NewL(
                                AiUtility::CContentPriorityMap& aContentPriorityMap)
    {
    CImageTransactionElement* self = new( ELeave ) CImageTransactionElement(
                                                            aContentPriorityMap );
    return self;
    }

CImageTransactionElement::~CImageTransactionElement()
    {
    delete iNewIcon;
    delete iImageDecoder;
    }

void CImageTransactionElement::InitializeL( CXnNodeAppIf& aTarget,
                                            CGulIcon* aIcon )
    {
    CheckTypeL( aTarget );
    
    LeaveIfNull( aIcon, KErrArgument );
    
    SetTarget( aTarget );
    iNewIcon = aIcon;
    }

void CImageTransactionElement::InitializeL( CXnNodeAppIf& aTarget,
                                            RFile& aFile )
    {
    CheckTypeL( aTarget );
   
    const TDesC8* nodeId = &KNullDesC8;
    
    CXnProperty* property = aTarget.GetPropertyL(XnPropertyNames::common::KId);
    if(property)
        {
        nodeId = &property->StringValue();
        }
    if ( iImageDecoder )
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    iImageDecoder = CKamikazeImageDecoder::NewL(
                        aFile,*nodeId,aTarget.Namespace(),*aTarget.UiEngineL()); 
    
    SetTarget( aTarget );
    }


void CImageTransactionElement::UpdateDataL()
    {
    if ( iImageDecoder )
        {
        iImageDecoder->DecodeLD();
        iImageDecoder = NULL; // self destroy
        }
    else
        {
        __ASSERT_DEBUG( iNewIcon, Panic( EBitmapNull ) );
    
        const TDesC8& type = LeaveIfNull( Target().Type(), KErrNotSupported )->Type();
        
        if ( type == XnImageInterface::MXnImageInterface::Type() )
            {
            MXnImageInterface* imageIntr = NULL;
            if ( !XnComponentInterface::MakeInterfaceL( imageIntr, Target() ) )
                {
                User::Leave( KErrNotSupported );
                }
            // Set new bitmaps. Ownership is transferred to MXnImageInterface
            iNewIcon->SetBitmapsOwnedExternally( ETrue );
            imageIntr->SetContentBitmaps( iNewIcon->Bitmap(), iNewIcon->Mask() );
            }
        else if ( type == XnNewstickerInterface::MXnNewstickerInterface::Type() )
            {
            XnNewstickerInterface::MXnNewstickerInterface* newsTicker = NULL;
            XnComponentInterface::MakeInterfaceL( newsTicker, Target() );
            LeaveIfNull( newsTicker, KErrNotSupported );
            
            User::LeaveIfError( newsTicker->SetSeparatorImageL( iNewIcon ) );
            
            // Ownership of icon transferred to newsticker
            
            iNewIcon = NULL;
            }
        // Menu softkey icons
        else if ( ( ( type == KXnMenuItem || type == KXnMenu ) &&
                    IsNodeVisible( Target() ) ) ||
                  type == XnPropertyNames::softkey::KNodeName )
            {
            CXnNodeAppIf* menuBar = Target().ParentL();
            
            XnMenuInterface::MXnMenuInterface::TSoftKeyPosition softkeyPosition = 
                XnMenuInterface::MXnMenuInterface::ELeft;
                        
            CXnProperty* property = 
                Target().GetPropertyL( XnPropertyNames::softkey::KTypeAttribute );     
            if ( property && property->StringValue() == XnPropertyNames::softkey::type::KRight )
                {
                softkeyPosition = XnMenuInterface::MXnMenuInterface::ERight;
                }
            else if ( property && property->StringValue() == XnPropertyNames::softkey::type::KLeft )
                {
                softkeyPosition = XnMenuInterface::MXnMenuInterface::ELeft;
                }
            else  // Image can be published only to RSK or LSK
                {
                delete iNewIcon;
                iNewIcon = NULL;            
                User::Leave( KErrNotSupported );
                }
                
            XnMenuInterface::MXnMenuInterface* menuIf = NULL;
            
            XnComponentInterface::MakeInterfaceL( menuIf, *menuBar );
            if ( menuIf )
                {
                // Use black mask and preserver aspect ratio
                TRAPD(err,
                    menuIf->SetSoftKeyImageL( iNewIcon->Bitmap(), 
                                              iNewIcon->Mask(), 
                                              softkeyPosition, 
                                              &Target(),
                                              ETrue, 
                                              ETrue ) 
                );
                // Menuinterface takes ownership
                if ( err == KErrNone )
                    {
                    iNewIcon->SetBitmapsOwnedExternally( ETrue );
                    }
                if( menuBar )
                    {
                    menuBar->UiEngineL()->RefreshMenuL();
                    }
                }
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }
    
    delete iNewIcon;
    iNewIcon = NULL;
    
    UpdateContentPriorityL();
    }

void CImageTransactionElement::Reset()
    {
    if ( iImageDecoder )
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    
    CTransactionElement::Reset();

    delete iNewIcon;
    iNewIcon = NULL;
    }

TBool CImageTransactionElement::IsSupported( CXnNodeAppIf& aTarget )
    {
    // Get type info
    CXnType* typeInfo = aTarget.Type();
    
    if ( !typeInfo )
        {
        return EFalse;
        }
    
    const TDesC8& type = typeInfo->Type();
    
    // image element and newsticker supported
    return ( type == XnImageInterface::MXnImageInterface::Type() ||
             type == XnNewstickerInterface::MXnNewstickerInterface::Type() ||
             type == XnMenuInterface::MXnMenuInterface::Type() ||
             type == KXnMenuItem ||
             type == KXnMenu ||
             type == XnPropertyNames::softkey::KNodeName );
    }

void CImageTransactionElement::CheckTypeL( CXnNodeAppIf& aTarget )
    {
    if ( !IsSupported( aTarget ) )
        {
        User::Leave( KErrNotSupported );
        }
    }
} // ns
