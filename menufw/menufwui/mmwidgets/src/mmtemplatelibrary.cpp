/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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



#include "mmtemplatelibrary.h"
#include "mmlctutils.h"
#include "mmwidgetsconstants.h"
#include <hnutils.h>
#include <gdi.h>
#include <hnconvutils.h>
#include "menudebug.h"
#include <hnglobals.h>
#include <AknUtils.h>
#include <AknDef.hrh>



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 HBuf16Hash( HBufC8* const &  aBuf )
    {
    return DefaultHash::Des8(*aBuf);
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool HBuf16Ident( HBufC8* const & aL, HBufC8* const & aR )
    {
    return DefaultIdentity::Des8(*aL, *aR);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//:
CMmTemplateLibrary::CMmTemplateLibrary() 
	: iWidgetType(EWidgetTypeNone),
	iTemplateSizesMap( &HBuf16Hash, &HBuf16Ident ), 
	iTemplateChildrenMap( &HBuf16Hash, &HBuf16Ident ),
	iMoveIndicatorRectsMap( &HBuf16Hash, &HBuf16Ident )
	{
	// No implementation required
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMmTemplateLibrary::~CMmTemplateLibrary()
	{
	CleanAndClearCache();
	}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmTemplateLibrary::CleanAndClearCache( )
    {

	THashMapIter<HBufC8*, TSize> iter( iTemplateSizesMap );
	while ( HBufC8* const * ptr = iter.NextKey() )
		{       
		delete *ptr;
		}    
	iTemplateSizesMap.Close();

	THashMapIter<HBufC8*, RArray<TTemplateChild> > iter2( iTemplateChildrenMap );
	while ( HBufC8* const * ptr = iter2.NextKey() )
		{       
		iter2.CurrentValue()->Close();
		delete *ptr;
		}    
	iTemplateChildrenMap.Close();
    
	THashMapIter<HBufC8*, TRect> iter4( iMoveIndicatorRectsMap );
	while ( HBufC8* const * ptr = iter4.NextKey() )
		{       
		delete *ptr;
		}    
	iMoveIndicatorRectsMap.Close();
    
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CMmTemplateLibrary* CMmTemplateLibrary::NewL()
	{
	CMmTemplateLibrary* self = CMmTemplateLibrary::NewLC();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CMmTemplateLibrary* CMmTemplateLibrary::NewLC()
    {
    CMmTemplateLibrary* self = new( ELeave ) CMmTemplateLibrary();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
	
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmTemplateLibrary::ConstructL()
	{
	//No implementation needed.
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TSize CMmTemplateLibrary::GetSize(  TMmWidgetType aWidgetType, const TDesC8& aTemplate, 
		TBool aLandscapeOrientation, TBool aHighlighted, TRect aParentRect )
	{
	TSize result;
	GetSize(result, aWidgetType, aTemplate, aLandscapeOrientation, 
			aHighlighted, aParentRect );
	return result;	
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CMmTemplateLibrary::GetSize(  TSize& aItemSize, 
        TMmWidgetType aWidgetType, const TDesC8& aTemplate, 
        TBool aLandscapeOrientation, TBool aHighlighted, TRect aParentRect )
    {
    if ( KNullDesC8()== aTemplate || aParentRect == TRect(TPoint(0,0), TPoint(0,0))
    		|| !aTemplate.Compare( KEmpty8 ) )
        {
        return KErrNotFound;
        }
    UpdateParentRect( aParentRect, aLandscapeOrientation );

    TInt err( KErrNone );
    HBufC8* lookup_string = LookupText( aTemplate, aWidgetType, iZoom, 
        aLandscapeOrientation, aHighlighted );    
    	
    TSize* itemSize = iTemplateSizesMap.Find( lookup_string );
    if ( !itemSize )
        {
        TRAP( err, LoadTemplateL( aWidgetType, aTemplate, aLandscapeOrientation ) );
        if ( err == KErrNone )
            {
            itemSize = iTemplateSizesMap.Find( lookup_string );
        	if( !itemSize )
        		{
                User::Panic( KMtlPanic, KErrNotFound );
        		}
            aItemSize = *itemSize;
            }
        }
    else
        {
        iWidgetType = aWidgetType;
        TSize layoutSize;
        TInt err( KErrNone );
        TRAP( err, layoutSize = GetLayoutSizeL( iWidgetType, aTemplate, 
                aLandscapeOrientation ) );
        if( err == KErrNone )
            {
            AdjustItemSize( *itemSize, layoutSize, aLandscapeOrientation );
            }
        aItemSize = *itemSize;
        }
	delete lookup_string;
    
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TSize CMmTemplateLibrary::GetLayoutSizeL( TMmWidgetType aWidgetType, const TDesC8& aTemplate, 
		TBool aLandscapeOrientation )
	{
	if ( KNullDesC8()== aTemplate )
		{
		User::Leave( -1 );
		}
	TSize* layoutSize = NULL;
	switch ( aWidgetType )
		{
		case EGrid:
			{
		    HBufC8* lookup_string = LookupLayoutText( aWidgetType, iZoom, 
                aTemplate, aLandscapeOrientation );
			CleanupStack::PushL( lookup_string );
		    layoutSize = iTemplateSizesMap.Find( lookup_string );
		    if (!layoutSize)
		        {
		        LoadTemplateL( aWidgetType, aTemplate, aLandscapeOrientation );
		        layoutSize = iTemplateSizesMap.Find( lookup_string );
		        if( !layoutSize )
		            User::Panic( KMtlPanic, -1);
		        }
			CleanupStack::PopAndDestroy( lookup_string );
			}
		
			break;
		case EListbox:
			return TSize( MmListBox::KCols,0 );
		default:
			User::Panic( KMtlPanic, -1);
		}
	return *layoutSize;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TRect CMmTemplateLibrary::GetMoveIndicatorRect(TMmWidgetType aWidgetType,
		const TDesC8& aTemplate, TBool aLandscapeOrientation,
		TBool aHighlighted)
	{
	ASSERT( KNullDesC8() != aTemplate && aTemplate.Compare( KEmpty8 ) );
	
	HBufC8* lookup_string = LookupText(aTemplate, aWidgetType, iZoom, 
        aLandscapeOrientation, aHighlighted );
	HBufC8* lookup_indicator_string = LookupIndicatorText( *lookup_string );    

	TRect* itemRect = iMoveIndicatorRectsMap.Find( lookup_indicator_string );
    if (!itemRect)
        {
    	TSize itemSize;
    	GetSize( itemSize, aWidgetType, aTemplate, aLandscapeOrientation, EFalse, GetParentRect( aLandscapeOrientation ) );
    	TRAPD( err, SetupMoveIndicatorTemplateChildrenL(
    	        *lookup_indicator_string, itemSize ) );
    	if ( KErrNone == err )
    	    {
            itemRect = iMoveIndicatorRectsMap.Find( lookup_indicator_string );
            if (!itemRect)
                {
                User::Panic( KMtlPanic, -1);
                }
    	    }
        }
    
    delete lookup_indicator_string;
    delete lookup_string;
    return *itemRect;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmTemplateLibrary::GetChildrenL(TMmWidgetType aWidgetType, RArray<
		TTemplateChild>& aArray, const TDesC8& aTemplate,
		TBool aLandscapeOrientation, TBool aHighlighted, TBool aIsEditMode)
	{
	ASSERT( KNullDesC8() != aTemplate && aTemplate.Compare( KEmpty8 ) );

	HBufC8* lookup_string = LookupText( aTemplate, aWidgetType, iZoom,
        aLandscapeOrientation, aHighlighted );
	CleanupStack::PushL( lookup_string );
    RArray<TTemplateChild>* children = iTemplateChildrenMap.Find( lookup_string );
    if ( !children )
        {
        LoadTemplateL( aWidgetType, aTemplate, aLandscapeOrientation );
        children = iTemplateChildrenMap.Find( lookup_string );
        if (!children)
        	{
            User::Panic( KMtlPanic, -1);    
        	}    
        }
    CleanupStack::PopAndDestroy( lookup_string );
    for (TInt i = 0; i < children->Count(); i++)
    	aArray.AppendL((*children)[i]);
    if ( aIsEditMode )
    	{
    	TSize size = GetSize( aWidgetType, aTemplate, aLandscapeOrientation, aHighlighted, GetParentRect(aLandscapeOrientation) );
        AppendEditModeTemplateL( aArray, size );
    	}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMmTemplateLibrary::GetMoveIndicatorChildrenL(TMmWidgetType aWidgetType,
		RArray<TTemplateChild>& aArray, const TDesC8& aTemplate,
		TBool aLandscapeOrientation, TBool aHighlighted)
	{
	ASSERT( KNullDesC8() != aTemplate && aTemplate.Compare( KEmpty8 ) );
	
	HBufC8* lookup_string = LookupText(aTemplate, aWidgetType, iZoom, 
        aLandscapeOrientation, aHighlighted );
	CleanupStack::PushL( lookup_string );
	HBufC8* lookup_indicator_string = LookupIndicatorText( *lookup_string );    
	CleanupStack::PushL( lookup_indicator_string );
	
    RArray<TTemplateChild>* children = iTemplateChildrenMap.Find( lookup_indicator_string );
    if ( !children )
    	{
    	TSize itemSize;
    	GetSize( itemSize, aWidgetType, aTemplate, aLandscapeOrientation, EFalse, GetParentRect( aLandscapeOrientation ) ); 
        SetupMoveIndicatorTemplateChildrenL( *lookup_indicator_string, itemSize );
        children = iTemplateChildrenMap.Find( lookup_indicator_string );
        if (!children)
        	User::Panic( KMtlPanic, -1);
    	}
    
    CleanupStack::PopAndDestroy( lookup_indicator_string );
	CleanupStack::PopAndDestroy( lookup_string );
	
    for (TInt i = 0; i < children->Count(); i++)
    	{
    	aArray.AppendL((*children)[i]);
    	}
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
HBufC8* CMmTemplateLibrary::LookupText( const TDesC8& aTemplate, 
    TMmWidgetType aWidgetType, TAknUiZoom aZoom, TBool aLandscapeOrientation, 
    TBool aHighlighted )
    {
    HBufC8* lookup_string = HBufC8::New( MmTemplateContants::KTemplateChildTextLength );
    if (lookup_string)
    	{
		TPtr8 lookup_string_ptr( lookup_string->Des() );
		lookup_string_ptr.Append( aTemplate );
		lookup_string_ptr.Append( KColon8 );
		switch ( aWidgetType )
			{
			case EListbox:
				lookup_string_ptr.Append( KListbox8 );
				break;
			case EGrid:
				lookup_string_ptr.Append( KGrid8 );
				break;
			}
		lookup_string_ptr.Append( KColon8 );
		lookup_string_ptr.AppendNum( aLandscapeOrientation );
		lookup_string_ptr.Append( KColon8 );
		lookup_string_ptr.AppendNum( aHighlighted );
		lookup_string_ptr.Append( KColon8 );
		switch ( aZoom )
            {
            case EAknUiZoomLarge :
                lookup_string_ptr.Append( KZoomLarge8 );
                break;
            case EAknUiZoomSmall :
            	lookup_string_ptr.Append( KZoomSmall8 );
            	break; 
            default :
                lookup_string_ptr.Append( KZoomNormal8 );
                break;
            }
    	}
    return lookup_string;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
HBufC8* CMmTemplateLibrary::LookupLayoutText( TMmWidgetType aWidgetType,
    TAknUiZoom aZoom, const TDesC8& aTemplate, TBool aLandscapeOrientation )
	{
    HBufC8* lookup_string = HBufC8::New( MmTemplateContants::KTemplateChildTextLength );
    if (lookup_string)
    	{
		TPtr8 lookup_string_ptr( lookup_string->Des() );
		switch ( aWidgetType )
			{
			case EListbox:
				lookup_string_ptr.Append( KListbox8 );
				break;
			case EGrid:
				lookup_string_ptr.Append( KGrid8 );
				break;
			}
		lookup_string_ptr.Append( KColon8 );
        switch ( aZoom )
            {
            case EAknUiZoomLarge:
                lookup_string_ptr.Append( KZoomLarge8 );
                break;
            case EAknUiZoomSmall :
            	lookup_string_ptr.Append( KZoomSmall8 );
                break;
            default:
                lookup_string_ptr.Append( KZoomNormal8 );
                break;
            }
        lookup_string_ptr.Append( KColon8 );
		lookup_string_ptr.Append( aTemplate );
		lookup_string_ptr.Append( KColon8 );
		lookup_string_ptr.AppendNum( aLandscapeOrientation );
    	}
    return lookup_string;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
HBufC8* CMmTemplateLibrary::LookupIndicatorText( const TDesC8& aLookupText )
	{
	HBufC8* lookup_string = HBufC8::New( MmTemplateContants::KTemplateChildTextLength );
	if (lookup_string)
		{
		TPtr8 lookup_string_ptr( lookup_string->Des() );
		lookup_string_ptr.Append( KMI8 );
		lookup_string_ptr.Append( KColon8 );
		lookup_string_ptr.Append( aLookupText );
		}
	
	return lookup_string;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
void CMmTemplateLibrary::LoadTemplateL( TMmWidgetType aWidgetType, 
		const TDesC8& aTemplate, TBool aLandscapeOrientation )
	{
	iWidgetType = aWidgetType;
	DEBUG(("_Mm_:CMmTemplateLibrary::LoadTemplateL IN")); 
	TMmTemplateType mmTemplateType;
	HBufC8* content = GetTemplateContentL( aTemplate, aWidgetType, mmTemplateType );
	CleanupStack::PushL( content );
    RXmlEngDOMImplementation domImpl;
    CleanupClosePushL( domImpl );
    RXmlEngDOMParser domParser;   
    CleanupClosePushL( domParser );
    DEBUG(("_Mm_:CMmTemplateLibrary::LoadTemplateL Opening domImpl"));
    domImpl.OpenL();
    User::LeaveIfError( domParser.Open( domImpl ) );
    DEBUG(("_Mm_:CMmTemplateLibrary::LoadTemplateL parsing content..."));
    RXmlEngDocument xmlDoc = domParser.ParseL( *content );
    CleanupClosePushL( xmlDoc );
    
    // first orientation
    RXmlEngNodeList<TXmlEngElement> orientElements;
    CleanupClosePushL( orientElements );
    xmlDoc.DocumentElement().GetChildElements( orientElements );
    TXmlEngElement orientationElement;
    while ( orientElements.HasNext() )
        {
        orientationElement = orientElements.Next();
        DEBUG(("_Mm_:CMmTemplateLibrary::LoadTemplateL Reading orientation"));
        TPtrC8 name = orientationElement.Name();
        DEBUG8(("\t_Mm_:orientation: %S", &name));
        if ( !name.Compare( KOrientation8 ) )
            {
            TBool landscapeOrientation(EFalse);
            if (orientationElement.AttributeValueL(KId8) == KLandscape8)
                landscapeOrientation = ETrue;
            if ( landscapeOrientation != aLandscapeOrientation )
            	{
            	continue;
            	}
            RXmlEngNodeList<TXmlEngElement> elements;
            orientationElement.GetChildElements( elements );
            CleanupClosePushL( elements );
            TXmlEngElement element;
            while ( elements.HasNext() )
                {
                element = elements.Next(); 
            	DEBUG(("_Mm_:iMmTemplateType != EMmTemplateMoveIndicator"));
            	if (element.AttributeValueL(KId8) == KHighlight8)
                    {
                    DEBUG(("_Mm_:AttributeValueL - id == highlight"));
                    ProcessElementL( mmTemplateType, element, aTemplate, landscapeOrientation, ETrue );
                    DEBUG(("ProcesElementL END"));
                    }
                else if (element.AttributeValueL(KId8) == KNoHighlight8)
                    {
                    DEBUG(("_Mm_:AttributeValueL - id == nohighlight"));
                    ProcessElementL( mmTemplateType, element, aTemplate, landscapeOrientation, EFalse );
                    DEBUG(("ProcesElementL END"));
                    if ( aWidgetType == EGrid )
                        {
                        DEBUG(("_Mm_:aWidgetType == EGrid"));
                        ProcessElementL( mmTemplateType, element, aTemplate, landscapeOrientation, ETrue );
                        DEBUG(("ProcesElementL END"));
                        }
                    }
                }
            CleanupStack::PopAndDestroy( &elements );
            }
        }
    
    CleanupStack::PopAndDestroy( &orientElements );
    CleanupStack::PopAndDestroy( &xmlDoc );
    CleanupStack::PopAndDestroy( &domParser );
    CleanupStack::PopAndDestroy( &domImpl );
    CleanupStack::PopAndDestroy(content);
    DEBUG(("_Mm_:CMmTemplateLibrary::LoadTemplateL OUT")); 
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
void CMmTemplateLibrary::ProcessElementL(TMmTemplateType aMmTemplateType, 
										 TXmlEngElement aElement, 
                                         const TDesC8& aTemplate, 
                                         TBool aLandscapeOrientation,
                                         TBool aHighlighted )
    {
    switch ( aMmTemplateType )
	    {
	    case ETemplateTypeLCT:
	    	ProcessLCTTemplateElementL(aElement, aTemplate, aLandscapeOrientation, aHighlighted );
	    	break;
	    case ETemplateTypeCustom:
	    	ProcessCustomTemplateElementL(aElement, aTemplate, aLandscapeOrientation, aHighlighted );
	    	break;
	    }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::ProcessLCTTemplateElementL(TXmlEngElement aElement, 
                                         const TDesC8& aTemplate, 
                                         TBool aLandscapeOrientation,
                                         TBool aHighlighted )
	{
	RXmlEngNodeList<TXmlEngElement> layoutElements;
	CleanupClosePushL( layoutElements );
	aElement.GetChildElements( layoutElements );
	TXmlEngElement layoutElement;
	TSize itemSize;
	while ( layoutElements.HasNext() )
		{
		layoutElement = layoutElements.Next();

		if ( !layoutElement.Name().Compare( KLayout8 ) )
			{
			TPtrC8 lctAtt = layoutElement.AttributeValueL(KLct8);
			
			// set layout for grid
			TInt variety;
			if ( Zoom() == EAknUiZoomLarge && iWidgetType == EGrid)
				{
				HnConvUtils::Str8ToInt(layoutElement.AttributeValueL(KVarietyZoom8), variety);
				}
			else 
				{
				HnConvUtils::Str8ToInt(layoutElement.AttributeValueL(KVariety8), variety);
				}
			
			TSize layoutSize;
			if ( iWidgetType == EGrid )
				{
				layoutSize = MmLCTUtils::GetLayoutSize( lctAtt, variety );
				CacheLayoutSizeL( layoutSize, aTemplate, aLandscapeOrientation );
				}
			else
				{
				layoutSize = GetLayoutSizeL( iWidgetType, aTemplate, aLandscapeOrientation );
				}

			TAknWindowLineLayout layout;
			TSize itemSize = GetLCTSize( lctAtt, variety, layout, aLandscapeOrientation );
			
			AdjustItemSize( itemSize, layoutSize, aLandscapeOrientation );
			HBufC8* lookup_string = LookupText( aTemplate, iWidgetType, iZoom,
                aLandscapeOrientation, aHighlighted );
			iTemplateSizesMap.InsertL( lookup_string, itemSize );
			
			// setup children
			RArray< TTemplateChild > childrenDefinition;
			CleanupClosePushL( childrenDefinition );
			RXmlEngNodeList<TXmlEngElement> childrenElements;
			CleanupClosePushL( childrenElements );
			layoutElement.GetChildElements( childrenElements );
			TXmlEngElement childElement;
			while ( childrenElements.HasNext() )
				{
				childElement = childrenElements.Next();
				TPtrC8 name = childElement.Name();
				if ( !name.Compare( KTextVisual8 ) || 
						!name.Compare( KImageVisual8 ) )
					{   
					TTemplateChild childTemplate;
					childTemplate.iLct = childElement.AttributeValueL(KLct8);
					TInt variety;
					if ( Zoom() != EAknUiZoomNormal && iWidgetType == EGrid )
						{
						HnConvUtils::Str8ToInt(childElement.AttributeValueL(KVarietyZoom8), variety);
						}
					else 
						{
						HnConvUtils::Str8ToInt(childElement.AttributeValueL(KVariety8), variety);
						}
					childTemplate.iVariety = variety;

					TPtrC8 ptr = childElement.AttributeValueL(KHAlign8);
                    if (!ptr.Compare(KNullDesC8))
                        {
                        childTemplate.iHAlign = EManualAlignUndefined;
                        }
                    else if (!ptr.Compare(KLeft8))
                        {
                        childTemplate.iHAlign = EManualAlignLeft;
                        }
                    else if (!ptr.Compare(KCenter8))
                        {
                        childTemplate.iHAlign = EManualAlignCenter;
                        }
                    else if (!ptr.Compare(KRight8))
                        {
                        childTemplate.iHAlign = EManualAlignRight;
                        }
					
					SetupLCTTemplateL( childTemplate, childElement, itemSize );
					childrenDefinition.AppendL( childTemplate );
					}
				}
			CleanupStack::PopAndDestroy( &childrenElements );
			// save children defintion in map
			lookup_string = LookupText(aTemplate, iWidgetType, iZoom,
                aLandscapeOrientation, aHighlighted );
			iTemplateChildrenMap.InsertL(lookup_string, childrenDefinition);
			CleanupStack::Pop( &childrenDefinition );			
			break;
	        }
        }
    CleanupStack::PopAndDestroy( &layoutElements );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::ProcessCustomTemplateElementL(TXmlEngElement aElement, 
                                         const TDesC8& aTemplate, 
                                         TBool aLandscapeOrientation,
                                         TBool aHighlighted )
	{
    RXmlEngNodeList<TXmlEngElement> layoutElements;
    CleanupClosePushL( layoutElements ); 
    aElement.GetChildElements( layoutElements );
    TXmlEngElement layoutElement;
    while ( layoutElements.HasNext() )
        {
        layoutElement = layoutElements.Next();
        if ( !layoutElement.Name().Compare( KLayout8 ) )
            {
            TSize itemSize;
            // set sizes
        	TInt height;
            TInt width;
            HBufC8* lookup_string;
            
            HnConvUtils::Str8ToInt(layoutElement.AttributeValueL(KHeight8), height);
            HnConvUtils::Str8ToInt(layoutElement.AttributeValueL(KWidth8), width); 
        	lookup_string = LookupText( aTemplate, iWidgetType, iZoom,
                aLandscapeOrientation, aHighlighted );
        	itemSize = TSize( width, height );      	
            
        	
            TSize layoutSize = ( aLandscapeOrientation ) ? 
            		TSize( MmGrid::KColsLandscapeZoomNormal, MmGrid::KRowsLandscapeZoomNormal ): 
            		TSize( MmGrid::KColsPortraitZoomNormal, MmGrid::KRowsPortraitZoomNormal ) ;
            layoutSize = ( iWidgetType == EGrid ) ? layoutSize: TSize(MmListBox::KCols,0);
            CacheLayoutSizeL( layoutSize, aTemplate, aLandscapeOrientation );
            
            AdjustItemWidth( itemSize, layoutSize, aLandscapeOrientation );
        	iTemplateSizesMap.InsertL(lookup_string, itemSize);

            // set children
            RArray< TTemplateChild > childrenDefinition;
            CleanupClosePushL( childrenDefinition ); 
            
            RXmlEngNodeList<TXmlEngElement> childrenElements;
            CleanupClosePushL( childrenElements );
            layoutElement.GetChildElements( childrenElements );
            TXmlEngElement childElement;
            while ( childrenElements.HasNext() )
                {
                childElement = childrenElements.Next();
                TPtrC8 name = childElement.Name();
                if ( !name.Compare( KTextVisual8 ) || 
                        !name.Compare( KImageVisual8 ) )
                    {   
                    TTemplateChild childTemplate;
                    SetupCustomTemplateL( childTemplate, childElement );
                    childrenDefinition.AppendL( childTemplate );
                    }
                }
            CleanupStack::PopAndDestroy( &childrenElements );
            // save children defintion in map
        	lookup_string = LookupText(aTemplate, iWidgetType, iZoom, 
                aLandscapeOrientation, aHighlighted );
        	iTemplateChildrenMap.InsertL(lookup_string, childrenDefinition);
        	CleanupStack::Pop( &childrenDefinition );
            // we found first layout, it is enough
            break;
            }
        }
    CleanupStack::PopAndDestroy( &layoutElements );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//    
HBufC8* CMmTemplateLibrary::GetTemplateContentL(const TDesC8& aTemplate,
		TMmWidgetType aWidgetType, TMmTemplateType& aMmTemplateType)
	{
	HBufC8* content;
	TRAPD( err , GetTemplateFileContentL( content, aTemplate, aWidgetType, aMmTemplateType ) );
	if ( KErrNone != err || !content )
		{
//		if LCT template was not loaded try to load a custom template
		TRAPD( err , GetTemplateFileContentL( content, aTemplate, aWidgetType, aMmTemplateType, ETrue ) );
		if ( KErrNoMemory == err )
		    {
		    User::Leave( KErrNoMemory );
		    }
		if ( KErrNone != err || !content )
			{
			User::Panic(KMtlPanic, -1);
			}
		}
	return content;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
void CMmTemplateLibrary::GetTemplateFileContentL( HBufC8*& aContent, 
		const TDesC8& aTemplate, TMmWidgetType aWidgetType, 
		TMmTemplateType& aMmTemplateType, TBool aLoadCustomTemplate )
	{
    DEBUG(("_Mm_:CMmTemplateLibrary::GetTemplateContentL IN")); 
    TFileName filename;
//  create filename  egz: akn_logical_template_3.xml
	filename.Copy(aTemplate);
	filename.Insert(0, KUnderline );
	filename.Insert(0, KAkn );
	filename.Append( KXmlExt );
	
//  append path egz: z:\\resource\\grid\\lct\\akn_logical_template_3.xml
	
	if ( !aLoadCustomTemplate )
		{
		filename.Insert(0, KBslash );
		filename.Insert(0, KLct );
		}
	else 
		{
		filename.Insert(0, KBslash );
		filename.Insert(0, KCustom );
		}
	
	switch (aWidgetType)
		{
		case EGrid:
			filename.Insert(0, KBslash );
			filename.Insert(0, KWidgetTypeGrid);
			break;
		case EListbox:
			filename.Insert(0, KBslash );
			filename.Insert(0, KWidgetTypeList );
			break;
		}
	
	filename.Insert(0, KZResource );    
	
	DEBUG(("\t_Mm_:tail: %S", &filename));
    DEBUG(("_Mm_:CMmTemplateLibrary::GetTemplateContentL OUT"));
    
//  load LCT Template or if it does not exist then load custom template
    aContent =  HnUtils::ReadFileLC(filename);
	CleanupStack::Pop( aContent );
	
	if ( aLoadCustomTemplate )
		{
		aMmTemplateType = ETemplateTypeCustom;
		}
	else 
		{
		aMmTemplateType = ETemplateTypeLCT;
		}
	return;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TSize CMmTemplateLibrary::GetLCTSize( const TDesC8& aLCTTemplate, TInt aVariety, 
		TAknWindowLineLayout& aWindowLayout, TBool aLandscapeOrientation )
	{
	TSize size = MmLCTUtils::GetLCTSize( aLCTTemplate, aVariety, 
			GetParentRect( aLandscapeOrientation ), aWindowLayout );
	return size;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::SetupLCTTemplateL(TTemplateChild& aChildTemplate, 
		TXmlEngElement& aChildElement, TSize aItemSize )
	{
    TPtrC8 name = aChildElement.Name();
    aChildTemplate.iData = aChildElement.AttributeValueL(KId8);
    SetupTemplateVisualId( aChildTemplate );
    
    // read attribute name (mm_title, mm_icon)
    RXmlEngNodeList<TXmlEngElement> attributeElements;
    CleanupClosePushL( attributeElements );
    aChildElement.GetChildElements( attributeElements );
    TXmlEngElement attElement;
    while ( attributeElements.HasNext() )
        {
        attElement = attributeElements.Next();   
        TPtrC8 nameAtt = attElement.AttributeValueL(KName8);
        if ( !name.Compare( KTextVisual8 ) &&
                !nameAtt.Compare( KText8 ) )
            {
            MmLCTUtils::SetupTextSubCellTemplate( aItemSize, aChildTemplate );
            }
        else if ( !name.Compare( KImageVisual8 )  &&
                !nameAtt.Compare( KImagePath8 ) )
            {
            MmLCTUtils::SetupGraphicSubCellTemplate(aItemSize, aChildTemplate);
            AdjustIconPosition(aItemSize, aChildTemplate);
            }
        }
    CleanupStack::PopAndDestroy( &attributeElements );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::SetupCustomTemplateL(TTemplateChild& aChildTemplate,
		TXmlEngElement& aChildElement)
	{
    TPtrC8 name = aChildElement.Name();
	TInt positionx; TInt positiony;
    TInt height; TInt width;
    
    HnConvUtils::Str8ToInt(aChildElement.AttributeValueL(KPositionX8), positionx);
    HnConvUtils::Str8ToInt(aChildElement.AttributeValueL(KPositionY8), positiony); 
    HnConvUtils::Str8ToInt(aChildElement.AttributeValueL(KHeight8), height);
    HnConvUtils::Str8ToInt(aChildElement.AttributeValueL(KWidth8), width);
    aChildTemplate.iRectAccordingToParent = TRect( TPoint(positionx,positiony), TSize(width, height) );
    
    aChildTemplate.iData = aChildElement.AttributeValueL(KId8);
    SetupTemplateVisualId( aChildTemplate );
    
    // read attribute name (mm_title, mm_icon)
    RXmlEngNodeList<TXmlEngElement> attributeElements;
    CleanupClosePushL( attributeElements );
    aChildElement.GetChildElements( attributeElements );
    TXmlEngElement attElement;
    while ( attributeElements.HasNext() )
        {
        attElement = attributeElements.Next();   
        TPtrC8 nameAtt = attElement.AttributeValueL(KName8);
        if ( !name.Compare( KTextVisual8 ) &&
                !nameAtt.Compare( KText8 ) )
            {
            aChildTemplate.iIsImage = EFalse;
            }
        else if ( !name.Compare( KImageVisual8 )  &&
                !nameAtt.Compare( KImagePath8 ) )
            {
            aChildTemplate.iIsImage = ETrue;
            aChildTemplate.iFontId = EAknLogicalFontSecondaryFont;
            aChildTemplate.iTextAlign = CGraphicsContext::ELeft ;
            }
        else if ( !name.Compare( KTextVisual8 )  &&
                !nameAtt.Compare( KStyle8 ) )
            {
            TPtrC8 font = attElement.AttributeValueL(KTargetValue8);
            if ( !font.Compare( KQfnPrimarySmall8 ) )
                {
                aChildTemplate.iFontId = EAknLogicalFontPrimarySmallFont;
                }
            else 
                {
                aChildTemplate.iFontId = EAknLogicalFontSecondaryFont;
                }
            }
        else if ( !name.Compare( KTextVisual8 )  &&
                !nameAtt.Compare( KHorizontalAlign8 ) )
            {
            TInt textAlign;
            HnConvUtils::Str8ToInt(attElement.AttributeValueL(KTargetValue8), textAlign);
            aChildTemplate.iTextAlign = static_cast<CGraphicsContext::TTextAlign>(textAlign);
            }
        }
    
    CleanupStack::PopAndDestroy( &attributeElements );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::SetupTemplateVisualId(TTemplateChild& aChildTemplate )
	{
	if( !aChildTemplate.iData.Compare( KMmBackdropIcon8 ))
        {
        aChildTemplate.iImageVisualId = EImageVisualIdEditMode;
        }
	else 
		{
		aChildTemplate.iImageVisualId = EImageVisualIdNormalMode;
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::AppendEditModeTemplateL(
		RArray<TTemplateChild>& aTemplateArray, TSize aSize)
	{
    //setup backdrop icon
    TTemplateChild childTemplate;
    childTemplate.iIsImage = ETrue;
    childTemplate.iFontId = EAknLogicalFontSecondaryFont;
    childTemplate.iTextAlign = CGraphicsContext::ELeft;
    childTemplate.iRectAccordingToParent = TRect( TPoint( 0,0 ), 
    		TPoint( aSize.iWidth, aSize.iHeight ) );
    childTemplate.iData = TBufC8< MmTemplateContants::KTemplateChildTextLength >( KMmBackdropIcon8 );
    SetupTemplateVisualId( childTemplate );
    aTemplateArray.AppendL( childTemplate );
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::SetupMoveIndicatorTemplateChildrenL( 
		const TDesC8& aLookupString, TSize aItemSize )
	{
	RArray< TTemplateChild > childrenDefinition;
    TTemplateChild childTemplate;
    childTemplate.iIsImage = ETrue;
    childTemplate.iImageVisualId = EImageVisualIdNormalMode;  
    childTemplate.iFontId = EAknLogicalFontSecondaryFont;
    childTemplate.iTextAlign = CGraphicsContext::ELeft;
	TPoint startingPosition = TPoint( 
			MmTemplateContants::KMoveIndicatorStartingPos, 
			MmTemplateContants::KMoveIndicatorStartingPos);
    
    //setup move_indicator_arrow_left
    childTemplate.iRectAccordingToParent = TRect(TPoint(0,aItemSize.iHeight/2 + startingPosition.iY/2), 
    		TPoint(startingPosition.iX, 
    				startingPosition.iY*3/2 + aItemSize.iHeight/2));
    childTemplate.iData = TBufC8< MmTemplateContants::KTemplateChildTextLength>( 
    		KMmMoveIndicatorArrowLeft8 );
    childrenDefinition.AppendL( childTemplate );
    
    //setup move_indicator_arrow_right
    childTemplate.iRectAccordingToParent = TRect(
    		TPoint(startingPosition.iX + aItemSize.iWidth , aItemSize.iHeight/2 + startingPosition.iY/2), 
    		TPoint(startingPosition.iX*2 + aItemSize.iWidth, 
    				startingPosition.iY*3/2 + aItemSize.iHeight/2));
    childTemplate.iData = TBufC8< MmTemplateContants::KTemplateChildTextLength>(
    		KMmMoveIndicatorArrowRight8 );
    childrenDefinition.AppendL( childTemplate );
    
    //setup move_indicator_arrow_top
    childTemplate.iRectAccordingToParent = TRect(TPoint( aItemSize.iWidth/2 + startingPosition.iX/2, 0 ), 
    		TPoint(aItemSize.iWidth/2 + startingPosition.iX*3/2, startingPosition.iY));
    childTemplate.iData = TBufC8< MmTemplateContants::KTemplateChildTextLength >(
    		KMmMoveIndicatorArrowTop8 );
    childrenDefinition.AppendL( childTemplate );
    
    //setup move_indicator_arrow_bottom
    childTemplate.iRectAccordingToParent = TRect( TPoint( aItemSize.iWidth/2 + startingPosition.iX/2, 
    		aItemSize.iHeight + startingPosition.iY + 2 ), 
    		TPoint(aItemSize.iWidth/2 + startingPosition.iX*3/2, 
    				aItemSize.iHeight + 2 *startingPosition.iY));
    childTemplate.iData = TBufC8< MmTemplateContants::KTemplateChildTextLength>( 
    		KMmMoveIndicatorArrowBottom8 );
    childrenDefinition.AppendL( childTemplate );

	HBufC8* lookup_string = aLookupString.AllocLC();
	iTemplateChildrenMap.InsertL(lookup_string, childrenDefinition);
	CleanupStack::Pop( lookup_string );
	lookup_string = NULL;
	
    lookup_string = aLookupString.AllocLC();
	TRect rectAccordingToParent = TRect( 
			TPoint( -startingPosition.iX, -startingPosition.iY), 
			TPoint(2*startingPosition.iX + aItemSize.iWidth , 
					2*startingPosition.iY + aItemSize.iHeight) );

	iMoveIndicatorRectsMap.InsertL( lookup_string, rectAccordingToParent );
	CleanupStack::Pop( lookup_string );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::SetScrollbarVisibilityL( TBool aIsScrollbarVisible )
	{
	if ( iIsScrollbarVisible != aIsScrollbarVisible )
		{
		iIsScrollbarVisible = aIsScrollbarVisible;
		CleanAndClearCache();
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CMmTemplateLibrary::GetScrollbarVisibility()
    {
    return iIsScrollbarVisible;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::SetScrollbarWidthL( TInt aScrollbarWidth )
	{
	if ( iIsScrollbarVisible )
		{
		if ( iScrollbarWidth != aScrollbarWidth )
			{
			iScrollbarWidth = aScrollbarWidth;
			CleanAndClearCache();
			}
		}
	else
		{
		if ( iScrollbarWidth != 0 )
			{
			iScrollbarWidth = 0;
			CleanAndClearCache();
			}
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CMmTemplateLibrary::ScrollbarWidth()
    {
    return iScrollbarWidth;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::AdjustItemWidth(TSize& aItemSize, TSize aLayoutSize,
		TBool aLandscapeOrientation)
	{
	if ( iIsScrollbarVisible )
		{
		aItemSize.iWidth = ( GetParentRect(aLandscapeOrientation).Width() - iScrollbarWidth ) / aLayoutSize.iWidth;
		}
	else 
		{
		aItemSize.iWidth = GetParentRect(aLandscapeOrientation).Width() / aLayoutSize.iWidth;
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::UpdateParentRect( TRect aParentRect, TBool aLandscapeOrientation )
	{
    if( GetParentRect( aLandscapeOrientation ) != aParentRect )
	    {
    	SetParentRect( aLandscapeOrientation, aParentRect );
	    }
	}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TRect CMmTemplateLibrary::GetParentRect( TBool aLandscapeOrientation )
	{
	if ( aLandscapeOrientation )
		return iParentRectLandscape;
	else
		return iParentRectPortrait;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::SetParentRect( TBool aLandscapeOrientation, TRect aParentRect)
	{
	if ( aLandscapeOrientation )
		iParentRectLandscape = aParentRect;
	else
		iParentRectPortrait = aParentRect;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::AdjustItemSize(TSize& aItemSize, TSize aLayoutSize,
		TBool aLandscapeOrientation)
	{
	switch ( iWidgetType )
		{
		case EGrid:
			aItemSize.iHeight = GetParentRect( aLandscapeOrientation ).Height() / aLayoutSize.iHeight;
			break;
		case EListbox:
			break;
		}
	AdjustItemWidth( aItemSize, aLayoutSize, aLandscapeOrientation );
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TAknUiZoom CMmTemplateLibrary::Zoom( )
	{
	return iZoom;
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::SetZoom( TAknUiZoom aZoom )
	{
	if ( (EAknUiZoomLarge == aZoom)
			|| (EAknUiZoomSmall == aZoom) )
		{
		iZoom = aZoom;
		}
	else
		{
		iZoom = EAknUiZoomNormal;
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::CacheLayoutSizeL(TSize aLayoutSize,
		const TDesC8& aTemplate, TBool aLandscapeOrientation)
	{
	HBufC8* lookup_layout_text = LookupLayoutText( iWidgetType, iZoom, 
        aTemplate, aLandscapeOrientation );

	if ( iTemplateSizesMap.Find( lookup_layout_text ) )
		{
		delete lookup_layout_text;
		}
	else
		{
		iTemplateSizesMap.InsertL( lookup_layout_text, aLayoutSize );
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMmTemplateLibrary::AdjustIconPosition(TSize aParentSize,
        TTemplateChild& aTemplateChild)
    {
    switch (aTemplateChild.iHAlign)
        {
        case EManualAlignLeft:
            break;
        case EManualAlignCenter:
            {
            TInt rectWidth =
                    aTemplateChild.iRectAccordingToParent.Size().iWidth;
            TInt lX = (aParentSize.iWidth - rectWidth) / 2;
            aTemplateChild.iRectAccordingToParent.Move(lX
                    - aTemplateChild.iRectAccordingToParent.iTl.iX, 0);
            break;
            }
        case EManualAlignRight:
            break;
        default:
            break;
        }
    }

// End of file
