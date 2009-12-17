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
* Description:  Xuikon Bitmap Plugin.
*
*/


#include "xnnodepluginif.h"
#include "xndomproperty.h"
#include "xnproperty.h"
#include "xncontroladapter.h"

#include "xnbitmapadapter.h"

_LIT(KSkin, "skin(");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnBitmapAdapter* CXnBitmapAdapter::NewL(CXnNodePluginIf& aNode)
    {
	CXnBitmapAdapter* self = new( ELeave ) CXnBitmapAdapter( aNode );

    CleanupStack::PushL( self );
    self->ConstructL( aNode );
    CleanupStack::Pop();

    return self;	
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::ConstructL(CXnNodePluginIf& aNode)
    {
    CXnControlAdapter::ConstructL( aNode );
    
    iAreBitmapsLoaded = EFalse;
    
    iFallbackPathChange = ETrue;
    }
    
// -----------------------------------------------------------------------------
// CXnBitmapAdapter::CXnBitmapAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnBitmapAdapter::CXnBitmapAdapter(CXnNodePluginIf& aNode) : iNode( aNode )    
    {
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::~CXnBitmapAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnBitmapAdapter::~CXnBitmapAdapter()
    {
    delete iPath;
    delete iFallbackPath;
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::SetContentBitmaps
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::SetContentBitmaps(CFbsBitmap* aBitmap, CFbsBitmap* aMask)
    {    
    CXnControlAdapter::SetContentBitmaps( aBitmap, aMask );
    
    iAreBitmapsSet = ETrue;
    
    TRAP_IGNORE( iNode.SetDirtyL() );
    }   

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::ContentBitmaps
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::ContentBitmaps(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask)
    {    
    CXnControlAdapter::ContentBitmaps( aBitmap, aMask );
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::Draw
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::Draw(const TRect& aRect) const
    {        
    CXnControlAdapter::Draw( aRect );    
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::SizeChanged
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::SizeChanged()
    {
    CXnControlAdapter::SizeChanged();
    
    if ( !iAreBitmapsSet )
        {
        TSize size( Rect().Size() );
        
        if( size != iBitmapSize )
            {            
            iBitmapSize = size;
            
            iAreBitmapsLoaded = EFalse;
            
            TRAP_IGNORE( InitializeBitmapsL() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::HandlePropertyChangeL
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::DoHandlePropertyChangeL( CXnProperty* aProperty )
    {
    TBool reConstruct( ETrue );

    if( aProperty )
        {
        const TDesC8& name( aProperty->Property()->Name( ) );
        
        //If display changed no need to reconstruct
        if( name == XnPropertyNames::style::common::KDisplay( ) )
            {
            reConstruct = EFalse;
            }        
        // Visibility changed, no need to do anything.
        else if( name == XnPropertyNames::style::common::KVisibility( ) )
            {
            return;
            }
        else if( name == XnPropertyNames::appearance::common::KFallBackImage )
            {
            HBufC* fallbackPath( aProperty->StringValueL( ) );
            
            if( fallbackPath )
                {
                CleanupStack::PushL( fallbackPath );
                
                if( iFallbackPath )
                    {
                    if( ( *fallbackPath ).Compare( *iFallbackPath ) )
                        {
                        delete iFallbackPath;
                        iFallbackPath = NULL;
                        iFallbackPath = fallbackPath->AllocL( );
                        iFallbackPathChange = ETrue;
                        }
                    }
                else
                    {
                    iFallbackPath = fallbackPath->AllocL( );
                    iFallbackPathChange = ETrue;
                    }
                    
                CleanupStack::PopAndDestroy( fallbackPath );
                }
            }
        }

    //If data API has been used don't load new bitmaps.
    if( !iAreBitmapsSet && reConstruct )
        {
        InitializeBitmapsL();
        }
    }

// -----------------------------------------------------------------------------
// CXnTextAdapter::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::HandleScreenDeviceChangedL( )
    {
    CXnControlAdapter::HandleScreenDeviceChangedL();        
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::HandlePointerEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CXnBitmapAdapter::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {        
    CXnControlAdapter::HandlePointerEventL( aPointerEvent );    
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::SkinChanged
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::SkinChanged()
    {
    CXnControlAdapter::SkinChanged();
    
    if( iPath )
        {
        TInt pos( iPath->FindF( KSkin ) );
        
        if( pos != KErrNotFound )
            {
            iBitmapSize = Rect().Size();
            
            iAreBitmapsLoaded = EFalse;
            
            TRAP_IGNORE( InitializeBitmapsL() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CXnBitmapAdapter::InitializeBitmapsL
// -----------------------------------------------------------------------------
//
void CXnBitmapAdapter::InitializeBitmapsL( )
    {
    TBool pathChanged( EFalse );
    
    CXnProperty* pathProperty( iNode.PathL( ) );
    CXnProperty* maskPathProperty( iNode.MaskPathL( ) );

    if ( !pathProperty )
        {
        return;
        }
        
    HBufC* path( pathProperty->StringValueL( ) );
    
    if( !path )
        {
        return;
        }
        
    CleanupStack::PushL( path );
    
    HBufC* maskPath( NULL );
    
    if( maskPathProperty )
        {
        maskPath = maskPathProperty->StringValueL( );
        }
        
    CleanupStack::PushL( maskPath );

    if( !iPath )
        {
        iPath = path->AllocL( );
        }

    if( path->Des( ) != iPath->Des( ) )
        {
        pathChanged = ETrue;
        }

    if( !iAreBitmapsLoaded || pathChanged || iFallbackPathChange )
        {
        delete iPath;
        iPath = NULL;
        iPath = path->AllocL( );
      
        CXnControlAdapter::SetContentBitmaps( *path, ( maskPath )
                                                ? maskPath->Des( ) : KNullDesC() );
        
        iAreBitmapsLoaded = ETrue;
        iFallbackPathChange = EFalse;
        
        iNode.SetDirtyL();
        }
            
    CleanupStack::PopAndDestroy( 2, path );
    }
    
// End of File

