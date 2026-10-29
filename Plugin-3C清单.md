# 高级 3C 插件研究清单

更新时间：2026-10-25。用户已检查并表示所需插件齐全；此清单记录学习与实验范围，不是下载或安装任务。优先使用 Epic 官方文档、引擎源码和高质量技术文档。

## 已核实的本地状态

- 工作区存在 `ALS/ALS.uproject`，其 `EngineAssociation` 为 `5.8`。它是当前可见的参考工程；尚未发现另一个可确认的主 Demo `.uproject`。
- `ALS/ALS.uproject` 中已声明启用：`AnimationWarping`、`PoseSearch`、`AnimationLocomotionLibrary`、`MotionWarping`、`Chooser`、`Mover`、`NetworkPrediction`、`ChaosMover`、`AnimationLayering` 等。声明启用不等于确认对应引擎已安装、项目已成功加载或功能已通过测试。
- 主 Demo 的引擎版本与逐项启用状态仍须在该工程创建后核实；用户报告的“插件齐全”不等于所有插件都应同时启用。

## 唯一的示例工程

- [Game Animation Sample（GASP）](https://www.fab.com/listings/880e319a-a59e-4ed2-b268-b32dac7fa016)：用户正在使用。用于对照 Motion Matching、起停、转向与穿越。先核实本地 `ALS/` 工程的实际身份，不从文件夹名称推断。

不主动推荐其他示例工程。不得未经分析把 GASP 的角色、移动组件、动画蓝图或镜头系统整体移入主 Demo。

## UE 插件：主线学习

以下项目的勾选框表示“研究实验已完成”，不表示“尚未安装”。按实验核对主工程的启用状态、自动依赖和实际效果。

| 研究完成 | 插件名（英文搜索词） | 对应实验 |
| --- | --- | --- |
| [ ] | Enhanced Input | 键鼠/手柄、输入上下文、点按/长按、动作优先级 |
| [ ] | IK Rig (`IKRig`) | 自制角色与参考动画的重定向 |
| [ ] | Control Rig (`ControlRig`) | 脚部、骨盆、上身和手部程序化姿势 |
| [ ] | Full Body IK (`FullBodyIK`) | 全身接触与多目标姿势约束；先与普通 IK 对照 |
| [ ] | Motion Warping (`MotionWarping`) | 翻越、攀爬、攻击的目标点对齐 |
| [ ] | Animation Warping (`AnimationWarping`) | 朝向、步幅、坡面修正 |
| [ ] | Pose Search (`PoseSearch`) | Motion Matching、Pose Search Database 与姿势查询 |
| [ ] | Chooser (`Chooser`) | 根据步态、姿态、场景选择动画数据库或资产 |
| [ ] | Animation Locomotion Library (`AnimationLocomotionLibrary`) | 距离匹配和移动动画辅助计算 |
| [ ] | Motion Trajectory (`MotionTrajectory`) | 轨迹预测与 Motion Matching 输入；UE 5.8 文档标记为 Experimental |
| [ ] | Animation Insights | 记录动画状态、曲线、Montage、姿势与性能；按编辑器提示启用 Insights Data Source Filters、Trace Data Filtering |

依据：[Epic 插件索引](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/PluginIndex)、[Motion Matching](https://dev.epicgames.com/documentation/en-us/unreal-engine/motion-matching-in-unreal-engine)、[Motion Warping](https://dev.epicgames.com/documentation/unreal-engine/motion-warping-in-unreal-engine)、[Animation Insights](https://dev.epicgames.com/documentation/unreal-engine/animation-insights-in-unreal-engine)。

## UE 插件：专题实验

| 研究完成 | 插件名（英文搜索词） | 实验目标 |
| --- | --- | --- |
| [ ] | Gameplay Cameras (`GameplayCameras`) | 镜头模式和过渡较复杂时，与自制相机方案对照；UE 5.8 文档标记为 Experimental |
| [ ] | Gameplay Abilities (`GameplayAbilities`) | 战斗、闪避、体力、取消窗口等规则复杂后，研究 GAS 方案 |
| [ ] | State Tree (`StateTree`) | 复杂状态选择与环境交互流程，对照自制状态系统 |
| [ ] | Smart Objects (`SmartObjects`) | 大量可交互场景点或 AI 共用交互规则时 |
| [ ] | Gameplay Interactions (`GameplayInteractions`) | 复杂的场景交互流程；先核实与当前 UE 版本的关系 |
| [ ] | Niagara | 脚步、滑铲、受击、落地粒子反馈 |
| [ ] | MetaSound (`Metasound`) | 表面脚步声、落地、滑铲、攻击声音变化 |
| [ ] | Mover + Mover Examples | 独立实验，比较新移动框架与 CharacterMovementComponent；UE 5.8 文档标记为 Experimental，不直接替换主角色 |
| [ ] | Network Prediction | 仅在 Mover 或多人预测专题中研究 |
| [ ] | Live Link + Live Link Control Rig | 只有接入实时动捕时使用 |
| [ ] | Modeling Tools Editor Mode | 快速制作穿越、镜头遮挡和战斗测试障碍 |

依据：[Gameplay Cameras](https://dev.epicgames.com/documentation/unreal-engine/gameplay-camera-system-quick-start)、[Mover](https://dev.epicgames.com/documentation/unreal-engine/mover-in-unreal-engine)、[Gameplay Abilities](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/PluginIndex/GameplayAbilities)、[StateTree](https://dev.epicgames.com/documentation/unreal-engine/API/PluginIndex/StateTree)。

## 研究与验收规则

1. 以主工程的 `.uproject` 与实际运行结果核实插件状态；参考工程与主工程分别记录，不从文件夹名称推断工程内容。
2. 每次围绕一个行为实验研究插件和其自动依赖，记录启用前后的 `.uproject` 差异、是否能打开、是否能打包、插件功能是否实际被使用。
3. 每个插件至少完成一个可玩的行为或可观察的调试实验，记录版本、用途、输入输出、性能与边界情况；仅启用插件不算完成。
4. 实验结束后决定保留在主工程、保留在独立实验工程，或弃用；不要把“插件数量”当作质量指标。
5. 解释引擎机制时优先引用当前版本的 Epic 官方文档或引擎源码。除 GASP 外，不再建议下载额外示例工程；除非用户明确提出，不写入 Obsidian。
