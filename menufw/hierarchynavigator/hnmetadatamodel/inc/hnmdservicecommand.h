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


#ifndef C_HNMDSERVICECOMMAND_H
#define C_HNMDSERVICECOMMAND_H

#include <e32base.h>

#include "hnglobals.h"

class TXmlEngElement;
class TXmlEngAttr;
class CHnMdBaseKey;

/**
 * Service Command.
 * 
 * This class represents the service command. In general, the service command
 * are parameters of the service queries.
 *
 * @lib hnmetadatamodel
 * @since S60 5.0 *** for example, S60 v3.0
 * @ingroup group_hnmetadatamodel
 */   
NONSHARABLE_CLASS( CHnMdServiceCommand ) : public CBase
{
public:
    
    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnMdServiceCommand* NewL( TXmlEngElement aElement );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully constructed object.
     */
    static CHnMdServiceCommand* NewLC( TXmlEngElement aElement );

    /**
     * Standard C++ virtual destructor.
     */
    virtual ~CHnMdServiceCommand();
    
    /**
     * Returns command.
     *
     * @since S60 5.0
     * @return Command name.
     */
    const TDesC8& GetCommandName() const;
    
    /**
     * Returns data.
     *
     * @since S60 5.0
     * @return A list of keys.
     */
    CHnMdBaseKey* GetCommandData() const;

    /**
     * Returns service mode.
     *
     * @since S60 5.0
     * @return Command mode.
     */
    TServiceMode GetCommandMode() const;
       
private:
    
    /**
     * Standard C++ constructor.
     */
    CHnMdServiceCommand();

    /**
     * Standard symbian 2nd phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     */
    void ConstructL( TXmlEngElement aElement );
        
    /**
     * Sets command parameters.
     * 
     * @param aElement Xml element.
     */  
    void SetCommandDataL( TXmlEngElement aElement );
    
    /**
     * Sets service name.
     *
     * @since S60 5.0
     * @param aAttr Xml attribute.
     */
    void SetCommandNameL( TXmlEngAttr aAttr );
       
    /**
     * Sets service mode.
     *
     * @since S60 5.0
     * @param aAttr Xml attribute.
     */
    void SetCommandModeL( TXmlEngAttr aAttr );
    
    /**
     * Removes redundant key.
     * TO BE REMOVED.
     * 
     * @param aElement Xml element.
     * @return Xml element with removed redundant key. 
     */
    TXmlEngElement RemoveRedundantKeyL( TXmlEngElement aElement );
    
private:
    
    /**
     * Service mode: synchronous / asynchronous
     */
    TServiceMode iMode;
    
    /**
     * Command name, own
     */ 
    RBuf8 iName;

    /**
     * Data - service parameters, own
     */
    CHnMdBaseKey* iData;
};

#endif /*HNMDSERVICECOMMAND_H_*/
