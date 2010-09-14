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
* Description:  Transaction element
*
*/


#ifndef C_TRANSACTIONELEMENT_H
#define C_TRANSACTIONELEMENT_H

#include <e32base.h>
#include "mtransactionelement.h"

class CXnNodeAppIf;
class MXnComponentInterface;

namespace AiUtility
    {
    class CContentPriorityMap;
    }

namespace AiXmlUiController
    {
    class CCssPropertyMap;
    }

// CLASS DECLARATION

namespace AiXmlUiController
    {

    /**
     *  @ingroup group_xmluicontroller
     * 
     * CTransactionElement is abstract base class for transaction elements.
     * CTransactionElement corresponds one publish operation within a
     * transaction scope.
     *
     * @lib AiXmlUiMain
     */
    class CTransactionElement : public CBase, public MTransactionElement
        {
        public:  // Constructors and destructor
            
            virtual ~CTransactionElement();

        public: // New functions
            
            void Reset();
            
            /**
             * Sets current content priority value.
             *
             * @param aPriority priority of current content
             * @post iContentPriority == aPriority
             */
            void SetContentPriority( TInt aPriority );
            
            /**
             * Sets CSS property map to use.
             *
             * @param aPropertyMap CSS property map
             */
            void SetCssPropertyMap( CCssPropertyMap* aPropertyMap );

            /**
             * Returns the target node.
             */
            CXnNodeAppIf& Target() const;
            
            /**
             * Returns offset to iElementLink.
             */
            inline static TInt LinkOffset();
            
            /**
             * Returns offset to iFactoryLink.
             */
            inline static TInt FactoryLinkOffset();
            
        protected:  // New functions
        
            CTransactionElement( AiUtility::CContentPriorityMap& aContentPriorityMap );
        
            /**
             * Sets the target node.
             */    
            void SetTarget(CXnNodeAppIf& aTarget);
            
            
            /**
             * Applies publishing policy to target UI element.
             * The policy is applied by modifying CSS property defined
             * by non-empty content.
             */
            void ApplyPublishingPolicy( RAiPolicyElementArray& aPolicyArray,
                                        RPropertyHashMap& aPropertyHashMap );
            
            void DoApplyPublishingPolicyL( RAiPolicyElementArray& aPolicyArray,
                                           RPropertyHashMap& aPropertyHashMap );
            
            /**
             * Updates current content priority into content priority map.
             */
            void UpdateContentPriorityL();
            
            /**
             * Clears current content priorty in the content priority map.
             */
            void ClearContentPriorityL();
            
            MXnComponentInterface& ComponentInterfaceL() const;
            
        private:
            
            /**
             * Content priority map
             */
            AiUtility::CContentPriorityMap& iContentPriorityMap;
            
            /**
             * CSS property map. Not own.
             */
            CCssPropertyMap* iPropertyMap;
        
            /**
             * Target node. Not own.
             */
            CXnNodeAppIf* iTarget;
            
            /**
             *  Content priority.
             */
            TInt iContentPriority;
            
            /**
             * Link in a doubly linked list of a transaction.
             */
            TDblQueLink iElementLink;
        
            /**
             * Link in a singly linked list of a factory.
             */
            TSglQueLink iFactoryLink;
            
        };
    
    TInt CTransactionElement::LinkOffset()
        {
        return _FOFF(CTransactionElement, iElementLink);
        }
        
    TInt CTransactionElement::FactoryLinkOffset()
        {
        return _FOFF(CTransactionElement, iFactoryLink);
        }
        
    }   // namespace AiXmlUiController

#endif      // C_TRANSACTIONELEMENT_H  
           
//  End of File
