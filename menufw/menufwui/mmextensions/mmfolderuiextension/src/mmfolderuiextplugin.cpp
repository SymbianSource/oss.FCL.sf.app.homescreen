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
* Description:  Folder UI extension plugin
 *
*/



#include <liwcommon.h>

#include "mmpluginsharedresources.h"
#include "menudebug.h"
#include "mmfolderuiextplugin.h"
#include "mmfolderuiextpluginconstants.h"
#include "mmfoldercommand.h"
#include "hnglobals.h"
#include "mmguiconstants.h"
#include <apgwgnam.h>
#include <w32std.h>
#include <apgtask.h>
#include <AknQueryDialog.h>

// ============================ MEMBER FUNCTIONS =============================
// ---------------------------------------------------------------------------
// Symbian factory function.
// ---------------------------------------------------------------------------
//
CMmFolderUiExtPlugin* CMmFolderUiExtPlugin::NewL(
        MMmActionRequest& aActionRequest )
	{
	CMmFolderUiExtPlugin* self =
	    new( ELeave ) CMmFolderUiExtPlugin( aActionRequest );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// Symbian second phase constructor.
// ---------------------------------------------------------------------------
//
void CMmFolderUiExtPlugin::ConstructL()
	{
	iSharedResources = NULL;
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMmFolderUiExtPlugin::~CMmFolderUiExtPlugin()
	{
    delete iCommand;
    iCommand = NULL;
    delete iSharedResources;
    iSharedResources = NULL;
	}


// ---------------------------------------------------------------------------
// Default C++ constructor.
// ---------------------------------------------------------------------------
//
CMmFolderUiExtPlugin::CMmFolderUiExtPlugin( MMmActionRequest& aActionRequest ):
        iActionRequest( aActionRequest )
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmFolderUiExtPlugin::ExecuteActionL( const TDesC& aCommand,
    					 CLiwGenericParamList* aEventParamList )
	{
	TInt err = KErrNone;
	DEBUG16( ( "\t_MM_: CMmFolderUiExtPlugin::ExecuteActionL(): %S", 
			&aCommand ) );
	
	if( !aCommand.Compare( KCommandDeleteDialog ) && iCommand )
        {
        iCommand->RemoveDialogL();
        }
	else if( IsMatrixInForegroundL() )
	    {
	    // Resources lazy initialization
	    if (!iSharedResources)
	    	{
	    	iSharedResources = CMmPluginSharedResources::NewL();
	    	}
	    
	    delete iCommand;
	    iCommand = NULL;
	    iCommand = CMmFolderCommand::FactoryL( aCommand );
	    if( iCommand )
	    	{
	    	iCommand->SetRequestInterface( &iActionRequest );
	    	iCommand->SetSharedResources( iSharedResources );
	    	iCommand->ExecuteActionL( aEventParamList );
	    	}
	    else
	    	{
	    	DEBUG16( ( "\t_MM_: Unsupported command: %S", &aCommand ) );
	    	err = KErrNotSupported;
	    	}
	    }
    
	return err;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CMmFolderUiExtPlugin::IsMatrixInForegroundL()
    {
    TBool res( EFalse );
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL( wsSession );
    TApaTaskList taskList = TApaTaskList( wsSession );
    TApaTask foregroundTask = taskList.FindByPos( 0 );
    CApaWindowGroupName* wgName =
        CApaWindowGroupName::NewLC( wsSession, foregroundTask.WgId() );
    if( wgName->AppUid() == KUidMatrixMenuApp )
        {
        res = ETrue;
        }
    CleanupStack::PopAndDestroy( wgName );
    CleanupStack::PopAndDestroy( &wsSession );
    return res;
    }


// end of file

