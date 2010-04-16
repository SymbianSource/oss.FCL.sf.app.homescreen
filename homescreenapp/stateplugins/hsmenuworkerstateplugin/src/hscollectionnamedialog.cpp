/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Menu name collection state
 *
 */

#include <hbaction.h>
#include <hbinputdialog.h>
#include <hblineedit.h>
#include <hbvalidator.h>
#include <hsmenuservice.h>
#include <QDebug>

#include "hscollectionnamedialog.h"

/*!
 \class HsCollectionNameDialog
 \ingroup group_hsmenustateplugin
 \brief Rename Collection State.
 Collection name state
 */

/*!
 Maximum length if collection name
 */
const int qMaxStrLength = 248;

/*!
 Constructor
 \param parent owner
 \retval void
 */
HsCollectionNameDialog::HsCollectionNameDialog(const int &itemId)
{
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::HsInputDialog");
    mCollectionsNames = HsMenuService::getCollectionNames();
    setInputMode(HbInputDialog::TextInput);
    setPromptText(hbTrId("txt_applib_title_collection_name"));
    lineEdit()->setMaxLength(qMaxStrLength);
    if (itemId) {
        setValue(HsMenuService::getName(itemId));
        mCollectionsNames.removeOne(value().toString());
    } else {
        setValue(hbTrId("txt_applib_dialog_entry_collection"));
    }
    HSMENUTEST_FUNC_EXIT("HsInputDialog::HsInputDialog");
}

/*!
 Destructor
 */
HsCollectionNameDialog::~HsCollectionNameDialog()
{
    mCollectionsNames.clear();
    mLastCollectionName.clear();
}

/*!
 Gets new collection's name.
 \param item_id Item's id.
 \retval Selected action.
 */
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP // Reason: Modal inputdialog exec
#endif //COVERAGE_MEASUREMENT
HbAction *HsCollectionNameDialog::exec()
{
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::exec");
    onTextChanged(value().toString());
    makeConnect();

    HbAction *action = HbInputDialog::exec();

    makeDisconnect();
    HSMENUTEST_FUNC_EXIT("HsInputDialog::exec");
    return action;
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP // Reason: Modal inputdialog exec
#endif //COVERAGE_MEASUREMENT
/*!
 Gets new collection's name.
 \param name name of collection.
 \param addLastName true
 if last found name is need to add to validation
 \retval new collection name.
 */
QString HsCollectionNameDialog::newName(const QString &name,
                                        bool addLastName)
{
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::newName");
    QString newName(name);
    int numToAppend(1);
    if (addLastName && mLastCollectionName.length() > 0
            && !mCollectionsNames.contains(mLastCollectionName)) {
        mCollectionsNames << mLastCollectionName;
    }
    while (mCollectionsNames.contains(newName)) {
        newName = name;
        newName.append("(");
        if (numToAppend < 10) {
            newName.append("0");
        }
        newName.append(QString::number(numToAppend));
        newName.append(")");
        numToAppend++;
        if (!addLastName) {
            if (mLastCollectionName.length() > 0
                    && !mCollectionsNames.contains(mLastCollectionName)) {
                mCollectionsNames << mLastCollectionName;
            }
            mCollectionsNames.removeOne(name);
            mLastCollectionName = name;
        }
    }
    HSMENUTEST_FUNC_EXIT("HsInputDialog::newName");
    return newName;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsCollectionNameDialog::makeConnect()
{
    /*connect(lineEdit(), SIGNAL(textChanged(QString&text)),
     SLOT(onTextChanged(QString&text)));*/

    connect(lineEdit(), SIGNAL(contentsChanged()),
            SLOT(onContentsChanged()));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsCollectionNameDialog::makeDisconnect()
{
    /*disconnect(lineEdit(), SIGNAL(textChanged(QString&text)),
     this, SLOT(onTextChanged(QString&text)));*/
    disconnect(lineEdit(), SIGNAL(contentsChanged()),
               this, SLOT(onContentsChanged()));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsCollectionNameDialog::onTextChanged(const QString &text)
{
    qDebug() << QString("HsInputDialog::onTextChanged( %1 )").arg(text);
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::onTextChanged");
    if (text.trimmed() == "") {
        primaryAction()->setEnabled(false);
    } else {
        primaryAction()->setEnabled(true);
    }

    QString newText = newName(text);
    if (newText != text) {
        makeDisconnect();
        lineEdit()->setText(newText);
        lineEdit()->setSelection(text.length(), newText.length()
                                 - text.length());
        makeConnect();
    }
    HSMENUTEST_FUNC_EXIT("HsInputDialog::onTextChanged");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsCollectionNameDialog::onContentsChanged()
{
    qDebug("HsInputDialog::onContentsChanged()");
    onTextChanged(value().toString());
}
