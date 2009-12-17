/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CPS Publishing for text editor.
*
*/

#ifndef XNTEXTEDITORPUBLISHER_H
#define XNTEXTEDITORPUBLISHER_H

#include <e32base.h>
#include <eikedwob.h>

// FORWARD DECLARATIONS
class CLiwServiceHandler;
class MLiwInterface;
class CXnTextEditorAdapter;

class CXnTextEditorPublisher : public CBase, public MEikEdwinObserver
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CXnTextEditorPublisher* NewL( CXnTextEditorAdapter& aAdapter, 
            const TDesC8& aNodeId );

        /**
        * Destructor.
        */
        virtual ~CXnTextEditorPublisher();
        
    private:
        CXnTextEditorPublisher( CXnTextEditorAdapter& aAdapter );
        void ConstructL( const TDesC8& aNodeId );

    public: // From MEikEdwinObserver
        /**
        * @see MEikEdwinObserver.
        */    
        void HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType);
        
        void PublishTextL( const TDesC& aText );

    private: // New functions
        void InitCpsInterfaceL();
        void AddDataToCpsL( const TDesC& aPublisherId, const TDesC& aContentType, 
            const TDesC& aContentId, const TDesC8& aDataKey, const TDesC& aData );
        void RemoveDataFromCpsL( const TDesC& aPublisherId, const TDesC& aContentType, 
            const TDesC& aContentId );


    private:
        CXnTextEditorAdapter&  iAdapter;
        HBufC*                 iNodeId; // Own
		MLiwInterface*         iCpsInterface; // Not own
		CLiwServiceHandler*    iServiceHandler;	// Own
		HBufC*                 iTextBuffer;   // Own
    };

#endif //XNTEXTEDITORPUBLISHER_H





