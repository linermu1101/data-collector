# KDudpDAQ - 高速数据采集系统

## 项目简介

KDudpDAQ（铀青软件）是一个基于Qt框架开发的高速数据采集系统，专为大规模多通道数据采集应用而设计。系统支持多达254张采集卡，提供实时数据可视化、UDP网络传输、数据导出等功能。

## 主要特性

### 数据采集
- **多卡支持**：支持最多254张采集卡同时工作
- **多通道采集**：每张卡支持4个独立采集通道
- **多种采样率**：支持20M、125M、200M、250M、500M、1G等多种采样率
- **灵活触发模式**：支持硬件触发和软件触发两种模式
- **前后触发**：可配置前触发和后触发时间

### 网络通信
- **UDP数据传输**：基于UDP协议的高速数据传输
- **多机箱管理**：支持多个机箱的统一配置和管理
- **实时监控**：实时显示网络连接状态和数据接收情况

### 数据处理
- **实时可视化**：使用QCustomPlot和OpenGL进行实时波形显示
- **数据校准**：支持通道系数校准，提高测量精度
- **数据导出**：支持CSV格式数据导出
- **历史数据分析**：支持历史数据回放和分析

### 用户界面
- **直观操作**：现代化的图形用户界面
- **参数配置**：灵活的采集参数配置界面
- **日志系统**：完善的日志记录和显示功能
- **单实例运行**：确保同一时间只有一个程序实例运行

## 系统架构

项目采用分层架构设计，主要分为以下几层：

```
┌─────────────────────────────────────────┐
│           表示层 (Presentation)          │
│  MainView, PlanarGraph, UI Components   │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│        业务逻辑层 (Business Logic)        │
│  MainLogical, DataInteractionLogical    │
│  DataShowLogical, ExportTask            │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│           数据层 (Data Layer)            │
│  Model: AcquisitionData, ChannelModel   │
│  Parser: DataParser                     │
│  Processing: DataProcessing             │
└─────────────────────────────────────────┘
```

### 核心模块

#### 1. 数据层 (DataLayer)
- **Model**: 数据模型定义
  - `AcquisitionData`: 采集数据容器
  - `ChannelModel`: 通道数据模型
- **Parser**: 数据解析
  - `DataParser`: 原始数据解析器
- **Processing**: 数据处理
  - `DataProcessing`: 数据处理实现
  - `IDataProcessor`: 数据处理接口

#### 2. 业务与任务层 (BusinessAndTaskLayer)
- **Exporters**: 数据导出
  - `IExporter`: 导出器接口
  - `CsvExporter`: CSV格式导出
  - `ChassisCsvExporter`: 机箱CSV导出
  - `ExporterFactory`: 导出器工厂
- **Workers**: 工作线程
  - `ExportWorker`: 导出工作线程
- **配置管理**
  - `ChassisConfig`: 机箱配置管理
  - `ExportTask`: 导出任务管理

#### 3. 核心组件
- `MainView`: 主界面视图
- `MainLogical`: 主逻辑控制
- `DataInteractionLogical`: 数据交互逻辑
- `DataShowLogical`: 数据显示逻辑
- `AcquisitionCard`: 采集卡配置
- `CardInformation`: 采集卡信息

## 技术栈

### 核心框架
- **Qt 5/6**: 跨平台C++图形界面框架
- **C++17**: 现代C++标准

### 图形与可视化
- **QCustomPlot**: 高性能绘图库
- **OpenGL**: 硬件加速图形渲染

### 网络通信
- **Qt Network**: 网络通信模块
- **UDP Socket**: 高速数据传输

### 第三方库
- **QsLog**: 轻量级日志库

### 系统依赖
- **Windows API**: 硬件交互
- **SetupAPI**: 设备管理

## 项目结构

```
daq_net/
├── 3rdparty/              # 第三方库
│   └── QsLog/            # 日志库
├── SKDudpPro/            # 主项目
│   ├── BusinessAndTaskLayer/    # 业务与任务层
│   │   ├── Exporters/          # 导出器
│   │   ├── Workers/            # 工作线程
│   │   ├── ChassisConfig.*     # 机箱配置
│   │   └── ExportTask.*        # 导出任务
│   ├── DataLayer/              # 数据层
│   │   ├── Model/              # 数据模型
│   │   ├── Parser/             # 数据解析
│   │   └── Processing/         # 数据处理
│   ├── pcietoolkits/           # PCIe工具包
│   ├── img/                    # 图片资源
│   ├── main.cpp                # 程序入口
│   ├── mainview.*              # 主视图
│   ├── mainlogical.*           # 主逻辑
│   ├── acquisitioncard.*       # 采集卡配置
│   ├── datainteractionlogical.* # 数据交互逻辑
│   ├── datashowlogical.*       # 数据显示逻辑
│   ├── parameter.h             # 参数定义
│   └── udp.*                   # UDP通信
├── WinSocket/            # Windows Socket模块
├── daq_net/              # 编译生成文件
├── chassis_config.ini    # 机箱配置文件
├── presetIP.ini         # 预设IP配置
├── channel_coefficient.ini  # 通道系数配置
└── KDudpDAQ.pro         # 项目主配置文件
```

## 配置文件说明

### chassis_config.ini
机箱配置文件，定义了各机箱包含的采集卡编号：
```ini
[Chassis_1]
cards="119,120,121,..."

[Chassis_2]
cards="139,140,141,..."
```

### presetIP.ini
预设IP地址和端口配置：
```ini
192.168.0.2:8080
192.168.0.4:8080
```

### channel_coefficient.ini
通道校准系数配置，格式为：
```
CH[卡号]-[通道号]:[系数a],[系数b]
```

## 编译与构建

### 环境要求
- Qt 5.15+ 或 Qt 6.x
- C++17 编译器
- Windows操作系统
- OpenGL支持

### 编译步骤

1. 安装Qt开发环境
2. 打开Qt Creator
3. 打开项目文件 `KDudpDAQ.pro`
4. 配置编译套件（Kit）
5. 构建项目

### 命令行编译
```bash
qmake KDudpDAQ.pro
make
```

## 使用说明

### 启动程序
运行编译生成的可执行文件，程序会自动检查是否已有实例运行。

### 基本操作流程

1. **网络配置**
   - 选择本地IP地址
   - 配置端口号
   - 点击"连接"按钮建立连接

2. **采集参数设置**
   - 选择机箱编号
   - 设置采样率
   - 配置采集时间
   - 设置触发模式
   - 配置前后触发时间

3. **通道选择**
   - 选择需要采集的卡和通道
   - 支持全选和按卡选择

4. **开始采集**
   - 点击"开始"按钮开始数据采集
   - 实时查看波形显示
   - 查看日志输出

5. **数据导出**
   - 设置文件保存路径
   - 配置文件名
   - 导出为CSV格式

### 高级功能

#### 通道校准
通过修改 `channel_coefficient.ini` 文件配置各通道的校准系数，提高测量精度。

#### 历史数据分析
点击"历史文件"按钮可以加载之前保存的数据进行分析。

#### 自动文件命名
勾选"自动文件命名"选项，系统会根据日期时间自动生成文件名。

## 关键参数说明

### 采样率
系统支持以下采样率：
- 20M: 20,000,000 samples/s
- 125M: 125,000,000 samples/s
- 200M: 200,000,000 samples/s
- 250M: 250,000,000 samples/s
- 500M: 500,000,000 samples/s
- 1G: 1,000,000,000 samples/s

### 触发模式
- **硬件触发**: 通过外部触发信号启动采集
- **软件触发**: 通过软件命令启动采集

### DDR地址映射
- 通道0: 0x20000000
- 通道1: 0x00000000
- 通道2: 0x60000000
- 通道3: 0x40000000

## 日志系统

程序运行日志保存在程序目录下的 `log/` 文件夹中：
- 日志文件: `log.txt`
- 日志轮转: 启用
- 最大文件大小: 512KB
- 最大历史文件数: 10个

日志级别包括：
- Trace: 详细跟踪信息
- Debug: 调试信息
- Info: 一般信息
- Warning: 警告信息
- Error: 错误信息
- Fatal: 致命错误

## 开发指南

### 添加新的导出格式
1. 实现 `IExporter` 接口
2. 在 `ExporterFactory` 中注册新的导出器
3. 在UI中添加相应的导出选项

### 添加新的数据处理算法
1. 实现 `IDataProcessor` 接口
2. 在数据处理流程中集成新的处理器

### 扩展机箱配置
修改 `chassis_config.ini` 文件添加新的机箱配置。

## 注意事项

1. **单实例运行**: 程序启动时会检查是否已有实例运行，避免重复启动
2. **网络配置**: 确保网络连接正常，IP地址配置正确
3. **数据存储**: 确保有足够的磁盘空间存储采集数据
4. **硬件兼容**: 确保采集卡硬件正常工作
5. **性能优化**: 高采样率采集时注意系统性能

## 故障排除

### 无法连接设备
- 检查网络连接
- 确认IP地址和端口配置正确
- 检查防火墙设置

### 数据丢失
- 检查网络带宽是否充足
- 降低采样率
- 检查系统性能

### 程序崩溃
- 查看日志文件
- 检查配置文件格式
- 确认硬件驱动正常

## 版本历史

已交付，后续维护请查看项目提交记录。

