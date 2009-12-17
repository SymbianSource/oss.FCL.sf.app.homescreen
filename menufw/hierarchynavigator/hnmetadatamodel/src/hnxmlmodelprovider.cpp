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

#include <sysutil.h> 
#include "hnxmlmodelprovider.h"
#include "hnxmlmodelcache.h"	
#include "hnxmlsuitefilesreg.h"
#include "hnglobals.h"
#include "hnconvutils.h"
#include "menudebug.h"
#include "hnhelper.inl"
#include "hnmdmodel.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::ConstructL()
	{
	iDomImpl.OpenL();
	User::LeaveIfError(iDomParser.Open(iDomImpl));
	User::LeaveIfError(iFs.Connect());
	iFileMan = CFileMan::NewL(iFs);

	iPath.CreateL(KMaxPath);
	User::LeaveIfError(iFs.PrivatePath(iPath));
	iPath.Insert(0, KDrive);
	iPath.Append(KEntriesSuffix);
	iCache = CHnXmlModelCache::NewL();
	
	CActiveScheduler::Add( this);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::DoCancel()
	{
	
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::SynchronizationFinishedL()
	{
	ResetCache();
	THnMdCommonPointers &cmnPtr = *THnMdCommonPointers::Static();
	CHnMdModel* model = cmnPtr.iModel;
	if ( iSuiteFilesUpdated )
	    {
	    model->ReloadStackSuitesL( cmnPtr.iContainer );
	    }
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::RunL()
	{
	if ( !iSuiteSetIterator )
		{
		CheckDrivesL();
	    iSuiteSetIterator = new ( ELeave ) THashSetIter<HBufC*>( iInstSuites );
	    iSuiteSetIterator->Reset();
	    iStatus = KRequestPending;
	    SetActive();
	    TRequestStatus* status = &iStatus;
	    User::RequestComplete( status, KErrNone );
		}
	else if ( iSuiteSetIterator->Next() )
		{
		TBool filesUpdated = CHnXmlSuiteFilesReg::SynchronizeL(
                iFs, **(iSuiteSetIterator->Current()) );
        iSuiteFilesUpdated = ( iSuiteFilesUpdated || filesUpdated ); 
	    iStatus = KRequestPending;
	    SetActive();
	    TRequestStatus* status = &iStatus;
	    User::RequestComplete( status, KErrNone );
		}
	else
		{
		delete iSuiteSetIterator;
		iSuiteSetIterator = NULL;
		SynchronizationFinishedL();
		}
	}
	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnXmlModelProvider* CHnXmlModelProvider::NewL()
	{
	CHnXmlModelProvider* self = CHnXmlModelProvider::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnXmlModelProvider* CHnXmlModelProvider::NewLC()
	{
	CHnXmlModelProvider* self = new (ELeave) CHnXmlModelProvider;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnXmlModelProvider::CHnXmlModelProvider() :
	CActive( EPriorityLow ),	
	iInstSuites(&HBuf16Hash, &HBuf16Ident),
	iEventMap(&HBuf16Hash, &HBuf16Ident), 
	iSuiteSetIterator( NULL )
	{
	 
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CHnXmlModelProvider::~CHnXmlModelProvider()
	{
	Cancel(); 
	
	delete iSuiteSetIterator;
	delete iCache;
	iDomParser.Close();
	iDomImpl.Close();
	delete iFileMan;
	iFs.Close();
	iPath.Close();

	// clean up eventmap
	THashMapIter<HBufC*, TInt> iter(iEventMap);
	while (HBufC* const * ptr = iter.NextKey())
		{
		delete *ptr;
		}
	iEventMap.Close();

	THashSetIter<HBufC*> iterSuites(iInstSuites);
	while ( iterSuites.Next())
		{
		delete *iterSuites.Current();
		}
	iInstSuites.Close();

	}

#ifdef _DEBUG
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::LogEventMapping(
		const RHashMap<HBufC*, TInt>& aEventMap)
	{
    DEBUG(("\t_MM_: CHnXmlModelProvider::LogEventMapping IN"));
    RFs fs;
    if ( !fs.Connect() )
        {
        RFile logFile;
        if ( !logFile.Replace( fs, KEventMapLogFile, EFileWrite ) )
            {
            // first line
            TBuf8<256> buf1;
            buf1.Append( _L8("EventName") );
            buf1.Append( _L8("\t") );
            buf1.Append( _L8("EventId") );
            buf1.Append( _L8("\n") );
            logFile.Write( 0, buf1 );                    
            THashMapIter<HBufC*, TInt> iter( aEventMap );
            while ( HBufC* const * ptr = iter.NextKey() )
                {
                TInt *v = iter.CurrentValue();
                TBuf8<256> buf2;
                buf2.Append( (*ptr)->Des() );
                buf2.Append( _L8("\t") );
                buf2.AppendNum( *v );
                buf2.Append( _L8("\n") );
                TInt size ( 0 );
                logFile.Size( size );
                logFile.Write( size, buf2 );
                }
            logFile.Close();
            }
        fs.Close();
        }
    DEBUG(("\t_MM_: CHnXmlModelProvider::LogEventMapping OUT"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CHnXmlModelProvider::LogSuiteModel( const TDesC& aName, 
                                                    RXmlEngDocument& aDoc )
    {
    DEBUG(("\t_MM_: CHnXmlModelProvider::LogSuiteModel IN"));
    TBuf8<256> buf1;
    TFileName logFileName;
    logFileName.Copy( KSuiteLoadingLogFile );
    logFileName.Append( Kbackslash );
    logFileName.Append( aName );
    logFileName.Append( KXmlExt );
    TRAP_IGNORE( aDoc.SaveL( logFileName, aDoc.DocumentElement() ) );
    DEBUG(("\t_MM_: CHnXmlModelProvider::LogSuiteModel OUT"));
    }
#endif

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
RXmlEngDocument CHnXmlModelProvider::ParseFileL( const TDesC& aPath )
    {
    RBuf8 doc( ReadFileLC( aPath ) );
    CleanupClosePushL(doc);
    
    RXmlEngDocument xmlDoc;
    CleanupClosePushL( xmlDoc );
    xmlDoc = ParseDocL(doc);
    CleanupStack::Pop( &xmlDoc );
    
    // clean up
    CleanupStack::PopAndDestroy( &doc );
    
    return xmlDoc;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
RXmlEngDocument CHnXmlModelProvider::ParseDocL( const TDesC8& aDoc )
    {
    RXmlEngDocument doc;
    CleanupClosePushL( doc );
    doc = iDomParser.ParseL( aDoc );
    CleanupStack::Pop( &doc );
    return doc;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC8* CHnXmlModelProvider::ReadFileLC(const TDesC& aPath)
    {
    RFile file;
    User::LeaveIfError( file.Open(iFs, aPath, EFileRead) );
    CleanupClosePushL( file );
    
    TInt fileSize(0);
    User::LeaveIfError( file.Size( fileSize ) );
    HBufC8* buf = HBufC8::NewLC( fileSize );
    TPtr8 bufPtr( buf->Des() );
    User::LeaveIfError( file.Read( bufPtr ) );

    // clean up
    CleanupStack::Pop(buf);
    CleanupStack::PopAndDestroy(&file);    

    return buf;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::ChangeEventsToIdsInChildrenL(
		TXmlEngElement & aElement)
	{
	RXmlEngNodeList<TXmlEngElement> children;
	CleanupClosePushL(children);
	aElement.GetChildElements(children);
	TInt amount = children.Count();
	for (TInt i(0); i < amount; i++)
		{
		TXmlEngElement child = children.Next();
		TPtrC8 childName = child.Name();

		if (!childName.Compare(KEventName8) || !childName.Compare(
				KMenuItemName8) ||!childName.Compare( KMenuItemSpecificName8 ) 
				|| !child.Name().Compare(KMenuToolbar8))
			{
			ChangeEventNamesToIdsL(child);
			}
		else if (!childName.Compare(KEditModeItem8))
			{
			ChangeEventsToIdsInChildrenL(child);
			}
		}

	CleanupStack::PopAndDestroy(&children);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnXmlModelProvider::CollectSuiteL(const TDesC& aSuiteName,
		RXmlEngDocument& aXmlDoc)
	{
	TInt err(KErrNotFound);
	TBuf<KMaxPath> filePath;
	TBuf<KMaxPath> KsuitePath;
	KsuitePath.Zero();
	KsuitePath.Copy(iPath);
	KsuitePath.Append(aSuiteName);
	KsuitePath.Append(Kbackslash);

	filePath.Zero();
	filePath.Copy(KsuitePath);
	filePath.Append(KSuiteDefFileName);

	RXmlEngDocument suiteDoc;
	CleanupClosePushL(suiteDoc);
	
	suiteDoc = ParseFileL(filePath);
    
    // one suite per file
    RXmlEngNodeList<TXmlEngElement> elements;
    suiteDoc.DocumentElement().GetChildElements( elements );
    TXmlEngElement suiteElement;
    while ( elements.HasNext() )
        {
        suiteElement = elements.Next();
        TPtrC8 t = suiteElement.Name();
        if ( !suiteElement.Name().Compare( KSuiteElementName8 ) )
            {
            ChangeEventsToIdsInChildrenL( suiteElement.AsElement() );    
            break;
            }
        }
    
    aXmlDoc.OpenL( iDomImpl, suiteDoc.DocumentElement() );            
    err = KErrNone;
    
    CleanupStack::PopAndDestroy( &suiteDoc );
    return err;             
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnXmlModelProvider::SuiteExistsL( const TDesC& aSuite )
    {
    TBuf<KMaxPath> filePath;
    TBuf<KMaxPath> KsuitePath;
    KsuitePath.Zero();
    KsuitePath.Copy( iPath );
    KsuitePath.Append( aSuite );
    KsuitePath.Append( Kbackslash );

    filePath.Zero();
    filePath.Copy( KsuitePath );
    filePath.Append( KSuiteDefFileName );
    
    return BaflUtils::FileExists( iFs, filePath );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::CollectItemsL( const TDesC& aSuiteName,
        RXmlEngDocument& aXmlDoc )
    {
    TBuf<KMaxPath> filePath;
    TBuf<KMaxPath> KsuitePath;
    KsuitePath.Zero();
    KsuitePath.Copy( iPath );
    KsuitePath.Append( aSuiteName ); 
    KsuitePath.Append( Kbackslash ); 
    CDir* fileList;                  
    iFs.GetDir( KsuitePath, KEntryAttMaskSupported, ESortByName, fileList);
    CleanupStack::PushL(fileList);

    //for each file with entries definitions         
    for( TInt i(0); i<fileList->Count(); i++ )  
        {            
        TPtrC fileName = (*fileList)[i].iName;   
        
        if(!fileName.Compare(KSuiteDefFileName))
            continue;
        
        filePath.Zero();
        filePath.Copy(KsuitePath);
        filePath.Append(fileName);  
            
        RXmlEngDocument itemDoc;
        CleanupClosePushL( itemDoc );
        
        TInt err( KErrNone );
        TRAP( err , itemDoc = ParseFileL(filePath) );
        if( !err )
            {
            RXmlEngNodeList< TXmlEngElement > children;
            CleanupClosePushL(children);
            itemDoc.DocumentElement().GetChildElements(children);
            
            TPtrC8 itemGenre =
                itemDoc.DocumentElement().AttributeNodeL(KGenreName8).Value();
                
            TInt amount = children.Count();
            for( TInt i(0); i<amount; i++ )
                {
                TXmlEngElement child = children.Next();
                TPtrC8 tempChildName = child.Name();
                // append localizations to root
                if (!child.Name().Compare(KLocalizationName8)) 
                    {                
                    aXmlDoc.DocumentElement().AsElement().AppendChildL(child);
                    child.ReconcileNamespacesL();
                    } 
                // append itmes to proper suite 
                else if (!child.Name().Compare(KItemName8) )
                    {                                    
                    // go througs item's children to change event names to ids
                    ChangeEventsToIdsInChildrenL(child);
                    
                    // edit_mode item
                    RXmlEngNodeList< TXmlEngElement > editModeItems;
                    CleanupClosePushL( editModeItems );
                    child.GetChildElements( editModeItems );
                    TInt count = editModeItems.Count();
                    for ( TInt ic( 0 ); ic < count; ic++ )
                        {
                        TXmlEngElement editModeItem = editModeItems.Next();
                        if ( !editModeItem.Name().Compare( KEditModeItem8 ))
                            {
							editModeItem.AddNewAttributeL(KSuiteElementName8,
									itemGenre);
							}
						}
					CleanupStack::PopAndDestroy(&editModeItems);
					// edit_mode items - end

					RXmlEngNodeList<TXmlEngElement> suites;
					CleanupClosePushL(suites);
					aXmlDoc.DocumentElement().GetChildElements(suites);
					TInt amountSuites = suites.Count();
					for (TInt i(0); i < amountSuites; i++)
						{
						TXmlEngElement childSuite = suites.Next();
						TPtrC8 tempName = child.Name();
						//find suite
						if (!childSuite.Name().Compare(KTitleName8))
							{
							TPtrC8 suiteGenre = childSuite.AttributeNodeL(
									KGenreName8).Value();

							//find proper suite to append item
							if (!suiteGenre.Compare(itemGenre))
								{
								child.AddNewAttributeL(KSuiteElementName8,
										itemGenre);
								childSuite.AppendChildL(child);
                                }                        
                            }
                        }
					aXmlDoc.DocumentElement().ReconcileNamespacesL();
                    CleanupStack::PopAndDestroy( &suites );                    
                    }                                
                }                   
            CleanupStack::PopAndDestroy( &children );
            }
        CleanupStack::PopAndDestroy( &itemDoc );
        } 
    
    CleanupStack::PopAndDestroy(fileList);
    }        


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnXmlModelProvider::GetNewEventId( HBufC* aEventName )
    {
    TInt res( KErrGeneral );

    if ( !aEventName->Compare( KKeyEmpty ) )
        {
        res = KKeyIdEmpty;
        }
    else if ( !aEventName->Compare( KKeySelect ) )
        {
        res = KKeyIdSelect;
        }
    else if ( !aEventName->Compare( KKeySelect2 ) )
        {
        res = KKeyIdSelect2;
        }
    else if ( !aEventName->Compare( KKeySelect3 ) )
        {
        res = KKeyIdSelect3;
        }
    else if ( !aEventName->Compare( KKeyCall ) )
        {
        res = KKeyIdCall;
        }
    else if ( !aEventName->Compare( KKeyClear ) )
        {
        res = KKeyIdClear;
        }
    else if ( !aEventName->Compare( KKeyMove ) )
        {
        res = KKeyIdMove;
        }
    else if ( !aEventName->Compare( KKeyMoveInto ) )
        {
        res = KKeyIdMoveInto;
        }
    else if ( !aEventName->Compare( KOnSuiteLoad ) )
        {
        res = KIdSuiteLoad;
        }
    else if ( !aEventName->Compare( KOnSuiteUnLoad ) )
        {
        res = KIdSuiteUnLoad;
        }
    else if ( !aEventName->Compare( KOnFocusGain ) )
        {
        res = KIdFocusGain;
        }
    else if ( !aEventName->Compare( KOnFocusLost ) )
        {
        res = KIdFocusLost;
        }
    else
        {
        THashMapIter<HBufC*, TInt> iter( iEventMap );
        res = KCustomKeyOffset;
        
        while ( TInt const * v = iter.NextValue() )
            {
            if ( v != NULL && *v >= KCustomKeyOffset )
                {
                res++;
                }
            }
        }
    
    return res;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::ChangeEventNamesToIdsL( TXmlEngElement& aElement )
    {
    if ( aElement.HasChildNodes() && ( !aElement.Name().Compare( KMenuItemName8 )  
            || !aElement.Name().Compare( KMenuToolbar8 ) ) )
        {
        
        RXmlEngNodeList< TXmlEngElement > children;
        CleanupClosePushL(children);
        aElement.GetChildElements(children);
        TInt count( children.Count() );
        for( TInt i( 0 ); i < count; i++ )
            {
            TXmlEngElement element = children.Next();
            if ( !element.Name().Compare( KMenuItemName8 ) ||
            		!element.Name().Compare( KMenuItemSpecificName8 ) ||
                    !element.Name().Compare( KMenuButton8 ) )
                {
                ChangeEventNamesToIdsL( element );
                }
            }
        CleanupStack::PopAndDestroy( &children );
        }
    else if( aElement.AttributeValueL( KEventAttrName8 ).Length() )
        {
        TBool bufferEmpty ( ETrue );
        TBuf8<KMaxPath> newEventIds;
             
        // loop through all event names

        // parsing events        
        HBufC* events = HnConvUtils::Str8ToStrLC(
            aElement.AttributeValueL( KEventName8 ) );
        
        TLex lex( events->Ptr() );
        TChar ch;
        
        TInt length( events->Length() );
        TInt cnt = 0;
        while( ch = lex.Get() != 0 )
            {
            cnt++;
            while ( ( ch = lex.Peek() ) != '|' && cnt < length)
                {
                lex.Inc();
                cnt++;
                }

            // iEventMap takes ownership of "en"
            HBufC* en = HBufC::NewL( lex.MarkedToken().Length() );
            en->Des().Copy( lex.MarkedToken() );

            TInt* resFind = iEventMap.Find( en );
            TInt res = -1;
            if ( resFind == NULL )
                {
                TInt newId( GetNewEventId( en ) );
                iEventMap.Insert( en, newId );                
                res = newId;
                }
            else
                {
                res = *resFind;
                delete en;
                }
            
            if ( bufferEmpty ) 
                {
                newEventIds.AppendNum( res );
                bufferEmpty = EFalse;
                }
			else
				{
				newEventIds.Append(KPipe);
				newEventIds.AppendNum(res);
				}

            lex.Inc();
            cnt++;
            lex.Mark();
            if (cnt >= length)
                break;
            }

        TXmlEngAttr att = aElement.AttributeNodeL( KEventName8 );
        att.SetValueL( newEventIds );
        aElement.SetAttributeNodeL( att );

        CleanupStack::PopAndDestroy( events );
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnXmlModelProvider::SynchronizeSuitesL()
    {
    MMPERF(("CHnXmlModelProvider::SynchronizeSuitesL - START"));
    iSuiteFilesUpdated = EFalse;
    TBool refresh( EFalse );   

    if (IsActive())
    	{
    	Cancel();
    	}
    
    delete iSuiteSetIterator;
    iSuiteSetIterator = NULL;
    
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
   
    MMPERF(("CHnXmlModelProvider::SynchronizeSuitesL - DONE"));
    return refresh;
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CHnXmlModelProvider::ResetCache()
    {
    iCache->Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
void CHnXmlModelProvider::SetupSuitePathL()
	{
	iPath.Close();
	iPath.CreateL(KMaxPath);
	User::LeaveIfError(iFs.PrivatePath(iPath));
	TChar drive;
	TBuf<1> driveLetter;
	
	iFs.DriveToChar( EDriveC, drive );
	iPath.Insert(0, KColon );
	driveLetter.Append( drive );
	iPath.Insert(0, driveLetter );
	iPath.Append(KEntriesSuffix);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
void CHnXmlModelProvider::SetupFailSafeSuitePathL()
	{
	iPath.Close();
	iPath.CreateL(KMaxPath);
	User::LeaveIfError(iFs.PrivatePath(iPath));
	TChar drive;
	TBuf<1> driveLetter;
	
	iFs.DriveToChar( EDriveZ, drive );
	iPath.Insert(0, KColon );
	driveLetter.Append( drive );
	iPath.Insert(0, driveLetter );
	iPath.Append(KSuitesDir);
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C void CHnXmlModelProvider::ReloadModelL()
	{
	SynchronizeSuitesL();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CHnXmlModelProvider::CreateModelL(const TDesC& aSuiteName,
        RXmlEngDocument& aXmlDoc )
    {
    MMPERF( ( "CHnXmlModelProvider::CreateModelL IN") );
    MMPERF( ( "    Suite name: %S", &aSuiteName ) );
    
    TBool suiteCollected = EFalse;
	
    SetupSuitePathL();	
    TRAPD( failSafeErr, suiteCollected = CollectSuiteL( aSuiteName, aXmlDoc ));
    if (failSafeErr)
    	{
    	SetupFailSafeSuitePathL();
    	suiteCollected = CollectSuiteL( aSuiteName, aXmlDoc );
    	}
    
    if ( !suiteCollected )
        {
        MMPERF(("CHnXmlModelProvider::CreateModelL - suite collected"));
        CollectItemsL( aSuiteName, aXmlDoc );
        MMPERF(("CHnXmlModelProvider::CreateModelL - items collected"));
        }
    MMPERF( ( "CHnXmlModelProvider::CreateModelL OUT") );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CHnXmlModelProvider::GetModelL(
    const TDesC& aSuiteName, RXmlEngDocument& aXmlDoc )
    {
    if ( !iCache->IsCachedL( aSuiteName ) )
        {
        RXmlEngDocument newXmlDoc;
        CleanupClosePushL( newXmlDoc );
        
        CreateModelL( aSuiteName, newXmlDoc );
        
        #ifdef _DEBUG
            DEBUG(("_MM_:CHnXmlModelProvider::GetModelL _DEBUG IN"));
            LogEventMapping( iEventMap );
            LogSuiteModel( aSuiteName, newXmlDoc );
            DEBUG(("_MM_:CHnXmlModelProvider::GetModelL _DEBUG OUT"));
        #endif

        // cache takes ownership of the suite name and the document  
        iCache->AddL( aSuiteName, newXmlDoc );
        
        CleanupStack::Pop( &newXmlDoc );
        }
    
    iCache->GetL( aSuiteName, aXmlDoc );
    
    return KErrNone;
    }   
   

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::SearchPathForSuitesL( const TDesC& aPath )
	{
	CDir* fileList = NULL;            
	iFs.GetDir( aPath, KEntryAttMaskSupported, ESortByName, fileList );  
	if ( fileList )                        
		{                
		CleanupStack::PushL( fileList );      
		for( TInt i( 0 ); i < fileList->Count(); i++ ) 
			{  
			if ( (*fileList)[ i ].IsDir() )
				{
				HBufC* suiteName = (*fileList)[i].iName.AllocLC();
				if ( !iInstSuites.Find(suiteName) )
					{
					iInstSuites.InsertL( suiteName );
					CleanupStack::Pop(suiteName);
					}
				else
					{
					CleanupStack::PopAndDestroy(suiteName);
					}
				}
			}
			CleanupStack::PopAndDestroy( fileList );
		}
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnXmlModelProvider::CheckDrivesL()
    { 
    TDriveList driveList;
    User::LeaveIfError( iFs.DriveList( driveList ) );
    
    TFixedArray< TInt, KMaxDrives > driveSearchOrder( KDriveSearchOrder, KMaxDrives );
    for(TInt iterator(0); iterator < driveSearchOrder.Count(); iterator++ )
     	{        
        if ( driveList[ driveSearchOrder[ iterator ] ] )
            {
            TBuf<KMaxPath> filePath;

            TChar driveLetter;
            User::LeaveIfError( iFs.DriveToChar( driveSearchOrder[ iterator ], driveLetter ) );  
            
            TBuf< KSingleChar > driveLetterConst;
            driveLetterConst.Append( driveLetter );
            User::LeaveIfError( iFs.PrivatePath( filePath ) );
            filePath.Insert( 0, driveLetterConst );
            filePath.Insert( 1, KColon );
            filePath.Append( KSuitesDir );
                         
            SearchPathForSuitesL( filePath );           
            }
    	}
    
    SearchPathForSuitesL( iPath );
    }

