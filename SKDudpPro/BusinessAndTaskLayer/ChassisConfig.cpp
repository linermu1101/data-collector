#include "ChassisConfig.h"
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include "QsLog.h"

ChassisConfig* ChassisConfig::instance = nullptr;

ChassisConfig* ChassisConfig::getInstance() {
    if (instance == nullptr) {
        instance = new ChassisConfig();
    }
    return instance;
}

ChassisConfig::ChassisConfig(QObject* parent) : QObject(parent) {
    // 默认加载配置
    loadConfig();
}

bool ChassisConfig::loadConfig(const QString& configPath) {
    // 使用相对于可执行文件的上级目录路径
    QString fullPath = QString("../") + (configPath.isEmpty() ? "chassis_config.ini" : configPath);
    
    // 检查文件是否存在
    QFile file(fullPath);
    if (!file.exists()) {
        QLOG_INFO() << "机箱配置文件不存在，将创建默认配置";
        
        // 创建默认配置
        QSettings settings(fullPath, QSettings::IniFormat);
        
        // 添加默认机箱示例
        settings.beginGroup("1");
        settings.setValue("cards", "1,2");
        settings.endGroup();
        
        settings.beginGroup("2");
        settings.setValue("cards", "3,4");
        settings.endGroup();
        
        settings.sync();
        
        if (settings.status() != QSettings::NoError) {
            QLOG_INFO() << "无法创建配置文件";
            return false;
        }
    }
    
    // 读取配置
    QSettings settings(fullPath, QSettings::IniFormat);
    if (settings.status() != QSettings::NoError) {
        QLOG_INFO() << "无法打开机箱配置文件:" << fullPath;
        return false;
    }
    
    // 清空现有配置
    chassisToCards.clear();
    
    // 读取所有机箱组
    QStringList chassisGroups = settings.childGroups();
    for (const QString& chassisName : chassisGroups) {
        settings.beginGroup(chassisName);
        
        // 读取卡号列表
        QString cardsStr = settings.value("cards", "").toString();
        QStringList cardStrings = cardsStr.split(',', QString::SkipEmptyParts);
        
        QSet<int> cardSet;
        for (const QString& cardStr : cardStrings) {
            bool ok;
            int cardNumber = cardStr.trimmed().toInt(&ok);
            if (ok) {
                cardSet.insert(cardNumber);
            }
        }
        
        chassisToCards[chassisName] = cardSet;
        settings.endGroup();
    }
    
    QLOG_INFO() << "成功加载机箱配置，共" << chassisToCards.size() << "个机箱";
    return true;
}

QString ChassisConfig::getChassisForCard(int cardNumber) const {
    for (auto it = chassisToCards.constBegin(); it != chassisToCards.constEnd(); ++it) {
        if (it.value().contains(cardNumber)) {
            return it.key();
        }
    }
    
    return QString(); // 如果卡不属于任何机箱，返回空字符串
}

QStringList ChassisConfig::getAllChassis() const {
    return chassisToCards.keys();
}