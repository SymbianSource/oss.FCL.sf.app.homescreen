/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Default Screensaver runtime.
*
*/

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneResizeEvent>
#include <QPainter>
#include <QDebug>
#include <QServiceManager>
#include <QServiceFilter>
#include <QServiceInterfaceDescriptor>
#include <XQSettingsManager>
#include <HbMainWindow>
#include <HbIndicatorInterface>

#include <screensaver.h>
#include <screensaverdomaincrkeys.h>
#include "snsrdevicedialog.h"

const char *gBigClockInterfaceName = "com.nokia.screensaver.ISnsrBigClockScreensaver";

/*!
    \class SnsrDeviceDialog
    \ingroup group_snsrdevicedialogplugin
    \brief Screensaver Device Dialog.
 */

extern const char *lViewType;

const char *SnsrDeviceDialog::dataKeyUnlock = "unlock";
const char *SnsrDeviceDialog::dataKeySwitchLights = "switch_lights";
const char *SnsrDeviceDialog::dataKeySwitchLowPower = "switch_low_power";

QTM_USE_NAMESPACE

/*!
    Constructor.
    \param viewType Initial view type (active or standby).
    \param parent Parent.
 */
SnsrDeviceDialog::SnsrDeviceDialog(const QVariantMap &parameters, QGraphicsItem *parent) :
        HbPopup(parent), 
        mScreensaver(0), 
        mLayout(0), 
        m_setManager(0)
{
    qDebug("SnsrDeviceDialog::SnsrDeviceDialog()");

    
    // load screensaver plugin
    // Workaround to QtSF bug. Create in stack after the bug is fixed.
    QServiceManager manager;
    QServiceFilter filter(gBigClockInterfaceName);
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(
        filter);
    Q_ASSERT(interfaces.count());

    mScreensaver = manager.loadLocalTypedInterface<Screensaver>(
        interfaces.first().interfaceName() );
    if (mScreensaver) {
        mScreensaver->setParent(this);
    } else {
        // TODO: error state
    }
    connect(mScreensaver, SIGNAL(viewChanged(QGraphicsWidget*)),
        SLOT(changeView(QGraphicsWidget*)));
    connect(mScreensaver, SIGNAL(faulted()),
        SLOT(screensaverFaulted()));
    connect( mScreensaver, SIGNAL(unlockRequested()),
        SLOT(requestUnlock()) );
    connect( mScreensaver, SIGNAL(screenPowerModeRequested(Screensaver::ScreenPowerMode)),
        SLOT(requestScreenMode(Screensaver::ScreenPowerMode)) );
    
    mScreensaver->initialize();

    setBackgroundItem(0);

    setDismissPolicy(HbPopup::NoDismiss);
    setTimeout(HbPopup::NoTimeout);
    
    mainWindow()->setAutomaticOrientationEffectEnabled(false);

    setDeviceDialogParameters( parameters );
}

/*!
    Destructor.
 */
SnsrDeviceDialog::~SnsrDeviceDialog()
{
    qDebug("SnsrDeviceDialog::~SnsrDeviceDialog()");
    QServiceManager serviceManager;
    serviceManager.removeService(gBigClockInterfaceName);
}

/*!
    Sets device dialog parameters.
    \param parameters Device Dialog's parameters to be set.
    \ret return true if parameters are correct.
 */
bool SnsrDeviceDialog::setDeviceDialogParameters(const QVariantMap &parameters)
{
    qDebug("SnsrDeviceDialog::setDeviceDialogParameters()");

    // default to initial state if no other state given
    int viewType( ViewTypeInitial );
        
    QVariantMap::const_iterator it = parameters.find(lViewType);
    if (it != parameters.end() && !it.value().isNull() && it.value().isValid()) {
        bool ok(false);
        viewType = it.value().toInt(&ok);
        if ( !ok ) {
            qWarning() << "SnsrDeviceDialog::setDeviceDialogParameters() - int cast failed";
            return false;
        }
    }
        
    // Check initial view from repository
    if (viewType == ViewTypeInitial ) {
        XQSettingsManager::Error error;
        int startupView = 0; 
        XQCentralRepositorySettingsKey settingsKey(
                 KCRUidScreensaverSettings.iUid, KScreensaverStartupView ); // TUid as same repository used in control panel via Symbian APIs 
        m_setManager = new XQSettingsManager(this);
        if (m_setManager) {
            startupView = m_setManager->readItemValue(settingsKey, XQSettingsManager::TypeInt).toInt();
            error = m_setManager->error();
            if (error == XQSettingsManager::NoError) {
                viewType = startupView;
            }
            delete m_setManager;
        }
    }

    switch (viewType) {
    case SnsrDeviceDialog::ViewTypeActive:
        mScreensaver->foreground();
        return true;
    case SnsrDeviceDialog::ViewTypeStandby:
        mScreensaver->partialForeground();
        return true;
    default:
        qWarning() << QString("SnsrDeviceDialog::setDeviceDialogParameters() "
          "- unspecefied view type: %1").arg(viewType);
    }
    
    return false;
}

/*!
    Returns error code.
    \ret error code.
 */
int SnsrDeviceDialog::deviceDialogError() const
{
    return 0;
}

/*!
    Closes device dialog.
    \param byClient If dialog is close by client.
 */
void SnsrDeviceDialog::closeDeviceDialog(bool byClient)
{
    qDebug("SnsrDeviceDialog::closeDeviceDialog()");
    Q_UNUSED(byClient)
    hide();
    emit deviceDialogClosed();
}

/*!
    Returns device dialog instance.
    \ret Device dialog instance.
 */
HbPopup *SnsrDeviceDialog::deviceDialogWidget() const
{
    return const_cast<SnsrDeviceDialog*>(this);
}

/*!
    Shows widget.
    \param event.
 */
void SnsrDeviceDialog::showEvent(QShowEvent *event)
{
    qDebug("SnsrDeviceDialog::showEvent()");

    HbPopup::showEvent(event);
    
    // workaround for duplicating connections and emiting two signals
    // showEvent is called for HbDeviceDialogManager showDeviceDialog()
    // and for HbPopup itemChange(), both events are set to QEvent::Show type
    disconnect(mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
        this, SLOT(changeLayout(Qt::Orientation)));
    connect(mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
        this, SLOT(changeLayout(Qt::Orientation)));

    changeLayout(mainWindow()->orientation());
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Hides widget.
    \param event.
 */
void SnsrDeviceDialog::hideEvent(QHideEvent *event)
{
    qDebug("SnsrDeviceDialog::hideEvent()");

    HbPopup::hideEvent(event);
    mScreensaver->background();
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Close widget.
    \param event.
 */
void SnsrDeviceDialog::closeEvent(QCloseEvent *event)
{
    qDebug("SnsrDeviceDialog::closeEvent()");

    HbPopup::closeEvent(event);

    disconnect(mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)), 
        this, SLOT(changeLayout(Qt::Orientation)));
    mScreensaver->close();
}

/*!
    Replaces layout's item.
    \param widget New item to be added to layout.
 */
void SnsrDeviceDialog::changeView(QGraphicsWidget *widget)
{
    qDebug("SnsrDeviceDialog::changeView() - widget: 0x%X", (int)widget);
    if (!mLayout) {
        mLayout = new QGraphicsLinearLayout(Qt::Vertical);
        mLayout->setContentsMargins(0,0,0,0);
        setLayout(mLayout);
    }
    
    if (mLayout->count()) {
        mLayout->removeAt(0);
    }
    if (widget) {
        widget->show();
        mLayout->addItem(widget);

        if ( isVisible() ) {
           changeLayout( mainWindow()->orientation() );
        }
    }
    

}

/*!
    Handles faults in loaded screensaver model.
 */
void SnsrDeviceDialog::screensaverFaulted()
{
    qWarning() << "SnsrDeviceDialog::screensaverFaulted()";
    // TODO: some error handling
}

/*!
    Resize device dialog.
    \param orientation New orientation value.
 */
void SnsrDeviceDialog::changeLayout(Qt::Orientation orientation)
{
    Q_UNUSED(orientation)
    
    QRectF rect = mainWindow()->layoutRect();
    setMinimumSize( rect.size() );
    setPreferredPos( QPointF(0,0) );
    QGraphicsSceneResizeEvent sceneResize;
    sceneResize.setNewSize( rect.size() );
    event( &sceneResize );
    mScreensaver->updateLayout();
}

/*!
    Send unlock signal to autolock.
 */
void SnsrDeviceDialog::requestUnlock()
{
    QVariantMap data;
    data.insert(dataKeyUnlock, 1);
    emit deviceDialogData(data);
}

/*!
    Send low power mode on/off request to autolock.
 */
void SnsrDeviceDialog::requestScreenMode(Screensaver::ScreenPowerMode mode)
{
    /* TEMPORARY FIX for the "black screen jam" bug. To be re-enabled when proper fix is found.
    QVariantMap data;
    QVariantList rowLimits;
    if ( mode == Screensaver::ScreenModeOff ) {
        data.insert(dataKeySwitchLights, 0);
        data.insert(dataKeySwitchLowPower, rowLimits); // empty list means "low power off"
    }
    else if ( mode == Screensaver::ScreenModeLowPower ) {
        //data.insert(dataKeySwitchLights, 0);
        int firstRow;
        int lastRow;
        mScreensaver->getActiveScreenRows(&firstRow, &lastRow);
        rowLimits.append( firstRow );
        rowLimits.append( lastRow );
        data.insert(dataKeySwitchLowPower, rowLimits);
    }
    else if ( mode == Screensaver::ScreenModeFullPower ) {
        data.insert(dataKeySwitchLights, 30);
        data.insert(dataKeySwitchLowPower, rowLimits); // empty list means "low power off"
    }
    
    emit deviceDialogData(data);
    */
}

/*!
    Called when the dialog is created if they are any currently
    activate universal indicators present.
 */
void SnsrDeviceDialog::indicatorsActivated(
        const QList<HbIndicatorInterface*> &activatedIndicators)
{
    mScreensaver->handleActiveIndicators(activatedIndicators);
}

/*!
    Called when some universal indicator is activated.
    Ownership of activatedIndicator is not transferred.
 */
void SnsrDeviceDialog::indicatorActivated(
        HbIndicatorInterface *activatedIndicator)
{
    mScreensaver->handleActivatedIndicator(activatedIndicator);
}

/*!
    Called when some universal indicator is deactivated.
 */
void SnsrDeviceDialog::indicatorRemoved(
        HbIndicatorInterface *indicatorRemoved)
{
    mScreensaver->handleDeactivatedIndicator(indicatorRemoved);
}

// end of file

