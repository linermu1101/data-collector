#ifndef EXPORTERFACTORY_H
#define EXPORTERFACTORY_H

#include "IExporter.h"

class ExporterFactory {
public:
    static IExporter* createExporter(); // 可根据配置决定返回哪种导出器
};

#endif // EXPORTERFACTORY_H