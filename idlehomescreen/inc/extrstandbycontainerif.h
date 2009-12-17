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


#ifndef C_EXTRSTANDBYCONTAINERIF_H
#define C_EXTRSTANDBYCONTAINERIF_H

#include <e32std.h>
#include <ecom/ecom.h>
#include <coecntrl.h>

#include "extrstandbycontainerconst.h"

// Standby container interface Uid.
const TUid KCExtrStandbyContainerIFUid = {KExtrStandbyContainerIFUid};

/**
*  It is the interface for the Active Idle.
*
*  @since Series 60 3.2
*/
class CExtrStandbyContainerIF : public CCoeControl
    {
public:

// Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aImplementationUid Implementation ID of the AI plugin
        * @param aInitData  Init data.
        * @return Pointer to the CAIContainer object.
        */
        inline static CExtrStandbyContainerIF* NewL(
            TUid aImplementationUid,
            CCoeControl *aContainer);

        /**
        * Destructor.
        */
        inline virtual ~CExtrStandbyContainerIF();

// New functions

        /**
        * Start Active Idle
        */
        virtual void StartL()=0;

        /**
        * Sets mode for External Homescreen (Full screen or normal)
        * @param TBool, ETrue for full screen EFalse otherwise
        * @since S60 3.2
        */
        virtual void SetFullScreenMode(TBool aFullScreenMode)=0;

private: // Data

        // ECOM framework requires this ID in object destructor
        TUid iDtor_ID_Key;

    };

    #include "extrstandbycontainerif.inl"

#endif  // C_EXTRSTANDBYCONTAINERIF_H

// End of File
