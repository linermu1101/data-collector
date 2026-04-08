#include "mainview.h"

#include <QApplication>

#include "QsLog.h"
#include "QsLogDest.h"


bool logConfig()
{
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    // Trace、Debug、Info、Warn、Error、Fatal、Off
    logger.setLoggingLevel(QsLogging::TraceLevel);

    // 设置log位置
    QString logFolderPath = QDir(QCoreApplication::applicationDirPath()).filePath("log");
    QDir logDir(logFolderPath);
    if (!logDir.exists())
    {
        if (!logDir.mkpath("."))
        {
            qWarning() << "Failed to create log directory:" << logFolderPath;
            return false;
        }
    }
    QString sLogPath = logDir.filePath("log.txt");

    // 添加两个destination
    QsLogging::DestinationPtr fileDestination(QsLogging::DestinationFactory::MakeFileDestination(
        sLogPath, QsLogging::EnableLogRotation, QsLogging::MaxSizeBytes(512000), QsLogging::MaxOldLogCount(10)));
    QsLogging::DestinationPtr debugDestination(QsLogging::DestinationFactory::MakeDebugOutputDestination());

    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /* 程序的单一实现 同名程序只能打开一个 */
    QString uniqueKey = QCoreApplication::applicationName();    // 创建唯一的标识符
    QSharedMemory sharedMemory(uniqueKey);                      // 全局对象名
    if (!sharedMemory.create(1))                                // 如果全局对象以存在则退出
    {
        QMessageBox::information(nullptr, "提示：",
                                "检测到已有相同程序打开\r\n"
                                                        "请检查是否有相同程序在后台运行！");
        return 0;
    }

	qRegisterMetaType<QVector<double>>("QVector<double>");
    qRegisterMetaType<QVector<double>>("QVector<int>");
    qRegisterMetaType<QList<int>>("QList<int>");
    qRegisterMetaType<QTextBlock>("QTextBlock");
    qRegisterMetaType<QTextBlock>("QTextCursor");
    qRegisterMetaType<QVector<double>>("QMap<QString,double>");


    logConfig();

    // LOG samples
    // QLOG_TRACE() << "Here's a" << QString::fromUtf8("trace") << "message";
    // QLOG_DEBUG() << "Here's a" << static_cast<int>(QsLogging::DebugLevel) << "message";
    QLOG_INFO() << "欢迎使用铀青软件 LOG LEVER" << static_cast<int>(QsLogging::DebugLevel);
    // QLOG_WARN()  << "Uh-oh!";
    // QLOG_ERROR() << "An error has occurred";
    // QLOG_FATAL() << "Fatal error!";


    MainView w;
    w.show();
    return a.exec();
}
