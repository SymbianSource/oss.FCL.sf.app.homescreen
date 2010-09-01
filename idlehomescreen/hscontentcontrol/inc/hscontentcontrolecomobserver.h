/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Observes for change in ecom registry of content controller
*  interface.  
*
*/

#ifndef HSCONTENTCONTROLECOMOBSERVER_H
#define HSCONTENTCONTROLECOMOBSERVER_H

// Class declaration
/**
* MHsContentControlEComObserver
* 
* @brief The observer of ecom registry changes. The derived class needs to implement
*  the functions below and will be notified by CHsContentControlEComListener.
*  
* @see CHsContentControlEComListener
*/
class MHsContentControlEComObserver
	{
public:
    /**
     * Notification of Ecom registry change.
     */
    virtual void HandleEComChangeEvent() = 0;
	};

#endif // HSCONTENTCONTROLECOMOBSERVER_H

// End of file
