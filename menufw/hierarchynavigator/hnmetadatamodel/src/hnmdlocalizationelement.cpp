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


#include <f32file.h>
#include "hnmdlocalizationelement.h"
#include "hnglobals.h"
#include "hnconvutils.h"
#include "hnutils.h"

const TText8 KSPChar = ' ';
const TText8 KNLChar = '\n';
const TInt KWholeFileReadLimit = 1024;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 HBufC8Hash( HBufC8* const &  aBuf )
    {
    return DefaultHash::Des8(*aBuf);
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool HBufC8Ident( HBufC8* const & aL, HBufC8* const & aR )
    {
    return DefaultIdentity::Des8(*aL, *aR);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalizationElement::ConstructL( const TDesC& aNamespace, 
                                           const TDesC& aSource )
    {
    iNamespace.CreateL( aNamespace );
    iSource.CreateL( aSource );
    iSource.LowerCase();
    LocateLanguageFileL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalizationElement::ConstructL( TXmlEngElement aElement )
    {
    ASSERT( aElement.HasAttributeL( KNameSpaceAttrName8 ) );

    SetNamespaceL( aElement.AttributeNodeL( KNameSpaceAttrName8 ) );
    SetSourceL( aElement.AttributeNodeL( KSourceAttrName8 ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalizationElement::SetNamespaceL( TXmlEngAttr aNamespace )
    {
    HBufC* namespac = HnConvUtils::Str8ToStrLC( aNamespace.Value() );
    
    iNamespace.Close();
    iNamespace.CreateL( *namespac );

    CleanupStack::PopAndDestroy( namespac );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalizationElement::SetSourceL( TXmlEngAttr aSource )
    {
    HBufC* source = HnConvUtils::Str8ToStrLC( aSource.Value() );
    iSource.Close();
    iSource.CreateL( *source );
    iSource.LowerCase();
    LocateLanguageFileL();
    CleanupStack::PopAndDestroy( source );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdLocalizationElement* CHnMdLocalizationElement::NewL(
        TXmlEngElement aElement )
    {
    CHnMdLocalizationElement* self = new( ELeave ) CHnMdLocalizationElement;
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdLocalizationElement* CHnMdLocalizationElement::NewL(const TDesC& aNamespace, 
                                                         const TDesC& aSource)
    {
    CHnMdLocalizationElement* self = new( ELeave ) CHnMdLocalizationElement;
    CleanupStack::PushL( self );
    self->ConstructL( aNamespace, aSource );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdLocalizationElement::CHnMdLocalizationElement():
    iResourceIDs( &HBufC8Hash, &HBufC8Ident )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdLocalizationElement::~CHnMdLocalizationElement()
    {
    iNamespace.Close();
    iSource.Close();
    delete iSourcePath;
    THashMapIter<HBufC8*, TInt> iter( iResourceIDs );
    while ( HBufC8* const * ptr = iter.NextKey() )
        {       
        delete *ptr;
        }    
    iResourceIDs.Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CHnMdLocalizationElement::Namespace() const
    {
    return iNamespace;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC& CHnMdLocalizationElement::Source() const
    {
    return iSource;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* CHnMdLocalizationElement::SourcePath() const
    {
    return iSourcePath;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdLocalizationElement::SourceExists() const
    {
    if ( iSourcePath )
    	{
        return iSourcePath->Compare(KNullDesC) != 0;
    	}
    else
    	{
    	return EFalse;
    	}
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalizationElement::LocateLanguageFileL()
	{
	delete iSourcePath;
    iSourcePath = NULL;
    iSourcePath = HnUtils::LocateNearestLanguageFileLC( iSource );
    CleanupStack::Pop( iSourcePath );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TInt& CHnMdLocalizationElement::FindResourceIdL( HBufC8* aResourceName )
    {
    if ( iState&EInitialized )
        {
        // check if we have it in cache
        TInt* result = iResourceIDs.Find(aResourceName);
        if (result)
            {
            return *result;
            }
        else if (iState&EWholeFileCached) // all should be in cache
            {
            User::Leave(KErrNotFound);
            }
        }
    
    if (!(iState&EInitialized) || iState&EUsedItemsCached )
        {
        // set initialized here, as we could have problem with file read
        // i.e. file is missing, but we dont want to go here next time
        iState |= EInitialized;
        HBufC8* rsgContent = ReadRsgContentLC( );
		if (rsgContent->Length() < KWholeFileReadLimit)
			{
			iState |= EWholeFileCached;
			ParseRsgContentL(*rsgContent);
			}
		else
			{
			iState |= EUsedItemsCached;
			ParseRsgContentL(*rsgContent, *aResourceName);
			}          
        //cleanup
        CleanupStack::PopAndDestroy(rsgContent);   
        }
    
    return iResourceIDs.FindL(aResourceName);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* CHnMdLocalizationElement::ReadRsgContentLC( )
    {
    HBufC8* result(NULL);
    if ( iSourcePath->Compare( KNullDesC ) )
    	{
    	HBufC* fPath = iSourcePath->AllocLC();
		// change extension to rsg        
		TInt extPos = fPath->LocateReverse(KExtDelimiter);
		fPath->Des().Replace(extPos, fPath->Length() - extPos, KRsg);
		result = HnUtils::ReadFileLC(*fPath);
		CleanupStack::Pop(result);
		CleanupStack::PopAndDestroy(fPath);
		CleanupStack::PushL(result);
    	}
    else
    	{
        result = KNullDesC8().AllocLC();   
    	}
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalizationElement::ParseRsgContentL( const TDesC8& aContent )
    {
    TPtrC8 content(aContent);
    TInt posEoL(-1);
    // iterate over each line
    while ((posEoL = content.Locate(KNLChar)) > 0)
        {
        TPtrC8 line = content.Left( posEoL - 1 );        
        TInt firstSpace  = line.Locate(KSPChar);
        TInt lastSpace  = line.LocateReverse(KSPChar);
        // resource name
        TPtrC8 rname =  line.Mid(firstSpace, lastSpace - firstSpace);
        // resource id
        TPtrC8 rid =  line.Right( line.Length() - lastSpace - 1 );
        // trim resource name
        HBufC8* rnameTrimmed = rname.AllocLC();
        rnameTrimmed->Des().Trim();                
        // parse numer and insert to map
        TInt id(0);
        User::LeaveIfError( HnConvUtils::Str8ToInt(rid, id) );        
        User::LeaveIfError( iResourceIDs.Insert(rnameTrimmed->AllocL(), id) );        
        CleanupStack::PopAndDestroy(rnameTrimmed);
        // start search from next line
        content.Set(content.Mid( posEoL + 1 ));
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdLocalizationElement::ParseRsgContentL( const TDesC8& aContent, 
                                                 const TDesC8& aResourceName )
    {
    TPtrC8 content(aContent);
    
    HBufC8* resourceName = HBufC8::NewLC( aResourceName.Length() 
                                          + 2 /* extra spaces */  );
    // add extra space to make sure that we find the right one    
    resourceName->Des().Append(KSPChar);
    resourceName->Des().Append(aResourceName);
    resourceName->Des().Append(KSPChar);    
    
    TInt position = content.Find(*resourceName);
    User::LeaveIfError(position); // we can not find resource
    TPtrC8 contentAfterMid = content.Mid( position + 
                                          resourceName->Length());

    TInt posResId = contentAfterMid.Locate(KNLChar);
    User::LeaveIfError(posResId); // we can not find resource
    TPtrC8 rid = contentAfterMid.Left(posResId - 1);    
    
    HBufC8* ridTrimmed = rid.AllocLC();
    ridTrimmed->Des().Trim();
    // trim also resource name
    resourceName->Des().Trim();

    TInt id(0);
    User::LeaveIfError( HnConvUtils::Str8ToInt( *ridTrimmed, id) );      
    User::LeaveIfError( iResourceIDs.Insert( resourceName->AllocL(), id) );
    // clean up
    CleanupStack::PopAndDestroy(ridTrimmed);
    CleanupStack::PopAndDestroy(resourceName);        
    }
