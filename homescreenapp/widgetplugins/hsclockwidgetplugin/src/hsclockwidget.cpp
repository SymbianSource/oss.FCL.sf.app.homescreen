/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Clock widget
*
*/

#include <QtGui>
#include <QGraphicsWidget>
#include <QSizePolicy>
#include <QGraphicsLinearLayout>
#include <hblabel.h>
#include <hbextendedlocale.h>
#include <hbdeviceprofile.h> // temp

#include "hsclockwidget.h"
#include "hsanalogclockwidget.h"
#include "hsdigitalclockwidget.h"

#ifdef Q_OS_SYMBIAN    
#include "hsclockwidgettype_symbian.h"
#include <apgtask.h>
#include <eikenv.h>
#endif //Q_OS_SYMBIAN

namespace
{
    const char ANALOG[] = "analog";
    const char DIGITAL[] = "digital";

    const int clockUpdateInterval = 1000; // msec
}

#ifdef Q_OS_SYMBIAN
#define KClockAppUid TUid::Uid(0x10005903)
_LIT (KClockAppExe, "clock.exe");
#endif //Q_OS_SYMBIAN

/*!
    \class HsClockWidget
    \ingroup group_hsclockwidgetplugin
    \brief Implementation for the homescreen clock widget.

*/


/*!
    \fn HsClockWidget::HsClockWidget(QGraphicsItem *parent, Qt::WindowFlags flags)

    Constructs widget.
*/
HsClockWidget::HsClockWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mTimer(0),
      mClockType(ANALOG),
      mWidget(0),
      mLayout(0),
      mWidgetShown(false)
{
#if 0
#ifdef Q_OS_SYMBIAN    
    HsClockWidgetType *clockType = new HsClockWidgetType(this);
    mClockType=clockType->type();
    connect(clockType, SIGNAL(typeChanged(QString)), this, SLOT(onTypeChanged(QString)));
#endif    
#endif
}

/*!
    \fn HsClockWidget::~HsClockWidget()

    Destructor.
*/
HsClockWidget::~HsClockWidget()
{
}

/*!
    Returns the clock type.
*/
QString HsClockWidget::clockType() const
{
    return mClockType;
}
  
/*!
    Sets the clock type;
*/
void HsClockWidget::setClockType(const QString &type)
{
    if (type == DIGITAL) {
        mClockType = DIGITAL;
    } else {
        mClockType = ANALOG;
    }
}

/*!
    \fn void HsClockWidget::onInitialize()

    Initializes clock widget
*/
void HsClockWidget::onInitialize()
{
    mLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mLayout->setContentsMargins(0,0,0,0);

    mWidget = loadClockWidget();
    mLayout->addItem(mWidget);
    setPreferredSize(mWidget->preferredSize());
    parentWidget()->resize(preferredSize()); // workaround for layouting
    
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(updateTime()));
    setLayout(mLayout);
}

/*!
    \fn void HsClockWidget::show()

    Shows the widget
*/
void HsClockWidget::onShow()
{
    mWidgetShown = true;
    mTimer->start(clockUpdateInterval);
}


/*!
    \fn void HsClockWidget::show()

    Hides the widget
*/
void HsClockWidget::onHide()
{
    mWidgetShown = false;
    mTimer->stop();
}

/*!
    Uninitializes the widget.
*/
void HsClockWidget::onUninitialize()
{
    mTimer->stop();
}

/*!
    \fn void HsClockWidget::updateTime()

    Draws the clock with every second
*/
void HsClockWidget::updateTime()
{
    if (mClockType == DIGITAL) {
        hide(); // workaround for clock not updating
        static_cast<HbLabel*>(mWidget)->setPlainText(
            HbExtendedLocale().format(QTime::currentTime(), r_qtn_time_usual_with_zero));
        show(); // workaround for clock not updating 
    } else {
        static_cast<HsAnalogClockWidget*>(mWidget)->tick();
    }
}

/*!
    Toggles the clock type.
*/
void HsClockWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!contains(event->pos())) {
        return;
    }

#ifndef Q_OS_SYMBIAN    
    mTimer->stop();
    toggleClockType();
    emit setPreferences(QStringList() << "clockType");

    hide();

    mLayout->removeItem(mWidget);
    delete mWidget;
    mWidget = 0;
    mWidget = loadClockWidget();
    mLayout->addItem(mWidget);
    setPreferredSize(mWidget->preferredSize());
    parentWidget()->resize(preferredSize());
    
    show();
    updateTime();
    update();

    mTimer->start(clockUpdateInterval);
#else //Q_OS_SYMBIAN
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());    
    TApaTask task = taskList.FindApp(KClockAppUid);    
    if (task.Exists()){
        task.BringToForeground();
    }    
    else {
        RProcess process;
        TInt error = process.Create(KClockAppExe, KNullDesC, EOwnerThread);
    
        if (error == KErrNone){
            // start the process running.
            process.Resume();
            process.Close();
        }
    }
#endif //Q_OS_SYMBIAN   
}

  
/*!
    Toggles the clock type.
*/
void HsClockWidget::toggleClockType()
{
    if (mClockType == ANALOG) {
        mClockType = DIGITAL;
    } else {
        mClockType = ANALOG;
    }
}

/*!
    Loads the digital or analog clock widget.
*/
HbWidget *HsClockWidget::loadClockWidget()
{
    HbWidget *clockWidget = 0;

    if (mClockType == DIGITAL) {
        clockWidget = new HsDigitalClockWidget();
        clockWidget->setBackgroundItem(HbStyle::P_Fade_background);
    } else {
        clockWidget = new HsAnalogClockWidget();
    }

    qreal unit = HbDeviceProfile::current().unitValue(); 
    clockWidget->setPreferredSize(QSizeF(25 * unit, 25 * unit)); // TODO: temp workaround

    return clockWidget;
}

void HsClockWidget::onTypeChanged(QString type)
{
  Q_UNUSED(type);
#if 0
    if (mClockType != type) {
        mTimer->stop();
        mClockType = type;
        emit setPreferences(QStringList() << "clockType");
        hide();
        mLayout->removeItem(mWidget);
        delete mWidget;
        mWidget = 0;
        mWidget = loadClockWidget();
        mLayout->addItem(mWidget);
        setPreferredSize(mWidget->preferredSize());
        parentWidget()->resize(preferredSize());
        if ( mWidgetShown ) {
            mTimer->start(clockUpdateInterval);
            show();
            }
    }    
#endif    
}
