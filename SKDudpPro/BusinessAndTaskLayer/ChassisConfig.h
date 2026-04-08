#ifndef CHASSISCONFIG_H
#define CHASSISCONFIG_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QString>
#include <QFile>
#include <QSettings>

/**
 * @brief 机箱配置管理类
 */
class ChassisConfig : public QObject {
    Q_OBJECT
public:
    static ChassisConfig* getInstance();

    /**
     * @brief 加载配置文件
     * @param configPath 配置文件路径
     * @return 是否加载成功
     */
    bool loadConfig(const QString& configPath = "");

    /**
     * @brief 获取卡所属的机箱名称
     * @param cardNumber 卡号
     * @return 机箱名称，如果卡不属于任何机箱则返回空字符串
     */
    QString getChassisForCard(int cardNumber) const;

    /**
     * @brief 获取所有机箱名称
     * @return 机箱名称列表
     */
    QStringList getAllChassis() const;

private:
    explicit ChassisConfig(QObject* parent = nullptr);
    static ChassisConfig* instance;

    // 机箱到卡号集合的映射
    QMap<QString, QSet<int>> chassisToCards;
};

#endif // CHASSISCONFIG_H