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
- `CreateExperimentSubPage.h/.cpp`：新增 `setPlateFieldSelectionSummary()`，右侧选择完成后可把孔板类型、孔位、分组和视野数量写回第 4 页及汇总页。
- `CreateExperimentSubPage.cpp`：`showCenteredIn()` 不再强制重置到第一页；新建实验入口单独调用 `resetToFirstPage()`，右侧选择完成后可回到当前第 4 页。
- `CreateExperimentSubPage.cpp`：初始化时清空第 4 页默认假数据，显示“暂未选择孔板和视野”。
- `scanpage.h/.cpp`：点击第 4 页“点击选择”后隐藏创建实验浮层，并直接进入右侧孔板选择模式，修复用户以为点击孔板无反应的问题。
- `scanpage.cpp`：孔区确认时若未选择任何孔位弹出提示；孔区确认后自动进入当前孔位视野选择。
- `scanpage.cpp`：视野确认时若未选择任何视野弹出提示；确认成功后禁用右侧选择控件，更新创建实验第 4 页汇总并重新显示浮层。

验证：
- `uic CreateExperimentSubPage.ui -o ui_CreateExperimentSubPage.h`：通过。
- `uic scanpage.ui -o ui_scanpage.h`：通过。
- MSVC + qmake + nmake Release：通过完整编译和链接。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 当前选择流程仍由 `ScanPage` 协调，后续如果流程继续变复杂，应迁入 `ScanPresenter`。
- 需要人工运行界面确认：第 4 页点击选择后浮层隐藏、孔板单击/框选可见、右侧确定后浮层回到第 4 页。
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

## 2026-07-09 孔板与视野状态接口调整

目标：
- 将右侧孔板控件整理为 5 种明确状态：默认、预览中、扫描前分组、扫描前已选择、扫描中已完成。
- 保留按 A1、A2、C4 等孔位名称读写状态的接口，方便外部 Presenter/服务调用。
- 让扫描页的 `buttonGroupColor` 作为当前分组颜色来源，点击选择孔区后用该颜色显示分组孔区背景。
- 参考 `Test_2` 中的选择/状态接口，避免照搬冗余实现。

计划：
- 优先改 `WellPlateWidget` 的状态枚举、批量接口和绘制逻辑，不引入硬件或业务服务。
- 让分组状态支持背景色和孔内填充色同时显示，避免二者冲突。
- 为 `FieldViewWidget` 补齐外部查询/批量设置接口，保持视野控件可被后续 Presenter 驱动。
- 在 `ScanPage` 中只接入轻量 UI 联动：选择孔区、取消选择、确认选择与分组颜色。

MVP/接口检查：
- View：自绘控件负责状态展示、点击/选择信号和基础状态读写。
- Presenter/页面协调：本轮 `ScanPage` 仅临时协调按钮与控件状态，后续可迁到 `ScanPresenter`。
- Model/接口：不新增持久化 model；孔位以 QStringList 暴露。
- 不做：不接相机、样品台、SDK、线程、真实扫描流程和图像算法。

实际改动：
- `WellPlateWidget.h/.cpp`：将孔板状态整理为 `Default`、`Previewing`、`Grouped`、`Selected`、`Completed` 五态，保留 `wellName()`、`setWellState()`、`setWellStates()`、`wellsByState()` 等 A1/A2/C4 字符串接口。
- `WellPlateWidget.h/.cpp`：新增选择模式、拖拽选择、`selectedWells()`、`setGroupedWells()`、`confirmSelectedAsGroup()`、`wellSelectionChanged()`，用于后续外部 Presenter 获取/写入孔区。
- `WellPlateWidget.cpp`：分组状态绘制支持背景色和孔内填充色同时存在；预览状态只显示背景色，避免和孔内状态冲突。
- `FieldViewWidget.h/.cpp`：将视野状态枚举同步为同一套五态命名，便于外部统一调用。
- `scanpage.h/.cpp`：让 `buttonGroupColor` 显示当前分组颜色；点击“选择孔区”进入孔板选择模式，取消选择清除临时选择，确认选择后用当前分组颜色写入孔板分组状态。

验证：
- MSVC + qmake + nmake Release：通过完整编译和链接。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 当前 `ScanPage` 仍临时承担按钮与控件联动，后续扫描流程稳定后建议迁入 `ScanPresenter`。
- 需要人工运行界面确认拖拽选择、分组颜色显示、取消/确认选择的交互是否符合预期。

## 2026-07-09 孔板视野选择与模拟扫描联动

目标：
- 右侧孔板支持类似 Windows 框选的快速选择，并且选中孔区背景使用当前分组颜色，不再让分组一混用预览色。
- 右侧孔板、视野和选择按钮默认不可操作，只有创建实验第 4 页触发选择孔区后才开放。
- 点击分组孔位后进入预览状态并允许为该孔位选择视野；没有视野时给用户提示。
- 左侧扫描按钮默认显示“开始扫描”，点击后用 QTimer 做 5 秒一次的模拟扫描状态流转。

计划：
- 调整 `WellPlateWidget`：预览改为叠加态，不破坏已分组状态；补扫描中状态；分组/选中只画小方块背景，圆形保持无填充。
- 调整 `FieldViewWidget`：补选择模式、点击/拖拽选择、选中/扫描中/完成状态绘制。
- 调整 `ScanPage`：增加选择阶段控制、当前孔位视野缓存、模拟扫描定时器。
- 保持本轮为 UI 联动和模拟数据，不做真实硬件、线程或图像算法。

MVP/接口检查：
- View：孔板/视野控件负责状态显示和选择输入。
- Presenter/页面协调：本轮 `ScanPage` 临时协调 UI 状态和模拟扫描，后续应迁入 `ScanPresenter`。
- Model/接口：用 `QString` 孔位名和视野索引缓存模拟数据，不新增持久化 model。
- 不做：不接相机、样品台、SDK、真实扫描线程和图像算法。

实际改动：
- `WellPlateWidget.h/.cpp`：新增 `Scanning` 状态，预览改为 `m_activeWell` 叠加态，不再破坏已分组/已完成状态；分组和临时选中只绘制当前分组色背景方块，圆形孔区保持无填充。
- `FieldViewWidget.h/.cpp`：新增选择开关、点击/拖拽选择、`selectedFieldIndexes()`、批量设置接口和 `Scanning` 状态；视野选中/完成显示黄色，扫描中显示绿色，预览显示蓝色。
- `scanpage.h/.cpp`：新增第 4 页选择入口控制，右侧孔板/视野和选择按钮默认禁用，只有创建实验子页发出选择孔区请求后才启用。
- `scanpage.h/.cpp`：点击分组孔位后设置孔板预览背景并恢复该孔位已选视野；没有孔位或没有视野时使用 `QMessageBox` 提示。
- `scanpage.h/.cpp`：左侧扫描按钮默认显示“开始扫描”，点击后使用 `QTimer` 每 5 秒推进一次模拟扫描，孔板扫描中为黄色圆形、完成为绿色圆形，视野扫描中为绿色、完成后恢复黄色。

验证：
- MSVC + qmake + nmake Release：通过完整编译和链接。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 当前模拟扫描仍在 `ScanPage` 内，后续建议迁移到 `ScanPresenter` 或 mock service。
- 需要人工运行界面确认选择孔区入口、视野选择提示和 5 秒模拟扫描的视觉状态是否符合预期。

## 2026-07-09 孔区视野分步确认流程

目标：
- 将右侧“选择孔区”改为孔区选择阶段的确认按钮，而不是使用底部“确定”确认孔区。
- 将右侧“选择视野”改为当前孔位视野选择阶段的确认按钮，而不是使用底部“确定”确认视野。
- 底部“确定”只作为最终完成按钮，检查所有已选孔位是否都已选择视野。
- 每个孔位独立保存视野，未给任一孔位选择视野时提示用户。

计划：
- 在 `ScanPage` 中新增按钮分派函数，让同一个按钮按当前阶段执行“开始选择/确认选择”。
- 孔区确认后记录本批孔位列表，自动进入第一个孔位的视野选择；视野确认后要求用户继续给未完成孔位选视野。
- 开始新一组孔区选择或最终确定前统一检查所有已分组孔位是否都有视野。
- 更新工作记录并使用 `uic` 与 MSVC + qmake + nmake 验证。

MVP/接口检查：
- View：`CreateExperimentSubPage` 只发出选择请求；自绘孔板只负责输入和状态展示。
- Presenter/页面协调：本轮仍由 `ScanPage` 临时协调浮层显示、右侧选择按钮、孔位视野缓存和模拟状态。
- Model/接口：继续使用孔位 QString 和视野索引缓存模拟数据，不新增持久化 model。
- 不做：不接相机、样品台、SDK、线程或真实扫描流程。

实际改动：
- `scanpage.h/.cpp`：新增 `handleSelectWellsButtonClicked()`、`handleSelectFieldsButtonClicked()` 和 `finishPlateFieldSelection()`，把孔区确认、视野确认、最终完成拆成三个明确动作。
- `scanpage.cpp`：`buttonSelectWells` 在孔区选择模式下负责确认当前框选孔位；否则在所有已有孔位都有视野后才允许进入下一组孔区选择。
- `scanpage.cpp`：`buttonSelectFields` 在视野选择模式下负责确认当前孔位视野；确认后自动跳到下一个未选择视野的孔位。
- `scanpage.cpp`：底部 `buttonConfirmSelection` 只做最终完成校验；若仍在孔区/视野选择阶段、未选孔位或任一孔位未选视野，都会给出提示。
- `scanpage.cpp`：新增 `groupedWells()` 和 `firstWellWithoutFields()`，统一检查“每个孔位必须有视野”的条件，并复用于新分组选择、最终确定和模拟扫描计划。

验证：
- `uic scanpage.ui -o ui_scanpage.h`：通过。
- `uic CreateExperimentSubPage.ui -o ui_CreateExperimentSubPage.h`：通过。
- MSVC + qmake + nmake Release：通过完整编译和链接。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 当前流程状态仍暂放在 `ScanPage`，后续如果继续增加规则，建议迁移到 `ScanPresenter`。
- 需要人工运行界面确认：框选孔位后点“选择孔区”确认、每个孔位逐个选择视野、底部“确定”只做最终完成。

## 2026-07-09 扫描实验完成态与多分组选区修正

目标：
- 修正多分组孔位汇总不完整的问题，避免只显示当前分组或部分孔位。
- 实验未完整创建完成前，修改、保存实验配置、开始扫描、数据浏览必须保持禁用。
- 站在显微镜扫描使用流程上补齐基础防呆：未完成实验配置不能开始扫描，未给每个孔位选择视野不能最终确认。

计划：
- 在 `ScanPage` 中维护轻量选择模型：孔位名 -> 分组序号，不再只从自绘控件单一状态反推汇总。
- 将实验配置完成状态独立为布尔状态，只在 `CreateExperimentSubPage::experimentFinished` 后解锁左侧操作按钮。
- 汇总显示使用孔位选择模型和视野缓存统一生成，确保多分组、多孔位计数一致。
- 继续保持 UI 联动和模拟数据范围，不接真实硬件或线程。

MVP/接口检查：
- View：自绘控件继续只负责显示和输入。
- Presenter/页面协调：`ScanPage` 暂时保存选择模型和完成态，后续可整体迁移到 `ScanPresenter`。
- Model/接口：使用 `QMap<QString, int>` 表示孔位分组，仍不新增持久化 model。
- 不做：不接相机、样品台、SDK、线程或真实扫描流程。

实际改动：
- `scanpage.h/.cpp`：新增 `m_bExperimentConfigured`，创建实验开始时保持左侧修改、保存、开始扫描、数据浏览禁用，只在 `experimentFinished` 后解锁。
- `scanpage.h/.cpp`：新增 `m_selectedGroupByWell`，用 `孔位名 -> 分组序号` 保存孔位选择模型，汇总和扫描计划不再只依赖自绘控件当前状态。
- `scanpage.cpp`：确认孔区时把所有选中孔位写入选择模型；汇总时按孔板顺序输出所有已选孔位，并显示所有涉及的分组。
- `scanpage.cpp`：新建实验时清空上一次孔区、视野和分组缓存，避免旧实验数据残留。
- `scanpage.cpp`：开始扫描前再次检查实验是否完成、所有孔位是否已选择视野。
- `CreateExperimentSubPage.h/.cpp`：新增第 4 页校验，未选择孔板和视野时不能继续下一步完成实验。

验证：
- `uic CreateExperimentSubPage.ui -o ui_CreateExperimentSubPage.h`：通过。
- `uic scanpage.ui -o ui_scanpage.h`：通过。
- MSVC + qmake + nmake Release：通过完整编译和链接。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 当前选择模型仍暂存在 `ScanPage`，后续建议迁移到 `ScanPresenter` 或独立轻量 model。
- 需要人工运行界面确认多分组 A1/A2/A3 + B2/B3 这类场景汇总能显示 5 个孔位，且左侧按钮只在完整创建后解锁。
