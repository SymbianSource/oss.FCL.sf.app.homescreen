/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
class CCarouselPluginEngine;

/**
 *  @ingroup group_mcsplugin
 *
 *  MCS Plugin Watcher Observer
 *
 *  @since S60 9.1
 */
class MCarouselPluginWatcherObserver
    {
    public:
    /**
     * HandleNotifyL
     */
    virtual void HandleNotifyL() = 0;
    };

/**
 *  @ingroup group_mcsplugin
 *
 *  This class implements MCS menu action watcher for async calls.
 * 
 *  @since S60 9.1
 */
class CCarouselPluginWatcher: public CActive
    {
    public:
        
        /**
         * enum Type
         */
        enum Type
            {
            EOperation,
            ENotify
            };
        
        /**
         * Two-phased constructor.
         * 
         * @param aType
         */
        static CCarouselPluginWatcher* NewL( const Type& aType );
        
        /**
         * Destructor.
         */
        virtual ~CCarouselPluginWatcher();

        /**
         * Watch menu operation or notification.
         * 
         * @param aOperation
         */
        void Watch( CMenuOperation* aOperation = NULL );
        
        /**
         * WatchNotify
         * 
         * @param aObserver
         */
        void WatchNotify( MCarouselPluginWatcherObserver* aObserver );
        
        /**
         * StopAndWatch
         * 
         * @param aOperation
         * @param aWaitScheduler
         */
        void StopAndWatch( CMenuOperation* aOperation, 
                           CActiveSchedulerWait* aWaitScheduler );
        
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
        CCarouselPluginWatcher( const Type& aType );

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
        
        /**
         * Wait scheduler
         * Not owned
         */
        CActiveSchedulerWait* iWaitScheduler;

        /** 
         * Observer reference
         * Not owned
         */
        MCarouselPluginWatcherObserver* iObserver;
        
        /** 
         * iType
         */
        Type iType;
    };

#endif /*__MCSPLUGINWATCHER_H*/
