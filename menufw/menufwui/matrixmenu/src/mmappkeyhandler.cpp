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
* Description:  Application UI class
*  Version     : %version: 2 % << Don't touch! Updated by Synergy at check-out.
*
*/

#include "mmappkeyhandler.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmAppkeyHandler::CMmAppkeyHandler() :
	CActive(EPriorityIdle) // Standard priority
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmAppkeyHandler* CMmAppkeyHandler::NewLC()
	{
	CMmAppkeyHandler* self = new (ELeave) CMmAppkeyHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmAppkeyHandler* CMmAppkeyHandler::NewL()
	{
	CMmAppkeyHandler* self = CMmAppkeyHandler::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmAppkeyHandler::ConstructL()
	{
	CActiveScheduler::Add(this); // Add to scheduler
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMmAppkeyHandler::~CMmAppkeyHandler()
	{
	Cancel(); // Cancel any request, if outstanding
//	 Delete instance variables if any
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmAppkeyHandler::DoCancel()
	{
//	no implementation.
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmAppkeyHandler::StartL()
	{
	Cancel(); //just in case
	iStatus = KRequestPending;
	SetActive(); // Tell scheduler a request is active
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMmAppkeyHandler::RunL()
	{
//	do nothing
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMmAppkeyHandler::RunError(TInt aError)
	{
	return aError;
	}
