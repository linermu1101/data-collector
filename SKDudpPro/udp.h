#ifndef UDP_H
#define UDP_H

#include <QObject>
#include <QComboBox>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QNetworkAddressEntry>

class udp : public QObject
{
    Q_OBJECT
public:
    explicit udp(QObject *parent = nullptr);

    // 本地IP查找
    void localIPFind(QComboBox *portBox);

private:
    // 获取主机的 IPv4 地址
    QStringList getHostAddress();
    // 过滤本机链路地址
    bool IsLinkLocalAddress(QHostAddress addr);

signals:

};

#endif // UDP_H
