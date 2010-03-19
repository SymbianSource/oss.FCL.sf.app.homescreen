#ifndef HSIPCHANNEL_H
#define HSIPCHANNEL_H

#include <QLocalSocket>
#include <QVariantMap>

#include "hsdomainmodel_global.h"

class QDataStream;

class HSDOMAINMODEL_EXPORT HsIpcChannel : public QObject
{
    Q_OBJECT
public:
    
    virtual ~HsIpcChannel();

    bool isConnected() const;
    bool sendMessage(const QVariantMap &message);
    bool waitForSendMessage(const QVariantMap &message);
    bool waitForReadMessage(QVariantMap &message);
    virtual void closeConnection()=0;

signals:
    void connectionEstablished();
    void connectionError();
    void messageReceived(const QVariantMap &message);
    void disconnected();
private slots:
    void onReadyRead();
    void onError(QLocalSocket::LocalSocketError socketError);

protected:
     HsIpcChannel(QObject *parent = 0);
     
protected:
    quint32 mBlockSize;
    QLocalSocket *mSocket;
};

#endif // HsIpcChannel_H
