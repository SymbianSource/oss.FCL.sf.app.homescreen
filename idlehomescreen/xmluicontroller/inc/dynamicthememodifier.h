/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains the header file of the CDynamicThemeModifier class.
*
*/


#ifndef __DYNAMICTHEMEMODIFIER_H__
#define __DYNAMICTHEMEMODIFIER_H__

// INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
//#include <xnclient.h>
//#include <xncontentaccessclient.h>
//#include <xnrequestclient.h>
//#include <xnresult.h>


//class MAiPSPropertyObserver;

namespace AiXmlUiController
{

// CLASS DECLARATION

/**
*  This class updates the theme dynamically as it is being somehow modified.
*
*  It connects to XUIKON Content Access API and modifies e.g. NT visibility
*  in theme level as user changes appropriate setting in GS. Appropriate CenRep
*  keys are being observed and theme modifications are launched based on changes
*  in those keys.
*
*  @since S60 3.2
*/
class CDynamicThemeModifier : public CBase,
                                	public MCenRepNotifyHandlerCallback,
									public MXnThemeManagementServiceObserver,
                                	public MXnContentAccessServiceObserver,
                                	public MXnClientRequestServiceObserver

    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDynamicThemeModifier* NewL();

        /**
        * Destructor.
        */
        ~CDynamicThemeModifier();


    private:

        /**
        * C++ default constructor.
        */
        CDynamicThemeModifier();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:	// New methods

        /**
         * Load AI2 themes.
         */
        void LoadThemesL();

        /**
         * Load settings from active theme.
         */
        void LoadSettingsFromThemeL();

        /**
         * Handle NT visibility in theme.
         * @param ETrue for showing NewsTicker, EFalse for hiding it.
         */
        void EnableNewsTickerL( TBool aEnabled );


    private:	// From base classes

        void HandleXnClientMessage( TXnServiceCompletedMessage aMessage );
        void HandleXnRequestClientMessage( TXnServiceCompletedMessage /*aMessage*/ ) {};
        void XnSettingsUpdated() {};
        void HandleNotifyInt( TUint32 aId, TInt aNewValue );



    private:    // Data

        CArrayPtrFlat<CXnODT>* iThemeArray;
        CXnClient* iXnClient;
        CXnContentAccessClient* iXnContentAccessClient;
        CXnRequestClient* iXnRequestClient;
        CXnResult* iXnResult;
        CXnDomDocument* iDomDocument;
        CXnODT* iOdt;
        CRepository* iNTCenRep;
        CRepository* iAiCenRep;
        CCenRepNotifyHandler* iNTCenRepObserver;
        TInt iNumberOfHeaders;
        TInt iEnabledInCenRep;

    };
    
}   // namespace 

#endif      // __DYNAMICTHEMEMODIFIER_H__

// End of File
