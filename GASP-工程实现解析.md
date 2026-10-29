# GASP 工程实现解析（UE 5.8，本地工程审阅）

> 审阅对象：`ALS/Content` 中除 `MyALS` 外的 Game Animation Sample 内容；项目描述、插件和关卡配置来自本地 `ALS.uproject` 与 `Config`。本文是**审阅记录**，不是把 GASP 结构作为 MyALS 的既定实现方案。2026-10-29 建立。

## 0. 阅读范围、证据等级与限制

本地 GASP 内容共 **3,898 个文件**（排除 `MyALS`）：`Characters` 2,823、`MetaHumans` 365、`Audio` 316、`Blueprints` 176、`IsolatedExamples` 94、`Levels` 56、`Widgets` 36、`Input` 28、`Movies` 3、`Misc` 1。其中 `Characters/UEFN_Mannequin/Animations` 有 2,345 个资源。文件清单用于确认范围，**不能等同于逐个读完资源内部配置**。

证据标记：**[图]** 表示通过 UECP MCP 读取了蓝图骨架、图节点或资产摘要；**[清单]** 表示从文件树或资产名确认存在；**[配置]** 表示从 `.uproject`／`.ini` 读取；**[推断]** 是依据已读结构的解释，需进一步看引脚、运行或调试。凡未标记“图”的逐项节点行为，不能当成已经验证的执行路径。

本轮已逐项读取核心角色蓝图、Controller、GameMode、Traversal 组件的骨架，数个关键函数图，两个主要 ABP 的状态机／函数结构，输入映射摘要和重要动画数据库目录。**尚未逐个打开 3,898 个资产、全部 2,345 条动画序列、全部 Chooser 行、全部 Notify 时间点、全部关卡实例，也未做 GASP 运行时录像与性能测量。**所以“完整阅读整个工程”的逐资产意义尚未达到；本文明确记录已核实的工程结构与后续审阅入口，不用未读内容填充结论。

## 1. 工程入口与双角色路线

`ALS.uproject` 声明 UE **5.8**，本地原始示例与 `MyALS` 共存，不能把 `MyALS` C++ 模块误认为 GASP 官方角色逻辑。[配置] `DefaultEngine.ini` 的启动和游戏地图都是 `/Game/Levels/DefaultLevel`。[配置] GASP 主要蓝图在 `/Game/Blueprints`，输入在 `/Game/Input`，动画在 `/Game/Characters/UEFN_Mannequin/Animations`。[清单]

`GM_Sandbox` 基于 `GameModeBase`，其 `PawnClasses_Soft` 明确列出 `/Game/Blueprints/SandboxCharacter_CMC` 与 `/Game/Blueprints/SandboxCharacter_Mover`；另有 `CyclePawn`、`CycleVisualOverride`、`GetDefaultPawnClassForController` 函数。`PC_Sandbox` 的 EventGraph 处理 `IA_NextPawn`、`IA_NextVisualOverride`、`IA_TeleportToTarget`，由 GameMode 执行切换。[图] 因而 GASP 是**两套可切换的角色运动实验**，不是一套角色加两个 ABP 而已。

| 路线 | 根类和运动 | 输入／模拟接口 | 动画蓝图 | 阅读重点 |
| --- | --- | --- | --- | --- |
| CMC | `SandboxCharacter_CMC : Character`，内置 `CharacterMovementComponent` | `SetupInput`、`UpdateMovement_PreCMC`、`UpdateRotation_PreCMC` | `SandboxCharacter_CMC_ABP` | 在传统 Character/CMC 上建立高级动画数据链 |
| Mover | `SandboxCharacter_Mover : Pawn`，`CharacterMoverComponent` 与 `NavMoverComponent` | `MoverInputProducerInterface`、`ProduceInput` | `SandboxCharacter_Mover_ABP` | 输入命令、模拟前后状态、自定义移动模式与高级姿势处理 |

两条路线都实现 `BPI_SandboxCharacter_Pawn`，并向其他系统提供 `Get_PropertiesForAnimation`、`Get_PropertiesForCamera`、`Get_PropertiesForTraversal`、`Get_MMIResult`、`Get_PropertiesForRagdoll` 等接口。[图] 这说明动画、镜头、穿越主要通过**角色数据接口**接入，而不是各自直接假设唯一角色类。CMC 角色还带 `MotionWarping`、`AC_PreCMCTick`、`AC_TraversalLogic`、`AC_FoleyEvents`、`GameplayCamera`、`AC_VisualOverrideManager`、`AC_SmartObjectAnimation`。Mover 角色有相应表现组件，并换成 `CharacterMover`、`NavMover`。[图]

### 对 MyALS 的意义

参考的是**接口边界和实验对照方式**，不是照搬 176 个 Blueprint。我们的 C++ Character/CMC 先验证基础规则；若实验 Mover，必须作为隔离路线对照胶囊体轨迹、输入响应、网络语义和动画质量，不能让 CMC 与 Mover 同时驱动同一角色。

## 2. 输入链：物理按键、操作意图、角色状态

`IMC_Sandbox` 有 **37 条映射**。[图] 主要键位：WASD/Gamepad Left2D → `IA_Move`；鼠标/右摇杆 → `IA_Look`/`IA_Look_Gamepad`；Ctrl/手柄右肩 → `IA_Walk`；Shift/左肩 → `IA_Sprint`；空格/手柄底键同时映射 `IA_Jump` 和 `IA_Traverse`；C/手柄右面键 → `IA_Crouch`；另有瞄准、横移、交互、处决、布娃娃、切换 Pawn／角色外观、传送等。[图] 同一个物理键映射两个 IA **不等于两个动作都会执行**：必须继续追踪角色事件图中的条件与优先级，不能只看 IMC。

`S_PlayerInputState` 的字段从角色蓝图默认值可见：`WantsToSprint`、`WantsToWalk`、`WantsToStrafe`、`WantsToAim`、`WantsToCrouch`。[图] CMC 角色把该结构存为 `CharacterInputState`，属性显示 Replicated；Mover 角色也有 `PlayerInputState`。[图] **“玩家想做什么”与“当前实际是什么运动／姿态”是两类数据**。我们的上一版用多个彼此锁死的开关让输入直接等于最终姿态，遗漏了这层状态解析。

CMC 角色的 `SetupInput` 图通过 `GetController → EnhancedInputLocalPlayerSubsystem → AddMappingContext` 添加输入上下文。[图] 角色 EventGraph 中有 `IA_Crouch`、`IA_Sprint`、`IA_Jump` 事件节点。[图] `GetDesiredGait` 是独立函数，读取 IA 移动值、`AnalogWalk/RunThreshold`、`MovementStickMode`、`CharacterInputState.WantsToWalk` 和 `CanSprint`，经分支返回 `E_Gait`。[图] 图中 `CanSprint` 的判断不仅看 `WantsToSprint`，还涉及朝向、加速度或待处理移动输入，并区分本地控制与非本地控制。[图] 这比“Shift 按下就永远 Sprint”多一层**可冲刺条件**。

`E_Gait` 资产有 3 个枚举值；UECP 摘要只回传 `NewEnumerator0..2`，未给显示名，因此不能仅凭序号认定每一项的可见标签。[图] CMC 角色的 `Gait` 默认序号为 1，`WalkSpeeds` 默认 `(200,180,150)`、`RunSpeeds` `(500,350,300)`、`SprintSpeeds` `(700,700,700)`，还有独立 Demo 速度组和 `CrouchSpeeds` `(225,200,180)`。[图] 三个分量与不同移动方向／朝向有关，精确含义需阅读 `CalculateMaxSpeed` 的引脚默认值和相关变量说明后再定。

**值得学的原则**：输入按键只改变意图；角色计算目标步态；CMC／Mover 决定真实速度；ABP 读取已发生的运动并选姿势。玩家按 C、空格、Shift 时，应先做**上下文动作解析**（例如蹲下时空格起身还是跳跃、面前障碍时翻越还是跳跃），再决定角色状态，不能让单个 IA 事件直接无条件调用所有动作。

## 3. CMC 路线：速度、转向、跳跃、阶段顺序

`SandboxCharacter_CMC` 有 `UpdateMovement_PreCMC`（22 节点）、`UpdateRotation_PreCMC`（22）、`GetDesiredGait`（28）、`CanSprint`（18）、`CalculateMaxAcceleration`（8）、`CalculateBrakingDeceleration`（4）、`CalculateBrakingFriction`（6）、`CalculateGroundFriction`（8）、`CalculateMaxSpeed`（25）、`CalculateMaxCrouchSpeed`（21）、`CanPerformJumpAction`（6）等函数。[图] 这是一整组**运动参数按当前条件动态计算**的设计，不能只抄一个 MaxWalkSpeed 数字。

`CalculateMaxSpeed` 读取速度相对角色朝向的方向、`Curve_StrafeSpeedMap`、当前 Gait，以及 Walk/Run/Sprint 三组速度向量，输出当下允许的速度。[图] `CalculateMaxCrouchSpeed` 同样依方向曲线和 Crouch 速度向量计算。[图] `CanPerformJumpAction` 检查 Mesh 的 AnimInstance 与 Slot 活跃状态。[图] 可见“能否跳”不只由 CharacterMovement 的默认跳跃条件决定，还会受某些动画动作占用约束；具体 Slot 和布尔极性仍需核对连线。

`AC_PreCMCTick` 是独立组件，CMC 角色存在 `PreCMCTick` 事件和 `OnCharacterMovementUpdated_Event`；还有 `AC_PostABPTick` 组件资产。[图/清单] 这提示 GASP 显式管理**在 CMC 模拟前计算输入／转向、模拟后供动画消费状态**的顺序。要研究的是数据何时有效、哪一帧被读取；不应把所有逻辑都堆进 Character Tick 或 ABP Update。

## 4. Mover 路线：输入命令与模拟结果

`SandboxCharacter_Mover` 直接继承 Pawn，拥有 `CharacterMoverComponent`，实现 `MoverInputProducerInterface`。[图] 其 `ProduceInput` 图（约 52 节点）组合 `MoverDefaultInputs_PreSim`、`MoverCustomInputs_PreSim`、移动方向、朝向意图、跳跃刚按下标志、控制旋转等，并通过 `Add Data To Collection` 写入 Mover 输入命令。[图] 不能把此路线上 `Get_Gait` 的结果误说成 CMC 的 `MaxWalkSpeed`；两条运动路径的时序和状态所有权不同。

Mover 蓝图显式列出 `MovementModeMap`：Walking、Falling、Sliding、Traversing、Ragdoll、Flying；另有 `On_PreSimulateTick`、`On_MovementPostFinalize`、`On_MovementModeChanged_PostFinalize`、`On_BasedMovementApplied`。[图] `MovementModes` 目录包含 Walking、Falling、Slide、Flying、Ragdoll 的模式蓝图和 Slide 的进出 Transition。[清单] Mover 角色还保存 PreSim/PostSim 的默认输入和自定义输入、速度历史、地面法线／位置、目标 Actor、瞄准方向等。[图] 这些字段的存在证明设计支持模拟前意图与模拟后结果分离；每项具体更新公式还要逐图验证。

## 5. 动画：状态不是单个 Walk/Run Blend Space

CMC ABP `/Game/Blueprints/SandboxCharacter_CMC_ABP` 有 76 个变量和主要 `State Controller` 状态机：`Idle Loop`、`Transition to Idle`、`Transition to Locomotion`、`Locomotion Loop`、`Transition to In Air`、`In Air Loop`、`Idle Break`，以及 Conduit。可见多数转场交叉淡化为 0.2 秒，部分过渡到循环为 0 秒；还有 `DefaultSlot`。[图] Mover ABP 有 119 个变量，额外包括 `Slide Transition/Loop`、`Debug Flying Loop`，并单列 `Ragdoll` 状态机。[图] 状态名不是“每个枚举一个状态”；它把**进入动画、循环动画、离开动画**分开组织。

两套 ABP 都存储 `MovementMode`、`Gait`、`Stance`、`RotationMode` 等当前和上帧值，并有轨迹、加速度、速度、数据库选择、选中动画、交互目标、旋转目标等字段。[图] CMC ABP 的 `Update_States` 实际读写 `MovementMode_LastFrame`、`MovementState_LastFrame`、`Gait_LastFrame`、`Stance_LastFrame`、`RotationMode_LastFrame` 等。[图] 这些历史量可用于判断**刚进入、刚退出、持续多久**，比只问“当前是否蹲下”更适合选择起步、急停、转身、落地和姿态转换。

CMC ABP 的函数图目录包含 `Update_Trajectory`、`Update_MotionMatching`、`Update_MotionMatching_PostSelection`、`SetBlendStackAnimFromChooser`、`Get_MMBlendTime`、`Get_MMInterruptMode`、`Get_DynamicPlayRate`、`Get_DesiredFacing`、`IsPivoting`、`ShouldTurnInPlace`、`ShouldSpinTransition`、`Update_MovementDirection`、`Get_FootPlacementPlantSettings` 等。[图] Mover ABP 进一步有 `Procedural_Feet`、`Procedural_PreLayering`、`Procedural_FullBody`、`AdditiveLeans`、`AimOffset`、`Update_InteractionConstraints`、`Get_StrideWarpAlpha`、`Get_StrafeWarpAlpha`、`AllowSlopeWarping`、`Update_MMIConstraints` 等。[图] **函数存在不代表每个节点在当前模式启用**；需核查 AnimGraph 连线、开关、Chooser 行与选中动画。

### Motion Matching 资源组织

`Characters/UEFN_Mannequin/Animations/MotionMatchingData` 含 `CHT_PoseSearchDatabases` 及 Dense、Sparse、ExtremeSparse、Relaxed、Mover 等选择器。[清单] `Databases/Dense` 按姿态、速度和阶段拆分：站立 Walk/Run/Sprint 的 Starts、Loops、Pivots、Stops、Lands、TurnInPlace，蹲伏 Walk 的 Starts/Loops/Pivots/Stops、Idle、TurnInPlace，以及 Jumps 等。[清单] 这种拆分表达了**候选集的语义**，避免在任何时刻让所有动画互相竞争；具体 Chooser 条件和 Pose Search Schema 权重仍需逐资产查看，不能从文件名反推。

`ExperimentalStateMachineData` 还有 CMC/Mover 的 PSS/PSD/Chooser 实验资产。[清单] 这些与主 ABP 状态机并列存在，不应把实验资源存在误认为默认运行路径。

## 6. 穿越：检测结果、动画选择、目标校正

`AC_TraversalLogic` 的 `TryTraversalAction` 图约 **244 节点**，含多次 Capsule Trace、可穿越块检测、`GetLedgeTransforms`、前后边缘和落点检查、空间检查、障碍高／深计算、结果结构写入、调试绘制、Chooser 输入／输出、`PerformTraversalAction` 与服务器入口。[图] `S_TraversalCheckResult` 的字段包括 Front/Back Ledge 的位置与法线、Back Floor、ObstacleHeight/Depth、HitComponent、ChosenMontage、StartTime、PlayRate 等。[图] 这不是“检测到墙就放一个 Montage”，而是先产生**可审阅的几何检测结果**，再挑动作和起始时间。

`SetWarpTargets` 图包含 `AddOrUpdateWarpTargetFromLocationAndRotation`、`RemoveWarpTarget`、读取 Montage 中 Motion Warping 窗口、动画曲线中的前后边缘距离，再计算目标位置。[图] 可验证的理解是：穿越动作把场景目标与动画位移对齐，并会更新／移除 Warp Target；具体目标名、窗口起止和根运动驱动方式还需查对应 Montage/Notify 和引脚。[推断] MyALS 应先定义检测几何、落点安全、碰撞体归属与失败路径，再比较 Motion Warping 方案。

## 7. 镜头、角色外观、程序化层与反馈

`Blueprints/Cameras` 包含 `CameraAsset_SandboxCharacter`、`CameraDirector_SandboxCharacter`、Camera Mode/Style 枚举、Camera Rig 选择器以及 Close/Medium/Far 的 Freecam、Strafe、Aim 组合，另有 CrouchOffset、CollisionOffset、Ragdoll、TwinStick、DebugView 等 Rig。[清单] CMC/Mover 角色都提供 `Get_PropertiesForCamera` 接口，包含 `GameplayCamera` 组件。[图] 可确定镜头是**按模式、距离和角色状态拆分的系统**；具体 Rig 选择条件、混合时间和遮挡算法仍须进一步读 Camera Director/Chooser 内容。

`AC_VisualOverrideManager`、多个 RetargetedCharacters 蓝图、Echo/Manny/Quinn/Twinblast/UE4 Mannequin、MetaHuman Kellan 构成外观替换与重定向实验。[图/清单] `AC_FoleyEvents`、众多脚步／落地／手撑／翻滚／打击 Notify 与 `Audio` 目录构成按动作时机触发音效的链路。[清单] `CR_Biped_FootPlacement`、`Blueprints/AnimModifiers` 中 FootSpeed、FootSteps、WarpingAlpha、DistanceFromLedge、MoveData_Speed 等资产说明项目还准备了动画预处理和程序化足部控制资产。[清单] 具体运行权重、参数和调用点未在本轮逐项核对。

## 8. AI、Smart Object 与隔离案例

`Blueprints/AI` 有 NPC Smart Object AIController、StateTree 及任务；`Blueprints/SmartObjects` 有 Bench、SmartObject 动画组件、PoseSearch/Chooser/StateTree 和交互目标选择资源。[清单] 这些是“角色与环境交互”能力的研究入口，但不能默认视为玩家基本移动链的必要依赖。

`IsolatedExamples` 至少有 `Locomotor` 机械角色和 `LookAtPOI` 注视目标案例，分别自带 Pawn/ABP/ControlRig/关卡或数据资产。[清单] 应作为独立专题阅读，不应把它们与沙盒主角色的运行链混为一谈。

## 9. 对当前 MyALS 争论的直接启发（设计判断，不是移植指令）

1. **输入意图与实现结果分开**：C、Shift、空格是请求；“能否蹲、是否冲刺、空间够不够、动画是否占用、运动当前状态”共同决定结果。GASP 的 `S_PlayerInputState` 和 `GetDesiredGait` 提供了结构上的证据。[图]
2. **蹲伏空格是上下文动作**：GASP 的 Space 同时映射 Jump/Traverse，证明可复用按键，但“蹲下按空格究竟跳还是先站”仍需看具体事件图和试玩，不能把 GASP 行为猜成答案。[图]
3. **三个速度常量不足以定义质量**：GASP 的方向速度组、加速度、刹车、摩擦和动态朝向判断值得单项 A/B 实验。[图]
4. **姿态切换要同时看胶囊体和动画**：状态历史、起止转场、Chooser/数据库和实际速度共同影响观感；ABP 不能代替 CMC 的碰撞和运动规则。[图/推断]
5. **不要立即复制 GASP 全套系统**：先用我们的角色跑通“Jog→Crouch→Space 起身→Shift Sprint→松开恢复”的可测链路，再按问题增加数据库、Chooser、Motion Warping 或程序化层。

## 10. 尚待逐项审阅的证据清单

| 专题 | 本轮状态 | 下一步必须读取／验证的内容 |
| --- | --- | --- |
| CMC 输入事件 | 已读函数骨架和事件节点 | EventGraph 的 IA_Crouch/Jump/Sprint 完整引脚、触发事件与冲突优先级 |
| Gait 计算 | 已读 `GetDesiredGait`、`CanSprint` 节点集合 | 分支每条返回路径、枚举显示名、模拟代理状态 |
| 动画选择 | 已读两个 ABP 的状态机和函数目录 | AnimGraph 到 Chooser/PoseSearch 的真实连线、选中数据库、权重、过渡参数 |
| Crouch 动作 | 已确认相关 PSD 分类 | 起身／蹲下动画、胶囊高度变化与相机偏移的时序及运行录像 |
| 穿越 | 已读检测与 Warp Target 节点集合 | 实际 Montage、Notify 窗口、失败路径、Root Motion、网络入口 |
| 镜头 | 已盘点资产族 | Director/Chooser 行、Rig 参数、遮挡和模式切换的实测 |
| Mover | 已读 Pawn/ProduceInput 的结构 | 每个 MovementMode 与 Pre/PostSim 的公式、网络预测与纠错 |
| 音效与 IK | 已盘点资产族 | Notify 时间点、Surface 路由、Control Rig 约束、足部轨迹测量 |
| 全资源 | 3,898 项完成目录盘点 | 尚未逐资产打开内部配置，不能称为“全部读完” |

## 11. 可复现的资产入口

- 工程：`ALS/ALS.uproject`、`ALS/Config/DefaultEngine.ini`
- 游戏入口：`/Game/Levels/DefaultLevel`、`/Game/Blueprints/GM_Sandbox`、`/Game/Blueprints/PC_Sandbox`
- 角色：`/Game/Blueprints/SandboxCharacter_CMC`、`/Game/Blueprints/SandboxCharacter_Mover`
- 动画：`/Game/Blueprints/SandboxCharacter_CMC_ABP`、`/Game/Blueprints/SandboxCharacter_Mover_ABP`
- 输入：`/Game/Input/IMC_Sandbox`、`/Game/Blueprints/Data/S_PlayerInputState`
- 穿越：`/Game/Blueprints/AC_TraversalLogic`
- 镜头：`/Game/Blueprints/Cameras/CameraAsset_SandboxCharacter`、`CameraDirector_SandboxCharacter`
- 数据库：`/Game/Characters/UEFN_Mannequin/Animations/MotionMatchingData`

这份记录以本地工程和 UECP MCP 的只读检查为依据。**没有修改 GASP 资产，也没有编译或声称完成了实际试玩验收。**
