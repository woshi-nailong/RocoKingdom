#include "View/GameIntroduce.h"
#include <graphics.h>
/*
 * 游戏说明
 * 负责人: kumaxi
 * 功能:
 *     1. 左右分栏显示完整的游戏规则
 *     2. 按回车键返回主菜单
 */
void GameIntroduceView() {

    //IMAGE kumaxi_bgImg;
    //loadimage(&kumaxi_bgImg, _T("figure//3.png"), 1427, 830);

    BeginBatchDraw();

    //putimage(0, 0, &kumaxi_bgImg);
    setbkcolor(WHITE);
    cleardevice();
    setbkmode(TRANSPARENT);

    // ═══════════════════════════════════════
    // 标题
    // ═══════════════════════════════════════
    settextcolor(RGB(255, 215, 0));
    settextstyle(36, 0, _T("微软雅黑"));
    outtextxy(550, 15, _T("游 戏 说 明"));

    settextcolor(RGB(0, 0, 0));
    settextstyle(22, 0, _T("微软雅黑"));

    // ═══════════════════════════════════════
    // 左半部分 (x: 50 ~ 713)
    // ═══════════════════════════════════════

    LPCTSTR leftText = _T("【游戏简介】\n")
        _T("双人同屏对战游戏。两位训练家在同一台电脑上轮流操作，\n")
        _T("指挥各自的3只宠物战斗，击败对手全部宠物即为获胜！\n\n")

        _T("【对战流程】\n")
        _T("首发选择 → 玩家1先选首发，玩家2后选\n")
        _T("          (玩家1选择时，玩家2请勿偷看！)\n")
        _T("行动阶段 → 玩家1先操作，确认后玩家2再操作\n")
        _T("          系统按速度判定出手顺序\n")
        _T("          (玩家1操作时，玩家2请回避视线)\n")
        _T("胜负判定 → 一方宠物全灭或逃跑即结束\n\n")

        _T("【属性克制关系】\n")
        _T("  🔥 火 → 克制 → 🌿 草\n")
        _T("  💧 水 → 克制 → 🔥 火\n")
        _T("  🌿 草 → 克制 → 💧 水\n")
        _T(" 克制攻击 ×2，被克制 ×0.5\n\n")

        _T("【对战礼仪】\n")
        _T("同屏对战请自觉遵守\"盲选\"规则：\n")
        _T("对手操作时请回避视线，确保公平！\n");

    RECT leftRect = { 50, 70, 713, 700 };
    drawtext(leftText, &leftRect, DT_WORDBREAK | DT_LEFT | DT_NOCLIP);

    // ═══════════════════════════════════════
    // 右半部分 (x: 714 ~ 1377)
    // ═══════════════════════════════════════

    LPCTSTR rightText = _T("【能量系统】\n")
        _T("● 每只宠物开局 10点能量（上限10点）\n")
        _T("● 释放技能需消耗能量，不足则无法使用\n")
        _T("● 聚能：消耗一回合，回复 5点能量\n")
        _T("  聚能期间仍会受到对手攻击\n\n")

        _T("【四大行动指令】\n")
        _T("  释放技能 → 从4个技能中选择一个\n")
        _T("  聚能 → 回复5能量\n")
        _T("  更换宠物 → 消耗一回合，清空换下宠物的状态\n")
        _T("  逃跑 → 立即结束战斗，逃跑方判负\n\n")

        _T("【队伍配置】\n")
        _T("每位玩家拥有 3只宠物。\n")
        _T("每只宠物可从技能池中挑选 4个技能装备。\n\n")

        _T("【性格系统】\n")
        _T("提升一项属性 +10%，降低另一项 -10%\n")
        _T("受影响的属性：生命、物攻、物防、魔攻、魔防、速度\n")
        _T("⚠ 不能选择同一种属性进行增减\n")
        _T("例：\"物攻+10% 魔攻-10%\"\n")
        _T("→ 物攻110%，魔攻90%\n\n")

        _T("【技能类型】\n")
        _T("⚔ 攻击 → 造成伤害，受属性克制影响\n")
        _T("🛡 防御 → 减伤，用后下回合冷却\n")
        _T("✨ 状态 → 增益自身或减益对手\n\n\n")
        _T("制作团队：（排名不分先后）\n")
        _T("码力觉醒组——组员7名\n")
        _T("Andy、Prospect、Naz、kumaxi、Hermir、iostream、哈哈哈\n");

    RECT rightRect = { 714, 70, 1377, 700 };
    drawtext(rightText, &rightRect, DT_WORDBREAK | DT_LEFT | DT_NOCLIP);

    // ═══════════════════════════════════════
    // 底部提示
    // ═══════════════════════════════════════
    settextcolor(RGB(255, 215, 0));
    settextstyle(28, 0, _T("微软雅黑"));
    outtextxy(530, 740, _T("按 回车 键回到主菜单"));

    FlushBatchDraw();

    ExMessage kumaxi_msg;
    while (true) {
        getmessage(&kumaxi_msg, EX_KEY);
        if (kumaxi_msg.message == WM_KEYDOWN && kumaxi_msg.vkcode == VK_RETURN) {
            EndBatchDraw();
            return;
        }
    }
}
