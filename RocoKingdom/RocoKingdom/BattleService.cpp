#include "Service/BattleService.h"
#include <algorithm>
#include "Model/type.h"
#include "Model/Skill.h"
#include "Model/Player.h"
#include "Model/Action.h"
#include <random>
#include <cstdlib>

// 每次聚能回复5能量
constexpr int UPEP = 5;

// 能量上限10
constexpr int MAX_EP = 10;


// SkillType的非法值
constexpr SkillType INVALID_SKILL_TYPE = static_cast<SkillType>(-1);


int random(int n)
{
    return (long long)rand() * rand() % n;
}


//计算技能克制抵抗
float getTypeMultiplier(PropertyType atkElement, PropertyType defElement)
{
    if (atkElement == FIRE)
    {
        if (defElement == WATER)
        {
            return 0.5;
        }
        else if (defElement == GRASS)
        {
            return 2.0;
        }
        else
        {
            return 1.0;
        }
    }
    else if (atkElement == WATER)
    {
        if (defElement == FIRE)
        {
            return 2.0;
        }
        else if (defElement == GRASS)
        {
            return 0.5;
        }
        else
        {
            return 1.0;
        }
    }
    else if (atkElement == GRASS)
    {
        if (defElement == WATER)
        {
            return 2.0;
        }
        else if (defElement == FIRE)
        {
            return 0.5;
        }
        else
        {
            return 1.0;
        }
    }
    else
    {
        return 1.0;
    }
}

//计算技能伤害
int calcDamage(const Skill& skill, PropertyType atkElement, PropertyType defElement, int atkStat, int defStat)
{
    return skill.power * getTypeMultiplier(atkElement, defElement) * atkStat / defStat;
}

//计算防御后的技能伤害
int calcDefenseDamage(int baseDamage, float reductionRate)
{
    return std::max(1, static_cast<int>(baseDamage *(1.0F - reductionRate) + 0.5F));
}

//得到技能类型
SkillType getActionCategory(const Player& player, const Action& action)
{



    auto type = player.playerPet[player.nowPetID].skills[action.targetIndex]->type;
    if (action.type == ACTION_SKILL)
    {
        if (type == ATTACK)
        {
            return ATTACK;
        }
        else if (type == STATUS)
        {
            return STATUS;
        }
        else if (type == DEFENSE)
        {
            return DEFENSE;
        }
    }
    else if (action.type == ACTION_CHANGEPET || action.type == ACTION_GATHER || action.type == ACTION_ESCAPE)
    {
        return STATUS;//这里存在问题
    }
}

//决定先手
int determineFirstAttacker(const Player& p1, const Player& p2)
{
    int speed1 = p1.playerPet[p1.nowPetID].speed * (1 + p1.playerPet[p1.nowPetID].attrBuffs[SPEED]) + 0.5F;
    int speed2 = p2.playerPet[p2.nowPetID].speed * (1 + p2.playerPet[p2.nowPetID].attrBuffs[SPEED]) + 0.5F;
    if (speed1 < speed2)
    {
        return 2;
    }
    else if (speed2 < speed1)
    {
        return 1;
    }
    else
    {
        int now = random(2);
        if (now == 1)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
}

//结算状态伤害
void applyStatusEffects(Pet* pet)
{
    for (int i = 0; i < pet->statusList.size(); i++)
    {
        auto& statu = pet->statusList[i];

        if (statu.type == STATE_BURN)
        {
            pet->currentHp -= statu.stack;
            statu.stack /= 2;
            pet->currentHp = std::max(pet->currentHp, 0);

        }
        else if (statu.type == STATE_NONE)
        {
            continue;
        }
    }
}

//切换宠物
bool switchPet(Player* player, int targetIndex)
{
    auto& nowpet = player->playerPet[player->nowPetID];
    auto& nextpet = player->playerPet[targetIndex];
    if (nextpet.currentHp <= 0)
    {
        return false;
    }
    else
    {
        player->nowPetID = targetIndex;
        nowpet.statusList.clear();
        for (int i = 0; i < 5; i++) {
            nowpet.attrBuffs[i] = 0;
        }
        
        return true;
    }
}

//判断是否还有宠物可用
bool hasAlivePet(const Player& player)
{
    for (int i = 0; i < 3; i++)
    {
        if (player.playerPet[i].currentHp > 0)return true;
    }
    return false;
}

//检查是否获胜
int checkWin(const Player& p1, const Player& p2)
{
    if (hasAlivePet(p1) && hasAlivePet(p2))
    {
        return 0;
    }
    else if (hasAlivePet(p1))
    {
        return 1;
    }
    else if (hasAlivePet(p2))
    {
        return 2;
    }
}



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
 *        回合收尾时执行状态效果（烧伤扣血/减层等）
 *
 *     ⑤ 阵亡处理：
 *        操作执行后若宠物阵亡，触发换宠流程
 *        先手方造成阵亡后，后手方仍正常结算
 * 参数:
 *     p1: 玩家1指针（会修改其宠物数据）
 *     p2: 玩家2指针（会修改其宠物数据）
 *     action1: 玩家1本回合操作
 *     action2: 玩家2本回合操作
 *
 *     玩家没有非法操作：能量不足不能点击，防御不能连续放
 *
 */
void executeRound(Player* p1, Player* p2, const Action& action1, const Action& action2) {                          // 根据双方选择结算一个回合。
    if (action1.type == ACTION_ESCAPE || action2.type == ACTION_ESCAPE) {                                          // 逃跑的优先级高于其他动作。
        if (action1.type == ACTION_ESCAPE) {                                                                       // 处理玩家1主动逃跑。
            executeAction(p1, p2, action1, false, 1.0F);                                                           // 将玩家1交给逃跑分支判负。
        }
        if (action2.type == ACTION_ESCAPE) {                                                                       // 处理玩家2主动逃跑。
            executeAction(p2, p1, action2, false, 1.0F);                                                           // 将玩家2交给逃跑分支判负。
        }
        p1->defenseCooldown = false;                                                                               // 清掉玩家1可能残留的防御冷却。
        p2->defenseCooldown = false;                                                                               // 清掉玩家2可能残留的防御冷却。
        return;                                                                                                    // 逃跑后不再结算其他动作。
    }


    bool canExecuteAction1 = true;                                                                                 // 默认允许玩家1执行动作。
    bool canExecuteAction2 = true;                                                                                 // 默认允许玩家2执行动作。

    SkillType category1 = INVALID_SKILL_TYPE;                                                   // 读取玩家1选择的动作类别。
    SkillType category2 = INVALID_SKILL_TYPE;                                                   // 读取玩家2选择的动作类别。

    int first_player = determineFirstAttacker(*p1, *p2);                                                           // 记录本回合的先手玩家。

    if (first_player == 1) {
        if (action1.type == ACTION_GATHER) {


            executeAction(p1, p2, action1, false, 1.0F);

            canExecuteAction1 = false;
        }
        else {
            category1 = getActionCategory(*p1, action1);
        }

        if (action2.type == ACTION_GATHER) {


            executeAction(p2, p1, action2, false, 1.0F);

            canExecuteAction2 = false;
        }
        else {
            category2 = getActionCategory(*p2, action2);
        }
    }
    else {
        if (action2.type == ACTION_GATHER) {


            executeAction(p2, p1, action2, false, 1.0F);

            canExecuteAction2 = false;
        }
        else {
            category2 = getActionCategory(*p2, action2);
        }

        if (action1.type == ACTION_GATHER) {


            executeAction(p1, p2, action1, false, 1.0F);

            canExecuteAction1 = false;
        }
        else {
            category1 = getActionCategory(*p1, action1);
        }
    }

    //if (!canExecuteAction1 && !canExecuteAction2) {
    //    return;
    //}





    if (category1 == DEFENSE) {                                                                                    // 只有防御技能需要检查玩家1的冷却。
        if (p1->defenseCooldown) {                                                                                 // 上一回合防御成功时，本回合不能再防御。
            canExecuteAction1 = false;                                                                             // 跳过玩家1这次防御。
            p1->defenseCooldown = false;                                                                        // 冷却只占用一个回合。
        }
        else {                                                                                                     // 玩家1当前可以尝试防御。
            const Skill* defenseSkill = p1->playerPet[p1->nowPetID].skills[action1.targetIndex];                   // 取得玩家1选择的防御技能。
            if (p1->playerPet[p1->nowPetID].currentEp >= defenseSkill->epCost) {                                   // 能量足够时防御才能生效。
                p1->defenseCooldown = true;                                                                        // 防御成功后，下一回合进入冷却。
            }
            else {                                                                                                 // 能量不足时防御释放失败。
                canExecuteAction1 = false;                                                                         // 失败的防御不会执行。
                p1->defenseCooldown = false;                                                                       // 未成功防御时不产生冷却。
            }
        }
    }
    else {                                                                                                         // 玩家1选择了非防御动作。
        p1->defenseCooldown = false;                                                                               // 非防御动作直接解除冷却。
    }

    if (category2 == DEFENSE) {                                                                                    // 只有防御技能需要检查玩家2的冷却。
        if (p2->defenseCooldown) {                                                                                 // 上一回合防御成功时，本回合不能再防御。
            canExecuteAction2 = false;                                                                             // 跳过玩家2这次防御。
            p2->defenseCooldown = false;                                                                           // 冷却只占用一个回合。
        }
        else {                                                                                                     // 玩家2当前可以尝试防御。
            const Skill* defenseSkill = p2->playerPet[p2->nowPetID].skills[action2.targetIndex];                   // 取得玩家2选择的防御技能。

            if (p2->playerPet[p2->nowPetID].currentEp >= defenseSkill->epCost) {                                   // 能量足够时防御才能生效。
                p2->defenseCooldown = true;                                                                        // 防御成功后，下一回合进入冷却。
            }
            else {                                                                                                 // 能量不足时防御释放失败。
                canExecuteAction2 = false;                                                                         // 失败的防御不会执行。
                p2->defenseCooldown = false;                                                                       // 未成功防御时不产生冷却。
            }
        }
    }
    else {                                                                                                         // 玩家2选择了非防御动作。
        p2->defenseCooldown = false;                                                                               // 非防御动作直接解除冷却。
    }

    if (action1.type == ACTION_CHANGEPET && action2.type == ACTION_CHANGEPET) {                                    // 双方都换宠时按速度决定顺序。
        if (first_player == 1) {                                                                                   // 玩家1取得换宠先手。
            executeAction(p1, p2, action1, false, 1.0F);                                                           // 玩家1先换上目标宠物。
            executeAction(p2, p1, action2, false, 1.0F);                                                           // 玩家2随后换宠。
        }
        else {                                                                                                     // 玩家2取得换宠先手。
            executeAction(p2, p1, action2, false, 1.0F);                                                           // 玩家2先换上目标宠物。
            executeAction(p1, p2, action1, false, 1.0F);                                                           // 玩家1随后换宠。
        }
    }
    else if (action1.type == ACTION_CHANGEPET) {                                                                   // 玩家1单独换宠时优先处理换宠。
        executeAction(p1, p2, action1, false, 1.0F);                                                               // 先换上玩家1选择的宠物。
        if (canExecuteAction2) {                                                                                   // 确认玩家2的动作可以执行。
            executeAction(p2, p1, action2, false, 1.0F);                                                           // 玩家2对新上场的宠物执行动作。
        }
    }
    else if (action2.type == ACTION_CHANGEPET) {                                                                   // 玩家2单独换宠时优先处理换宠。
        executeAction(p2, p1, action2, false, 1.0F);                                                               // 先换上玩家2选择的宠物。
        if (canExecuteAction1) {                                                                                   // 确认玩家1的动作可以执行。
            executeAction(p1, p2, action1, false, 1.0F);                                                           // 玩家1对新上场的宠物执行动作。
        }
    }
    else {                                                                                                         // 双方都没有换宠。
        if (category1 == ATTACK && category2 == DEFENSE) {                                                         // 处理玩家1攻击、玩家2防御。
            if (canExecuteAction2) {                                                                               // 玩家2本回合可以防御。
                const Skill* defenseSkill = p2->playerPet[p2->nowPetID].skills[action2.targetIndex];               // 取得玩家2使用的防御技能。
                const float defenseRate = static_cast<float>(defenseSkill->power) / 100.0F;                        // 把技能百分比换算成伤害保留比例。

                executeAction(p2, p1, action2, false, 1.0F);                                                       // 先扣除玩家2防御技能的能量。
                executeAction(p1, p2, action1, true, defenseRate);                                                 // 玩家1的攻击按防御比例减伤。
                if (defenseSkill->name == "火焰护盾") {
                    p2->playerPet[p2->nowPetID].attrBuffs[DEF] += 0.2f;
                    p2->playerPet[p2->nowPetID].attrBuffs[SP_DEF] += 0.2f;
                }
                if (defenseSkill->name == "纤维化") {
                    Pet& defPet = p2->playerPet[p2->nowPetID];
                    if (defPet.currentHp > 0) {
                        defPet.currentHp = std::min(defPet.maxHp, defPet.currentHp + defPet.maxHp / 5);
                    }
                }
            }
            else {                                                                                                 // 玩家2的防御没有成功。
                executeAction(p1, p2, action1, false, 1.0F);                                                       // 玩家1的攻击造成完整伤害。
            }
        }
        else if (category1 == DEFENSE && category2 == ATTACK) {                                                    // 处理玩家1防御、玩家2攻击。
            if (canExecuteAction1) {                                                                               // 玩家1本回合可以防御。
                const Skill* defenseSkill = p1->playerPet[p1->nowPetID].skills[action1.targetIndex];               // 取得玩家1使用的防御技能。
                const float defenseRate = static_cast<float>(defenseSkill->power) / 100.0F;                        // 把技能百分比换算成伤害保留比例。

                executeAction(p1, p2, action1, false, 1.0F);                                                       // 先扣除玩家1防御技能的能量。
                executeAction(p2, p1, action2, true, defenseRate);                                                 // 玩家2的攻击按防御比例减伤。
                if (defenseSkill->name == "火焰护盾") {
                    p1->playerPet[p1->nowPetID].attrBuffs[DEF] += 0.2f;
                    p1->playerPet[p1->nowPetID].attrBuffs[SP_DEF] += 0.2f;
                }
                if (defenseSkill->name == "纤维化") {
                    Pet& defPet = p1->playerPet[p1->nowPetID];
                    if (defPet.currentHp > 0) {
                        defPet.currentHp = std::min(defPet.maxHp, defPet.currentHp + defPet.maxHp / 5);
                    }
                }
            }
            else {                                                                                                 // 玩家1的防御没有成功。
                executeAction(p2, p1, action2, false, 1.0F);                                                       // 玩家2的攻击造成完整伤害。
            }
        }
        else {                                                                                                     // 其他动作组合按照速度执行。
            if (first_player == 1) {                                                                               // 玩家1取得普通动作先手。
                if (canExecuteAction1) {                                                                           // 确认玩家1的动作可以执行。
                    executeAction(p1, p2, action1, false, 1.0F);                                                   // 玩家1先行动。
                }

                if (canExecuteAction2 && p2->playerPet[p2->nowPetID].currentHp > 0) {                                                                           // 确认玩家2的动作可以执行。
                    executeAction(p2, p1, action2, false, 1.0F);                                                   // 玩家2后行动。
                }
            }
            else {                                                                                                 // 玩家2取得普通动作先手。
                if (canExecuteAction2) {                                                                           // 确认玩家2的动作可以执行。
                    executeAction(p2, p1, action2, false, 1.0F);                                                   // 玩家2先行动。
                }

                if (canExecuteAction1 && p1->playerPet[p1->nowPetID].currentHp > 0) {                                                                           // 确认玩家1的动作可以执行。
                    executeAction(p1, p2, action1, false, 1.0F);                                                   // 玩家1后行动。
                }
            }
        }
    }

    applyStatusEffects(&p1->playerPet[p1->nowPetID]);                                                              // 结算玩家1当前宠物的持续状态。
    applyStatusEffects(&p2->playerPet[p2->nowPetID]);                                                              // 结算玩家2当前宠物的持续状态。

    if (!hasAlivePet(*p1) || !hasAlivePet(*p2)) {                                                                  // 任意一方全队阵亡时清理对局状态。
        p1->defenseCooldown = false;                                                                               // 清掉玩家1的防御冷却。
        p2->defenseCooldown = false;                                                                               // 清掉玩家2的防御冷却。
    }
}

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
void executeAction(Player* self, Player* opponent, const Action& action, bool isDefensed, float defenseRate) {     // 执行 self 本回合选中的一个动作。
    Pet& selfPet = self->playerPet[self->nowPetID];                                                                // 先拿到操作方当前出战宠物，下面几个分支都会用。

    switch (action.type) {                                                                                         // 根据动作类型进入对应分支。
    case ACTION_ESCAPE:                                                                                            // 处理逃跑。
        for (Pet& pet : self->playerPet) {                                                                         // 遍历逃跑方队伍里的三只宠物。
            pet.currentHp = 0;                                                                                     // Player 里没有逃跑标记，只能把全队血量清零交给 checkWin 判断。
        }
        break;                                                                                                     // 不再匹配其他动作类型。

    case ACTION_GATHER:                                                                                            // 处理聚能。


        selfPet.currentEp = std::min(MAX_EP, selfPet.currentEp + UPEP);                                                // 给当前宠物回复固定的能量值，目前还没有设置上限。
        break;


    case ACTION_CHANGEPET:                                                                                         // 处理主动换宠。
        switchPet(self, action.targetIndex);                                                                       // 目标能不能上场由 switchPet 自己判断。
        break;


    case ACTION_SKILL: {                                                                                           // 处理技能动作，额外作用域方便保存 skill 引用。
        const Skill& skill = *selfPet.skills[action.targetIndex];                                                  // targetIndex 对应当前宠物的技能槽。

        if (selfPet.currentEp < skill.epCost) {                                                                    // 当前能量不够支付技能消耗。
            return;                                                                                                // 技能释放失败，能量和其他数据都不改。
        }

        selfPet.currentEp -= skill.epCost;                                                                         // 技能能用，先扣除本次消耗。

        if (skill.type == ATTACK) {                                                                                // 攻击技能需要计算并扣除对方血量。
            Pet& opponent_pet = opponent->playerPet[opponent->nowPetID];                                           // 攻击目标是对方当前出战宠物。
            int selfAtk = selfPet.atk * (1 + selfPet.attrBuffs[ATK]);;                                                                             // 默认先取物理攻击。
            int opponent_def = opponent_pet.def * (1 + opponent_pet.attrBuffs[DEF]);                                                                   // 默认先取对方物理防御。

            if (skill.attr == SP_ATK) {                                                                            // SP_ATK 表示这个技能走魔攻公式。
                selfAtk = selfPet.spAtk * (1 + selfPet.attrBuffs[SP_ATK]);                                                                           // 攻击数值换成魔法攻击。
                opponent_def = opponent_pet.spDef * (1 + opponent_pet.attrBuffs[SP_DEF]);                                                                 // 防御数值也换成魔法防御。
            }

            int demage = calcDamage(skill, skill.element, opponent_pet.data->m_element, selfAtk, opponent_def);    // 计算属性克制和攻防共同决定的基础伤害。

            //一些特殊技能的处理
            if (skill.name == "吹火")
                selfPet.attrBuffs[ATK] += 0.2f;      // 使用后，下次起效

            if (skill.name == "火苗")
                selfPet.currentEp = std::min(MAX_EP, selfPet.currentEp + 1);

            if (skill.name == "蔚蓝之风")
                selfPet.attrBuffs[SPEED] += 0.2f;

            if (skill.name == "藤鞭") {
                selfPet.attrBuffs[DEF] += 0.2f;
                selfPet.attrBuffs[SP_DEF] += 0.2f;
            }



            if (isDefensed) {                                                                                      // 对面本回合的防御技能成功了。
                demage = calcDefenseDamage(demage, defenseRate);                                                   // 按防御比例重新计算实际伤害。
            }

            opponent_pet.currentHp = std::max(0, opponent_pet.currentHp - demage);                                 // 扣血，并把最低值卡在零。



        }
        else if (skill.type == STATUS) {                                                                           // 当前的状态技能统一按烧伤处理。

            Pet& opponent_pet = opponent->playerPet[opponent->nowPetID];                                           // 状态加到对方当前宠物身上。
            int burnAtk = std::max(1, skill.power);                                                                // 暂时用 power 表示烧伤层数，至少加一层。
            bool hasburnAtk = false;                                                                               // 记录目标身上是否已经有烧伤。

            //状态技能的处理
            if (skill.name == "祈雨") {
                selfPet.currentHp = std::min(selfPet.maxHp, selfPet.currentHp + selfPet.maxHp / 2);
            }
            if (skill.name == "力量增效") {
                selfPet.attrBuffs[ATK] += 1.0f;
            }
            if (skill.name == "怒水") {
                selfPet.attrBuffs[SP_ATK] += 0.7f;
            }


            if (skill.power > 0) {
                for (StatusEffect& statue : opponent_pet.statusList) {                                                 // 遍历对方已有状态，避免重复建烧伤记录。
                    if (statue.type == STATE_BURN) {                                                                   // 找到原有的烧伤状态。
                        statue.stack += burnAtk;                                                                       // 直接叠加这次技能带来的层数。
                        hasburnAtk = true;                                                                             // 记下已经找到烧伤。
                        break;                                                                                         // 找到烧伤后停止遍历。
                    }
                }


                if (!hasburnAtk) {                                                                                     // 原先没有烧伤，需要新建一条状态。
                    opponent_pet.statusList.push_back({ STATE_BURN, burnAtk });                                        // 保存烧伤类型和初始层数。
                }
            }
            }

            

        break;

    }
    }
}
