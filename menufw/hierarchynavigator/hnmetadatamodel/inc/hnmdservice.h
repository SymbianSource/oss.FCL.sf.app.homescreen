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


#ifndef C_HNMDSERVICE_H
#define C_HNMDSERVICE_H

//#include <mnaiwservices.h>
#include <liwcommon.h>
#include <e32base.h>
#include <e32hashtab.h>
#include <xmlengelement.h> 

class CLiwServiceHandler;
class CHnMdQuery;
class CLiwGenericParamList;
class MLiwInterface;
class CLiwDefaultMap;
class CHnMdBaseKey;
class CHnMdServiceCommand;
class CHnServiceHandler;

/**
 * Service.
 * 
 * This class is desinged to facilitate dealing with services. 	
 *
 * @lib hnmetadatamodel
 * @since S60 5.0 *** for example, S60 v3.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdService) : public CBase
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully cosntructed object.
     */
    static CHnMdService* NewL( TXmlEngElement aElement );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     * @return Fully cosntructed object.
     */
    static CHnMdService* NewLC( TXmlEngElement aElement );

    /**
     * Standard C++ virtual destructor.
     *
     * @since S60 5.0
     */
    virtual ~CHnMdService();
    
    /**
     * Gets service.
     *
     * @since S60 5.0
     * @return Service name.
     */
    const TDesC8& GetServiceName() const;
    
    /**
     * Gets interface.
     *
     * @since S60 5.0
     * @return Interface name.
     */
    const TDesC8& GetInterfaceName() const;
    
    /**
     * Gets command.
     *
     * @since S60 5.0
     * @return Service command.
     */
    const CHnMdServiceCommand& GetCommand() const;   
    
    /**
     * returns query results
     *
     * since S60 v5.0
     * @return Result of the query to the service.
     */
    const CLiwGenericParamList& Output() const;

    /**
     * Returns service query constructor.
     *
     * since S60 v5.0
     * @return A structure of the key objects.
     */
    CHnMdBaseKey* GetConstructor() const;
   
    /**
     * Reads the consturctor into the model
     *
     * since S60 v5.0
     * @param aElement Xml element.
     * @return A structure of the key objects.
     */
    void CHnMdService::SetConstructorL( TXmlEngElement aElement );
    
    /**
     * Evaluates constructor.
     * 
     * @param aList Evaluation parameter.
     * @param aPos Position of the record in the parameter's list.
     * @return Evaluated generic parameters list.
     */
    CLiwGenericParamList* EvaluateConstructorL( const CLiwGenericParamList& aList,
            TInt aPos = 0);
    
    /**
     * Evaluates constructor as key.
     * 
     * @param aList Evaluation parameter.
     * @param aPos Position of the record in the parameter's list.
     * @return Evaluated key.
     */
    CHnMdBaseKey* EvaluateConstructorAsKeyL( const CLiwGenericParamList& aList,
            TInt aPos = 0);
    /**
     * Evaluates command.
     * 
     * @param aList Evaluation parameter.
     * @param aPos Position of the record in the parameter's list.
     * @return Evaluated generic parameters list.
     */
    CLiwGenericParamList* EvaluateCommandL( const CLiwGenericParamList& aList, 
            TInt aPos = 0);
    
    /**
     * Evaluates command as key.
     * 
     * @param aList Evaluation parameter.
     * @param aPos Position of the record in the parameter's list.
     * @return Evaluated key.
     */
    CHnMdBaseKey* EvaluateCommandAsKeyL( const CLiwGenericParamList& aList, 
            TInt aPos = 0);


private:

    /**
     * Sets interface.
     * 
     * @param aAttr Xml attribute.
     */
    void SetInterfaceL( TXmlEngAttr aAttr );
    
    /**
     * Sets service.
     * 
     * @param aAttr Xml attribute.
     */
    void SetServiceL( TXmlEngAttr aAttr );
    
    /**
     * Standard C++ constructor.
     *
     * @since S60 5.0
     */
    CHnMdService(); 


    /**
     * Standard symbian 2nd phase contructor.
     *
     * @since S60 5.0
     * @param aElement Xml element.
     */
    void ConstructL( TXmlEngElement aElement );
  
private: // data

    /**
     * Own - Service.
     */
    RBuf8 iServiceName;

    /**
     * Own - Interface.
     */
    RBuf8 iInterfaceName;
    
    /**
     * Own - Service command.
     */
    CHnMdServiceCommand* iServiceCommand;
    
    /**
     * Stores input parameters (not data) of the service search method
     * (ExecuteServiceCmdL).
     * Own.
     */
    CHnMdBaseKey* iConstructor;
     
    };

#endif // C_HNMDSERVICEHANDLER_H
