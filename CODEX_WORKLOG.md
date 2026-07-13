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

## 2026-07-10 视野扫描顺序固定

目标：
- 固定每个孔位内的视野扫描顺序为从左到右、从上到下。
- 避免 `QSet` 无序迭代导致模拟扫描顺序不稳定。

计划：
- 只调整 `ScanPage::buildMockScanPlan()` 的模拟扫描计划生成。
- 保持孔位顺序使用孔板顺序；视野索引排序后再加入扫描计划。
- 不改自绘控件、不接真实硬件、不引入新 model。

MVP/接口检查：
- View：不改。
- Presenter/页面协调：`ScanPage` 临时生成稳定模拟扫描计划。
- Model/接口：继续使用视野索引，按索引排序表达从左到右、从上到下。
- 不做：不接相机、样品台、SDK、线程或真实扫描流程。

实际改动：
- `scanpage.cpp`：在 `buildMockScanPlan()` 中将当前孔位的视野索引从 `QSet` 转为 `QList` 后排序，再加入模拟扫描计划。
- 视野索引仍使用 `row * columnCount + column` 语义，因此排序后扫描顺序稳定为从左到右、从上到下。

验证：
- `uic scanpage.ui -o ui_scanpage.h`：通过。
- `uic CreateExperimentSubPage.ui -o ui_CreateExperimentSubPage.h`：通过。
- MSVC + qmake + nmake Release：通过完整编译和链接。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 当前仍是模拟扫描计划；真实扫描执行层接入时也需要沿用同一排序规则。

## 2026-07-10 孔区和视野确认语义修正

目标：
- 进入创建实验第 4 页选择流程后，孔区直接允许鼠标选择，不需要先点击“选择孔区”才能开始。
- “选择孔区”只确认当前鼠标选择的孔区；“取消选择”只清除当前临时孔区选择。
- “选择视野”只确认当前孔位的视野；“取消选择”只清除当前临时视野选择。
- 确认孔区后不自动跳转视野选择，用户需要自己点击想要选择视野的孔位。
- 未给该分组/全部孔位选择视野时，进入下一组或最终确定都要提示。

计划：
- 保留 `ScanPage` 当前轻量选择模型，不接硬件和真实扫描服务。
- 简化孔区/视野按钮分派，删除自动跳转下一个孔位视野的逻辑。
- 让取消按钮只清当前自绘控件的临时选择，不退出选择流程。
- 使用源码外构建目录进行增量验证，减少源码目录临时文件清理。

MVP/接口检查：
- View：自绘控件继续只负责鼠标选择和状态显示。
- Presenter/页面协调：`ScanPage` 临时协调确认按钮、取消按钮和选择模型。
- Model/接口：继续使用孔位名和视野索引缓存选择，不新增持久化 model。
- 不做：不接相机、样品台、SDK、线程或真实扫描流程。

实际改动：
- `scanpage.h/.cpp`：让进入第 4 页选择流程时直接开启孔区鼠标选择，不再要求先点“选择孔区”才能开始。
- `scanpage.cpp`：将“选择孔区”改为确认当前临时孔区；“取消选择”只清除当前临时孔区，保持孔区选择模式不退出。
- `scanpage.cpp`：将“选择视野”改为确认当前孔位的临时视野；“取消选择”清除当前孔位临时视野并同步清掉该孔位视野缓存，避免最终确认误通过。
- `scanpage.cpp`：删除确认孔区后自动跳转视野、确认视野后自动跳到下一个孔位的逻辑；用户需要点击已确认孔位后才进入该孔位视野选择。
- `scanpage.cpp`：切换分组、最终确认和开始新分组选择前检查是否存在未选择视野的孔位，存在时提示具体孔位并阻止继续。
- `scanpage.h/.cpp`：删除不再使用的 `beginFieldSelection()` 和 `hasSelectedFieldsForActiveWell()`，并使用 `#include <QTimer>` 替代 Qt 类型前置声明。

验证：
- `git diff --check -- scanpage.cpp scanpage.h CODEX_WORKLOG.md`：通过。
- 源码外 Release 构建：`qmake D:\qt\QtMingMe\LiveCell\LiveCell.pro -spec win32-msvc CONFIG+=release && nmake`：通过。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 当前孔区/视野选择流程仍由 `ScanPage` 临时协调；若后续规则继续增多，建议迁移到 `ScanPresenter`。
- 需要人工运行界面确认：第 4 页点击选择后浮层隐藏、孔区可直接鼠标选择、确认孔区不自动跳视野、点击孔位后再选择视野、未完成视野时分组/最终确认提示。

## 2026-07-10 孔区与视野取消选择行为修正

目标：
- 点击已确认孔位后，“取消选择”可以删除当前孔位及其视野缓存。
- 用户切换到其他孔位时，如果当前视野只是临时选择且未点击“选择视野”确认，则自动清除，不弹窗提示。
- 用户点击“取消选择”后仍保持当前选择模式，不需要再点击“选择孔区”或“选择视野”才能继续选择。
- 不恢复“自动跳转到下一个孔位”的逻辑，孔位切换继续由用户鼠标决定。

计划：
- 继续只调整 `ScanPage` 的 UI 状态协调，不引入真实硬件、线程或 SDK。
- 让孔区取消优先清临时选中；没有临时选中时删除当前激活孔位。
- 让视野切换和取消只清未确认临时选择，并保持视野选择模式可继续操作。
- 使用源码外 Release build + 增量 `nmake` 验证。

MVP/接口检查：
- View：自绘控件仍负责展示和鼠标输入。
- Presenter/页面协调：`ScanPage` 临时维护孔位/视野选择缓存和按钮状态。
- Model/接口：仍使用孔位名和视野索引缓存，不新增持久化 model。
- 不做：不接相机、样品台、SDK、线程或真实扫描流程。

实际改动：
- `scanpage.cpp`：切换分组时若存在未确认视野，先丢弃临时视野状态，再按“是否有孔位缺视野”统一校验。
- `scanpage.cpp`：点击其他已确认孔位时，如果当前孔位有未确认视野选择，自动清除临时视野并切换到新孔位，不再弹出“请先点击选择视野”的提示。
- `scanpage.cpp`：`buttonCancelWellSelection` 现在会优先清除当前临时孔位；若临时孔位本身已在分组模型中，则从分组和视野缓存中删除。
- `scanpage.cpp`：没有临时孔位时，`buttonCancelWellSelection` 会删除当前激活孔位及其视野缓存，然后继续保持孔区选择模式。
- `scanpage.cpp`：`buttonCancelFieldSelection` 保持当前视野选择模式可继续操作，不需要重新点击“选择视野”才能继续选择。
- `scanpage.cpp/.h`：新增 `discardUnconfirmedFieldSelection()` 和 `removeCurrentWellSelection()`，把临时视野丢弃和当前孔位删除拆成明确的页面协调方法。

验证：
- `git diff --check -- scanpage.cpp scanpage.h CODEX_WORKLOG.md`：通过。
- 源码外 Release 增量构建：`cd /d D:\qt\QtMingMe\LiveCell_build_release && nmake`：通过。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 当前选择流程仍由 `ScanPage` 协调，后续若规则继续增加，建议迁移到 `ScanPresenter` 或独立轻量选择 model。
- 需要人工运行界面确认：选中已分组孔位后点“取消选择”能删除该孔位；未确认视野时点击其他孔位不弹窗且临时视野会清空；取消后仍能继续直接选择孔区/视野。

## 2026-07-10 孔区确认与取消解耦

目标：
- “选择孔区”和“取消选择”不再互斥，也不再作为彼此的前置条件。
- “选择孔区”始终可点击；有临时鼠标选中的孔位时，将这些孔位提交到当前分组。
- “取消选择”只移除当前鼠标临时选中的孔位，不再删除当前激活孔位。
- 当前分组里存在未选择视野的孔位时，仍允许继续选择同一分组的新孔位。
- 删除前一轮为了删除当前激活孔位引入的冗余逻辑。

计划：
- 精简 `ScanPage` 的孔区按钮状态机，去掉同组继续选孔位时的缺失视野拦截。
- 保留跨分组和最终确定前的视野完整性校验。
- 移除不再符合语义的 `removeCurrentWellSelection()`。
- 使用源码外 Release 增量 `nmake` 验证。

MVP/接口检查：
- View：自绘孔板仍只负责临时选中和状态显示。
- Presenter/页面协调：`ScanPage` 继续临时协调按钮语义和选择缓存。
- Model/接口：沿用孔位名到分组、孔位名到视野集合的轻量缓存。
- 不做：不接相机、样品台、SDK、线程或真实扫描流程。

实际改动：
- `scanpage.cpp`：`enablePlateFieldSelection()` 直接进入孔区选择模式，不再因已有孔位未选视野而阻止进入。
- `scanpage.cpp`：`handleSelectWellsButtonClicked()` 去掉缺失视野拦截；按钮始终可点击，有临时选中孔位时提交到当前分组，没有临时选中时仅开启孔区选择模式。
- `scanpage.cpp`：`cancelWellSelection()` 只处理当前临时选中的孔位；没有临时选中孔位时不再删除当前激活孔位，也不再清视野。
- `scanpage.cpp`：`updatePlateFieldControls()` 让“选择孔区”和“取消选择”在孔板选择流程中保持可点击，不再互斥。
- `scanpage.h/.cpp`：移除不再符合语义的 `removeCurrentWellSelection()` 冗余逻辑。

验证：
- `git diff --check -- scanpage.cpp scanpage.h CODEX_WORKLOG.md`：通过。
- 源码外 Release 增量构建：`cd /d D:\qt\QtMingMe\LiveCell_build_release && nmake`：通过。

遗留事项：
- 本轮提交不包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 跨分组和最终确定前仍保留“已选孔位必须有视野”的校验；同一分组内允许先继续添加孔位。
- 需要人工运行界面确认：A1 未选视野时仍可继续在分组 1 选择 A3/B1 等孔位；“取消选择”只移除当前临时选中孔位。

## 2026-07-10 孔板鼠标选择常驻化

目标：
- 去掉“必须点一次选择孔区才能开始鼠标选择、再点一次才确认”的双态逻辑。
- 孔区选择流程打开后，鼠标选择孔区常驻可用。
- `buttonSelectWells` 只确认当前鼠标临时选择的孔区。
- `buttonCancelWellSelection` 只取消当前鼠标临时选择的孔区。
- 已分组孔位点击后用于进入该孔位视野选择，不再被孔区临时选择逻辑吞掉。

计划：
- 调整 `WellPlateWidget`：选择可用时，未分组孔位参与临时选择，已分组/已完成孔位发出点击信号给页面进入视野选择。
- 调整 `ScanPage`：确认孔区后继续保持孔板选择可用；确认视野后回到孔板选择常驻状态。
- 删除取消孔区里的分组删除分支，保持取消按钮单一职责。
- 使用源码外 Release 增量 `nmake` 验证。

MVP/接口检查：
- View：`WellPlateWidget` 只区分鼠标输入语义和状态显示。
- Presenter/页面协调：`ScanPage` 只协调确认/取消和缓存写入。
- Model/接口：继续沿用孔位名和视野索引缓存，不新增业务服务。
- 不做：不接相机、样品台、SDK、线程或真实扫描流程。

实际改动：
- `WellPlateWidget.cpp`：鼠标选择启用后，未分组孔位直接参与临时选择；已分组/已完成孔位不再被临时选择覆盖，而是发出点击信号给页面进入该孔位视野选择。
- `WellPlateWidget.cpp`：拖拽临时选择时跳过已分组孔位，避免把已确认分组孔位重新变成临时选中状态。
- `scanpage.cpp`：`buttonSelectWells` 只确认当前鼠标临时选中的孔区；没有临时选中孔位时只提示用户先用鼠标选择，不再进入/重启孔区选择模式。
- `scanpage.cpp`：`buttonCancelWellSelection` 只清除当前临时选中的孔区，不删除已分组孔位、不清视野、不切换模式。
- `scanpage.cpp`：确认孔区或确认视野后保持孔板鼠标选择可用，用户可继续直接点/拖选择同组新孔位。

验证：
- `git diff --check -- scanpage.cpp WellPlateWidget.cpp CODEX_WORKLOG.md`：通过，仅有 CRLF 行尾转换提示。
- `cmd.exe /c "call C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat && cd /d D:\qt\QtMingMe\LiveCell_build_release && nmake"`：通过。

遗留事项：
- 本轮仍未包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 需要人工运行界面重点确认：进入第 4 页后不点“选择孔区”也能直接鼠标选孔位；“选择孔区”只确认临时选中的孔位；“取消选择”只清除临时选中的孔位；已分组孔位点击后进入对应视野选择。

## 2026-07-10 孔区确认状态与视野入口修正

目标：
- 未确认的临时孔区不能进入视野选择。
- 点击“取消选择”后，临时孔区必须回到未选中状态。
- 恢复临时孔区的 Windows 风格蓝色选区显示，避免和分组颜色混淆。
- 点击已确认孔位进入视野选择时，不再被鼠标释放阶段的旧拖拽逻辑干扰。

计划：
- 调整 `WellPlateWidget` 的鼠标释放逻辑，只在真实拖拽/临时选择时更新选择。
- 调整 `WellPlateWidget` 的 `Selected` 绘制为蓝色临时选区，`Grouped` 保持当前分组色。
- 在 `ScanPage` 中监听临时孔区选择变化，若正在选择视野则清掉未确认视野并关闭视野选择。
- 使用源码外 Release 增量 `nmake` 验证。

MVP/接口检查：
- View：孔板控件只负责状态显示、鼠标临时选择和已确认孔位点击信号。
- Presenter/页面协调：`ScanPage` 负责“临时孔区选择会关闭视野选择”的页面状态联动。
- Model/接口：继续用孔位名 `A1/A2` 和视野索引集合传递数据，不接真实硬件。
- 不做：不接相机、样品台、SDK、线程或图像算法。

实际改动：
- `WellPlateWidget.cpp`：鼠标释放阶段只有在本次确实处于拖拽/临时选择时才更新选择，避免点击已确认孔位后被旧拖拽快照覆盖。
- `WellPlateWidget.cpp`：`Selected` 临时孔区恢复为蓝色背景和蓝色圆环，`Grouped` 确认孔区继续使用当前分组颜色。
- `scanpage.h/.cpp`：新增 `handleWellSelectionChanged()`，在视野选择过程中一旦出现未确认孔区，自动关闭视野选择、清空当前预览孔位，避免给未确认孔区选视野。
- `scanpage.cpp`：点击已确认孔位前若还存在临时未确认孔区，会提示用户先确认孔区或取消选择，不进入视野选择。

验证：
- `git diff --check -- WellPlateWidget.cpp scanpage.cpp scanpage.h CODEX_WORKLOG.md`：通过，仅有 CRLF 行尾转换提示。
- `cmd.exe /c "call C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat && cd /d D:\qt\QtMingMe\LiveCell_build_release && nmake"`：通过。

遗留事项：
- 本轮仍未包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 需要人工运行界面确认：未确认蓝色孔区无法进入视野选择；取消选择后蓝色孔区恢复未选中；点击已确认孔位可以进入视野选择。

## 2026-07-10 取消已确认孔位

目标：
- 当前鼠标点中的已确认孔位，点击“取消选择”后应从分组中移除并恢复未选中状态。
- 删除该孔位对应的视野缓存，避免后续汇总和扫描仍包含它。
- 保留“有蓝色临时孔位时，取消选择只清蓝色临时孔位”的优先级。

计划：
- 在 `WellPlateWidget` 暴露单孔位清除接口，清状态同时清背景色。
- 在 `ScanPage::cancelWellSelection()` 中，当没有临时孔位时处理当前 active well。
- 更新工作记录并用源码外 Release 增量 `nmake` 验证。

MVP/接口检查：
- View：`WellPlateWidget` 提供按孔位名清状态的接口，不保存业务分组关系。
- Presenter/页面协调：`ScanPage` 负责移除分组缓存和视野缓存，并写回 UI。
- Model/接口：继续使用 `A1/A2` 孔位名和视野索引集合，不接真实硬件。
- 不做：不接相机、样品台、SDK、线程或图像算法。

实际改动：
- `WellPlateWidget.h/.cpp`：新增 `clearWell(const QString &well)`，按 `A1/A2` 这类孔位名清除单个孔位状态、背景色和激活状态。
- `scanpage.cpp`：`cancelWellSelection()` 保持临时蓝色孔区优先清除；没有临时孔区时，移除当前 active well 的分组缓存和视野缓存，并将该孔位恢复为未选中。
- `scanpage.cpp`：取消已确认孔位后关闭视野选择并清空视野控件，避免被删除孔位继续显示旧视野。

验证：
- `git diff --check -- WellPlateWidget.h WellPlateWidget.cpp scanpage.cpp CODEX_WORKLOG.md`：通过，仅有 CRLF 行尾转换提示。
- `cmd.exe /c "call C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat && cd /d D:\qt\QtMingMe\LiveCell_build_release && nmake"`：通过。

遗留事项：
- 本轮仍未包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 需要人工运行界面确认：点击已确认孔位后按“取消选择”，该孔位恢复未选中，左侧/弹窗汇总不再包含该孔位和它的视野。

## 2026-07-10 取消选择不清临时孔位

目标：
- `buttonCancelWellSelection` 不再把 `Selected` 临时孔位改回未选中状态。
- `buttonCancelWellSelection` 只处理当前已确认/激活孔位的取消。

计划：
- 精简 `ScanPage::cancelWellSelection()`，删除 `selectedWells()` 分支。
- 保留 active well 的分组缓存和视野缓存清除逻辑。
- 使用源码外 Release 增量 `nmake` 验证。

MVP/接口检查：
- View：`WellPlateWidget` 的临时选择状态不由取消按钮直接清理。
- Presenter/页面协调：`ScanPage` 只根据 active well 移除业务缓存并写回 UI。
- Model/接口：继续使用孔位名和视野索引集合，不接真实硬件。
- 不做：不接相机、样品台、SDK、线程或图像算法。

实际改动：
- `scanpage.cpp`：删除 `cancelWellSelection()` 中清理 `ui->wellPlateWidget->selectedWells()` 的分支。
- `scanpage.cpp`：`cancelWellSelection()` 只根据 `activeWell()` 删除已确认孔位、分组缓存和视野缓存。

验证：
- `git diff --check -- scanpage.cpp CODEX_WORKLOG.md`：通过，仅有 CRLF 行尾转换提示。
- `cmd.exe /c "call C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat && cd /d D:\qt\QtMingMe\LiveCell_build_release && nmake"`：通过。

遗留事项：
- 本轮仍未包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 需要人工运行界面确认：蓝色临时孔位存在时点击“取消选择”不会被清除；点击已确认 active well 后点击“取消选择”仍会删除该已确认孔位。

## 2026-07-10 防止未确认孔位误激活已确认孔位

目标：
- 存在蓝色临时孔位时，点击其他已确认孔位不能把该已确认孔位设为 active。
- 后续点击“确定”或“取消选择”不能误影响未操作的已确认孔位。
- 文案不再提示用“取消选择”清理蓝色临时孔位，避免与最新按钮语义冲突。

计划：
- 调整 `WellPlateWidget`：点击孔位只发 `wellClicked`，不自行设置 active。
- 保留 `ScanPage::beginFieldSelectionForWell()` 作为唯一激活已确认孔位的入口。
- 精简临时孔位阻断提示文案。
- 使用源码外 Release 增量 `nmake` 验证。

MVP/接口检查：
- View：`WellPlateWidget` 只发用户点击信号，不决定页面业务状态。
- Presenter/页面协调：`ScanPage` 负责校验临时孔位、决定是否激活已确认孔位。
- Model/接口：继续使用孔位名和视野索引集合，不接真实硬件。
- 不做：不接相机、样品台、SDK、线程或图像算法。

实际改动：
- `WellPlateWidget.cpp`：点击已确认孔位时只发出 `wellClicked`，不再由控件自行调用 `setActiveWell()`。
- `scanpage.cpp`：保持 `beginFieldSelectionForWell()` 作为唯一激活已确认孔位的入口，只有临时孔位校验通过后才会激活孔位。
- `scanpage.cpp`：存在蓝色临时孔位时的提示改为只要求点击“选择孔区”确认当前蓝色孔位，不再提示用“取消选择”清除。

验证：
- `git diff --check -- WellPlateWidget.cpp scanpage.cpp CODEX_WORKLOG.md`：通过，仅有 CRLF 行尾转换提示。
- `cmd.exe /c "call C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat && cd /d D:\qt\QtMingMe\LiveCell_build_release && nmake"`：通过。

遗留事项：
- 本轮仍未包含用户/Designer 已改动的 `CreateExperimentSubPage.ui`、`scanpage.ui` 和未跟踪的 `AENGRTS.md`。
- 需要人工运行界面确认：蓝色临时孔位存在时，点击其他绿色已确认孔位不会把它设为 active；之后点击“确定”或“取消选择”不会误影响该绿色孔位。
## 2026-07-10 视野临时选择与确认状态拆分

目标：
- “取消选择”恢复为既能取消蓝色临时孔位，也能取消当前已确认孔位。
- 视野临时选择显示为灰色，点击“选择视野”后才写入黄色确认状态。
- 点击“选择视野”后继续停留在当前孔位，选择视野和取消选择按钮保持可用，由用户自己点击切换孔位。

计划：
- 在 `FieldViewWidget` 增加按状态读取视野索引的轻量接口。
- 在 `ScanPage` 中只把灰色临时视野作为“未确认”判断依据。
- 不接入真实硬件、线程或 SDK，不提交、不编译，按用户要求只做小范围逻辑修正。

实际改动：
- `FieldViewWidget.h/.cpp`：新增 `fieldIndexesByState()`，用于页面层判断是否存在灰色临时视野；将 `FieldState::Selected` 绘制为灰色，保留 `FieldState::Completed` 为黄色确认状态。
- `scanpage.cpp`：`buttonCancelWellSelection` 恢复为优先取消蓝色临时孔位；没有蓝色临时孔位时取消当前 active 的已确认孔位。
- `scanpage.cpp`：`buttonSelectFields` 确认当前孔位视野后写入黄色状态，继续停留在当前孔位，视野选择和取消按钮保持可用。
- `scanpage.cpp`：最终“确定”只在存在灰色临时视野时提示先确认视野，不再因为处于视野模式就直接拦截。
- `scanpage.cpp`：恢复当前孔位视野时使用黄色确认状态，避免已确认视野重新显示成临时选择。

验证：
- `git diff --check -- scanpage.cpp FieldViewWidget.cpp FieldViewWidget.h CODEX_WORKLOG.md`：通过，仅有 CRLF 行尾提示。
- 按用户最新要求，本轮小改动未运行编译，未提交，等待相关 bug 合并后统一提交。

遗留事项：
- 需要人工运行界面确认：灰色临时视野点击“选择视野”后变黄色；确认后仍停留当前孔位；取消孔区可清蓝色临时孔位，也可在无临时孔位时取消当前已确认孔位。

## 2026-07-13 孔位预览与视野编辑资格分离

目标：
- 第 1 步“创建实验”和“导入实验”保持互斥。
- 存在蓝色临时孔位时，仍可点击已确认或临时孔位进行预览，也可切换分组；不再出现阻塞预览的提示。
- 未确认孔位只允许单视野预览，不能确认或取消视野；只有已确认分组孔位且没有蓝色临时孔位时才能编辑视野。

计划：
- 用 `QButtonGroup` 统一管理创建/导入模式的互斥状态。
- 由 `ScanPage` 根据当前预览孔位和临时孔位计算视野编辑资格，避免以按钮状态反推业务状态。
- 为 `FieldViewWidget` 增加非编辑的单视野预览接口及信号，不接入相机、样品台、SDK、线程或图像算法。

实际改动：
- `CreateExperimentSubPage.h/.cpp`：增加创建方式按钮组，`buttonCreateMode` 与 `buttonImportMode` 现在互斥，默认仍选中“创建实验”。
- `WellPlateWidget.cpp`：只有空孔位进入拖拽框选；已确认分组孔位和蓝色临时孔位会发出点击信号，因此可随时点击预览。
- `FieldViewWidget.h/.cpp`：增加独立于视野编辑的单视野预览接口和 `fieldPreviewed(int)` 信号；预览以蓝色覆盖显示，不改变灰色临时选择或黄色已确认视野数据。
- `FieldViewWidget.h`：按项目约定改用 `#include <QMouseEvent>`，移除 Qt 基础类前置声明。
- `scanpage.h/.cpp`：移除临时蓝色孔位对孔位预览、分组切换的阻塞；仅在“当前孔位已分组且不存在蓝色临时孔位”时启用“选择视野/取消选择”。页面通过 `fieldPreviewRequested(well, fieldIndex)` 提供后续 mock 或 Presenter 对接入口。
- `scanpage.cpp`：最终确认仍会阻止未确认的蓝色孔位或灰色临时视野，防止不完整实验配置进入下一步。

验证：
- `git diff --check -- CreateExperimentSubPage.cpp CreateExperimentSubPage.h FieldViewWidget.cpp FieldViewWidget.h WellPlateWidget.cpp scanpage.cpp scanpage.h CODEX_WORKLOG.md`：通过，仅有 Git CRLF 行尾提示。
- 按用户要求，本轮为小范围交互修正，未编译、未提交。

遗留事项：
- 请人工验证：保留蓝色临时孔位时，点击 A1/A2/A3 或临时孔位均可显示视野；此时单击视野只出现蓝色预览，两个视野提交/取消按钮不可用；清除或确认蓝色孔位后，已确认孔位恢复灰色选择和黄色确认流程。

## 2026-07-13 单一当前预览孔位

目标：
- 进入新的临时孔位框选时，清除旧孔位的 active 高亮和旧视野预览，避免同时出现两个蓝色状态。
- 保留一次拖拽框选多个孔位的能力；“唯一”仅指当前预览孔位，不改变已确认分组数据。

计划：
- `WellPlateWidget` 在空孔位开始鼠标框选时主动清空 active well。
- `ScanPage` 收到新的临时孔位选择后退出旧视野预览，由用户点击目标孔位开启唯一的新预览。
- 不接入硬件、SDK、线程或图像算法；不编译、不提交，按用户小改动规则执行。

实际改动：
- `WellPlateWidget.h/.cpp`：删除从未写入的 `WellState::Previewing`。
- `scanpage.h/.cpp`：删除只写不读的 `m_bWellSelectionMode`，不再以冗余模式变量推断孔位交互。
- `WellPlateWidget.cpp`：`activeWell` 保留为唯一当前预览标识，但绘制改为橙色外框，不再覆盖分组背景或临时选择蓝色背景。
- `scanpage.cpp`：点击已确认孔位或蓝色临时孔位时始终切换 `activeWell`，无需先点击“取消选择”；视野预览随之切换。

当前孔板状态：
- `Default`：未分组、未临时框选的孔位。
- `Grouped`：已确认并归属某个分组，显示该分组背景色。
- `Selected`：鼠标临时框选，显示 Windows 风格蓝色背景；可多个，点击“选择孔区”后转为 `Grouped`。
- `Scanning`：mock 扫描过程中的当前孔位。
- `Completed`：mock 扫描已完成的孔位。
- `activeWell`：不是 `WellState`，是唯一当前预览孔位，以橙色外框叠加显示，可位于 `Grouped` 或 `Selected` 状态上。

验证：
- `rg -n "Previewing|m_bWellSelectionMode" WellPlateWidget.cpp WellPlateWidget.h scanpage.cpp scanpage.h`：无残留引用。
- `git diff --check -- WellPlateWidget.cpp WellPlateWidget.h scanpage.cpp scanpage.h CODEX_WORKLOG.md`：通过，仅有 Git CRLF 行尾提示。
- 按用户要求，本轮小改动未编译、未提交。

实际改动：
- `WellPlateWidget.cpp`：空孔位开始拖拽/单击临时选择前，清空旧 active well，旧孔位不再保留预览蓝色高亮。
- `scanpage.cpp`：产生新的蓝色临时孔位后，清空旧视野预览和当前预览孔位；用户随后点击已确认或临时孔位即可建立新的预览。
- `scanpage.cpp`：存在蓝色临时孔位时，孔板不再额外绘制当前预览孔位的 active 蓝底，避免预览高亮与临时选择同时出现在不同孔位而造成状态混淆。

验证：
- `git diff --check -- WellPlateWidget.cpp scanpage.cpp CODEX_WORKLOG.md`：通过，仅有 Git CRLF 行尾提示。
- 按用户要求，本轮小改动未编译、未提交。

遗留事项：
- 请人工验证：框选新空孔位后，旧 active 高亮立即消失；存在蓝色临时孔位时点击 A1/A2/A3 仍能更新视野预览但孔板不额外出现第二个 active 蓝底；清除或确认临时孔位后，点击单个已确认孔位恢复唯一 active 高亮。
## 2026-07-13 空 QSS 文件接入

目标：
- 创建一个项目级 QSS 文件，当前不写任何样式代码。
- 在程序启动时加载并应用该 QSS，为后续统一界面美化预留入口。

计划：
- 新增空的 `styles/app.qss`。
- 新增 Qt 资源文件，把 QSS 打包到程序资源中，避免运行目录依赖。
- 在 `main.cpp` 中从资源读取 QSS 并设置到 `QApplication`。
- 更新 `LiveCell.pro`，让 qmake 构建时包含资源文件。

MVP/接口检查：
- View：不修改具体页面控件和布局。
- Presenter/页面协调：不新增页面协调逻辑。
- Model/接口：不新增业务数据结构。
- 不做：不写实际 QSS 样式、不接硬件、不调整现有 UI 交互。

实际改动：
- `styles/app.qss`：新增空 QSS 文件，当前不写任何样式代码。
- `resources.qrc`：新增 Qt Resource，把 `styles/app.qss` 打包到 `:/styles/app.qss`。
- `main.cpp`：程序启动时读取 `:/styles/app.qss` 并应用到 `QApplication`。
- `LiveCell.pro`：新增 `RESOURCES += resources.qrc`，让 qmake 构建包含 QSS 资源。

验证：
- `git diff --check -- main.cpp LiveCell.pro resources.qrc styles/app.qss CODEX_WORKLOG.md`：通过，仅有 CRLF 行尾提示。
- 源码外 Release 构建：`qmake D:\qt\QtMingMe\LiveCell\LiveCell.pro -spec win32-msvc CONFIG+=release && nmake`：通过。

遗留事项：
- 本轮未提交；当前工作区仍有用户此前未提交的 `CreateExperimentSubPage.*`、`FieldViewWidget.*`、`WellPlateWidget.*`、`scanpage.*`、`AENGRTS.md` 等改动。
# 2026-07-13 Well Preview Switching Fix

Goal:
- Make a click on a confirmed grouped well such as A4 immediately switch the active well and allow its field preview/selection.
- Clear any temporary blue well selection, such as B4, when switching to a confirmed grouped well.

Plan:
- Trace the `WellPlateWidget::wellClicked` to `ScanPage` selection flow.
- Centralize grouped-well activation in one page coordination method.
- Keep the change within the lightweight UI selection mock; do not add hardware, SDK, threads, or image processing.

MVP/interface check:
- View input comes from `WellPlateWidget::wellClicked`.
- The existing `wellClicked` signal is forwarded to `ScanPage::handleWellClicked`.
- `ScanPage` coordinates clearing temporary selection and writing the active well/field view state.

Actual changes:
- `scanpage.cpp`: clicking a confirmed grouped or completed well now clears temporary blue well selections before activating that well for field preview and selection.

Verification:
- `git diff --check -- scanpage.cpp CODEX_WORKLOG.md`: passed; only Windows CRLF conversion warnings were reported.
- No build run: this is a small UI-state fix and the user will compile locally.

Open items:
- Manually verify that clicking A4 clears B4's blue temporary state and enables field selection for A4.

# 2026-07-13 Group Validation And Summary Completion

Goal:
- Prevent switching away from a group when any confirmed well in that group has no confirmed fields.
- Populate the existing step 6 experiment and scan summary controls from the values collected on earlier pages.

Plan:
- Validate the current group in the `comboScanGroup` change handler before committing the new group selection.
- Reuse `CreateExperimentSubPage` getters to fill the step 6 read-only controls.
- Keep coordination in `ScanPage` and `CreateExperimentSubPage`; do not introduce hardware, SDK, threads, or image processing.

MVP/interface check:
- View data is read from the group combo box and the existing Create Experiment page controls.
- Existing Qt signals deliver group selection changes and page navigation; no new external service dependency is introduced.
- `ScanPage` validates group navigation, while `CreateExperimentSubPage` writes its own summary controls.

Actual changes:
- `scanpage.h/.cpp`: added a current-group missing-field lookup and block group switching until each confirmed well in that group has confirmed fields.
- `CreateExperimentSubPage.h/.cpp`: added `updateSummary()` to fill the existing step 6 experiment and scan summary fields when entering that page and when the plate/field summary changes.

Verification:
- `git diff --check -- scanpage.cpp scanpage.h CreateExperimentSubPage.cpp CreateExperimentSubPage.h CODEX_WORKLOG.md`: passed; only Windows CRLF conversion warnings were reported.
- No build run: the user requested local compilation for small changes.

Open items:
- Manually verify that a group containing A1 without fields cannot switch to group 2, and that step 6 shows the earlier experiment and scan values.
- Changes are intentionally uncommitted because related UI-selection fixes are still being batched; existing user changes to `CreateExperimentSubPage.ui`, `scanpage.ui`, and `AENGRTS.md` remain untouched.

# 2026-07-13 Field Cancellation, Objective Sync, And Experiment Editing

Goal:
- Make field cancellation remove the active confirmed well's saved field selection.
- Reflect the Preview page objective in the Scan page experiment settings.
- Open the existing Create Experiment dialog from the Scan page Modify button without clearing the current experiment.

Plan:
- Keep field deletion in `ScanPage`, which owns the temporary field-selection cache.
- Reuse `CaptureSettings::objective` between `PreviewPage` and `ScanPage`; MainWindow only wires the two top-level pages.
- Add a dedicated edit-dialog entry point that resets wizard navigation only, not the experiment data.
- Do not add hardware, SDK, threads, or image processing.

MVP/interface check:
- View data comes from `BasicSettingPage::captureSettings()` and `ScanPage`'s existing per-well field-selection cache.
- `PreviewPage` forwards the selected objective through `objectiveMagnificationChanged`; `MainWindow` only connects the two top-level pages; `ScanPage` owns field-cache deletion and UI refresh.
- UI write targets are `lineEditObjective`, `FieldSelectionWidget`, the page-6 selection summary, and the existing Create Experiment dialog.

Actual:
- `cancelFieldSelection()` now removes all saved fields for the active confirmed well, clears the field grid, and refreshes the selection summary.
- `PreviewPage` exposes and forwards the current `CaptureSettings::objective`; `ScanPage` displays the resulting `10X` or `20X` value in the scan configuration.
- The Scan page `修改` action now reopens the existing Create Experiment popup at page 1 without calling the new-experiment reset path, so current configuration data is preserved for editing.

Validation:
- Ran `git diff --check`; only existing CRLF normalization warnings were reported.
- No build was run because this is a small UI interaction change and the user will compile locally.

Open:
- Verify locally that cancelling fields for the active well removes its confirmed yellow fields and then prevents group switching until fields are selected again.
- Verify that changing the Preview objective immediately updates the Scan objective field.
- Verify that `修改` opens the existing wizard with previous form data intact.
- Changes remain uncommitted to batch with related fixes. User-modified `.ui` files and untracked `AENGRTS.md` were not edited.

# 2026-07-13 System Settings Popup

Goal:
- Add a lightweight Qt Widgets system settings popup opened by `buttonSettings`.
- Provide Chinese labels for the four settings pages shown in the reference images.
- Keep settings data independent from QWidget code and leave styling to the project QSS.

Plan:
- Add a `SystemSettings` value model with no QWidget dependency.
- Add `SystemSettingsSubPage` as the settings View for navigation, input collection, and confirmation/cancellation signals.
- Let `MainWindow` own the current settings value, open the popup, and receive confirmed values.
- Keep file browsing and color selection as View-level input collection; expose position calibration as a future-facing signal without real hardware behavior.

MVP/interface check:
- View data comes from the controls in `SystemSettingsSubPage.ui`.
- `SystemSettingsSubPage::settingsAccepted` carries the structured `SystemSettings` value to `MainWindow`.
- `MainWindow` writes the confirmed value to its model and only handles the global settings popup entry point.
- No real file persistence, hardware calibration, SDK, threads, or image processing is added.

Actual changes:
- `models/SystemSettings.h`: added a QWidget-independent structured settings value with generic MVP defaults.
- `SystemSettingsSubPage.h/.cpp/.ui`: added the four-page settings View, Chinese UI text, input collection, storage-directory browsing, color selection, and future-facing position calibration signal.
- `SystemSettingsSubPage.cpp`: reopens the popup on the basic settings page while preserving the confirmed model value.
- `SystemSettingsSubPage.ui`: moved all static visible Chinese text into the UTF-8 form file; removed the previous runtime label translation block from `SystemSettingsSubPage.cpp`.
- Naming follows the referenced convention for the new class/files, methods, members, and readable English `objectName` values; document comment examples were not copied.
- `mainwindow.h/.cpp`: connected `buttonSettings` to the centered settings popup and accepted-value writeback.
- `LiveCell.pro`, `LiveCell.vcxproj`, and `LiveCell.vcxproj.filters`: added the new model, source, header, and form files.

Verification:
- XML parsing for `SystemSettingsSubPage.ui`: passed.
- Qt 6.5.3 MSVC `uic SystemSettingsSubPage.ui`: passed; generated temporary header was removed by explicit file path.
- Verified no static `setText(QStringLiteral(...))`, `setItemText`, or `setWindowTitle` calls remain for UI translation.
- `git diff --check`: passed; only existing LF/CRLF conversion warnings were reported.
- No full build was run, per the current small-change workflow.

Open:
- Settings are currently held in `MainWindow` memory only; persistence and real calibration remain intentionally unimplemented.
- Existing user changes in `datawidget.ui` and untracked `AENGRTS.md` were not edited.

# 2026-07-13 Settings Language Selection Follow-up

Goal:
- Fix the language combo box appearing blank when the current in-memory language is `中文`.
- Keep all static UI text in the UTF-8 `.ui` file and remove escaped Chinese literals from the settings View.

Plan:
- Match combo-box entries by trimmed text and select a valid fallback index.
- Remove the old runtime language-item setup and use direct UTF-8 Chinese for the two dynamic dialog titles.

MVP/interface check:
- Read language and plate type from the settings View controls.
- Transfer the selected values through the existing `SystemSettings` value interface.
- Keep `SystemSettingsSubPage` responsible for writing control state and `MainWindow` responsible only for accepted settings ownership.

Actual:
- Normalized `SystemSettingsSubPage.cpp` to valid UTF-8 and removed the old runtime language-item setup.
- Initialized the language combo to its `.ui` item and restored language/plate selections by trimmed item text and index.
- Made language and plate-type combo items single-line Chinese text in `SystemSettingsSubPage.ui` so XML indentation cannot enter the displayed values.
- Trimmed language and plate values when creating the `SystemSettings` value.
- Replaced escaped Chinese dialog titles with direct UTF-8 Chinese literals.

Verification:
- UTF-8 decoding check for `SystemSettingsSubPage.cpp`: passed without replacement characters.
- UTF-8 XML parsing for `SystemSettingsSubPage.ui`: passed.
- Qt 6.5.3 MSVC `uic SystemSettingsSubPage.ui`: passed; generated temporary header was removed by explicit file path.
- `git diff --check`: passed; only existing LF/CRLF conversion warnings were reported.
- No full build was run, per the current small-change workflow.
