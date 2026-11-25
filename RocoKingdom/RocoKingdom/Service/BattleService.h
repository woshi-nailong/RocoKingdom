#pragma once
#include "../Model/Player.h"
#include "../Model/Action.h"

/*
 * 计算属性克制倍率
 * 负责人: 哈哈哈
 * 功能:
 *     根据攻击属性和防御属性，返回克制倍率
 *     火→草×2.0  火→水×0.5
 *     水→火×2.0  水→草×0.5
 *     草→水×2.0  草→火×0.5
 *     其他情况（含 NONE）→ 1.0
 * 参数:
 *     atkElement: 攻击方属性
 *     defElement: 防御方属性
 * 返回值:
 *     克制倍率(2.0 克制的倍率 / 1.0 正常 / 0.5 抵抗)
 */
int random(int n);
float getTypeMultiplier(PropertyType atkElement, PropertyType defElement);

/*
 * 计算技能伤害
 * 负责人: 哈哈哈
 * 功能:
 *     根据技能威力、属性克制、攻防数值计算最终伤害
 *     物攻技能: 伤害 = 威力 × 克制倍率 × (攻击方atk / 防御方def)
 *     魔攻技能: 伤害 = 威力 × 克制倍率 × (攻击方spAtk / 防御方spDef)
 *     伤害最小为 1
 *     此为基础伤害，后续可能被防御减伤
 * 参数:
 *     skill: 使用的技能
 *     atkElement: 攻击方属性
 *     defElement: 防御方属性
 *     atkStat: 攻击方对应攻击力（物攻或魔攻）
 *     defStat: 防御方对应防御力（物防或魔防）
 * 返回值:
 *     计算后的基础伤害值
 */
int calcDamage(const Skill& skill, PropertyType atkElement, PropertyType defElement, int atkStat, int defStat);

/*
 * 计算防御减伤后的伤害
 * 负责人: 哈哈哈
 * 功能:
 *     攻击方使用攻击技能、防御方使用防御技能时：
 *     双方同时出招，伤害按减免比例计算
 *     减免比例由防御技能决定（如80%、90%）
 * 参数:
 *     baseDamage: calcDamage 算出的基础伤害
 *     reductionRate: 减免比例（0.0~1.0，如0.8表示减免到80%）
 * 返回值:
 *     减免后的实际伤害
 */
int calcDefenseDamage(int baseDamage, float reductionRate);

/*
 * 获取动作类别
 * 负责人: 哈哈哈
 * 功能:
 *     根据玩家本回合的操作，判断其动作属于哪个类别
 *     用于后续的出手顺序判定
 *     使用技能: 根据技能的 SkillType 判断（ATTACK/STATUS/DEFENSE）
 *     聚能:     归类为 STATUS（聚能算状态类）
 *     换宠:     归类为 STATUS（但优先级高于其他操作）
 *     逃跑:     归类为 STATUS
 * 参数:
 *     player: 执行操作的玩家（用于获取当前出战宠物的技能信息）
 *     action: 本回合操作
 * 返回值:
 *     SkillType 类别 (ATTACK / STATUS / DEFENSE)
 *     注: 换宠和逃跑虽然是 STATUS，但 executeRound 内部会优先处理
 */
SkillType getActionCategory(const Player& player, const Action& action);

/*
 * 判定先手方
 * 负责人: 哈哈哈
 * 功能:
 *     比较双方当前出战宠物的速度，决定谁先出手
 *     速度快的先手
 *     速度相同时随机决定（双方各50%概率）
 * 参数:
 *     p1: 玩家1
 *     p2: 玩家2
 * 返回值:
 *     1: 玩家1先手
 *     2: 玩家2先手
 */
int determineFirstAttacker(const Player& p1, const Player& p2);

/*
 * 执行一回合战斗
 * 负责人: hirmir
 * 功能:
 *     根据双方操作，按以下优先级规则执行完整的一回合：
 *     
 *     ① 换宠优先处理：
 *        如果一方操作是换宠，优先执行换宠
 *        双方都换宠则按速度先后执行
 *     
 *     ② 类型匹配判定出手顺序：
 *        - 同类（攻击vs攻击 / 防御vs防御 / 状态vs状态）
 *          按速度先后出手，速度相同随机50%
 *        - 攻击 vs 防御
 *          双方同时出招，防御方按减免比例减伤
 *        - 攻击 vs 状态 / 防御 vs 状态
 *          按速度快慢出手，可能有特殊效果
 *          如攻击应对状态时，可提前打断状态效果
 *     
 *     ③ 执行操作效果：
 *        - 使用技能：计算伤害 → 扣血 → 扣能量
 *        - 聚能：回复能量
 *        - 换宠：切换出战宠物（已提前处理）
 *        - 逃跑：直接判负
 *     
 *     ④ 状态结算：
 *        回合结束时执行状态效果（烧伤扣血/减层等）
 *     
 *     ⑤ 阵亡处理：
 *        操作执行后若宠物阵亡，触发换宠流程
 *        先手方造成阵亡后，后手方仍正常结算
 * 参数:
 *     p1: 玩家1指针（会修改其宠物数据）
 *     p2: 玩家2指针（会修改其宠物数据）
 *     action1: 玩家1本回合操作
 *     action2: 玩家2本回合操作
 */
void executeRound(Player* p1, Player* p2, const Action& action1, const Action& action2);

/*
 * 执行单个操作
 * 负责人: hirmir
 * 功能:
 *     执行一个玩家的本回合操作
 *     根据 Action 类型分别处理：
 *     ACTION_SKILL:      根据技能类型计算伤害/效果
 *     ACTION_GATHER:     回复当前出战宠物的能量
 *     ACTION_CHANGEPET:  切换出战宠物
 *     ACTION_ESCAPE:     标记该玩家逃跑
 * 参数:
 *     self: 操作方玩家指针（会修改其宠物数据）
 *     opponent: 对手玩家指针（用于计算伤害）
 *     action: 本回合操作
 *     isDefensed: 是否需要防御减伤（攻击vs防御时为true）
 *     defenseRate: 防御减伤比例（isDefensed=true时有效）
 */
void executeAction(Player* self, Player* opponent, const Action& action, bool isDefensed, float defenseRate);

/*
 * 执行状态效果
 * 负责人: 哈哈哈
 * 功能:
 *     每回合结束时，对玩家的当前出战宠物应用状态效果
 *     烧伤：层数减半，按层数扣血
 * 参数:
 *     pet: 当前出战宠物指针（会修改其状态和血量）
 */
void applyStatusEffects(Pet* pet);

/*
 * 更换出战宠物
 * 负责人: 哈哈哈
 * 功能:
 *     将玩家的当前出战宠物切换到指定索引
 *     被换下的宠物重置状态（statusList 清空）
 *     被换下的宠物保持当前血量和能量不变（下次上场时恢复）
 *     如果目标宠物已阵亡（currentHp <= 0），切换失败
 * 参数:
 *     player: 玩家指针（会修改 nowPetID）
 *     targetIndex: 目标宠物索引(0~2)
 * 返回值:
 *     true: 切换成功
 *     false: 切换失败（目标宠物阵亡）
 */
bool switchPet(Player* player, int targetIndex);

/*
 * 检查玩家是否有存活宠物
 * 负责人: 哈哈哈
 * 功能:
 *     遍历玩家的三只宠物，检查 currentHp > 0 的宠物数量
 * 参数:
 *     player: 要检查的玩家
 * 返回值:
 *     true: 还有至少一只存活宠物
 *     false: 所有宠物都已阵亡
 */
bool hasAlivePet(const Player& player);

/*
 * 判断游戏胜负
 * 负责人: 哈哈哈
 * 功能:
 *     检查双方是否仍有存活宠物
 *     某一方所有宠物阵亡则对方获胜
 * 参数:
 *     p1: 玩家1
 *     p2: 玩家2
 * 返回值:
 *     0: 游戏未结束
 *     1: 玩家1胜利
 *     2: 玩家2胜利
 */
int checkWin(const Player& p1, const Player& p2);


