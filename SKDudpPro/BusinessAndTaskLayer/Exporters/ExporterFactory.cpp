#include "ExporterFactory.h"
#include "ChassisCsvExporter.h"  // 默认使用 ChassisCsvExporter

IExporter* ExporterFactory::createExporter() {
    return new ChassisCsvExporter();
}