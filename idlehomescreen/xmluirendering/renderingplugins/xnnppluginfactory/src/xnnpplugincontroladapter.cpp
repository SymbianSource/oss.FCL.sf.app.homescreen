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
* Description:  The plugin can use this interface to communicate with the browser.
*
*/



// INCLUDE FILES
#include    "xnnpplugincontroladapter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnNPPluginControlAdapter::PluginConstructedL
// Other items were commented in header.
// -----------------------------------------------------------------------------
//
void CXnNPPluginControlAdapter::PluginConstructedL( CCoeControl* aControl )
    {
    iNPPControlAdapter = aControl;
    }

// -----------------------------------------------------------------------------
// CXnNPPluginControlAdapter::PluginFinishedL
// Other items were commented in header.
// -----------------------------------------------------------------------------
//
void CXnNPPluginControlAdapter::PluginFinishedL()
    {
    }
    
// -----------------------------------------------------------------------------
// CXnNPPluginControlAdapter::GetParentControl
// Other items were commented in header.
// -----------------------------------------------------------------------------
//
CCoeControl* CXnNPPluginControlAdapter::GetParentControl()
    {
    return iParentControlAdapter;
    }

// -----------------------------------------------------------------------------
// CXnNPPluginControlAdapter::GetParentControlObserver
// Other items were commented in header.
// -----------------------------------------------------------------------------
//      
MCoeControlObserver* CXnNPPluginControlAdapter::GetParentControlObserver()
    {
    return iParentControlAdapter->Observer();
    }

// -----------------------------------------------------------------------------
// CXnNPPluginControlAdapter::SetPluginNotifier
// Other items were commented in header.
// -----------------------------------------------------------------------------
//
void CXnNPPluginControlAdapter::SetPluginNotifier( MPluginNotifier* aNotifier )
    {
    iPluginNotifier = aNotifier;    
    }
// -----------------------------------------------------------------------------
// CXnNPPluginControlAdapter::SetContentInteractive
// Other items were commented in header.
// -----------------------------------------------------------------------------
//
void CXnNPPluginControlAdapter::SetContentInteractive(TBool /*aInteractive*/ )
    {
    }

    
//  End of File  
