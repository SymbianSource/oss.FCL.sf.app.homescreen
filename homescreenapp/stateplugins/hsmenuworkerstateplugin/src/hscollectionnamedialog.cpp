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
 \ingroup group_hsworkerstateplugin
 \brief Rename Collection State.
 Collection name state
 */

/*!
 \var HsCollectionNameDialog::mCollectionsNames
 Collections names list from data model.
 */

/*
 \var HsCollectionNameDialog::mLastCollectionName
 Last found collection name.
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
void HsCollectionNameDialog::open(QObject* receiver, const char* member)
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionNameDialog::open");
    this->setAttribute(Qt::WA_DeleteOnClose);

    onTextChanged(value().toString());
    makeConnect();

    HbInputDialog::open(receiver, member);

    HSMENUTEST_FUNC_EXIT("HsCollectionNameDialog::open");
}

/*!
    \reimp 
    Disconnects signals and calls base class impl. which emits finished(HbAction*) 
 */
void HsCollectionNameDialog::closeEvent ( QCloseEvent * event )
{
    qDebug("HsCollectionNameDialog::closeEvent");
    makeDisconnect();
    HbDialog::closeEvent(event); // emits finished(HbAction*) 
}

/*!
 * Gets new collection's name.
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

/*!
 Connects edit line signals to slots.
 */
void HsCollectionNameDialog::makeConnect()
{
    /*connect(lineEdit(), SIGNAL(textChanged(QString&text)),
     SLOT(onTextChanged(QString&text)));*/

    connect(lineEdit(), SIGNAL(contentsChanged()),
            SLOT(onContentsChanged()));
}

/*!
 Disconnects edit line signals from slots.
 */
void HsCollectionNameDialog::makeDisconnect()
{
    /*disconnect(lineEdit(), SIGNAL(textChanged(QString&text)),
     this, SLOT(onTextChanged(QString&text)));*/
    disconnect(lineEdit(), SIGNAL(contentsChanged()),
               this, SLOT(onContentsChanged()));
}

/*!
 This slot is received whenever the text changes.
 This slot is also received when the text is
 changed programmatically,
 for example, by calling setText().
 \param text the new text.
 */
void HsCollectionNameDialog::onTextChanged(const QString &text)
{
    qDebug() << QString("HsInputDialog::onTextChanged( %1 )").arg(text);
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::onTextChanged");
    if (text.trimmed() == "" ) {
		actions()[0]->setEnabled(false);
    } else {
        actions()[0]->setEnabled(true);
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

/*!
 This slot is received whenever the contents changes.
 */
void HsCollectionNameDialog::onContentsChanged()
{
    qDebug("HsInputDialog::onContentsChanged()");
    onTextChanged(value().toString());
}
