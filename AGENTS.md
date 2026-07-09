# LiveCell Codex 工作规则

本文件是本仓库的项目级约束。每次修改代码前必须先阅读本文件，并结合当前用户最新要求执行。

## 架构原则

1. 必须遵守轻量 MVP 思路。
   - View 只负责控件展示、布局、用户输入收集和信号发出。
   - Presenter 负责页面状态转换、模拟数据组织、UI 联动调度。
   - Model/Settings 只保存结构化数据，不夹杂 QWidget 依赖。
   - 真实硬件、SDK、线程、图像算法不在 UI 页面中提前实现。

2. 必须面向接口编程。
   - UI 层不得直接依赖相机、样品台、SDK 或真实业务服务。
   - 需要外部能力时先定义接口，例如 `ICameraService`、`IStageService`。
   - 当前阶段可用 mock/service stub 验证 UI 联动，不接真实硬件。

3. MainWindow 职责必须收敛。
   - MainWindow 只负责整体布局、顶层页面切换、全局导航。
   - 不把采集、扫描、数据页的业务联动堆进 MainWindow。
   - 子页面尽量拆成独立 QWidget/.ui 和必要的 Presenter。

4. Qt Widgets 约束。
   - 使用 Qt Widgets，不使用 QML。
   - 优先使用 `.ui` 搭建结构，必要时再写自定义 QWidget。
   - 所有主要区域必须使用 layout，避免大量 fixed geometry。
   - objectName 必须清晰，便于后续绑定业务逻辑和统一 QSS。

## 代码修改纪律

1. 修改前先看现有模式。
   - 先读相关 `.h/.cpp/.ui` 和已存在的 interfaces/models/presenters。
   - 不凭空发明复杂抽象；只在能减少耦合或重复时新增类。

2. 每次新增 UI 联动时必须回答这三个问题。
   - 数据从哪个 View 读出？
   - 通过哪个信号/接口传递？
   - 哪个 Presenter 或页面方法负责写回 UI？

3. 避免垃圾复杂代码。
   - 不写重复 connect、重复 lambda、重复 slider/lineEdit 绑定。
   - 不为了未来不确定需求提前写硬件、线程、图像算法。
   - 不引入不必要的第三方库。

4. 文件编辑要求。
   - 手动编辑使用 `apply_patch`。
   - 不批量删除文件或目录。
   - 不使用 `del /s`、`rd /s`、`rmdir /s`、`Remove-Item -Recurse`、`rm -rf`。
   - 需要删除文件时，只能一次删除一个明确路径的文件。
   - 遇到用户已有未提交改动，不得擅自回滚或覆盖。

## 工作记录要求

1. 每次进行非纯问答的代码任务，都要查看并维护 `CODEX_WORKLOG.md`。
2. 开始实现前，在工作记录中写清楚本轮目标和计划。
3. 完成后，更新实际改动、验证命令、未完成事项。
4. 如果没有提交，必须记录原因，尤其是工作区混有无关改动时。

## 验证要求

1. UI 文件改动后至少运行 `uic` 验证对应 `.ui` 能生成头文件。
2. C++/Qt 改动后优先使用 MSVC + qmake + nmake 编译验证。
3. 若 Debug 链接失败是因为 exe 正在运行，需要说明；可用 Release 验证完整链接。
4. 验证产生的根目录临时文件要按明确文件路径清理，不删除目录。
