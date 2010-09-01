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
* Description:  action presentation model
*
*/


#include <liwcommon.h>
#include "hnsuitemodelcontainer.h"
#include "hnactionmodel.h"
#include "hnmdbasekey.h"
#include "hnmdmodel.h"
#include "hneventhandler.h"
#include "hnhelper.inl"

using namespace LIW;

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnActionModel* CHnActionModel::NewL()
    {
    CHnActionModel* self = new( ELeave ) CHnActionModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnActionModel::~CHnActionModel()
    {
    delete iConstructor;
    delete iCommand;
    
    iService.Close();
    iInterface.Close();
    iCommandName.Close();
    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CHnActionModel::ConstructL()
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
EXPORT_C CLiwGenericParamList*  CHnActionModel::ConstructorLC()
    {
    CLiwGenericParamList* constructor = CLiwGenericParamList::NewL();
    CleanupStack::PushL( constructor );
    if ( iConstructor )
        {
        iConstructor->ToGenericParamListL( *constructor );
        }
    return constructor;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHnActionModel::Service() const
    {
    return iService;
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CLiwGenericParamList* CHnActionModel::CommandLC() const
    {
    CLiwGenericParamList* command = CLiwGenericParamList::NewL();
    CleanupStack::PushL( command );
    if ( iCommand )
        {
        iCommand->ToGenericParamListL( *command );        
        }
    return command;
    }


        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHnActionModel::Interface() const
    {
    return iInterface;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnActionModel::SetConstructorL( CHnMdBaseKey* aData )
    {
    delete iConstructor;
    iConstructor = aData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnActionModel::SetServiceL( const TDesC8& aSrevice )
    {
    iService.Close();
    iService.CreateL( aSrevice );
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnActionModel::SetCommand( CHnMdBaseKey* aCommand )
    {
    delete iCommand;
    iCommand = aCommand;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnActionModel::SetInterfaceL( const TDesC8& aInterface )
    {
    iInterface.Close();
    iInterface.CreateL( aInterface );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnActionModel::SetCommandNameL( const TDesC8& aCommandName )
    {
    iCommandName.Close();
    iCommandName.CreateL( aCommandName );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CHnActionModel::SetServiceModeL( const TServiceMode aMode )
    {
    iMode = aMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHnActionModel::CommandName() const
{
    return iCommandName;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TServiceMode CHnActionModel::ServiceMode() const
{
    return iMode;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnActionModel::CHnActionModel()
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnActionModel* CHnActionModel::ConcretizeL( const CHnActionModel & aActionModel,
        CLiwGenericParamList* aGenericParamList )
    {
    CHnActionModel* newInstance = CHnActionModel::NewL();
    CleanupStack::PushL( newInstance );
    
    newInstance->iService.CreateL( aActionModel.Service() );
    newInstance->iInterface.CreateL( aActionModel.Interface() );
    newInstance->iCommandName.CreateL( aActionModel.CommandName() );
    newInstance->iMode = aActionModel.iMode;
    
    if ( aActionModel.iCommand )
        {
        if ( aGenericParamList )
            {
            newInstance->iCommand = aActionModel.iCommand->EvaluateLC( *aGenericParamList );
            }
        else
            {
            newInstance->iCommand = aActionModel.iCommand->CopyLC();
            }
        CleanupStack::Pop( newInstance->iCommand );
        }
    
    if (  aActionModel.iConstructor )
        {
        if ( aGenericParamList )
            {
            newInstance->iConstructor = aActionModel.iConstructor->EvaluateLC( *aGenericParamList );
            }
        else
            {
            newInstance->iConstructor = aActionModel.iConstructor->CopyLC();
            }
        CleanupStack::Pop( newInstance->iConstructor );
        }

    CleanupStack::Pop( newInstance );
    return newInstance;
    }

// ---------------------------------------------------------------------------
// 
// ------------------ ---------------------------------------------------------
//
TInt CHnActionModel::ExecuteL( CHnEventHandler & aEventHandler, 
        CLiwGenericParamList *aExecutionParams )
{
    CHnActionModel* actionModel = ConcretizeL( *this, aExecutionParams );
    CleanupStack::PushL( actionModel );
    TRAPD( err, aEventHandler.ExecuteActionL( actionModel ) );
    // forward KErrNoMemory error to most outer TRAP.
    if ( KErrNoMemory == err || KErrDiskFull == err )
        {
        User::Leave( err );
        }
    CleanupStack::PopAndDestroy( actionModel );
    return err;
}



