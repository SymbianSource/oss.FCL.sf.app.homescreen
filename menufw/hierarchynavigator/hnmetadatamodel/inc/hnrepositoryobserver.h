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


#ifndef HNREPOSITORYOBSERVER_H_
#define HNREPOSITORYOBSERVER_H_

#include <liwcommon.h>
#include <e32base.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include "hnmdmodel.h"
#include "hnservicehandler.h"
#include "hnsuiteobserver.h"

/**
 * Repository Observer base class.
 *
 * It is used to be derived by repository observers.
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnRepositoryObserver ):
    public CBase,
    public MCenRepNotifyHandlerCallback
  {
public:

    /**
     * Default C++ Destructor.
     *
     * @since S60 v5.0
     */
    virtual ~CHnRepositoryObserver();


protected:

    /**
     * Defautlt C++ Constructor.
     *
     * @param aCmnPtrs Common pointers.
     * @since S60 v5.0
     */
    CHnRepositoryObserver( THnMdCommonPointers* aCmnPtrs );


    /**
     * Defautlt C++ Constructor.
     *
     * @param aCmnPtrs Common pointers.
     * @since S60 v5.0
     */
    CHnRepositoryObserver( THnMdCommonPointers* aCmnPtrs, TUint32 iId );

    /**
     * Second stage constructor.
     *
     * @since S60 v5.0
     */
    void ConstructL( const TUid aRepositoryUid );

protected:

    /**
     * Own - Central Repository access class.
     */
    CRepository *iRepository;

    /**
     * Id of the CR entry to be watched.
     */
    TUint32 iId;

    /**
     * Common pointers.
     */
    THnMdCommonPointers* iCmnPtrs;

    /**
     * Own - Central repository notification handler.
     */
    CCenRepNotifyHandler* iNotifyHandler;

  };

/**
 * Widget Type Repository Observer.
 *
 * It is used to inform hierarchy navigator of changes in the suites repository.
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS( CHnRepositoryWidgetTypeObserver ): public CHnRepositoryObserver
  {
public:

    /**
     * Default C++ Destructor.
     *
     * @since S60 v5.0
     */
    virtual ~CHnRepositoryWidgetTypeObserver();

    /**
     * Factory function.
     *
     * @param aCmnPtrs Common pointers.
     * @return Repository Observer.
     * @since S60 v5.0
     */
    static CHnRepositoryWidgetTypeObserver* NewL( THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid  );

    /**
     * Factory function.
     *
     * @param aCmnPtrs Common pointers.
     * @return Repository Observer.
     * @since S60 v5.0
     */
    static CHnRepositoryWidgetTypeObserver* NewLC( THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid  );

    /**
     * Change handler method.
     * It is invoked whenever notifier detects a change in the CR.
     *
     * @param aId Id of the entry in CR.
     * @since S60 v5.0
     */
    void HandleNotifyGeneric(TUint32 aId);

    /**
     * Change handler method.
     * It is invoked whenever notifier detects a change in the CR.
     *
     * @param aId Id of the entry in CR.
     * @since S60 v5.0
     */
    void HandleNotifyGenericL(TUint32 aId);

private:

    /**
     * Defautlt C++ Constructor.
     *
     * @param aCmnPtrs Common pointers.
     * @since S60 v5.0
     */
  CHnRepositoryWidgetTypeObserver( THnMdCommonPointers* aCmnPtrs );

private:

    /**
     * Second stage constructor.
     *
     * @since S60 v5.0
     */
  void ConstructL( const TUid aRepositoryUid );

private:

  /**
   * Array holding ids of widgets being switched.
   * It is used to eliminate double refresh for such suites.
   */
  RArray<TUint32> iWidgetSwitches;

  };



NONSHARABLE_CLASS( CHnRepositoryShowFolderObserver ): 	public CHnRepositoryObserver,
                            public MLiwNotifyCallback
  {
public:

    /**
     * Default C++ Destructor.
     *
     * @since S60 v5.0
     */
    virtual ~CHnRepositoryShowFolderObserver();

    /**
   * Factory function.
   *
   * @param aCmnPtrs Common pointers.
   * @return Repository Observer.
   * @since S60 v5.0
   */
  static CHnRepositoryShowFolderObserver* NewL(
      THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid,
      const TUint32 aId);

  /**
   * Factory function.
   *
   * @param aCmnPtrs Common pointers.
   * @return Repository Observer.
   * @since S60 v5.0
   */
  static CHnRepositoryShowFolderObserver* NewLC(
      THnMdCommonPointers* aCmnPtrs, const TUid aRepositoryUid,
      const TUint32 aId);

    /**
     * Change handler method.
     * It is invoked whenever notifier detects a change in the CR.
     *
     * @param aId Id of the entry in CR.
     * @since S60 v5.0
     */
    void HandleNotifyString(TUint32 aId, const TDesC16& aNewValue );

    /**
     * Change handler method.
     * It is invoked whenever notifier detects a change in the CR.
     *
     * @param aId Id of the entry in CR.
     * @since S60 v5.0
     */
    void HandleNotifyStringL(TUint32 aId, const TDesC16& aNewValue );


    /**
     * From MLiwNotifyCallback
     */
    TInt HandleNotifyL(
        TInt aCmdId,
        TInt aEventId,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& aInParamList);

private:

    /**
     * Defautlt C++ Constructor.
     *
     * @param aCmnPtrs Common pointers.
     * @since S60 v5.0
     */
  CHnRepositoryShowFolderObserver( THnMdCommonPointers* aCmnPtrs, TUint32 iId  );

    /**
     * Extracts the folder name from the CR key.
     *
     * @param aNewValue The CR key.
     * @since S60 v5.0
     */
  void ExtractCRKeyShowFolderName( const TDesC& aNewValue );

  /**
   * Request get list for parent folder of an application.
   *
   * @param aFolderId A parent folder id
   * @since S60 v5.0
   */
    void GetShowFolderL( TUint32 aFolderId );

    /**
     * Request get list for a folder with given application group name
     * contained in iCRKeyFolderName.
     *
     * @since S60 v5.0
     */
    void GetShowFolderGroupNameL();

private:
    /**
     * Type of a flag describing the type of notification.
     * APP_ID - MCS id is given
     * APP_UID - Application UID is given
     * SECOND_NOTIFY - handled notify caused by the second get list
     */
    enum TNotifyType { EAPP_ID=1, EAPP_UID=2, ESECOND_NOTIFY=4 };

private:

    /**
     * Second stage constructor.
     *
     * @since S60 v5.0
     */
  void ConstructL( const TUid aRepositoryUid );

private:

  /**
     * Folder application group name.
     */
  TBuf8<KApaMaxAppGroupName> iCRKeyFolderName;

    /**
     * Mcs id for an application item in a folder with a given app group name.
     */
  TBuf8<KUidStringLength> iCRKeyFolderItemUid;

  /**
     * Own.
     * Intance of the service handler.
     */
    CHnServiceHandler* iServiceHandler;

    /**
       * Flag describing the type of notification.
     */
    TInt iNotifyType;
  };

#endif /*HNREPOSITORYOBSERVER_H_*/

//End of file

