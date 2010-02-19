/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Builder class for XML UI node identifiers.
*
*/


#ifndef C_XMLNODEIDGENERATOR_H
#define C_XMLNODEIDGENERATOR_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CHsContentPublisher;
class THsPublisherInfo;
struct TAiContentItem;

namespace AiXmlUiController
{
/**
*  @ingroup group_xmluicontroller
* 
*  Xml node id generator.Generates an identifier that is used to look up 
*  settings node from XML UI model.
*
*  @lib AiXmlUiMain
*/
NONSHARABLE_CLASS( CXmlNodeIdGenerator ) : public CBase
    {
public: 
    // Constructor and destructor

    static CXmlNodeIdGenerator* NewL();
    
    ~CXmlNodeIdGenerator();

public:    
    // New functions

    /**
     * Generates an identifier that is used to look up settings node from 
     * XML UI model.
     *
     * @param aPublisherInfo Active Idle publisher info to use in the 
     *                       identifier generation.
     * @return content node identifier.
     */
    TPtrC SettingsNodeIdL( const THsPublisherInfo& aPublisherInfo );
    
    /**
     * Generates an identifier that is used to look up content node from 
     * XML UI model.
     *
     * @param aPlugin       Active Idle publisher to use in the identifier 
     *                      generation.
     * @param aContentItem  Active Idle content item to use in the 
     *                      identifier generation.
     * @return content node identifier.
     */
    TPtrC ContentNodeIdL( CHsContentPublisher& aPlugin, 
        const TAiContentItem& aContentItem );
    
    /**
     * Generates an identifier that is used to look up content node from 
     * XML UI model.
     *
     * @param aPublisherInfo Active Idle publisher info to use in the 
     *                       identifier generation.
     * @param aContentItem   Active Idle content item to use in the 
     *                       identifier generation.
     * @return content node identifier.
     */
    TPtrC ContentNodeIdL( const THsPublisherInfo& aPublisherInfo, 
        const TAiContentItem& aContentItem );
    
    /**
     * Generates an identifier that is used to look up resource node from 
     * XML UI model.
     *
     * @param aPlugin       Active Idle publisher to use in the identifier 
     *                      generation.
     * @param aContentItem  Active Idle content item to use in the 
     *                      identifier generation.
     * @return resource node identifier.
     */
    TPtrC ResourceNodeIdL( CHsContentPublisher& aPlugin, 
        const TAiContentItem& aContentItem );
    
    /**
     * Generates an identifier that is used to look up resource node from 
     * XML UI model.
     *
     * @param aPublisherInfo Active Idle publisher info to use in the 
     *                       identifier generation.
     * @param aContentItem   Active Idle content item to use in the 
     *                       identifier generation.
     * @return resource node identifier.
     */
    TPtrC ResourceNodeIdL( const THsPublisherInfo& aPublisherInfo, 
        const TAiContentItem& aContentItem );
    
private: 
    // Constructors
    
    /**
     * C++ default constructor
     */
    CXmlNodeIdGenerator();
        
private: 
    // data

    /** Content node id buffer, Owned */
    HBufC* iContentNodeIdBuf;    
    };
        
}  // namespace AiXmlUiController

#endif // C_XMLNODEIDGENERATOR_H

// End of file
