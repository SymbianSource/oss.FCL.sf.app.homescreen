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


#include <coemain.h>
#include <bautils.h>
#include <StringLoader.h>

#include "hnmdlocalization.h"
#include "hnmdlocalizationelement.h"
#include "hnglobals.h"
#include "hnconvutils.h"
#include "hnutils.h"

// ======== MEMBER FUNCTIONS ========

/** Max param count */
static const TInt KMaxParamsCount( 10 );
const TInt KFileExtensionLength( 4 );

_LIT(KLocalizationOneDes, "%U");
_LIT(KLocalizationMoreDes, "%?U");
_LIT(KLocalizationOneInt, "%N");
_LIT(KLocalizationMoreInt, "%?N");

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalization::ConstructL()
    {
    iCoeEnv = CCoeEnv::Static();
    ASSERT( iCoeEnv );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdLocalization* CHnMdLocalization::NewL()
    {
    CHnMdLocalization* self = CHnMdLocalization::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdLocalization* CHnMdLocalization::NewLC()
    {
    CHnMdLocalization* self = new( ELeave ) CHnMdLocalization;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalization::AppendLocalizationsL( TXmlEngElement aElement )
    {
    RXmlEngNodeList< TXmlEngElement > children;
    CleanupClosePushL( children );
    aElement.GetChildElements( children );

    RPointerArray<CHnMdLocalizationElement> tempArray;
    CleanupClosePushL( tempArray );
    TInt count = children.Count();
    for ( TInt i( 0 ); i < count; i++ )
        {
        TXmlEngElement child = children.Next();
        
        if ( !child.Name().Compare( KLocalizationElementName8 ) )
            {
            RBuf8 namespac;
            CleanupClosePushL( namespac );
            if ( child.HasAttributeL( KNameSpaceAttrName8 ) )
                {
                HnUtils::SetAttributeL( child, KNameSpaceAttrName8, namespac );
                if ( !IsDuplicateL( namespac ) )
                    {
                    CHnMdLocalizationElement* element =
                        CHnMdLocalizationElement::NewL( child );
            
                    AppendElementL( element ); // ownership transfered
                    }
                }
            CleanupStack::PopAndDestroy( &namespac );
            }
        }
    
    CleanupStack::PopAndDestroy( &tempArray );
    CleanupStack::PopAndDestroy( &children );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CHnMdLocalizationElement* CHnMdLocalization::ElementByNamespace(
        const TDesC& aNamespace ) const
    {
    TInt count = iInternalLocalization.Count();
    const CHnMdLocalizationElement* element = NULL;
    
    for ( TInt i = 0; i < count; i++ )
        {
        const CHnMdLocalizationElement* tmp = iInternalLocalization[i];
        if ( !tmp->Namespace().Compare( aNamespace) )
            {
            element = tmp;
            break;
            }
        }
    if( !element )
        {
        count = iDynamicLocalization.Count();
            
        for ( TInt i = 0; i < count; i++ )
            {
            const CHnMdLocalizationElement* tmp = iDynamicLocalization[i];
            if ( !tmp->Namespace().Compare( aNamespace) )
                {
                element = tmp;
                break;
                }
            }
        }

    return element;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdLocalization::CHnMdLocalization()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdLocalization::~CHnMdLocalization()
    {
    iDynamicLocalization.ResetAndDestroy();
    for( TInt i( 0 ); i < iDynamicOffset.Count(); i++ )
        {
        iCoeEnv->DeleteResourceFile( iDynamicOffset[i] );
        }
    iDynamicOffset.Close();
    
    iInternalLocalization.ResetAndDestroy();
    for( TInt i( 0 ); i < iInternalOffset.Count(); i++ )
        {
        iCoeEnv->DeleteResourceFile( iInternalOffset[i] );
        }
    iInternalOffset.Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalization::ReloadResourceFilesL()
    {
    for( TInt i( 0 ); i < iDynamicLocalization.Count(); i++ )
        {
        iDynamicLocalization[i]->LocateLanguageFileL();
        if( iDynamicLocalization[i]->SourcePath()  )
            {        
            TInt offset( 0 );
            TRAPD( err, offset = iCoeEnv->AddResourceFileL(
                    *(iDynamicLocalization[i]->SourcePath()) ) );
            if ( !err )
                {
                iDynamicOffset.AppendL( offset );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalization::ReleaseResourceFiles()
    {
    iDynamicLocalization.ResetAndDestroy();
    for( TInt i( 0 ); i < iDynamicOffset.Count(); i++ )
        {
        iCoeEnv->DeleteResourceFile( iDynamicOffset[i] );
        }
    iDynamicOffset.Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
void CHnMdLocalization::AppendElementL( CHnMdLocalizationElement* aElement )
    {
    if( IsInternalL( aElement->Source() ) )
        {
        iInternalLocalization.AppendL( aElement );
        if( IsResourceFile (aElement->Source()) )
            {
            TInt offset( 0 );
            TRAPD( err, offset = iCoeEnv->AddResourceFileL( *(aElement->SourcePath()) ) ); 
            if ( !err )
                {
                iInternalOffset.AppendL( offset );
                }
            }
        }
    else
        {
        iDynamicLocalization.AppendL( aElement );
        if( IsResourceFile (aElement->Source()) )
            {
            TInt offset( 0 );
            TRAPD( err, offset = iCoeEnv->AddResourceFileL( *(aElement->SourcePath()) ) ); 
            if ( !err )
                {
                iDynamicOffset.AppendL( offset );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TBool CHnMdLocalization::IsDuplicateL( TDesC8& aNamespace )
	{
	HBufC* namespac = HnConvUtils::Str8ToStrLC( aNamespace );
	TBool isDuplicate( IsDuplicateL( *namespac ) );
    CleanupStack::PopAndDestroy( namespac );
    return isDuplicate;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TBool CHnMdLocalization::IsDuplicateL( TDesC& aNamespace )
    {
    TBool isDuplicate( EFalse );
    
    if ( ElementByNamespace( aNamespace ) != NULL )
        {
        isDuplicate = ETrue;
        }
    
    return isDuplicate;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TBool CHnMdLocalization::IsInternalL( const TDesC& aName )
    {
    TBool isInternal( EFalse );
    _LIT( KDot, "." );
    
    TInt offset( aName.Find( KDot ) );
    if( offset != KErrNotFound )
        {
        if( aName.Left( offset ).Compare( KFolderSuite ) == KErrNone )
            {
            isInternal = ETrue;
            }
        }
    
    return isInternal;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
TBool CHnMdLocalization::IsResourceFile( const TDesC& aFileName )
    {
    TInt pos = aFileName.Find( KExtension );
    if( aFileName.Right( KFileExtensionLength ).Compare( KExtensionRsc ) )
    	{
        TBuf<2> tail;
        tail.Copy(aFileName.Right(2));
        for (TInt i = 0; i < tail.Length(); i++ )
        	{
            TChar c = tail[i];
            if ( !c.IsDigit() )
            	{
                pos = KErrNotFound;
                break;
            	}
        	}
    	}
    return pos == ( aFileName.Length() - KFileExtensionLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
HBufC* CHnMdLocalization::LoadL( const TDesC& aResourceName,
    const CDesC16Array* aDesParams, const CArrayFix<TInt>* aIntParams )
    {
    TInt position = aResourceName.Locate(':');
    HBufC* res = NULL;
    if( position!= KErrNotFound )
        {
        TPtrC resourceName = aResourceName.Mid(position + 1);
        TPtrC locName = aResourceName.Left(position);
        
        const CHnMdLocalizationElement* localization = 
            ElementByNamespace( locName );
        // if there is not namaspace defined, check may it is file in format
        // myapp.rsc:id
        if (!localization && !IsDuplicateL( locName ) && IsResourceFile( locName ))
            {
            // let's try to add namespace
            CHnMdLocalizationElement* element = 
                CHnMdLocalizationElement::NewL( locName, locName);
            AppendElementL(element); // ownership transfered
            localization =  ElementByNamespace( locName );
            }
        
        if( localization && IsResourceFile( localization->Source() ) && localization->SourceExists() )
            {
			HBufC8* resourceName8 = HBufC8::NewLC(resourceName.Length());
			resourceName8->Des().Copy(resourceName);
			TInt resourceId (KErrNotFound);
			TRAPD(err,  resourceId = 
				const_cast<CHnMdLocalizationElement*>(localization)->FindResourceIdL(resourceName8) );
			if ( err != KErrNone ) // maybe it was only number
				{
				err = HnConvUtils::Str8ToInt(*resourceName8, resourceId);
				}
			User::LeaveIfError(err); 
			if ( iCoeEnv->IsResourceAvailableL( resourceId ) )
				{
				res = iCoeEnv->AllocReadResourceL( resourceId );
				}

			CleanupStack::PopAndDestroy( resourceName8 );
            }
        }
    
    if (!res) // it mean that we dont have localization item for it
        {
        res = aResourceName.AllocL();
        }
    if ((aDesParams && aDesParams->Count()) ||
        (aIntParams && aIntParams->Count()))
        {
        CleanupStack::PushL(res);
        HBufC* formatted = FormatTextL(*res, aDesParams, aIntParams);
        CleanupStack::PopAndDestroy(res);
        res = formatted;
        }
     
    return res;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* CHnMdLocalization::FormatTextL( const TDesC& aTextToFormat,
    const CDesC16Array* aDesParams, const CArrayFix<TInt>* aIntParams )
    {
    HBufC* res = aTextToFormat.AllocL();
    TInt noParams(0);
    
    if (aDesParams)
        noParams += aDesParams->Count();
    if (aIntParams)
        noParams += aIntParams->Count();   
    
    if (noParams > KMaxParamsCount)
        User::Leave( KErrNotSupported );
    
    // let's check if we have descriptors to format
    if( aDesParams && aDesParams->Count() )
         {
         // if we have only one we match to %U
         if (aDesParams->Count() == 1)
             {
             if ( res->FindC(KLocalizationOneDes) != KErrNotFound )
                 {
                 TPtrC16 param = aDesParams->MdcaPoint(0);
                 HBufC* dest = HBufC::NewL( res->Length() +
                                            param.Length() * KMaxParamsCount );
                 TPtr destDes(dest->Des());
                 StringLoader::Format( destDes, *res, -1, param );
                 delete res;
                 res = dest;
                 }
             }
         // and we match for format %(index)U
         TBufC<3> descformat(KLocalizationMoreDes);
         TInt desIndexUsed(0);
         for (TInt i = 0; i < noParams && desIndexUsed < aDesParams->Count(); i++)
             {
             descformat.Des()[1] = '0'+ i; // replace index infromation
             if ( res->FindC( descformat ) != KErrNotFound )
                 {
                 
                 TPtrC16 param = aDesParams->MdcaPoint(desIndexUsed++);
                 HBufC* dest = HBufC::NewL( res->Length() + 
                                            param.Length() * KMaxParamsCount );
                 TPtr destDes(dest->Des());
                 StringLoader::Format( destDes, *res, i, param );
                 delete res;
                 res = dest;
                 }
             }
         }
    // let's check if we have numbers to format
    if( aIntParams && aIntParams->Count() )
          {
          // if we have only one we match to %N
          if (aIntParams->Count() == 1)
              {
              if ( res->FindC(KLocalizationOneInt) != KErrNotFound )
                  {
                  HBufC* dest = HBufC::NewL( res->Length() +
                                             sizeof(TInt) * KMaxParamsCount );
                  TPtr destDes(dest->Des());
                  StringLoader::Format( destDes, *res, -1, aIntParams->At(0) );
                  delete res;
                  res = dest;
                  }
              }
          // and we match for format %(index)N
          TBufC<3> format(KLocalizationMoreInt);
          TInt numberUsed(0);
          for (TInt i = 0; i < noParams && numberUsed < aIntParams->Count(); i++)
              {
              format.Des()[1] = '0'+ i;
              if ( res->FindC(format) != KErrNotFound )
                  {
                  HBufC* dest = HBufC::NewL( res->Length() +
                                             sizeof(TInt)* KMaxParamsCount );
                  TPtr destDes(dest->Des());
                  StringLoader::Format( destDes, *res, i, aIntParams->At(numberUsed++) );
                  delete res;
                  res = dest;
                  }
              }
          }
    return res;
    }


