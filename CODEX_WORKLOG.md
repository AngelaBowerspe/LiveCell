# Codex 工作记录

本文件用于记录 Codex 在本仓库中的计划、改动、验证和遗留事项。每次代码任务开始前先阅读，结束前更新。

## 使用规则

1. `目标` 写清楚本轮要解决什么，不扩大范围。
2. `计划` 写准备怎么做，尽量对应轻量 MVP 和面向接口。
3. `实际改动` 写改了哪些文件和为什么改。
4. `验证` 写执行过的命令和结果。
5. `遗留事项` 写未完成、未提交、需要人工确认的点。

## 记录模板

```markdown
## YYYY-MM-DD 任务标题

目标：
-

计划：
-

MVP/接口检查：
- View：
- Presenter/页面协调：
- Model/接口：
- 不做：

实际改动：
-

验证：
-

遗留事项：
-
```

## 2026-07-09 创建实验子页接入与浮层修正

目标：
- 将用户已创建的 `CreateExperimentSubPage.ui` 接入扫描页。
- 修正创建实验页显示方式，使其作为扫描页内的悬浮子控件，而不是独立 Dialog/新窗口。
- 创建实验数据按向导页逐步提交到左侧实验信息，不在点击“创建实验”时一次性灌入默认值。
- 保留后续 Presenter/业务层可调用的数据接口。

计划：
- 先规范 `CreateExperimentSubPage.ui` 中关键控件 objectName。
- 在 `CreateExperimentSubPage` 中保留 View 级 getter 和按页 accepted 信号。
- 在 `ScanPage` 中持有子页实例，点击创建实验时调用子页自己的浮层显示接口。
- 左侧实验信息只在页面校验通过并发出 accepted 信号后更新。

MVP/接口检查：
- View：`CreateExperimentSubPage` 负责控件读取、页切换、校验提示、浮层显示。
- 页面协调：`ScanPage` 暂时负责接收向导页 accepted 信号并同步左侧 UI。
- Model/接口：当前仅保留 getter 和 `AcceptedPage`，后续可抽成 `ExperimentConfig` model 与 Presenter。
- 不做：不接真实相机、样品台、SDK、线程、图像算法，不做数据持久化。

实际改动：
- `CreateExperimentSubPage.ui`：规范关键 objectName，保留用户设计的布局结构。
- `CreateExperimentSubPage.h/.cpp`：增加 `showCenteredIn(QWidget *)`、按父容器居中、标题栏拖动、按页 accepted 信号和数据 getter。
- `scanpage.h/.cpp`：持有 `CreateExperimentSubPage` 子控件，点击创建实验时作为扫描页内浮层显示；按页同步左侧实验信息。
- `LiveCell.pro`、`LiveCell.vcxproj`、`LiveCell.vcxproj.filters`：加入创建实验子页文件；同时当前工作区里存在 `form.*` 删除，需要人工确认是否属于本轮范围。

验证：
- `uic CreateExperimentSubPage.ui -o ui_CreateExperimentSubPage.h`：通过。
- MSVC + qmake + nmake Release：通过完整编译和链接。
- 已确认不再使用 `Qt::Dialog`、`setWindowModality`、`activateWindow` 或图像区域硬编码坐标显示创建实验页。

遗留事项：
- 当前工作区混有 `BasicSettingPage.ui`、`form.*` 删除、`AENGRTS.md`、`LiveCell.sln` 等非本轮改动，暂未提交。
- 后续建议新增轻量 `ExperimentConfig` model 和 `ScanPresenter`，把 `ScanPage::applyAcceptedExperimentPage` 从页面中迁出去。
- 需要人工运行界面确认浮层尺寸、拖动体验和缩放效果。

## 2026-07-09 延时扫描次数联动

目标：
- 让创建实验第 3 页的 `labelAutoCycleCountValue` 根据间隔时长和总时长自动计算扫描次数。

计划：
- 在 `CreateExperimentSubPage` 内增加轻量 View 层计算函数。
- 监听第 3 页四个时间输入框变化，实时刷新 label。
- 保持现有按页提交接口不变，让 `loopCountText()` 继续读取该 label。

MVP/接口检查：
- View：读取本页 QLineEdit，计算并显示自动扫描次数。
- Presenter/页面协调：不新增页面协调逻辑。
- Model/接口：不新增 model；后续若抽 `ExperimentConfig`，该值可作为派生字段。
- 不做：不接扫描线程、不触发真实扫描、不做持久化。

实际改动：
- `CreateExperimentSubPage.h/.cpp`：新增 `updateAutoCycleCount()`、间隔/总时长分钟数读取函数和扫描次数计算函数。
- `CreateExperimentSubPage.cpp`：监听 `lineEditIntervalHours`、`lineEditIntervalMinutes`、`lineEditTotalHours`、`lineEditTotalMinutes` 的 `textChanged` 信号，输入变化时实时刷新 `labelAutoCycleCountValue`。
- `CreateExperimentSubPage.cpp`：初始化默认时间后立即刷新自动次数；`loopCountText()` 保持读取 `labelAutoCycleCountValue`，后续左侧实验信息可直接拿到计算后的次数。

验证：
- `uic CreateExperimentSubPage.ui -o ui_CreateExperimentSubPage.h`：通过。
- MSVC + qmake + nmake Release：通过完整编译和链接。

遗留事项：
- 本轮提交仅包含 `CreateExperimentSubPage.h/.cpp` 和 `CODEX_WORKLOG.md`；当前工作区仍保留用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 需要人工运行界面确认输入变化时 `labelAutoCycleCountValue` 的显示符合预期。

## 2026-07-09 单次扫描禁用延时输入

目标：
- 创建实验第 3 页选中“单次扫描”时，禁用间隔时长和总时长的 4 个 QLineEdit；选中“循环扫描”时恢复可编辑。
- 单次扫描模式下自动扫描次数显示为 `1次`。

计划：
- 在 `CreateExperimentSubPage` 内新增扫描模式相关的轻量 View 状态函数。
- 用扫描模式按钮的 `toggled` 信号统一刷新延时输入状态和自动次数。
- 保持现有 getter/accepted 信号不变，不引入 Presenter 或业务服务。

MVP/接口检查：
- View：根据本页扫描模式按钮状态启用/禁用本页输入框。
- Presenter/页面协调：不新增页面协调逻辑。
- Model/接口：不新增 model；后续若抽 `ExperimentConfig`，扫描模式可作为配置字段。
- 不做：不触发真实扫描、不接线程/SDK、不做硬件或算法。

实际改动：
- `CreateExperimentSubPage.h/.cpp`：新增 `updateDelaySettingState()` 和 `isLoopScanMode()`，统一处理延时输入可用状态。
- `CreateExperimentSubPage.cpp`：选中单次扫描时禁用 `lineEditIntervalHours`、`lineEditIntervalMinutes`、`lineEditTotalHours`、`lineEditTotalMinutes`，并将自动扫描次数显示为 `1次`。
- `CreateExperimentSubPage.cpp`：选中循环扫描时恢复 4 个时间输入框可编辑，并继续按总时长/间隔时长计算次数。
- `CreateExperimentSubPage.cpp`：单次扫描模式下 `intervalTimeText()` 返回 `-`，避免左侧实验信息显示已禁用的旧时间。

验证：
- `uic CreateExperimentSubPage.ui -o ui_CreateExperimentSubPage.h`：通过。
- MSVC + qmake + nmake Release：通过完整编译和链接。

遗留事项：
- 本轮提交仅包含 `CreateExperimentSubPage.h/.cpp` 和 `CODEX_WORKLOG.md`；当前工作区仍保留用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 需要人工运行界面确认单次/循环扫描切换时禁用状态、自动次数和左侧实验信息显示符合预期。
