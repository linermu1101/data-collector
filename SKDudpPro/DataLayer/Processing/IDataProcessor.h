#ifndef IDATAPROCESSOR_H
#define IDATAPROCESSOR_H

#include "AcquisitionData.h"

class IDataProcessor {
public:
    virtual ~IDataProcessor() = default;
    virtual void process(AcquisitionData &data) = 0; // 处理方法
};

#endif // IDATAPROCESSOR_H
