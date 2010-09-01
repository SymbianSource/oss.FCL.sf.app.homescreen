/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Define the inteface for the container of externale standby screen plug-ins
*
*/


// -----------------------------------------------------------------------------
// CExtrStandbyContainerIF::NewL
// -----------------------------------------------------------------------------
//
inline CExtrStandbyContainerIF* CExtrStandbyContainerIF::NewL(
    TUid aImplementationUid, 
    CCoeControl *aContainer )
    {
    return REINTERPRET_CAST( CExtrStandbyContainerIF*, 
        REComSession::CreateImplementationL( 
            aImplementationUid, 
            _FOFF( 
                CExtrStandbyContainerIF, 
                iDtor_ID_Key ), 
            aContainer ) ) ;
    }
    
// -----------------------------------------------------------------------------
// CExtrStandbyContainerIF::~CExtrStandbyContainerIF
// -----------------------------------------------------------------------------
//
inline CExtrStandbyContainerIF::~CExtrStandbyContainerIF()
    {
    // Destroy any instance variables and then
    // inform the framework that this specific
    // instance of the interface has been destroyed.
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
