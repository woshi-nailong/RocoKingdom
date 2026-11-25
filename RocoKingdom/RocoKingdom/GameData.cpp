#include "Service/GameData.h"
#include "Model/type.h"
#include"Model/Player.h"
#include<iostream>
#include <windows.h>
#include <mmeapi.h>
#include <cstdio>
#pragma comment(lib, "winmm.lib")
#include <graphics.h>

PetTemplate petDatabase[3];

// ── 玩家数据 ──
Player player1;
Player player2;

void initPetTemplate() {
// ═══════════════════════════════════════
//  技能定义
// ═══════════════════════════════════════

 // 火系
    Skill F_Blow = { 0, "吹火",       FIRE, ATK,        ATTACK,  50,  1, "造成物伤,每次使用后,物攻+20" };
    Skill F_FlameShield = { 1, "火焰护盾",   FIRE, ATK,        DEFENSE, 70,   2, "减伤70%,如果对方使用攻击技能，双防增加20%" };
    Skill F_Ember = { 2, "火苗",       FIRE, ATK,        ATTACK,  30,  0, "造成物伤,回复1点能量" };
    Skill F_Ram = { 3, "猛烈撞击",   NONE, ATK,        ATTACK,  65,  1, "造成物伤" };
    Skill F_FlameCut = { 4, "火焰切割",   FIRE, ATK,        ATTACK,  100, 3, "造成物伤" };
    Skill F_FireChariot = { 5, "火云车",     FIRE, ATK,        ATTACK,  130, 5, "对敌方精灵造成物理伤害" };
    Skill F_StarStrike = { 6, "星星撞击",   NONE, SP_ATK,     ATTACK,  90,  2, "对敌方精灵造成魔法伤害" };
    Skill F_PowerUp = { 8, "力量增效",   NONE, ATTRIBUTE_NONE, STATUS, 0, 1, "自己获得物攻+100%" };
    Skill F_Shield = { 9, "防御",       NONE, ATTRIBUTE_NONE, DEFENSE,80, 1, "减伤80%" };

    // 水系
    Skill W_WaterGun = { 0,"水枪",       WATER, SP_ATK,     ATTACK,  40,  0, "向对手喷出高压水柱,造成魔法伤害" };
    Skill W_Bubble = { 1,"泡沫",       WATER, SP_ATK,     ATTACK,  50,  1, "向对手喷吐大量泡沫,造成魔法伤害" };
    Skill W_WaterPulse = { 2,"水之波动",   WATER, SP_ATK,     ATTACK,  80,  2, "召唤水之波纹冲击对手,造成较高魔法伤害" };
    Skill W_AzureWind = { 3,"蔚蓝之风",   WATER, SP_ATK,     ATTACK,  70,  2, "吹出冰寒之风造成魔法伤害,自身速度增加20%" };
    Skill W_DeepImpact = { 4,"深海冲击",   WATER, SP_ATK,     ATTACK,  110, 4, "引动深海之力猛击对手,造成大量魔法伤害" };
    Skill W_Slap = { 5,"拍击",       NONE, ATK,        ATTACK,  65,  1, "对敌方精灵造成物理伤害" };
    Skill W_BubbleMirage = { 6,"泡沫幻影",   WATER, ATTRIBUTE_NONE, DEFENSE,80, 1, "减伤80%" };
    Skill W_RainDance = { 7,"祈雨",       WATER, ATTRIBUTE_NONE, STATUS, 0, 2, "回复血量50%" };
    Skill W_Fury = { 8, "怒水",       WATER, ATTRIBUTE_NONE, STATUS, 0, 1, "自己获得魔攻+70%" };

    // 草系
    Skill G_LeafBlade = { 0,"叶刃",       GRASS, ATK,        ATTACK,  40,  0, "挥动叶片切割对手,造成物理伤害" };
    Skill G_VineWhip = { 1,"藤鞭",       GRASS, ATK,        ATTACK,  55,  1, "使用藤蔓鞭打对手,提高自身双防20%" };
    Skill G_RazorLeaf = { 2,"飞叶快刀",   GRASS, ATK,        ATTACK,  70,  2, "高速发射锋利的叶片,造成较高物理伤害" };
    Skill G_Thorn = { 3,"棘突",       GRASS, SP_ATK,     ATTACK,  100, 3, "对敌方精灵造成魔法伤害" };
    Skill G_LeafShield = { 4,"护盾",       GRASS, ATTRIBUTE_NONE, DEFENSE,80, 1, "绿叶护盾,减伤80%" };
    Skill G_SolarBeam = { 5,"阳光烈焰",   GRASS, SP_ATK,     ATTACK,  120, 4, "积蓄阳光发动强力光束,造成大量魔法伤害" };
    Skill G_VineCrush = { 6,"藤绞",       GRASS, ATK,        ATTACK,  100, 3, "对敌方精灵造成物理伤害" };
    Skill G_StarHit = { 7,"星星撞击",   NONE, SP_ATK,     ATTACK,  90,  2, "对敌方精灵造成魔法伤害" };
    Skill G_Fiber = { 8,"纤维化",     GRASS, ATTRIBUTE_NONE, DEFENSE,80, 2, "减伤80%，如果对方使用攻击，自身血量回复20%" };


    // ═══════════════════════════════════════
    //  全局宠物数据
    // ═══════════════════════════════════════

    PetTemplate PT_AquaCup = {
        "水杯灵", WATER,
        260, 180, 222, 262, 268, 212,
        { W_WaterGun, W_Bubble, W_WaterPulse, W_AzureWind,
          W_DeepImpact, W_Slap, W_BubbleMirage, W_RainDance, W_Fury }
    };

    PetTemplate PT_CrimsonScythe = {
        "赤镰姬", FIRE,
        250, 278, 222, 184, 196, 266,
        { F_Blow, F_FlameShield, F_Ember, F_Ram, F_FlameCut,
          F_FireChariot, F_StarStrike, F_PowerUp, F_Shield }
    };

    PetTemplate PT_MoonLotus = {
        "月华萝", GRASS,
        240, 242, 208, 242, 292, 176,
        { G_LeafBlade, G_VineWhip, G_RazorLeaf, G_Thorn,
          G_LeafShield, G_SolarBeam, G_VineCrush, G_StarHit, G_Fiber }
    };

    petDatabase[0] = PT_CrimsonScythe;
    petDatabase[1] = PT_AquaCup;
    petDatabase[2] = PT_MoonLotus;
}



void initPlayerDefault(Player& player)
{
    player.nowPetID = 0;

    for (int i = 0; i < 3; i++)
    {
        Pet& p = player.playerPet[i];
        int petIdx = i;

        p.data = &petDatabase[petIdx];

        // 性格：默认无
        p.buffStat = NATURE_NONE;
        p.debuffStat = NATURE_NONE;
        p.maxHp = petDatabase[petIdx].m_maxHp;
        p.atk = petDatabase[petIdx].m_atk;
        p.def = petDatabase[petIdx].m_def;
        p.spAtk = petDatabase[petIdx].m_spAtk;
        p.spDef = petDatabase[petIdx].m_spDef;
        p.speed = petDatabase[petIdx].m_speed;

        // 战斗初始值
        p.currentHp = p.maxHp;
        p.currentEp = 10;
        p.statusList.clear();
    }
}

void initBattleStart(Player& player1, Player& player2) {
    for (int i = 0; i < 3; i++)
    {
        Pet& p1 = player1.playerPet[i];
        Pet& p2 = player2.playerPet[i];

        // 战斗初始值
        p1.currentHp = p1.maxHp;
        p1.currentEp = 10;
        p1.statusList.clear();
        p2.currentHp = p2.maxHp;
        p2.currentEp = 10;
        p2.statusList.clear();
        player1.defenseCooldown = false;
        player2.defenseCooldown = false;

        for (int j = 0; j < 5; j++) p1.attrBuffs[j] = 0;
        for (int j = 0; j < 5; j++) p2.attrBuffs[j] = 0;
    }
}


void initGameData()
{
    int petNumber = 3;
    initPetTemplate();
    initPlayerDefault(player1);
    initPlayerDefault(player2);
    player1.playerID = 1;
    player2.playerID = 2;
}



// 全局音量设置
int kumaxi_volume = 800;      // 默认80%
bool kumaxi_isMuted = false;  // 默认非静音
wchar_t kumaxi_currentAlias[32] = L"bgm0";

// 场景背景音乐切换函数
void kumaxi_playSceneBGM(const wchar_t* kumaxi_audioPath) {
    if (kumaxi_audioPath == NULL) return;

    // 先关闭上一个场景的音频设备
    mciSendString(L"close all", NULL, 0, NULL);

    // 打开新场景的音频
    wchar_t openCmd[512];
    swprintf_s(openCmd, L"open \"%ls\" alias bgm", kumaxi_audioPath);
    mciSendString(openCmd, NULL, 0, NULL);

    // 绑定当前 initgraph 窗口句柄
    HWND hWnd = GetHWnd();
    if (hWnd != NULL) {
        wchar_t bindCmd[128];
        swprintf_s(bindCmd, L"window bgm handle %p", hWnd);
        mciSendString(bindCmd, NULL, 0, NULL);
    }

    // 设置全局音量（静音时设为0，非静音设为 kumaxi_volume）
    wchar_t volCmd[128];
    swprintf_s(volCmd, L"setaudio bgm volume to %d", kumaxi_isMuted ? 0 : kumaxi_volume);
    mciSendString(volCmd, NULL, 0, NULL);

    // 开始循环播放
    mciSendString(L"play bgm repeat", NULL, 0, NULL);
}