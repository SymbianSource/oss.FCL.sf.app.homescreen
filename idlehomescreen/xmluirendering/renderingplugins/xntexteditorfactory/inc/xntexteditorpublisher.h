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
#include <frmtview.h>

// FORWARD DECLARATIONS
class CLiwServiceHandler;
class MLiwInterface;
class CXnTextEditorAdapter;

class CXnTextEditorPublisher : public CBase, public CTextView::MObserver
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
    // constructors
    
    /**
     * C++ constructor
     */
    CXnTextEditorPublisher( CXnTextEditorAdapter& aAdapter );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL( const TDesC8& aNodeId );

public: 
    // from CTextView::MObserver
    
    /**
    * @see CTextView::MObserver
    */           
    void OnReformatL( const CTextView* aTextView );
    
public:
    // new functions
    
    /**
     * Publishes given text to CPS
     * 
     * @since S60 5.2
     * @param aText Text to publish
     */
    void PublishTextL( const TDesC& aText );

private: 
    // new functions
    
    void InitCpsInterfaceL();
    
    void AddDataToCpsL( const TDesC& aPublisherId, const TDesC& aContentType, 
        const TDesC& aContentId, const TDesC8& aDataKey, const TDesC& aData );
    
    void RemoveDataFromCpsL( const TDesC& aPublisherId, const TDesC& aContentType, 
        const TDesC& aContentId );

private:
    // data
    
    /** Text editor adapter, not owned */
    CXnTextEditorAdapter& iAdapter;
    /** <texteditor> element node id, owned */
    HBufC* iNodeId; 
    /** CPS intertace, owned */
    MLiwInterface* iCpsInterface; 
    /** Service handler, owned */
    CLiwServiceHandler* iServiceHandler;	
    /** Text publish buffer, owned */
    HBufC* iTextBuffer;   
    /** Text publish temp buffer, owned */
    HBufC* iTempBuffer;   
    };

#endif //XNTEXTEDITORPUBLISHER_H





