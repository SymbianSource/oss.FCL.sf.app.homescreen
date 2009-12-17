/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for a popup
*
*/


#include "xnpopupadapter.h"

#include "xncomponentnodeimpl.h"
#include "xnnodepluginif.h"
#include "xnproperty.h"
#include "xndompropertyvalue.h"
#include "xndomlist.h"
#include "xncomponent.h"

#include <AknUtils.h>

const TInt KStartDelay = 1000000;
const TInt KDisplayTime = 0;

// -----------------------------------------------------------------------------
// Control
// Gets a CXnControlAdapter from node
// -----------------------------------------------------------------------------
static CXnControlAdapter* Control( CXnNodePluginIf* aNode )
    {      
    if( aNode )
        {
    	CXnComponent* component( NULL );
    			
        if( aNode->ComponentNodeImpl() )
            {
            component = aNode->ComponentNodeImpl()->Component();
            }
                                    
        if( component )
            {
            return component->ControlAdapter();               
            }        
        }
        
    return NULL;            
    }

// -----------------------------------------------------------------------------
// IsNodeDisplayedL
// 
// -----------------------------------------------------------------------------
static TBool IsNodeDisplayedL( CXnNodePluginIf* aNode )
    {
    CXnProperty* visibilityProp( aNode->VisibilityL() );    
    
    // Am I visible?
    if( visibilityProp )
        {
        const TDesC8& visibility( visibilityProp->StringValue() );
        
        if( visibility != XnPropertyNames::style::common::visibility::KVisible )
            {
            return EFalse;
            }
        }                
            
    // Am I displayed?
    CXnProperty* displayProp( aNode->DisplayL() );
    
    if( displayProp )
        {
        const TDesC8& display( displayProp->StringValue() );
        
        if( display != XnPropertyNames::style::common::display::KBlock )
            {
            return EFalse;
            }                        
        }
    
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// class CXnPopupController
// 
// -----------------------------------------------------------------------------
class CXnPopupController : public CActive
    {
    public:
        static CXnPopupController* NewL(CXnControlAdapter& aAdapter)
            {
            CXnPopupController* self = new (ELeave) CXnPopupController;
            CleanupStack::PushL(self);
            self->ConstructL(aAdapter);
            CleanupStack::Pop(self);
            return self;
            }
        ~CXnPopupController()
            {
            iDestroying = ETrue;
            Cancel();
            iTimer.Close();
            }
        void SetTimes(TTimeIntervalMicroSeconds32 aShowDelay, TTimeIntervalMicroSeconds32 aDisplayTime)
            {
            Cancel();
            iShowDelay = aShowDelay;
            iDisplayTime = aDisplayTime;
            iShowing = ETrue;
            iTimer.After(iStatus, aShowDelay);
            SetActive();
            }
    private:
        CXnPopupController() : CActive(0)
            {
            }
        void ConstructL(CXnControlAdapter& aAdapter)
            {
            iAdapter = &aAdapter;
            User::LeaveIfError(iTimer.CreateLocal());
            CActiveScheduler::Add(this);
            }
        void RunL()
            {
            if (iShowing)
                {
                iAdapter->ActivateL();
                iAdapter->MakeVisible(ETrue);
                
                // Check are childs visible
                CXnNodePluginIf* node( iAdapter->Component()->Node() );
                
                RPointerArray<CXnNodePluginIf> children( node->ChildrenL() );
                CleanupClosePushL( children );
                
                TInt visibleCount( 0 );
                
                for( TInt i = 0; i < children.Count(); i++ )
                    {
                    CXnNodePluginIf* child( children[i] );
                    
                    TBool visible( IsNodeDisplayedL( child ) );
                                                           
                    CCoeControl* control( Control( child ) );
                    
                    if( control )
                        {
                        control->MakeVisible( visible );
                        }
                    
                    if( visible )
                        {
                        visibleCount++;                                                                                                
                        }
                    }
                    
                CleanupStack::PopAndDestroy( &children );
                
                if( visibleCount == 0 )
                    {
                    iAdapter->MakeVisible( EFalse );
                    // Nothing to show
                    return;
                    }
                                                
                iAdapter->DrawNow();
                iShowing = EFalse;
                if (iDisplayTime.Int() != 0)
                    {
                    iHiding = ETrue;
                    iTimer.After(iStatus, iDisplayTime);
                    SetActive();
                    }
                }
            else if (iHiding)
                {
                iAdapter->MakeVisible(EFalse);
                iHiding = EFalse;
                }
            }
        void DoCancel()
            {
            if (!iDestroying)
                {
                iAdapter->MakeVisible(EFalse);
                }
   
            iTimer.Cancel();
            }
    private:
        TBool iDestroying;
        CXnControlAdapter* iAdapter;            
        RTimer iTimer;
        TBool iShowing;
        TBool iHiding;
        TTimeIntervalMicroSeconds32 iShowDelay;
        TTimeIntervalMicroSeconds32 iDisplayTime;
    };
    

static CActive* CreateActiveObjectL(CXnControlAdapter& aAdapter)
    {
    return CXnPopupController::NewL(aAdapter);
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnPopup::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnPopupAdapter* CXnPopupAdapter::NewL(CXnNodePluginIf& aNode, CXnControlAdapter* aParent)
    {
	CXnPopupAdapter* self = new( ELeave ) CXnPopupAdapter;

    CleanupStack::PushL( self );
    self->ConstructL(aNode, aParent);
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnPopupAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnPopupAdapter::ConstructL(CXnNodePluginIf& aNode, CXnControlAdapter* /*aParent*/)
    {
    MakeVisible(EFalse);
    CXnProperty* positionHintProp = aNode.GetPropertyL(XnPropertyNames::tooltip::KPositionHint);
    if( positionHintProp )
        {
        const TDesC8& displayHintVal = positionHintProp->StringValue();
        if( displayHintVal == XnPropertyNames::tooltip::positionhint::KAboveLeft )
            {
            iPositionHint = EAboveLeft;
            }
        else if( displayHintVal == XnPropertyNames::tooltip::positionhint::KAboveRight )
            {
            iPositionHint = EAboveRight;
            }
        else if( displayHintVal == XnPropertyNames::tooltip::positionhint::KBelowLeft )
            {
            iPositionHint = EBelowLeft;
            }
        else if( displayHintVal == XnPropertyNames::tooltip::positionhint::KBelowRight )
            {
            iPositionHint = EBelowRight;
            }        
        else if( displayHintVal == XnPropertyNames::tooltip::positionhint::KRight )
            {
            iPositionHint = ERight;
            }        
        else if( displayHintVal == XnPropertyNames::tooltip::positionhint::KLeft )
            {
            iPositionHint = ELeft;
            }
        }    
    if(iPositionHint == ENone)
        {
        // Use default value
        if(AknLayoutUtils::LayoutMirrored())
            {
            iPositionHint = EAboveRight;
            }
        else
            {
            iPositionHint = EAboveLeft; 
            }
        }

    CreateWindowL();    
    
    if( CAknEnv::Static()->TransparencyEnabled() )
        {
        // try to enable window transparency
        if ( Window().SetTransparencyAlphaChannel() == KErrNone )
            {
            TRgb color( ~0 );
            Window().SetBackgroundColor( color );
            }
        }
    
    CXnControlAdapter::ConstructL(aNode);
    iNode = &aNode;
    iAppUi = CCoeEnv::Static()->AppUi();
    }
    
// -----------------------------------------------------------------------------
// CXnPopupAdapter::CXnPopupAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnPopupAdapter::CXnPopupAdapter(): iPositionHint( ENone )
    {
    }

// -----------------------------------------------------------------------------
// CXnPopupAdapter::~CXnPopupAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnPopupAdapter::~CXnPopupAdapter()
    {
    delete iActiveObject;
    }

// -----------------------------------------------------------------------------
// CXnPopupAdapter::ShowPopupL
// Shows the popup
// -----------------------------------------------------------------------------
//
void CXnPopupAdapter::ShowPopupL(const TRect& aContainerRect)
    {
    // Don't show tooltip if display property is none.
    CXnProperty* displayProperty( iNode->DisplayL() );
    
    if( displayProperty )
    	{
    	const TDesC8& displayValue = displayProperty->StringValue();
    	
    	if( displayValue == XnPropertyNames::style::common::display::KNone )
    		{
    		return;
    		}
    	}
    	
    CXnProperty* visibilityProp( iNode->VisibilityL() );	
    
    if( visibilityProp )
        {
        const TDesC8& value( visibilityProp->StringValue() );
        
        if( value == XnPropertyNames::style::common::visibility::KHidden )
            {
            return;
            }
        }
    
    TTimeIntervalMicroSeconds32 startDelay(KStartDelay);
    TTimeIntervalMicroSeconds32 displayTime(KDisplayTime);
    CXnProperty* startDelayProperty = iNode->GetPropertyL(XnPropertyNames::tooltip::KStartDelay);
    if (startDelayProperty)
        {
        CXnDomPropertyValue* value = static_cast<CXnDomPropertyValue*>(startDelayProperty->Property()->PropertyValueList().Item(0));
        TInt factor = 1;
        if (value->PrimitiveValueType() == CXnDomPropertyValue::ES)
            {
            factor = 1000000;
            }
        else if (value->PrimitiveValueType() == CXnDomPropertyValue::EMs)
            {
            factor = 1000;
            }
        startDelay = factor * static_cast<TInt>(startDelayProperty->FloatValueL());
        }
    CXnProperty* displayTimeProperty = iNode->GetPropertyL(XnPropertyNames::tooltip::KDisplayTime);
    if (displayTimeProperty)
        {
        CXnDomPropertyValue* value = static_cast<CXnDomPropertyValue*>(displayTimeProperty->Property()->PropertyValueList().Item(0));
        TInt factor = 1;
        if (value->PrimitiveValueType() == CXnDomPropertyValue::ES)
            {
            factor = 1000000;
            }
        else if (value->PrimitiveValueType() == CXnDomPropertyValue::EMs)
            {
            factor = 1000;
            }
        displayTime = factor * static_cast<TInt>(displayTimeProperty->FloatValueL());
        }
    if (displayTime.Int() == 0 )
        {
        return;
        }
    ShowPopupL(aContainerRect, startDelay, displayTime);
    }

// -----------------------------------------------------------------------------
// CXnPopupAdapter::ShowPopupL
// Shows the popup
// -----------------------------------------------------------------------------
//
void CXnPopupAdapter::ShowPopupL(const TRect& aContainerRect, TTimeIntervalMicroSeconds32 aShowDelay, TTimeIntervalMicroSeconds32 aDisplayTime)
    {
    if (!iActiveObject)
        {
        iActiveObject = CreateActiveObjectL(*this);
        }
    
    iActiveObject->Cancel();        
    
    // Calculates position according to position hint and 
    // set new rect for CCoeControl.
    CalculatePosition(aContainerRect);        

    static_cast<CXnPopupController*>(iActiveObject)->SetTimes(aShowDelay, aDisplayTime);
    }

// -----------------------------------------------------------------------------
// CXnPopupAdapter::HidePopupL
// Hides the popup
// -----------------------------------------------------------------------------
//
void CXnPopupAdapter::HidePopupL()
    {
    if (!iActiveObject)
        {
        MakeVisible(EFalse);
        return;
        }
    iActiveObject->Cancel();        
    MakeVisible(EFalse);   
    }

// -----------------------------------------------------------------------------
// CXnPopupAdapter::Draw
// Draws the popup.
// -----------------------------------------------------------------------------
//
void CXnPopupAdapter::Draw(const TRect& aRect) const
    {    
    CXnControlAdapter::Draw(aRect);
    }
    
// -----------------------------------------------------------------------------
// CXnPopupAdapter::DoEnterPowerSaveModeL
// Hides the popup when power save mode is entered.
// -----------------------------------------------------------------------------
//    
void CXnPopupAdapter::DoEnterPowerSaveModeL( TModeEvent /*aEvent*/ )
    {
    HidePopupL();
    }
    
// -----------------------------------------------------------------------------
// CXnPopupAdapter::Draw
// Hides the popup when screen device is changed.
// -----------------------------------------------------------------------------
//       
void CXnPopupAdapter::HandleScreenDeviceChangedL()
	{
	HidePopupL();
    CXnControlAdapter::HandleScreenDeviceChangedL();
	}

// -----------------------------------------------------------------------------
// CXnPopupAdapter::DoHandlePropertyChangeL
// Handles property changes
// -----------------------------------------------------------------------------
//       	
void CXnPopupAdapter::DoHandlePropertyChangeL(CXnProperty* aProperty)
    {
    if( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name() );
                
        if( name == XnPropertyNames::style::common::KDisplay )
            {
            const TDesC8& display( aProperty->StringValue() );
            
            if( display != XnPropertyNames::style::common::display::KBlock )
                {
                HidePopupL();
                }                                    
            }        
        else if( name == XnPropertyNames::style::common::KVisibility )
            {
            const TDesC8& visibility( aProperty->StringValue() );
            
            if( visibility != XnPropertyNames::style::common::visibility::KVisible )
                {
                HidePopupL();
                }            
            }           
        }
    }

// -----------------------------------------------------------------------------
// CalculatePosition
// 
// -----------------------------------------------------------------------------
void CXnPopupAdapter::CalculatePosition( TRect aPopupRect )
    {
    TRect clientRect = static_cast<CEikAppUi&>(*iAppUi).ClientRect();
    TPoint offset(clientRect.iTl);
    TRect controlRect = iNode->BorderRect();
    TRect contentRect(0, 0, clientRect.iBr.iX, clientRect.iBr.iY); // entire screen except control pane
    TRect rect;
    switch(iPositionHint)
        {
        case CXnPopupAdapter::EAboveLeft:
            rect = TRect(TPoint(aPopupRect.iTl.iX, aPopupRect.iTl.iY - controlRect.Height()), TPoint(aPopupRect.iTl.iX + controlRect.Width(), aPopupRect.iTl.iY));
            break;
        case CXnPopupAdapter::EAboveRight:
            rect = TRect(TPoint(aPopupRect.iBr.iX - controlRect.Width(), aPopupRect.iTl.iY - controlRect.Height()), TPoint(aPopupRect.iBr.iX, aPopupRect.iTl.iY));
            break;
        case CXnPopupAdapter::EBelowLeft:
            rect = TRect(TPoint(aPopupRect.iTl.iX, aPopupRect.iBr.iY), TPoint(aPopupRect.iTl.iX + controlRect.Width(), aPopupRect.iBr.iY + controlRect.Height()));
            break;
        case CXnPopupAdapter::EBelowRight:
            rect = TRect(TPoint(aPopupRect.iBr.iX - controlRect.Width(), aPopupRect.iBr.iY), TPoint(aPopupRect.iBr.iX, aPopupRect.iBr.iY + controlRect.Height()));
            break;
        case CXnPopupAdapter::ERight:
            rect = TRect(TPoint(aPopupRect.iBr.iX, aPopupRect.iTl.iY), TPoint(aPopupRect.iBr.iX + controlRect.Width(), aPopupRect.iTl.iY + controlRect.Height()));
            break;
        case CXnPopupAdapter::ELeft:
            rect = TRect(TPoint(aPopupRect.iTl.iX - controlRect.Width(), aPopupRect.iTl.iY), TPoint(aPopupRect.iTl.iX, aPopupRect.iTl.iY + controlRect.Height()));
            break;
        default:
            break;
        }
    rect.Move(offset);           
    if(rect.iTl.iY < contentRect.iTl.iY)
        {
        rect.Move(0, contentRect.iTl.iY - rect.iTl.iY);
        }
    if(rect.iTl.iX < contentRect.iTl.iX)
        {
        rect.Move(contentRect.iTl.iX - rect.iTl.iX, 0);
        }
    if(rect.iBr.iY > contentRect.iBr.iY)
        {
        rect.Move(0, contentRect.iBr.iY - rect.iBr.iY);
        }
    if(rect.iBr.iX > contentRect.iBr.iX)
        {
        rect.Move(contentRect.iBr.iX - rect.iBr.iX, 0);
        } 
    this->SetRect( rect );
    }

// End of File
