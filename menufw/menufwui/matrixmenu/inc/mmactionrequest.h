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



#ifndef C_HNENGINE_REQUEST_H
#define C_HNENGINE_REQUEST_H

class CLiwGenericParamList;
/**
 * @ingroup group_hnengine
 *  Hierarchy Navigator engine request. Implemented by the hierarchy navigator engine to handle 
 *  incoming requests defined using the liw generic parameters list ( CLiwGenericParamList ).
 *
 * @lib hnengine
 * @since S60 v3.2
 */
class MMmActionRequest
    {
public:
    
    /**
     * Enables to issue a request to Hn engine.
     * 
     * @since S60 v5.0
     * @param aParam Input Parameters.
     * @param aOutput Target list for output.  
     */
    virtual void HandleRequestL( const CLiwGenericParamList& aParam, 
                                CLiwGenericParamList* aOutput = NULL ) = 0;
    
    };

#endif //C_HNENGINE_REQUEST_H
