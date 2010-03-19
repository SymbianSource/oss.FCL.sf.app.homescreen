#ifndef HSCONTENTPUBLISHHANDLER_H
#define HSCONTENTPUBLISHHANDLER_H

#include <QObject>
#include <QVariant>
#include "hsdomainmodel_global.h"

class HsIpcChannelHost;
class HSDOMAINMODEL_EXPORT HsContentPublishHandler : public QObject
{
    Q_OBJECT

public:
    HsContentPublishHandler(HsIpcChannelHost *channel,QObject *parent=0);
    ~HsContentPublishHandler();
public slots:
     void onMessageReceived(const QVariantMap &message);
signals:
     void finished();
private:
    bool addWidget(const QString &uri, const QVariantMap &preferences);
private:
    HsIpcChannelHost* mChannel;
};

#endif // HSCONTENTPUBLISHHANDLER_H
