#include "udp.h"

udp::udp(QObject *parent) : QObject(parent)
{
}

// 本地IP查找
void udp::localIPFind(QComboBox *portBox)
{
    // 清除comboBox的内容，防止显示重复的IP
    portBox->clear();
    portBox->addItem("0.0.0.0");

    QStringList hostAddrList = getHostAddress();
    int nListSize = hostAddrList.size();

    for (int i = 0; i < nListSize; i++)
    {
        portBox->addItem(hostAddrList.at(i));
    }
}

// 获取主机的 IPv4 地址
QStringList udp::getHostAddress()
{
    QStringList tmp;
    QStringList ethernetIPv4Addresses;

    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses(); // 先获取所有IP
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
        // 过滤以太网接口
        if (interface.type() == QNetworkInterface::Ethernet) {
            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            for (const QNetworkAddressEntry &entry : entries) {
                // 只考虑IPv4地址
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    ethernetIPv4Addresses << entry.ip().toString();
                }
            }
        }
    }

    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && // 过滤回环地址
            ipAddressesList.at(i).toIPv4Address() && // 只保留ipv4地址
            !IsLinkLocalAddress(ipAddressesList.at(i))) // 过滤本机链路地址
        {
            tmp << ipAddressesList.at(i).toString(); // 添加到列表
        }
    }

    if (tmp.isEmpty()) // 如果没有找到，则以本地IP地址为IP
    {
        tmp << QHostAddress(QHostAddress::LocalHost).toString();
    }

    QStringList AlladdList = tmp + ethernetIPv4Addresses;
    QSet<QString> uniqueAddresses = QSet<QString>::fromList(AlladdList);
    return uniqueAddresses.toList();
}

// 过滤本机链路地址
bool udp::IsLinkLocalAddress(QHostAddress addr)
{
    quint32 nIPv4 = addr.toIPv4Address();
    quint32 nMinRange = QHostAddress("169.254.1.0").toIPv4Address(); // 最小地址
    quint32 nMaxRange = QHostAddress("169.254.254.255").toIPv4Address(); // 最大地址
    if ((nIPv4 >= nMinRange) && (nIPv4 <= nMaxRange)) // 如果是本机链路地址，返回true
    {
        return true;
    }
    else
    {
        return false;
    }
}
