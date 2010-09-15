/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  factory settings for active space
*
*/


#include <centralrepository.h>
#include <menudebug.h>
#include <apadef.h>
#include <AknDef.hrh>
#include "hnrepositoryobserver.h"
#include "hnglobals.h"
#include "hnsuitemodelcontainer.h"
#include "hnsuitemodel.h"
#include "hnmdsuite.h"
#include "hnitemsorder.h"
#include "hnliwutils.h"
#include "hnmdbasekey.h"
#include "hnmdkeyfactory.h"
#include "hnconvutils.h"
#include "hnsuiteobserver.h"
#include "hnmdmodel.h"

class CHnServiceHandler;
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryObserver::ConstructL( const TUid aRepositoryUid )
    {
    iRepository = CRepository::NewL( aRepositoryUid );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryObserver::CHnRepositoryObserver( THnMdCommonPointers* aCmnPtrs )
    : iCmnPtrs(aCmnPtrs)
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryObserver::CHnRepositoryObserver( THnMdCommonPointers* aCmnPtrs, TUint32 aId  )
: iId( aId ), iCmnPtrs(aCmnPtrs)
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryObserver::~CHnRepositoryObserver()
    {
    if (iNotifyHandler)
        iNotifyHandler->StopListening();
    delete iNotifyHandler;
    delete iRepository;
    }


// class CHnRepositoryWidgetTypeObserver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryWidgetTypeObserver * CHnRepositoryWidgetTypeObserver::NewL(
        THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid )
    {
    CHnRepositoryWidgetTypeObserver * self = CHnRepositoryWidgetTypeObserver::NewLC( aCmnPtrs, aRepositoryUid );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryWidgetTypeObserver * CHnRepositoryWidgetTypeObserver::NewLC(
        THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid )
    {
    CHnRepositoryWidgetTypeObserver * self = new (ELeave) CHnRepositoryWidgetTypeObserver( aCmnPtrs );
    CleanupStack::PushL(self);
    self->ConstructL( aRepositoryUid );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryWidgetTypeObserver::ConstructL( const TUid aRepositoryUid )
    {
    CHnRepositoryObserver::ConstructL( aRepositoryUid );
    iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iRepository );
    iNotifyHandler->StartListeningL();
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryWidgetTypeObserver::CHnRepositoryWidgetTypeObserver( THnMdCommonPointers* aCmnPtrs )
    : CHnRepositoryObserver( aCmnPtrs )
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryWidgetTypeObserver::~CHnRepositoryWidgetTypeObserver()
    {
    iWidgetSwitches.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryWidgetTypeObserver::HandleNotifyGeneric(TUint32 aId)
    {
    TRAP_IGNORE( HandleNotifyGenericL(aId) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryWidgetTypeObserver::HandleNotifyGenericL(TUint32 aId)
    {
    DEBUG(("_MM_:CHnRepositoryObserver::HandleNotifyGeneric IN"));
    DEBUG(("\tID: %d",aId));
    THnSuiteWidgetType suiteState( EUnspecified );

    iRepository->Get( aId, (int &) suiteState);
    TInt id = iWidgetSwitches.Find(aId);

    if ( suiteState == EChangeWidget )
        {
        iWidgetSwitches.AppendL(aId);
        }
    else if ( id != KErrNotFound )
        {
        iWidgetSwitches.Remove(id);
        return;
        }

    CHnSuiteModel* lastSuiteModel = iCmnPtrs->iContainer->GetLastSuiteModel();
    TBuf <NCentralRepositoryConstants::KMaxUnicodeStringLength> suiteName;
    if ( iRepository->Get( aId - 1, suiteName ) == KErrNone && lastSuiteModel
            && lastSuiteModel->WidgetType() != suiteState )
        {
        DEBUG16(("\t\tSuite Name: %S",&suiteName));
        lastSuiteModel->GetItemsOrder()->MarkSuiteUninitialized();

        TInt32 suiteId = lastSuiteModel->CustomId();

        CLiwGenericParamList* inParam = CLiwGenericParamList::NewL();
        CleanupStack::PushL( inParam );

        CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

        map->InsertL( KGenreName8, TLiwVariant(suiteName) );
        map->InsertL( KId8, TLiwVariant(suiteId) );
        inParam->AppendL(TLiwGenericParam( KFilterElementItem8, TLiwVariant(map) ));
        CleanupStack::PopAndDestroy( map );

        iCmnPtrs->iModelEventObserver->HandleModelEventL( KReevaluateMdEvent, *inParam );

        CleanupStack::PopAndDestroy( inParam );
        }
    DEBUG(("_MM_:CHnRepositoryObserver::HandleNotifyGeneric OUT"));
    }


// class CHnRepositoryShowFolderObserver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryShowFolderObserver * CHnRepositoryShowFolderObserver::NewL(
        THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid, const TUint32 aId )
    {
    CHnRepositoryShowFolderObserver * self = CHnRepositoryShowFolderObserver::NewLC( aCmnPtrs, aRepositoryUid, aId );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryShowFolderObserver * CHnRepositoryShowFolderObserver::NewLC(
        THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid, const TUint32 aId )
    {
    CHnRepositoryShowFolderObserver * self = new (ELeave) CHnRepositoryShowFolderObserver( aCmnPtrs, aId );
    CleanupStack::PushL(self);
    self->ConstructL( aRepositoryUid );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryShowFolderObserver::ConstructL( const TUid aRepositoryUid )
    {
    CHnRepositoryObserver::ConstructL( aRepositoryUid );
    TInt ret = iRepository->Create( iId, KBlank );
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iRepository,
            CCenRepNotifyHandler::EStringKey, iId );
    iNotifyHandler->StartListeningL();
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryShowFolderObserver::CHnRepositoryShowFolderObserver( THnMdCommonPointers* aCmnPtrs, TUint32 aId  )
    : CHnRepositoryObserver( aCmnPtrs, aId )
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryShowFolderObserver::~CHnRepositoryShowFolderObserver()
    {
    delete iServiceHandler;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryShowFolderObserver::HandleNotifyString(TUint32 aId, const TDesC16& aNewValue )
    {
    TRAP_IGNORE( HandleNotifyStringL(aId, aNewValue) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryShowFolderObserver::HandleNotifyStringL(TUint32 aId, const TDesC16& aNewValue)
    {
    DEBUG(("_MM_:CHnRepositoryShowFolderObserver::HandleNotifyStringL IN"));
    DEBUG(("\tID: %d : %S", aId, &aNewValue));

    //++Show Folder
    if( aId == iId && aNewValue.Length())
        {
        iNotifyType = 0;
        ExtractCRKeyShowFolderName( aNewValue );

        // iCRKeyFolderItemUid can contain mcs id or uid
        // in case of uid it will be replaced whith id in HandleNotifyL callback
        if( iCRKeyFolderItemUid.Length()>0 )
            {
            if( iCRKeyFolderItemUid.FindF( KHexPrefix8 )==0 )
                {
                iNotifyType |= EAPP_UID;
                }
            else
                {
                iNotifyType |= EAPP_ID;
                }
            }

            CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
                KInData8, KKeyTypeMap, KDefaultParentId8 );
            CleanupStack::PushL( inDataKey );

            CHnMdBaseKey* filter( NULL );
            HBufC8* id( NULL );
            if ( iCRKeyFolderName.Length()>0 && !( iNotifyType & EAPP_UID ) )
                {
                // app group name is known
                // id is optional
                id = HnConvUtils::NumToStr8LC( KRootId );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuFolder8 ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KIdParam, KKeyTypeInteger, *id ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KRecursiveSearch8, KKeyTypeBoolean, KStringTrue8 ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KFlatResult8, KKeyTypeBoolean, KStringTrue8 ) );

                filter = HnMdKeyFactory::CreateL(
                        KFilter8, KKeyTypeMap, KStringTrue8 );
                CleanupStack::PushL( filter );
                filter->AddSubKeyL( HnMdKeyFactory::CreateL( KMcsAppGroupName8, KKeyTypeString, iCRKeyFolderName ) );
                filter->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuFolder8 ) );
                filter->AddSubKeyL( HnMdKeyFactory::CreateL( KHidden8, KKeyTypeBoolean, KStringFalse8 ) );
                }
            else if( iNotifyType & EAPP_ID )
                {
                // no app group name, id is known
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuApplication8 ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KIdParam, KKeyTypeInteger, iCRKeyFolderItemUid ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KParentOnly8, KKeyTypeBoolean, KStringTrue8 ) );
                }
            else if( iNotifyType & EAPP_UID )
                {
                // uid is known
                // app group name is optional
                id = HnConvUtils::NumToStr8LC( KRootId );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuFolder8 ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KIdParam, KKeyTypeInteger, *id ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KRecursiveSearch8, KKeyTypeBoolean, KStringTrue8 ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KFlatResult8, KKeyTypeBoolean, KStringTrue8 ) );

                filter = HnMdKeyFactory::CreateL(
                    KFilter8, KKeyTypeMap, KStringTrue8 );
                CleanupStack::PushL( filter );
                filter->AddSubKeyL( HnMdKeyFactory::CreateL( KUid8, KKeyTypeInteger, iCRKeyFolderItemUid ) );
                filter->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuApplication8 ) );
                filter->AddSubKeyL( HnMdKeyFactory::CreateL( KHidden8, KKeyTypeBoolean, KStringFalse8 ) );
                }
            else
                {
                // No app group name, no uid. Search for root.
                id = HnConvUtils::NumToStr8LC( KRootId );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuFolder8 ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KIdParam, KKeyTypeInteger, *id ) );
                inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KParentOnly8, KKeyTypeBoolean, KStringTrue8 ) );
                }

            RPointerArray< CHnMdBaseKey > data;
            CleanupClosePushL( data );
            data.AppendL( inDataKey );
            if ( filter )
                {
                data.AppendL( filter );
                }

            CHnMdBaseKey* serviceData = HnMdKeyFactory::CreateL(
                KServiceContentName, KKeyTypeString, KMatrixMenuData );
            CleanupStack::PushL( serviceData );

            RPointerArray< CHnMdBaseKey> constructor;
            CleanupClosePushL( constructor );
            constructor.AppendL( serviceData );
            CLiwGenericParamList* constructorLiw = CLiwGenericParamList::NewLC();
            CLiwGenericParamList* commandLiw = CLiwGenericParamList::NewLC();
            HnLiwUtils::SetGenericParamListL( constructor, *constructorLiw );
            HnLiwUtils::SetGenericParamListL( data, *commandLiw );

            TBuf8< KMaxLength > interface( KMCSDataSourceInterface8 );

            delete iServiceHandler;
            iServiceHandler = NULL;
            iServiceHandler = CHnServiceHandler::NewL(
                    KMCSService8, interface, KCmdGetList8,
                    EServiceModeAsynchronous, constructorLiw, commandLiw );

            CleanupStack::Pop( commandLiw );
            CleanupStack::Pop( constructorLiw );

            iServiceHandler->ExecuteL( this );

            CleanupStack::PopAndDestroy( &constructor );
            CleanupStack::PopAndDestroy( serviceData );
            CleanupStack::PopAndDestroy( &data );
            if( filter )
                {
                CleanupStack::PopAndDestroy( filter );
                }
            if( id )
                {
                CleanupStack::PopAndDestroy( id );
                }
            CleanupStack::PopAndDestroy( inDataKey );
        }

    DEBUG(("_MM_:CHnRepositoryShowFolderObserver::HandleNotifyStringL OUT"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryShowFolderObserver::ExtractCRKeyShowFolderName( const TDesC& aNewValue )
    {
    DEBUG(( "_MM_:CHnRepositoryShowFolderObserver::ExtractCRKeyShowFolderName IN" ));
    iCRKeyFolderName.Copy( KBlank );
    iCRKeyFolderItemUid.Copy( KBlank );

    int keyValueLength = aNewValue.Length();
    if( keyValueLength )
        {
        TApaAppGroupName folder;
        TBuf<KTimeStampBufferLength + KUidStringLength + 1> tempBuf;
        TBuf<KTimeStampBufferLength> timeStamp;

        TInt ret = aNewValue.Find( KComma );
        if( ( ret == KErrNotFound ) || ( ret == ( keyValueLength - 1 ) ) )
            {
            DEBUG(( "_MM_:CHnRepositoryShowFolderObserver::ExtractCRKeyShowFolderName (bad CR key value input) OUT" ));
            return; //bad CR key value input
            }
        if( ret > KApaMaxAppGroupName )
            {
            return;
            }
        folder.Copy( aNewValue.Left( ret ) );
        if( aNewValue.Length() - ret > tempBuf.MaxLength() )
            {
            return;
            }
        tempBuf.Copy( aNewValue.Mid( ret + 1 ) );

        TInt posUid = tempBuf.Find( KComma );
        if( KErrNotFound != posUid )
            {
            if( posUid > KTimeStampBufferLength )
                {
                return;
                }
            timeStamp.Copy( tempBuf.Left( posUid ) );
            if( tempBuf.Length() - ( posUid + 1 ) > KUidStringLength )
                {
                return;
                }
            iCRKeyFolderItemUid.Copy( tempBuf.Mid( posUid + 1 ) );
            }
        else
            {
            if( tempBuf.Length() > KTimeStampBufferLength )
                {
                return;
                }
            timeStamp.Copy( tempBuf );
            }

        TTime currentTime;
        currentTime.HomeTime();
        TTimeIntervalSeconds interval;

        TTime timeStampTime;
        ret = timeStampTime.Set( timeStamp );

        if( ret == KErrGeneral )
            {
            DEBUG(( "_MM_:CHnRepositoryShowFolderObserver::ExtractCRKeyShowFolderName (bad timestamp) OUT" ));
            return; // bad time stamp value
            }

        ret = currentTime.SecondsFrom( timeStampTime, interval );

        if( interval.Int() < 0 )
            {//negative timestamp is set ahead of current time...!
            DEBUG(( "_MM_:CHnRepositoryShowFolderObserver::ExtractCRKeyShowFolderName (negative interval) OUT" ));
            return;
            }

        if( ( interval.Int() ) > KTimeStampCutOff )
            {//positive timestamp but more than 5 seconds
            DEBUG(( "_MM_:CHnRepositoryShowFolderObserver::ExtractCRKeyShowFolderName (more than 5 seconds) OUT" ));
            return;
            }
        else
            {
            iCRKeyFolderName.Copy( folder );
            }
        }
    DEBUG(( "_MM_:CHnRepositoryShowFolderObserver::ExtractCRKeyShowFolderName OUT" ));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryShowFolderObserver::GetShowFolderL( TUint32 aFolderId )
    {

    CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
        KInData8, KKeyTypeMap, KDefaultParentId8 );
    CleanupStack::PushL( inDataKey );
    HBufC8* id = HnConvUtils::NumToStr8LC( aFolderId );
    inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuFolder8 ) );
    inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KIdParam, KKeyTypeInteger, *id ) );
    inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KParentOnly8, KKeyTypeBoolean, KStringTrue8 ) );

    RPointerArray< CHnMdBaseKey > data;
    CleanupClosePushL( data );
    data.Append( inDataKey );

    CHnMdBaseKey* serviceData = HnMdKeyFactory::CreateL(
        KServiceContentName, KKeyTypeString, KMatrixMenuData );
    CleanupStack::PushL( serviceData );

    RPointerArray< CHnMdBaseKey> constructor;
    CleanupClosePushL( constructor );
    constructor.Append( serviceData );
    CLiwGenericParamList* constructorLiw = CLiwGenericParamList::NewLC();
    CLiwGenericParamList* commandLiw = CLiwGenericParamList::NewLC();
    HnLiwUtils::SetGenericParamListL( constructor, *constructorLiw );
    HnLiwUtils::SetGenericParamListL( data, *commandLiw );

    TBuf8< KMaxLength > interface( KMCSDataSourceInterface8 );

    delete iServiceHandler;
    iServiceHandler = NULL;
    iServiceHandler = CHnServiceHandler::NewL(
            KMCSService8, interface, KCmdGetList8,
            EServiceModeAsynchronous, constructorLiw, commandLiw );

    CleanupStack::Pop( commandLiw );
    CleanupStack::Pop( constructorLiw );

    iServiceHandler->ExecuteL( this );

    CleanupStack::PopAndDestroy( &constructor );
    CleanupStack::PopAndDestroy( serviceData );
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( id );
    CleanupStack::PopAndDestroy( inDataKey );

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryShowFolderObserver::GetShowFolderGroupNameL()
    {
    CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
        KInData8, KKeyTypeMap, KDefaultParentId8 );
    CleanupStack::PushL( inDataKey );
    HBufC8* id = HnConvUtils::NumToStr8LC( KRootId );
    inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuFolder8 ) );
    inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KIdParam, KKeyTypeInteger, *id ) );
    inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KRecursiveSearch8, KKeyTypeBoolean, KStringTrue8 ) );
    inDataKey->AddSubKeyL( HnMdKeyFactory::CreateL( KFlatResult8, KKeyTypeBoolean, KStringTrue8 ) );

    CHnMdBaseKey* filter = HnMdKeyFactory::CreateL(
            KFilter8, KKeyTypeMap, KStringTrue8 );
    CleanupStack::PushL( filter );
    filter->AddSubKeyL( HnMdKeyFactory::CreateL( KMcsAppGroupName8, KKeyTypeString, iCRKeyFolderName ) );
    filter->AddSubKeyL( HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuFolder8 ) );
    filter->AddSubKeyL( HnMdKeyFactory::CreateL( KHidden8, KKeyTypeBoolean, KStringFalse8 ) );

    RPointerArray< CHnMdBaseKey > data;
    CleanupClosePushL( data );
    data.AppendL( inDataKey );
    if ( filter )
        {
        data.AppendL( filter );
        }

    CHnMdBaseKey* serviceData = HnMdKeyFactory::CreateL(
        KServiceContentName, KKeyTypeString, KMatrixMenuData );
    CleanupStack::PushL( serviceData );

    RPointerArray< CHnMdBaseKey> constructor;
    CleanupClosePushL( constructor );
    constructor.Append( serviceData );
    CLiwGenericParamList* constructorLiw = CLiwGenericParamList::NewLC();
    CLiwGenericParamList* commandLiw = CLiwGenericParamList::NewLC();
    HnLiwUtils::SetGenericParamListL( constructor, *constructorLiw );
    HnLiwUtils::SetGenericParamListL( data, *commandLiw );

    TBuf8< KMaxLength > interface( KMCSDataSourceInterface8 );

    delete iServiceHandler;
    iServiceHandler = NULL;
    iServiceHandler = CHnServiceHandler::NewL(
            KMCSService8, interface, KCmdGetList8,
            EServiceModeAsynchronous, constructorLiw, commandLiw );

    CleanupStack::Pop( commandLiw );
    CleanupStack::Pop( constructorLiw );

    iServiceHandler->ExecuteL( this );

    CleanupStack::PopAndDestroy( &constructor );
    CleanupStack::PopAndDestroy( serviceData );
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( id );
    CleanupStack::PopAndDestroy( inDataKey );

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnRepositoryShowFolderObserver::HandleNotifyL(
        TInt /*aCmdId*/,
        TInt aEventId,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& /*aInParamList*/)
    {
    DEBUG(("_MM_:CHnRepositoryShowFolderObserver::HandleNotifyL IN"));
    if( aEventId == KLiwEventCanceled )
        {
        return KErrNotFound;
        }
    else
        {
        TInt pos( 0 );
        const TLiwGenericParam* first = aEventParamList.FindFirst( pos, KReturnValue8() );
        if( pos==KErrNotFound )
            {
            return KErrNotFound;
            }

        TInt count = first->Value().AsList()->Count();
        if( !( iNotifyType & ESECOND_NOTIFY ) ) //first notify
            {
            if( iCRKeyFolderName.Length()==0 )
                {
                // no app group name
                // it must be found using parent id
                if ( count == 0 )
                    {
                    return KErrNotFound;
                    }
                iNotifyType |= ESECOND_NOTIFY;
                TLiwVariant varId; varId.PushL();
                TLiwVariant varParentId; varParentId.PushL();

                HnLiwUtils::GetVariantL( aEventParamList, KIdPath8, 0, varId );
                HnLiwUtils::GetVariantL( aEventParamList, KParentIdPath8, 0, varParentId );

                TInt32 id( 0 );
                TInt32 parentId( 0 );
                varId.Get( id );
                iCRKeyFolderItemUid.Num( id );
                varParentId.Get( parentId );

                CleanupStack::PopAndDestroy( &varParentId );
                CleanupStack::PopAndDestroy( &varId );

                GetShowFolderL( parentId );
                return KErrNone;
                }
            else
                {
                // app group name is known
                if( iNotifyType & EAPP_UID )
                    {
                    // first get list was used to found id for app item
                    iNotifyType |= ESECOND_NOTIFY;
                    if ( count )
                        {
                        // uid was given, id is found
                        // and replaces uid
                        TLiwVariant varId; varId.PushL();
                        HnLiwUtils::GetVariantL( aEventParamList, KIdPath8, 0, varId );
                        TInt32 id( 0 );
                        varId.Get( id );
                        iCRKeyFolderItemUid.Num( id );
                        CleanupStack::PopAndDestroy( &varId );
                        }
                    GetShowFolderGroupNameL();
                    return KErrNone;
                    }
                else
                    {
                    // intentionally empty
                    // no uid was given so only one get list is needed
                    }
                }
            }
            if ( count == 0 )
                {
                return KErrNotFound;
                }

            TLiwVariant varId; varId.PushL();
            TLiwVariant varAppGroupName; varAppGroupName.PushL();
            TLiwVariant varSuiteName; varSuiteName.PushL();
            TLiwVariant varChildrenCount; varChildrenCount.PushL();

            HnLiwUtils::GetVariantL( aEventParamList, KIdPath8, 0, varId );
            HnLiwUtils::GetVariantL( aEventParamList, KAppGroupNamePath8, 0, varAppGroupName );
            HnLiwUtils::GetVariantL( aEventParamList, KTitleNamePath8, 0, varSuiteName );
            HnLiwUtils::GetVariantL( aEventParamList, KChildrenCountPath8, 0, varChildrenCount );

            CLiwGenericParamList* paramList = CLiwGenericParamList::NewL();
            CleanupStack::PushL( paramList );

            CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

            paramList->AppendL( TLiwGenericParam( KSuiteName8,
                    TLiwVariant( KFolderSuite ) ) );

            map->InsertL( KSuiteName8, varSuiteName );
            map->InsertL( KFolderId8, varId );
            map->InsertL( KRemoveLocked8, TLiwVariant( KStringFalse8 ) );
            map->InsertL( KParentFolderId8, varId );
            map->InsertL( KTmpParentFolderId8, varId );
            map->InsertL( KMcsAppGroupName8, varAppGroupName );

            paramList->AppendL( TLiwGenericParam( KParams8, TLiwVariant( map ) ) );

            CleanupStack::PopAndDestroy( map );

            while ( iCmnPtrs->iModel->LoadedSuitesCount() > 1 )
                {
                CHnMdSuite* suite = iCmnPtrs->iModel->GetLastSuite();
                iCmnPtrs->iContainer->PopSuiteModelL( suite->SuiteName() );
                iCmnPtrs->iModel->DeleteLastSuite();
                }

            if ( iCmnPtrs->iContainer->GetLastSuiteModel()->CustomId() != varId.AsTInt64() )
                {
                iCmnPtrs->iModelEventObserver->HandleModelEventL( KNewSuiteLoadedMdEvent, *paramList );
                iCmnPtrs->iModel->QueueForeground( CHnMdModel::EWhenCurrentTopSuiteIsEvaluated );
                }
            else
                {
                iCmnPtrs->iContainer->GetLastSuiteModel()->NotifyObserversL( ESuiteModelInitialized );
                TLex8 lex( iCRKeyFolderItemUid );
                TInt64 id (0);
                TInt err = lex.Val( id );
                TLiwVariant varItemCustomId; varItemCustomId.PushL();
                varItemCustomId.Set( id );
                TLiwVariant varId; varId.PushL();
                varId.Set( iCmnPtrs->iContainer->GetLastSuiteModel()->CustomId() );
                CLiwGenericParamList* pl = CLiwGenericParamList::NewL();
                CleanupStack::PushL( pl );
                pl->AppendL( TLiwGenericParam( KSuiteCustomId8, varId ) );
                pl->AppendL( TLiwGenericParam( KItemCustomId8, varItemCustomId ) );
                iCmnPtrs->iModelEventObserver->HandleModelEventL( KSetFocusEvent, *pl );
                iCmnPtrs->iModelEventObserver->HandleModelEventL( KAppGainForeground, *pl );
                CleanupStack::PopAndDestroy( pl );
                CleanupStack::PopAndDestroy( &varId );
                CleanupStack::PopAndDestroy( &varItemCustomId );

                iCRKeyFolderItemUid.Copy( KBlank );
                }

            iNotifyType = 0;
            iCRKeyFolderName.Copy( KBlank );
            iRepository->Set( iId, KBlank );

            CleanupStack::PopAndDestroy( paramList );
            CleanupStack::PopAndDestroy( &varChildrenCount );
            CleanupStack::PopAndDestroy( &varSuiteName );
            CleanupStack::PopAndDestroy( &varAppGroupName );
            CleanupStack::PopAndDestroy( &varId );
        }

  DEBUG(("_MM_:CHnRepositoryShowFolderObserver::HandleNotifyL OUT"));
  return KErrNone;
  }

// class CHnRepositoryOpenItemObserver
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryOpenItemObserver * CHnRepositoryOpenItemObserver::NewL(
        THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid, const TUint32 aId )
    {
    CHnRepositoryOpenItemObserver * self = CHnRepositoryOpenItemObserver::NewLC(
            aCmnPtrs, aRepositoryUid, aId );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryOpenItemObserver * CHnRepositoryOpenItemObserver::NewLC(
        THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid, const TUint32 aId )
    {
    CHnRepositoryOpenItemObserver * self = new (ELeave) CHnRepositoryOpenItemObserver(
            aCmnPtrs, aId );
    CleanupStack::PushL(self);
    self->ConstructL( aRepositoryUid );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryOpenItemObserver::ConstructL( const TUid aRepositoryUid )
    {
    CHnRepositoryObserver::ConstructL( aRepositoryUid );
    TInt ret = iRepository->Create( iId, KBlank );
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iRepository,
            CCenRepNotifyHandler::EStringKey, iId );
    iNotifyHandler->StartListeningL();
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryOpenItemObserver::CHnRepositoryOpenItemObserver( 
        THnMdCommonPointers* aCmnPtrs, TUint32 aId  ) :
            CHnRepositoryObserver( aCmnPtrs, aId )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnRepositoryOpenItemObserver::~CHnRepositoryOpenItemObserver()
    {
    iRepository->Delete( iId );
    delete iServiceHandler;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryOpenItemObserver::HandleNotifyString(
        TUint32 aId, const TDesC16& aNewValue )
    {
    TRAP_IGNORE( HandleNotifyStringL( aId, aNewValue ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryOpenItemObserver::HandleNotifyStringL(
        TUint32 aId, const TDesC16& aNewValue )
    {
    DEBUG(("_MM_:CHnRepositoryOpenItemObserver::HandleNotifyStringL IN"));
    DEBUG(("\tID: %d : %S", aId, &aNewValue));

    // Open item
    if( aId == iId && aNewValue.Length())
        {
        ShowFolderL( aNewValue );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnRepositoryOpenItemObserver::HandleNotifyL(
        TInt /*aCmdId*/,
        TInt aEventId,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& /*aInParamList*/)
    {
    DEBUG(("_MM_:CHnRepositoryOpenItemObserver::HandleNotifyL IN"));
    if( aEventId == KLiwEventCanceled )
        {
        return KErrNotFound;
        }
    else
        {
        TInt pos( 0 );
        const TLiwGenericParam* first = aEventParamList.FindFirst(
                pos, KReturnValue8() );
        if( pos == KErrNotFound )
            {
            return KErrNotFound;
            }

        TInt count = first->Value().AsList()->Count();
        if( count == 0 )
            {
            return KErrNotFound;
            }

        TLiwVariant varId;
        varId.PushL();
        TLiwVariant varAppGroupName;
        varAppGroupName.PushL();
        TLiwVariant varSuiteName;
        varSuiteName.PushL();
        TLiwVariant varChildrenCount;
        varChildrenCount.PushL();

        HnLiwUtils::GetVariantL( aEventParamList, KIdPath8, 0, varId );
        HnLiwUtils::GetVariantL( aEventParamList, KAppGroupNamePath8, 0, varAppGroupName );
        HnLiwUtils::GetVariantL( aEventParamList, KTitleNamePath8, 0, varSuiteName );
        HnLiwUtils::GetVariantL( aEventParamList, KChildrenCountPath8, 0, varChildrenCount );

        CLiwGenericParamList* paramList = CLiwGenericParamList::NewL();
        CleanupStack::PushL( paramList );

        CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

        paramList->AppendL( TLiwGenericParam( KSuiteName8, TLiwVariant(
                KFolderSuite ) ) );

        map->InsertL( KSuiteName8, varSuiteName );
        map->InsertL( KFolderId8, varId );
        map->InsertL( KRemoveLocked8, TLiwVariant( KStringFalse8 ) );
        map->InsertL( KParentFolderId8, varId );
        map->InsertL( KTmpParentFolderId8, varId );
        map->InsertL( KMcsAppGroupName8, varAppGroupName );

        paramList->AppendL( TLiwGenericParam( KParams8, TLiwVariant( map ) ) );

        CleanupStack::PopAndDestroy( map );

        while( iCmnPtrs->iModel->LoadedSuitesCount() > 1 )
            {
            CHnMdSuite* suite = iCmnPtrs->iModel->GetLastSuite();
            iCmnPtrs->iContainer->PopSuiteModelL( suite->SuiteName() );
            iCmnPtrs->iModel->DeleteLastSuite();
            }

        if( iCmnPtrs->iContainer->GetLastSuiteModel()->CustomId()
                != varId.AsTInt64() )
            {
            iCmnPtrs->iModelEventObserver->HandleModelEventL(
                    KNewSuiteLoadedMdEvent, *paramList );
            iCmnPtrs->iModel->QueueForeground(
                    CHnMdModel::EWhenCurrentTopSuiteIsEvaluated );
            }
        else
            {
            iCmnPtrs->iContainer->GetLastSuiteModel()->NotifyObserversL(
                    ESuiteModelInitialized );
            CLiwGenericParamList* paramList = CLiwGenericParamList::NewL();
            CleanupStack::PushL( paramList );
            iCmnPtrs->iModelEventObserver->HandleModelEventL(
                    KAppGainForeground, *paramList );
            CleanupStack::PopAndDestroy( paramList );
            }

        iRepository->Set( iId, KBlank ); // reset key repository to empty string

        CleanupStack::PopAndDestroy( paramList );
        CleanupStack::PopAndDestroy( &varChildrenCount );
        CleanupStack::PopAndDestroy( &varSuiteName );
        CleanupStack::PopAndDestroy( &varAppGroupName );
        CleanupStack::PopAndDestroy( &varId );
        }

    DEBUG(("_MM_:CHnRepositoryOpenItemObserver::HandleNotifyL OUT"));
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnRepositoryOpenItemObserver::ShowFolderL( const TDesC16& aFolderId )
    {
    CHnMdBaseKey* inDataKey = HnMdKeyFactory::CreateL(
            KInData8, KKeyTypeMap, KDefaultParentId8 );
    CleanupStack::PushL( inDataKey );
    inDataKey->AddSubKeyL(
            HnMdKeyFactory::CreateL( KType8, KKeyTypeString, KMenuFolder8 ) );
    inDataKey->AddSubKeyL(
            HnMdKeyFactory::CreateL( KIdParam, KKeyTypeInteger, aFolderId ) );
    inDataKey->AddSubKeyL(
            HnMdKeyFactory::CreateL( KParentOnly8, KKeyTypeBoolean, KStringTrue8 ) );

    RPointerArray< CHnMdBaseKey > data;
    CleanupClosePushL( data );
    data.Append( inDataKey );

    CHnMdBaseKey* serviceData = HnMdKeyFactory::CreateL(
        KServiceContentName, KKeyTypeString, KMatrixMenuData );
    CleanupStack::PushL( serviceData );

    RPointerArray< CHnMdBaseKey> constructor;
    CleanupClosePushL( constructor );
    constructor.Append( serviceData );
    CLiwGenericParamList* constructorLiw = CLiwGenericParamList::NewLC();
    CLiwGenericParamList* commandLiw = CLiwGenericParamList::NewLC();
    HnLiwUtils::SetGenericParamListL( constructor, *constructorLiw );
    HnLiwUtils::SetGenericParamListL( data, *commandLiw );

    TBuf8< KMaxLength > interface( KMCSDataSourceInterface8 );

    delete iServiceHandler;
    iServiceHandler = NULL;
    iServiceHandler = CHnServiceHandler::NewL(
            KMCSService8, interface, KCmdGetList8,
            EServiceModeAsynchronous, constructorLiw, commandLiw );

    CleanupStack::Pop( commandLiw );
    CleanupStack::Pop( constructorLiw );

    iServiceHandler->ExecuteL( this );

    CleanupStack::PopAndDestroy( &constructor );
    CleanupStack::PopAndDestroy( serviceData );
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( inDataKey );

    }

// End of File
