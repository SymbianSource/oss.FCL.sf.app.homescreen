#ifndef HSIPCCONNECTIONDISPATCHER_H
#define HSIPCCONNECTIONDISPATCHER_H

#include <QObject>
#include <QVariant>
#include "hsdomainmodel_global.h"

class HsIpcServer;
class HsContentPublishHandler;

class HSDOMAINMODEL_EXPORT HsIpcConnectionDispatcher : public QObject
{
    Q_OBJECT

public:
    HsIpcConnectionDispatcher(const QString &connectionName,QObject *parent=0);
    ~HsIpcConnectionDispatcher();

    bool start();
    void stop();
private slots:
    void onConnectionEstablished();

private:
    QString mConnectionName;
    HsIpcServer *mIpcServer;
};

#endif // HSIPCCONNECTIONDISPATCHER_H
