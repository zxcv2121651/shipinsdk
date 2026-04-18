# 工业级短视频 SDK (C++ 核心) - 架构设计与研发路线图

## 1. 架构愿景 (Architectural Vision)
对标顶级短视频平台（如 TikTok / 抖音），本项目的核心目标是打造一个**极端的零拷贝渲染、支持跨平台底层硬件加速、并且能稳定运行在海量低端设备上的音视频处理核心 (C++ MVP)**。

### 核心基建 (Phase 1 MVP) 总结：
1. **纯原生多后端渲染 (Native Multi-Backend RHI)**
   - 彻底废弃笨重的 ANGLE 翻译层。直接通过 **RHI (Render Hardware Interface)** 对接 Vulkan (Android) 和 Metal (iOS)。
   - `sdk/core/rhi/` 定义了 `IRHI`, `RHITexture` 和 `RHICommandBuffer`，将渲染细节与上层业务彻底解耦。
2. **绝对零拷贝数据模型**
   - 统一的 `MediaFrame` 数据结构，封装硬件缓冲区句柄（如 Android `HardwareBuffer` 和 iOS `CVPixelBufferRef`）。
   - 抛弃低效的拷贝，所有数据通过 `std::shared_ptr<RHITexture>` 在 GPU 显存内流转。
3. **线程安全防 OOM 队列**
   - 实现 `FrameQueue`，自带多线程互斥锁与条件变量。
   - 强制丢包策略（Drop Oldest）：在编码器或渲染器跟不上时，自动丢弃最老的视频帧，防止低端机瞬间 OOM 崩溃。

---

## 2. 研发体系标准 (SDLC 规范)
为了支撑史诗级的重构与开发，我们为 SDK 核心定义了 **15 道极其严苛的软件研发工序**。所有核心组件必须通过全流程检验：

1. 架构设计与头文件定义 (禁用拷贝构造)
2. 核心逻辑实现 (现代 C++17)
3. 内存与零拷贝优化 (消除 Hot Path 堆分配)
4. 线程安全加固 (TSan 零报警)
5. 高并发锁竞争解除 (汇编级伪共享优化)
6. 单元测试 (GTest, 行覆盖率 > 90%)
7. 模块集成与 DAG 联调
8. 异常熔断与 OOM 恢复演练
9. 性能 Profiling 与汇编调优 (SIMD/NEON)
10. 渲染与 GPU 降耗优化 (合并 RenderPass)
11. APM 性能埋点注入
12. 跨平台 CMake CI/CD 构建
13. 内存泄漏检测 (ASan 长时间压测)
14. Fuzz 模糊测试 (libFuzzer 防崩溃验证)
15. 架构委员会 Code Review 准入

---

## 3. 全局技术路线图 (30 大核心子系统)

在接下来的阶段中，SDK 将围绕以下 **30 个工业级子系统** 展开数百个底层组件的开发，累计包含 **6750 项独立开发任务**：

### 基础底座与渲染层
- **1. 核心内存与基建 (Core Foundation)**：无锁队列、内存池、原子操作。
- **2. RHI 统一渲染抽象接口**：多态引擎调度。
- **3. RHI Vulkan 后端 (Android)**：直接封装 VkImage 和 Swapchain。
- **4. RHI Metal 后端 (iOS)**：直接封装 id<MTLTexture> 和 CommandQueue。
- **5. RHI OpenGL 兜底后端**：适配老旧机型。
- **6. DAG 管线引擎 (Pipeline Engine)**：支持多路流转和分支图编排。

### 采集与硬件加速编解码
- **7/8. 安卓/iOS 极低延迟音频采集**：Oboe / AudioUnit。
- **9/10. 安卓/iOS 视频采集**：Camera2 / AVFoundation。
- **11/12. 硬件编码 (MediaCodec / VideoToolbox)**：直通底层，绕开系统高延迟封装。
- **13/14. 高音质音频编码 (AAC) & MP4 封装器**：极速 Ftyp/Moov 前置封装。
- **15. 极致音画同步引擎 (AV Sync)**：以音频时间戳 (Master Clock) 为基准驱动视频渲染。
- **21. 硬件解码加速引擎 (Hardware Decoding)**：非线性编辑基础。

### 商业级特效与 AI 矩阵
- **16. 特效与滤镜系统 (Effect Framework)**：LUT 与 UberShader。
- **22. AR / 人脸与骨骼追踪对接层**：极速特征点解析。
- **23. 贴纸与 2D/3D 渲染系统**：模型加载与粒子渲染。
- **24. 高级美颜与微整形引擎**：GPU 驱动的网格变形。
- **25. 音频混音与音效矩阵**：变声、混响与降噪。
- **26. 端侧 AI 模型推理引擎**：基于 GPU/NPU 的深度学习加速。

### 上层业务与监控
- **17. APM 性能监控与降级**：实时降频、降帧、熔断保护。
- **18/19. Android JNI / iOS ObjC++ 桥接层**：供业务层无缝调用。
- **20. CI/CD 自动化与质量保障**：确保主干分支永不宕机。
- **27. 网络推流与 RTC 传输层**：直播推流能力衍生。
- **28. 视频编辑与非线编转场引擎**：Timeline 与切片管理。
- **29. 色彩管理与 HDR 管线**：Dolby Vision / HLG 到 SDR 映射。
- **30. 动效脚本虚拟机 (Script VM)**：热更新特效脚本执行器。

> *详细的 6750 项开发任务清单请查阅同级目录下的 `TikTok_SDK_6750_Tasks_Plan.md` 文件。*
