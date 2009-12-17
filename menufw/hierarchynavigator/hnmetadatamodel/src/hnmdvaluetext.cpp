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
* Description:   
*
*/

#include <xmlengdom.h>
#include <liwservicehandler.h>
#include <hnmdbasekey.h>
#include <hnconvutils.h>

#include "hnmdvaluetext.h"
#include "hnglobals.h"
#include "hnmdmodel.h"
#include "hnliwutils.h"
#include "hnutils.h"
#include "hnstringhandler.h"
#include "hnattributetext.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnMdValueText* CHnMdValueText::NewL(
        const TXmlEngElement& aElement, THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdValueText* self = 
        new (ELeave)CHnMdValueText();
    CleanupStack::PushL(self);
    self->ConstructL( aElement, aCmnPtrs );
    CleanupStack::Pop(self); 
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CHnMdValueText::ConstructL( const TXmlEngElement& aElement,
        THnMdCommonPointers* aCmnPtrs )
    {
    iCmnPtrs = aCmnPtrs;

    if ( !aElement.Name().Compare( KTextElementItem8 ) )
        {
        // handle text node
        HnUtils::SetAttributeL( aElement, KResourceAttrName8 , iResourceFile );
        HnUtils::SetAttributeL( aElement, KIdAttrName8 , iResourceId );
        HnUtils::SetAttributeL( aElement, KValueAttrName8 , iSimpleValue);
        
        if (  aElement.HasChildNodes() )
            {
            iParameters = new(ELeave)CDesC8ArraySeg(1);
            iCachedDesParameters = new(ELeave)CDesC16ArraySeg(1);
            iCachedIntParameters = new(ELeave)CArrayFixSeg<TInt>(1);
            // get child element from output and pass to set value
            RXmlEngNodeList< TXmlEngElement > childElements;
            CleanupClosePushL( childElements );
            aElement.GetChildElements( childElements );
            TInt amount = childElements.Count();

            for (TInt i(0); i<amount; i++)
                {
                TXmlEngElement child = childElements.Next();
                RBuf8 tmp;
                CleanupClosePushL(tmp);
                HnUtils::SetAttributeL( child, KValueAttrName8 , tmp);
                if (tmp.Length() > 0)
                    {
                    iParameters->AppendL( tmp );
                    }
                CleanupStack::PopAndDestroy( &tmp );
                }
            CleanupStack::PopAndDestroy( &childElements );
            }
        }
    else 
        {
        // it can accept element that have text define in element node
        // or value attribute
        if ( iSimpleValue.Length() == 0 )
            {
            HnUtils::SetAttributeL( aElement, KValueAttrName8 , iSimpleValue );
            }
        HnUtils::SetAttributeL(aElement, iSimpleValue);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnMdValueText::CHnMdValueText()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnMdValueText::~CHnMdValueText()
    {
    iSimpleValue.Close();
    iResourceFile.Close();
    iResourceId.Close();
    iCachedValue.Close();
    delete iParameters;
    delete iCachedDesParameters;
    delete iCachedIntParameters;
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//  
void CHnMdValueText::FillParamsL( const CLiwGenericParamList* aQueryList, 
                                   TInt aPos, 
                                   CDesC16Array& aDesParams,
                                   CArrayFix<TInt>& aNumericParams)
    {
    if (iParameters)
        {
        // remove old values
        aDesParams.Reset();
        aNumericParams.Reset();
        
        for (TInt i(0); i < iParameters->Count(); i++)
            {
            TLiwVariant value;
            CleanupStack::PushL( 
                    TCleanupItem( TLiwVariant::VariantCleanup , &value) );

            HnLiwUtils::GetVariantL( *aQueryList, 
                                  iParameters->MdcaPoint(i), 
                                  aPos, 
                                  value );
            
            if ( value.TypeId() == LIW::EVariantTypeTInt32 )
                {
                aNumericParams.AppendL( value.AsTInt32() );
                } 
            else if ( value.TypeId() == LIW::EVariantTypeTUint )
                {
                aNumericParams.AppendL( value.AsTUint() );
                }
            else if ( value.TypeId() == LIW::EVariantTypeTBool )
                {
                aNumericParams.AppendL( value.AsTBool() );
                }
            else if ( value.TypeId() == LIW::EVariantTypeTTime )
                {
                // PW add time formating
                }
            else if ( value.TypeId() != LIW::EVariantTypeNull )
                {
                RBuf vbuf;
                CleanupClosePushL(vbuf);
                HnLiwUtils::VariantToStringL( value, vbuf );
                aDesParams.AppendL(vbuf);
                CleanupStack::PopAndDestroy(&vbuf);
                }
            CleanupStack::PopAndDestroy(&value); 
            }
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnMdValueText::GetFromResourceFileL( 
        const CLiwGenericParamList* aParamList, TInt aPos, RBuf& aValue )
    {
    TInt retFile(KErrNotFound);
    TInt retId(KErrNotFound);
    RBuf resourceFile;
    RBuf resourceId;

    CleanupClosePushL(resourceFile);
    CleanupClosePushL(resourceId);
     
    retFile = HnLiwUtils::GetStringL( *aParamList, iResourceFile, aPos, resourceFile );
    retId = HnLiwUtils::GetStringL( *aParamList, iResourceId, aPos, resourceId );
    if ( retFile != KErrBadDescriptor && retId != KErrBadDescriptor )
         {
         RBuf resourceName;
         CleanupClosePushL(resourceName);
         resourceName.CreateL( resourceFile.Length()+ 1 + resourceId.Length() );
         // concatenate resource file with id, localization understands it
         resourceName.Append(resourceFile);
         resourceName.Append(KColon);
         resourceName.Append(resourceId);

         FillParamsL( aParamList, aPos, *iCachedDesParameters,
                 *iCachedIntParameters);
         
         HnStringHandler::LocaliseL( aValue, resourceName, 
                 iCachedDesParameters, iCachedIntParameters, iCmnPtrs );
         
         CleanupStack::PopAndDestroy(&resourceName);
         }
        
    CleanupStack::PopAndDestroy(&resourceId);
    CleanupStack::PopAndDestroy(&resourceFile);
    
    return (retFile != KErrNone) ? retFile : retId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnMdValueText::GetSimpleValueL( 
        const CLiwGenericParamList* aParamList, TInt aPos, RBuf& /*aValue*/ )
    {
    TInt ret(KErrNotFound);
    RBuf vbuf;
    CleanupClosePushL(vbuf);
    
    ret = HnLiwUtils::GetStringL( *aParamList, iSimpleValue, aPos, vbuf );
    if ( ret != KErrNotFound )
        {            
        FillParamsL( aParamList, aPos, *iCachedDesParameters,
                *iCachedIntParameters);
        
        HnStringHandler::LocaliseL( iCachedValue, vbuf,  iCachedDesParameters,                
                iCachedIntParameters, iCmnPtrs );
        }
    CleanupStack::PopAndDestroy(&vbuf); 
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TInt CHnMdValueText::GetL( 
        const CLiwGenericParamList* aParamList, TInt aPos,
        TPtrC& aValue )
    {
    TInt ret(KErrNone);
    iCachedValue.Close();
    // first let's check resource file and id
    if (iResourceFile.Length() > 0 && iResourceId.Length() > 0)
        {
        ret = GetFromResourceFileL(aParamList, aPos, iCachedValue);
        }
    // if still cached value is empty, let's try with simple value
    if (iSimpleValue.Length() > 0 && iCachedValue.Length() == 0 )
        {
        ret = GetSimpleValueL(aParamList, aPos, iCachedValue);
        }
    
    aValue.Set(iCachedValue);
    return ret;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CHnAttributeBase* CHnMdValueText::CreateAttributeL( 
            const CLiwGenericParamList* aQueriesResultsList,
            TInt aPos )
    {
    CHnAttributeText* attr = 0;
    TPtrC val;
    if( GetL(aQueriesResultsList, aPos, val) != KErrNotFound )
        {
        attr = CHnAttributeText::NewLC();
        HBufC8* bufVal( HnConvUtils::StrToStr8LC( val ) );
        attr->SetValueL( *bufVal );
        
        // clean up
        CleanupStack::PopAndDestroy( bufVal );
        CleanupStack::Pop( attr );
        }
    return attr;
    }

