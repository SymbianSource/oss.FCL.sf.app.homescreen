#ifndef HSIPCCHANNELCLIENT_H
#define HSIPCCHANNELCLIENT_H

#include "hsipcchannel.h"

class HSDOMAINMODEL_EXPORT HsIpcChannelClient : public HsIpcChannel
{
    Q_OBJECT
public:
    HsIpcChannelClient(QObject *parent = 0);
    ~HsIpcChannelClient();

    bool openConnection(const QString &serverName);
    bool waitForOpenConnection(const QString &serverName);
    void closeConnection();
    bool waitForCloseConnection();

private slots:
    void onConnected();
};

#endif 
