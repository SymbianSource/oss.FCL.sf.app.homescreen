/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for wrapper for a Netscape Plugin API.
*
*/


// INCLUDE FILES
#include "xnnppluginadapter.h"
#include "xnnpplugincontroladapter.h"
#include "xnnppluginhandler.h"
#include "xnproperty.h"
#include "xndomproperty.h"
#include "xndompropertyvalue.h"
#include "xndomlist.h"
#include "xnnodepluginif.h"
#include "xnuienginepluginif.h"
#include "xnresource.h"
#include "c_xnutils.h"

// MACROS
_LIT8( KContentFile, "contentfile" );
_LIT8( KMediaType, "mediatype" );

// LOCAL FUNCTION PROTOTYPES
const TDesC8& GetStringPropertyL( CXnNodePluginIf& aNode, const TDesC8& aProperty );
CXnResource* FindResource(TXnUiEnginePluginIf& aEngine, const TDesC& aResourceFileName );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// GetStringProperty Retrieves the string property for the node.
// Returns: CXnNodePluginIf& aNode A node.
//          TDesC8& aProperty A name of the property.
//          const TDesC8 The value of property.
// -----------------------------------------------------------------------------
// 
const TDesC8& GetStringPropertyL(
    CXnNodePluginIf& aNode,
    const TDesC8& aProperty )
    {
    CXnProperty* prop = aNode.GetPropertyL( aProperty );
    if( prop )
        {
        CXnDomPropertyValue* value = 
        static_cast<CXnDomPropertyValue*>(prop->Property()->PropertyValueList().Item(0));
        if( CXnDomPropertyValue::EIdent == value->PrimitiveValueType() ||
            CXnDomPropertyValue::EString == value->PrimitiveValueType() )
            {
            return prop->StringValue();
            }
        }
    return KNullDesC8;
    }

// -----------------------------------------------------------------------------
// FindResource Locates a resource from the resource list.
// Returns: TXnUiEnginePluginIf& Pointer to UIEngine.
//          const TDesC& The name of the resource file.
//          CCnResource The actual resource.
// -----------------------------------------------------------------------------
//
CXnResource* FindResource(
    TXnUiEnginePluginIf& aEngine,
    const TDesC& aResourceFileName )    
    {
    if (aResourceFileName == KNullDesC)
        {
        return NULL;
        }
    CArrayPtrSeg<CXnResource>& resources = aEngine.Resources();
    for (TInt i = resources.Count() - 1; i >= 0; --i)
        {
        CXnResource* res = resources.At(i);
        if (res->ResourceId().Compare(aResourceFileName) == 0 )
            {
            return res;
            }
        }
    return NULL;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNPPluginAdapter::NewL
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnNPPluginAdapter* CXnNPPluginAdapter::NewL(
    CXnControlAdapter* aParent,
    CXnNodePluginIf& aNode )
    {
    CXnNPPluginAdapter* self = new( ELeave ) CXnNPPluginAdapter;

    CleanupStack::PushL( self );
    self->ConstructL( aParent, aNode );
    CleanupStack::Pop();

    return self;    
    }

// -----------------------------------------------------------------------------
// CXnNPPluginAdapter::CXnNPPluginAdapter
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnNPPluginAdapter::CXnNPPluginAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CXnNPPluginAdapter::~CXnNPPluginAdapter
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnNPPluginAdapter::~CXnNPPluginAdapter()
    {
    //Clean up the content file from private folder.
    iFs.Close(); 
    delete iContentFile;    
    
    if( iPluginFuncs )
        {        
        iPluginFuncs->destroy(iInstance,NULL);
        }
    delete iPluginFuncs;
    delete iPluginController;
    delete iPluginHandler;
    delete iInstance;
    }
    
// -----------------------------------------------------------------------------
// CXnNPPluginAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnNPPluginAdapter::ConstructL( 
    CXnControlAdapter* aParent,
    CXnNodePluginIf& aNode )
    {    
    iNode = &aNode;
    
    CXnControlAdapter::ConstructL( aNode );
    
    iPluginHandler = CXnNPPluginHandler::NewL();

    iPluginController = new (ELeave)CXnNPPluginControlAdapter;
    ConstructPluginL( aParent );   
    }
    

// -----------------------------------------------------------------------------
// CXnNPPluginAdapter::ConstructPluginL
// Function returns if something goes wrong in plugin loading.
// -----------------------------------------------------------------------------
//    
TInt CXnNPPluginAdapter::ConstructPluginL( CXnControlAdapter* aParent )
    {
    const TDesC8& mimeType = GetStringPropertyL( *iNode, KMediaType );
    const TDesC8& contentFile = GetStringPropertyL( *iNode, KContentFile );
    
    iContentFile = HBufC::NewL( contentFile.Length() );
    TPtr cFilePtr = iContentFile->Des();
    cFilePtr.Copy( contentFile );
    
    //Connect to file server
    User::LeaveIfError( iFs.Connect() );
    iFs.CreatePrivatePath( EDriveC );    
    
    // Return if plugin could not be loaded.
    if( KErrNone != iPluginHandler->LoadSpecificPluginL( mimeType, &iPluginFuncs ))
        {
        return KErrGeneral;
        }    

    // Call parent
    iPluginController->iParentControlAdapter = aParent;
    
    iInstance = ( NPP ) User::AllocL( sizeof( NPP_t ));
    iInstance->ndata = ( void* ) iPluginController;
    iInstance->pdata = NULL;
    
    CDesCArray* argn = new( ELeave ) CDesCArrayFlat(1);
    CDesCArray* argv = new( ELeave ) CDesCArrayFlat(1);

    argn->AppendL( KNullDesC );
    argv->AppendL( KNullDesC );

    if (( iPluginFuncs ) && ( iPluginFuncs->newp ))
        {
        TDesC8* string( NULL );
        iPluginFuncs->newp(*string, iInstance, NP_EMBED, CONST_CAST( CDesCArray*, argn ),
                           CONST_CAST( CDesCArray*, argv ), NULL);
        }
    else
        {
        delete argn;
        delete argv;
        return KErrGeneral;
        }
    
    TRect rect = iNode->Rect();
    // Coordinates for the plugin always start from (0,0) and are relative 
    // to the parent CCoeControl
    NPWindow window;
    window.x = 0;
    window.y = 0;
    window.width = rect.Width();
    window.height = rect.Height();
    window.type = NPWindowTypeDrawable;
    window.window = ( void* ) iPluginController;
    window.clipRect.top = window.x;
    window.clipRect.bottom = window.x + window.height;
    window.clipRect.left = window.y;
    window.clipRect.right = window.y + window.width;
    // Finally set the window.       
    iPluginFuncs->setwindow(iInstance,&window);
    
    NPStream* npstream = ( NPStream* )User::AllocL( sizeof( NPStream ));
    npstream->url = NULL;
    npstream->notifyData = NULL;
    
    TPtr resFileName = iContentFile->Des();
    /*TInt bitmapIndex;
    CXnResource* resource = CXnUtils::FindResource( iNode->UiEngineL()->Resources(),
                                                   resFileName, bitmapIndex );
    if( !resource )
        {
        User::Leave( KErrNotFound );
        }
    TParsePtrC pathParser( resource->FileName() );
    TPtrC driveAndPath = pathParser.DriveAndPath();*/
    
    npstream->url = resFileName.AllocL();
    
    iPluginFuncs->asfile( iInstance, npstream, resFileName );
    
    // Tell the plugin to destroy its stream
    iPluginFuncs->destroystream( iInstance, npstream, 0 );
    // Delete the NPStream object    
    delete npstream->url;
    delete npstream;
    delete argn;
    delete argv;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CXnNPPluginAdapter::SizeChanged
// (Other items were commented in header.)
// -----------------------------------------------------------------------------
//      
void CXnNPPluginAdapter::SizeChanged()
    {
    if( iPluginFuncs )
        {        
        TRect rect = iNode->Rect();
        // Coordinates for the plugin always start from (0,0) and are relative 
        // to the parent CCoeControl
        NPWindow window;
        window.x = 0;
        window.y = 0;
        window.width = rect.Width();
        window.height = rect.Height();
        window.type = NPWindowTypeDrawable;
        window.window = ( void* ) iPluginController;
        window.clipRect.top = window.x;
        window.clipRect.bottom = window.x + window.height;
        window.clipRect.left = window.y;
        window.clipRect.right = window.y + window.width;
        // Finally set the window.
        iPluginFuncs->setwindow(iInstance,&window);
        // Set the correct position to the parent CCoeControl.
        iPluginController->iNPPControlAdapter->SetPosition( TPoint( rect.iTl.iX, rect.iTl.iY ));
        }
    }
// -----------------------------------------------------------------------------
// CXnNPPluginAdapter::DoEnterPowerSaveModeL
// Enter power save mode
// -----------------------------------------------------------------------------
void CXnNPPluginAdapter::DoEnterPowerSaveModeL(TModeEvent /*aEvent*/)
    {
    if ( iPluginController->iPluginNotifier )
        {
         iPluginController->iPluginNotifier->NotifyL
        (MPluginNotifier::EApplicationFocusChanged, NULL);      
        }
    }

// -----------------------------------------------------------------------------
// CXnNPPluginAdapter::DoExitPowerSaveModeL
// Exit power save mode
// -----------------------------------------------------------------------------
void CXnNPPluginAdapter::DoExitPowerSaveModeL(TModeEvent /*aEvent*/)
    {
    if ( iPluginController->iPluginNotifier )
        {
        TBool param(ETrue);
        iPluginController->iPluginNotifier->NotifyL 
            (MPluginNotifier::EApplicationFocusChanged, &param );  
       
        }
    }   
// End of file
