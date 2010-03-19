#ifndef HSIPCCHANNELHOST_H
#define HSIPCCHANNELHOST_H

#include <QObject>
#include "hsipcchannelclient.h"


class HSDOMAINMODEL_EXPORT HsIpcChannelHost : public HsIpcChannel
{
    Q_OBJECT

public:
    HsIpcChannelHost(QLocalSocket *socket,QObject *parent=0);
    ~HsIpcChannelHost();

    void closeConnection();
private:
    
};

#endif // HSIPCCHANNELHOST_H
