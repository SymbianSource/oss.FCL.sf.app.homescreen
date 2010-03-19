#ifndef HSIPCSERVER_H
#define HSIPCSERVER_H

#include <QObject>
#include <QQueue>
#include "hsdomainmodel_global.h"

class QLocalServer;
class HsIpcChannelHost;

class HSDOMAINMODEL_EXPORT HsIpcServer: public QObject
{
    Q_OBJECT
public:
    HsIpcServer(QObject *parent = 0);
    ~HsIpcServer();

    bool startServer(const QString &serverName);
    void stopServer();
    
    HsIpcChannelHost* takeConnection();
signals:
    void newConnection();
private slots:
    void onNewConnection();

private:
    QLocalServer *mServer;
    QQueue<HsIpcChannelHost*> mConnections;
};

#endif // HSIPCSERVER_H
