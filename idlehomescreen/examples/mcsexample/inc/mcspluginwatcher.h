/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MCSPLUGINWATCHER_H
#define __MCSPLUGINWATCHER_H

#include <e32base.h>

class CMenuOperation;
class CMCSPluginEngine;
class CMCSExampleAppView;


/**
 *
 *  This class implements MCS menu action watcher for async calls.
 * 
 */
class CMCSExampleWatcher: public CActive
    {
    public:
        
        
        /**
         * Two-phased constructor.
         * 
         * @param aType
         */
        static CMCSExampleWatcher* NewL( );
        
        /**
         * Destructor.
         */
        virtual ~CMCSExampleWatcher();

        /**
         * Watch menu operation or notification.
         * 
         * @param aOperation
         */
        void Watch( CMenuOperation* aOperation, CMCSExampleAppView* aObserver );

        
        /**
         * GetStatus 
         */
        TInt GetStatus();

    private:
    
        /** 
         * Default constructor.
         * 
         * @param aType
         */
        CMCSExampleWatcher();

        /** 
         * ConstructL
         */
        void ConstructL();
        
        /**
        * Inherited from CActive class 
        */ 
        void RunL();
        
        /**
        * Inherited from CActive class 
        */ 
        void DoCancel();
    
    private:
        
        /** 
         * Menu operation
         * Owned
         */
        CMenuOperation* iOperation; 
        CMCSExampleAppView* iObserver;
    };

#endif /*__MCSPLUGINWATCHER_H*/
