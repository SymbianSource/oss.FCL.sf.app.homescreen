#include <QDebug>
#include "hscontentpublishhandler.h"
#include "hsipcchannelhost.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswidgetdata.h"
#include "hswidgethost.h"

HsContentPublishHandler::HsContentPublishHandler(
    HsIpcChannelHost *channel, QObject *parent)
    : QObject(parent),mChannel(channel)
{
    connect(mChannel,SIGNAL(disconnected()),SLOT(deleteLater()));
    connect(mChannel, SIGNAL(messageReceived(const QVariantMap&)), 
        SLOT(onMessageReceived(const QVariantMap&)),Qt::QueuedConnection);
    qDebug() << "HsContentPublishHandler::HsContentPublishHandler" << (int)this;
    
}

HsContentPublishHandler::~HsContentPublishHandler()
{
    qDebug() << "HsContentPublishHandler::~HsContentPublishHandler" << (int)this;
    delete mChannel;
}

void HsContentPublishHandler::onMessageReceived(const QVariantMap &message)
{
    qDebug() << "HsContentPublishHandler message: " << message;

    QString messageType = message.value("messageType").toString();
    
    if(messageType == "addWidget"){
        QString uri = message.value("uri").toString();
        QVariantMap preferences = message.value("preferences").toMap();

        QString result("true");
        if(!addWidget(uri,preferences)){
            result = "false";
        }
        
        // write response
        QVariantMap response;
        response.insert("messageType", "response");
        response.insert("result", result);
        response.insert("request", message);
        if(!mChannel->sendMessage(response)){
            qDebug() << "HsContentPublishHandler response failed: ";    
        }
        
    }else{
        qDebug() << "HsContentPublishHandler messageType not supported!";
    }

    
}

bool HsContentPublishHandler::addWidget(
    const QString &uri, const QVariantMap &preferences)
{
    HsWidgetData widgetData;
    widgetData.setUri(uri);

    QScopedPointer<HsWidgetHost> widget(HsWidgetHost::createInstance(widgetData, preferences));

    if (!widget.data()) {
        qDebug() << "HsWidgetHost::createInstance failed";
        return false;
    }
    HsPage* activePage = HsScene::instance()->activePage();
    if (!widget->load() || !activePage->addNewWidget(widget.data())) {
        widget->deleteFromDatabase();
        qDebug() << "widget->load() || activePage->addNewWidget failed";
        return false;
    }
    HsWidgetHost* taken = widget.take();
    taken->initializeWidget();
    taken->showWidget();
    activePage->layoutNewWidgets();
    activePage->resetNewWidgets();
    
    
    return true;
}