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
* Description:
*
*/

#include <HbMessageBox>
#include <HbLabel>
#include <HbAction>
#include "hsmessageboxwrapper.h"

/*!
    \class HsMessageBoxWrapper
    \ingroup group_hsutils
    \brief 
*/


struct HsMessageBoxWrapperImpl{
	 QString mHeader;
   QString mQueryText;

};
/*!

*/
HsMessageBoxWrapper::HsMessageBoxWrapper(QObject *parent)
  : QObject(parent),mImpl(new HsMessageBoxWrapperImpl)
{
}

/*!

*/
HsMessageBoxWrapper::~HsMessageBoxWrapper()
{
  delete mImpl;
}

/*!

*/
void HsMessageBoxWrapper::setHeader(const QString &header)
{
    mImpl->mHeader = header;
}
/*!

*/
void HsMessageBoxWrapper::setQueryText(const QString &queryText)
{
    mImpl->mQueryText = queryText;
}
/*!

*/
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif 

void HsMessageBoxWrapper::show()
{
    HbMessageBox *box = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setHeadingWidget(new HbLabel(mImpl->mHeader));
    box->setText(mImpl->mQueryText);
    box->setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);

    box->open(this,SLOT(onDialogClosed(int)));
}
/*!

*/
void HsMessageBoxWrapper::onDialogClosed(int action)
{
    if (action == HbMessageBox::Yes){
       emit accepted();  
    } else {
       emit rejected();
    }
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
