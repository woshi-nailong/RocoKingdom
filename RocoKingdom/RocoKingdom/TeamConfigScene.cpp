#include <graphics.h>
#include <conio.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cwchar>

#include "View/TeamConfigScene.h"
#include "Service/GameData.h"
#include "Service/DataService.h"

using namespace std;

// ==========================================
// 1. 布局与尺寸常量
// ==========================================
constexpr int WIN_W = 1427;
constexpr int WIN_H = 830;
constexpr COLORREF BG_COLOR = RGB(135, 206, 235); // 天蓝色
constexpr COLORREF BTN_BG = RGB(255, 255, 255);
constexpr COLORREF BTN_TEXT = RGB(50, 50, 50);
constexpr COLORREF SELECTED_BG = RGB(200, 200, 200);
constexpr COLORREF HIGHLIGHT_BG = RGB(100, 200, 100); // 选中高亮绿色

// 左侧宠物栏
constexpr int PANEL_LEFT_X = 100;
constexpr int PANEL_LEFT_W = 120;
constexpr int PANEL_LEFT_H = 60;

// 中部属性面板
constexpr int AVATAR_RADIUS = 100;

// 箭头按钮位置
constexpr int ARROW_X_OFFSET = 60;
constexpr int ARROW_Y_BASE = 430;
constexpr int ARROW_SIZE = 40;

// 技能栏 (2列 x 4行)
constexpr int SKILL_START_X = 820;
constexpr int SKILL_START_Y = 180;
constexpr int SKILL_W = 100;
constexpr int SKILL_H = 55;
constexpr int SKILL_GAP = 25;
constexpr int SKILL_COUNT = 8;

// ==========================================
// 2. 字符串编码转换
// ==========================================
// PetChoiceScene 的转换函数，复制过来用
static wstring toWide(const string& str) {
    if (str.empty()) return L"";
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    if (len == 0) {
        len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
        wstring wstr(len, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
        return wstr;
    }
    wstring wstr(len, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}

// ==========================================
// 2. 属性名 → NatureStat 映射
// ==========================================
static const wchar_t* attrNames[6] = { L"物攻", L"魔攻", L"物防", L"魔防", L"速度", L"生命" };

NatureStat uiRowToNature(int row) {
    switch (row) {
    case 0: return NATURE_ATK;
    case 1: return NATURE_SP_ATK;
    case 2: return NATURE_DEF;
    case 3: return NATURE_SP_DEF;
    case 4: return NATURE_SPEED;
    case 5: return NATURE_HP;
    default: return NATURE_NONE;
    }
}

int natureToUiRow(NatureStat ns) {
    switch (ns) {
    case NATURE_ATK:    return 0;
    case NATURE_SP_ATK: return 1;
    case NATURE_DEF:    return 2;
    case NATURE_SP_DEF: return 3;
    case NATURE_SPEED:  return 4;
    case NATURE_HP:     return 5;
    default:            return -1;
    }
}

const wchar_t* natureName(NatureStat ns) {
    switch (ns) {
    case NATURE_HP:     return L"生命";
    case NATURE_ATK:    return L"物攻";
    case NATURE_DEF:    return L"物防";
    case NATURE_SP_ATK: return L"魔攻";
    case NATURE_SP_DEF: return L"魔防";
    case NATURE_SPEED:  return L"速度";
    default:            return L"无";
    }
}

// ==========================================
// 3. 绘图辅助函数（宽字符版）
// ==========================================

void DrawButton(int x, int y, int w, int h, const wchar_t* text, COLORREF bg = BTN_BG, COLORREF fg = BTN_TEXT) {
    setfillcolor(bg);
    setlinecolor(RGB(180, 180, 180));
    fillrectangle(x, y, x + w, y + h);
    rectangle(x, y, x + w, y + h);

    settextcolor(fg);
    setbkmode(TRANSPARENT);
    int textW = textwidth(text);
    int textH = textheight(text);
    outtextxy(x + (w - textW) / 2, y + (h - textH) / 2, text);
}

// 头像图片缓存（和战斗场景共用同一组图片）
static IMAGE g_avatarImgs[3];
static bool g_avatarLoaded = false;

void loadAvatarImages() {
    if (g_avatarLoaded) return;
    for (int i = 0; i < 3; i++) {
        wchar_t path[64];
        swprintf_s(path, L"..\\image\\avatar_%d.png", i);
        loadimage(&g_avatarImgs[i], path, AVATAR_RADIUS * 2, AVATAR_RADIUS * 2);
    }
    g_avatarLoaded = true;
}

void DrawAvatar(int x, int y, int r, int petIndex = -1) {
    loadAvatarImages();
    if (petIndex >= 0 && petIndex < 3 && g_avatarImgs[petIndex].getwidth() > 0) {
        // 画圆形头像
        setfillcolor(RGB(120, 120, 120));
        solidcircle(x, y, r);
        putimage(x - r, y - r, &g_avatarImgs[petIndex]);
        setlinecolor(RGB(0, 0, 0));
        circle(x, y, r);
    } else {
        // 没有图片时画灰色圆
        setfillcolor(RGB(120, 120, 120));
        setlinecolor(RGB(200, 200, 200));
        fillcircle(x, y, r);
        setlinecolor(RGB(0, 0, 0));
        circle(x, y, r);
    }
}

void DrawAttrBar(int x, int y, const wchar_t* name, bool highlighted = false) {
    setfillcolor(highlighted ? RGB(255, 200, 50) : RGB(30, 144, 255));
    fillrectangle(x, y, x + 50, y + 22);
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    outtextxy(x + 5, y + 3, name);
}

void DrawArrowButton(int x, int y, bool isUp, int percent = 0) {
    setfillcolor(RGB(220, 220, 220));
    setlinecolor(RGB(150, 150, 150));
    fillrectangle(x, y, x + ARROW_SIZE, y + ARROW_SIZE);

    int cx = x + ARROW_SIZE / 2;
    int cy = y + ARROW_SIZE / 2;
    setfillcolor(BLACK);

    POINT pts[3];
    if (isUp) {
        pts[0] = { cx, y + 5 };
        pts[1] = { x + 5, y + ARROW_SIZE - 5 };
        pts[2] = { x + ARROW_SIZE - 5, y + ARROW_SIZE - 5 };
    }
    else {
        pts[0] = { cx, y + ARROW_SIZE - 5 };
        pts[1] = { x + 5, y + 5 };
        pts[2] = { x + ARROW_SIZE - 5, y + 5 };
    }
    fillpolygon(pts, 3);

    if (percent != 0) {
        settextcolor(percent > 0 ? RGB(255, 0, 0) : RGB(0, 0, 255));
        wchar_t txt[10];
        swprintf_s(txt, L"%d%%", percent);
        outtextxy(x + 5, y + ARROW_SIZE + 2, txt);
    }
}

// ==========================================
// 4. 主界面绘制逻辑（宽字符版）
// ==========================================
void DrawInterface(const Player& player, int selectedPetIdx,
    int buffRow, int debuffRow,
    const int skillSelectedIds[4], int skillCount,
    const int skillPage) {

    const Pet& curPet = player.playerPet[selectedPetIdx];
    const PetTemplate* curTmpl = curPet.data;

    BeginBatchDraw();
    cleardevice();

    // ── 全屏背景图 ──
    static IMAGE g_bk;
    static bool g_bkLoaded = false;
    if (!g_bkLoaded) {
        loadimage(&g_bk, L"figure\\3.png", WIN_W, WIN_H);
        g_bkLoaded = true;
    }
    putimage(0, 0, &g_bk);

    // 1. 标题栏
    settextcolor(YELLOW);
    settextstyle(40, 0, L"微软雅黑");
    outtextxy(50, 20, L"宠物配置");

    DrawButton(450, 20, 100, 50, L"玩家1",
        player.playerID == 1 ? SELECTED_BG : BTN_BG);
    DrawButton(550, 20, 100, 50, L"玩家2",
        player.playerID == 2 ? SELECTED_BG : BTN_BG);

    // 2. 左侧宠物列表
    for (int i = 0; i < 3; i++) {
        COLORREF bg = (i == selectedPetIdx) ? RGB(200, 200, 200) : RGB(240, 240, 240);
        wstring pname = player.playerPet[i].data
            ? toWide(player.playerPet[i].data->m_name) : L"空";
        DrawButton(PANEL_LEFT_X, 150 + i * 90, PANEL_LEFT_W, PANEL_LEFT_H, pname.c_str(), bg);
    }

    // 3. 中间卡片
    int cardX = 280, cardY = 160, cardW = 500, cardH = 1000;
    setfillcolor(WHITE);
    setlinecolor(RGB(200, 200, 200));
    fillrectangle(cardX, cardY, cardX + cardW, cardY + cardH);
    rectangle(cardX, cardY, cardX + cardW, cardY + cardH);

    // 名称
    settextcolor(BLACK);
    settextstyle(24, 0, L"微软雅黑");
    int centerX = cardX + cardW / 2;
    wstring petNameW = curTmpl ? toWide(curTmpl->m_name) : L"未知";
    outtextxy(centerX - textwidth(petNameW.c_str()) / 2, cardY + 20, petNameW.c_str());

    // 属性图标文字
    if (curTmpl) {
        const wchar_t* elemStr = L"";
        switch (curTmpl->m_element) {
        case FIRE:  elemStr = L"🔥火"; break;
        case WATER: elemStr = L"💧水"; break;
        case GRASS: elemStr = L"🌿草"; break;
        default:    elemStr = L""; break;
        }
        settextcolor(RGB(100, 100, 100));
        settextstyle(18, 0, L"微软雅黑");
        outtextxy(centerX - 20, cardY + 55, elemStr);
    }

    // 头像
    DrawAvatar(centerX, cardY + 120, AVATAR_RADIUS, selectedPetIdx);

    // 属性列
    int colStartY = cardY + 390;
    int colStartX = cardX + 40;

    DrawArrowButton(colStartX, colStartY - ARROW_SIZE - 20, true, 10);
    DrawArrowButton(colStartX + 100, colStartY - ARROW_SIZE - 20, false, -10);

    for (int col = 0; col < 2; col++) {
        int currentColX = colStartX + col * 100;
        for (int row = 0; row < 6; row++) {
            int x = currentColX;
            int y = colStartY + row * 30;
            bool hilite = (col == 0 && row == buffRow) || (col == 1 && row == debuffRow);
            DrawAttrBar(x, y, attrNames[row], hilite);
        }
    }

    // 5. 战斗能力文字
    int infoX = cardX + 320;
    int infoY = colStartY + 10;
    settextcolor(BLACK);
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(infoX, infoY, L"战斗能力");

    wchar_t buff[100];
    swprintf_s(buff, L"生命: %d  物防: %d", curPet.maxHp, curPet.def);
    outtextxy(infoX, infoY + 35, buff);
    swprintf_s(buff, L"物攻: %d  魔防: %d", curPet.atk, curPet.spDef);
    outtextxy(infoX, infoY + 65, buff);
    swprintf_s(buff, L"魔攻: %d  速度: %d", curPet.spAtk, curPet.speed);
    outtextxy(infoX, infoY + 95, buff);

    // 性格信息
    if (buffRow >= 0 && debuffRow >= 0 || (buffRow == -1 && debuffRow == -1)) {
        settextcolor(RGB(85, 177, 85));
        settextstyle(16, 0, L"微软雅黑");
        wchar_t natureText[64];
        if (buffRow >= 0 && debuffRow >= 0)swprintf_s(natureText, L"性格: %s+10%%  %s-10%%", attrNames[buffRow], attrNames[debuffRow]);
        else {
            swprintf_s(natureText, L"平衡");
        }
        outtextxy(infoX, infoY + 130, natureText);
    }
    else {
        settextcolor(RGB(200, 80, 0));
        settextstyle(16, 0, L"微软雅黑");
        wchar_t natureText[64];
        swprintf_s(natureText, L"等待选择");
        outtextxy(infoX, infoY + 130, natureText);
    }


    // 6. 技能
    int poolSize = curTmpl ? (int)curTmpl->skillPool.size() : 0;
    int startIdx = skillPage * SKILL_COUNT;
    for (int i = 0; i < SKILL_COUNT; i++) {
        int idx = startIdx + i;
        int col = i % 2;
        int row = i / 2;
        int sx = SKILL_START_X + col * (SKILL_W + SKILL_GAP);
        int sy = SKILL_START_Y + row * (SKILL_H + SKILL_GAP);
        if (idx < poolSize) {
            const Skill& sk = curTmpl->skillPool[idx];
            bool isSelected = false;
            for (int s = 0; s < skillCount; s++) {
                if (skillSelectedIds[s] == sk.id) { isSelected = true; break; }
            }
            wchar_t skBtnText[64];
            swprintf_s(skBtnText, L"%s(%d)", toWide(sk.name).c_str(), sk.epCost);
            const wchar_t* typeTag = L"";
            if (sk.type == DEFENSE) typeTag = L"[盾]";
            else if (sk.type == STATUS) typeTag = L"[状]";
            wchar_t fullText[80];
            swprintf_s(fullText, L"%s%s", typeTag, skBtnText);
            DrawButton(sx, sy, SKILL_W, SKILL_H, fullText,
                isSelected ? HIGHLIGHT_BG : BTN_BG);
        }
        else {
            DrawButton(sx, sy, SKILL_W, SKILL_H, L"--空--", RGB(230, 230, 230));
        }
    }

    // 7. 翻页箭头
    int arrowX = SKILL_START_X + 2 * (SKILL_W + SKILL_GAP) + 15;
    int arrowY = SKILL_START_Y + 3 * (SKILL_H + SKILL_GAP) + SKILL_H / 2;
    setfillcolor(RGB(240, 240, 240));
    setlinecolor(RGB(150, 150, 150));
    fillcircle(arrowX, arrowY, 20);
    settextcolor(BLACK);
    settextstyle(20, 0, L"微软雅黑");
    outtextxy(arrowX - 5, arrowY - 12, L">");

    // 已选技能
    settextcolor(RGB(80, 80, 80));
    settextstyle(15, 0, L"微软雅黑");
    wchar_t selText[128] = L"已选技能: ";
    wchar_t tmp[32];
    for (int s = 0; s < skillCount; s++) {
        if (curTmpl) {
            for (int k = 0; k < poolSize; k++) {
                if (curTmpl->skillPool[k].id == skillSelectedIds[s]) {
                    swprintf_s(tmp, L"[%s]", toWide(curTmpl->skillPool[k].name).c_str());
                    wcscat_s(selText, tmp);
                    break;
                }
            }
        }
    }
    outtextxy(SKILL_START_X, SKILL_START_Y + 4 * (SKILL_H + SKILL_GAP) + 10, selText);

    // 8. 底部按钮
    int btnY = WIN_H - 80;
    /* DrawButton(850, btnY, 100, 40, L"显示技能信息");*/
    DrawButton(980, btnY, 100, 40, L"保存");
    DrawButton(1110, btnY, 100, 40, L"返回");

    EndBatchDraw();
}

// ==========================================
// 5. 辅助函数
// ==========================================

static Player& curPlayer(int id) {
    return (id == 1) ? player1 : player2;
}

void loadPlayerState(int playerId, int& selectedPetIdx, int& buffRow, int& debuffRow,
    int skillSel[3][4], int skillCnt[3], int& skillPage) {
    Player& player = curPlayer(playerId);
    selectedPetIdx = 0;
    skillPage = 0;
    for (int p = 0; p < 3; p++) {
        const Pet& pet = player.playerPet[p];
        skillCnt[p] = 0;
        for (int j = 0; j < 4; j++) {
            if (pet.skills[j] != nullptr) {
                skillSel[p][skillCnt[p]++] = pet.skills[j]->id;
            }
        }
    }
    const Pet& cp = player.playerPet[0];
    buffRow = natureToUiRow(cp.buffStat);
    debuffRow = natureToUiRow(cp.debuffStat);
}

// ==========================================
// 6. 队伍配置主函数
// ==========================================
void teamConfigView() {
    initgraph(WIN_W, WIN_H);
    setbkcolor(BG_COLOR);

    int currentPlayerId = 1;
    int selectedPetIdx = 0;
    int skillPage = 0;

    int skillSel[3][4];
    int skillCnt[3] = { 0,0,0 };
    int buffRow = -1, debuffRow = -1;
    loadPlayerState(currentPlayerId, selectedPetIdx, buffRow, debuffRow,
        skillSel, skillCnt, skillPage);

    Player& player = curPlayer(currentPlayerId);
    DrawInterface(player, selectedPetIdx, buffRow, debuffRow,
        skillSel[selectedPetIdx], skillCnt[selectedPetIdx], skillPage);

    int cardX = 280, cardY = 160;
    int colStartY = cardY + 390;
    int colStartX = cardX + 40;

    ExMessage msg;
    bool running = true;
    int hoverSkillIdx = -1;  // 当前悬停的技能按钮索引，-1 表示无悬停
    while (running) {
        if (peekmessage(&msg, EM_MOUSE)) {
            // ═══════════════════════════════════════
            // 鼠标悬停：显示技能描述浮窗
            // ═══════════════════════════════════════
            if (msg.message == WM_MOUSEMOVE) {
                const PetTemplate* h_tmpl = curPlayer(currentPlayerId).playerPet[selectedPetIdx].data;
                int h_poolSize = h_tmpl ? (int)h_tmpl->skillPool.size() : 0;
                int h_startIdx = skillPage * SKILL_COUNT;

                int newHover = -1;
                for (int i = 0; i < SKILL_COUNT; i++) {
                    int idx = h_startIdx + i;
                    if (idx >= h_poolSize) break;
                    int col = i % 2;
                    int row = i / 2;
                    int sx = SKILL_START_X + col * (SKILL_W + SKILL_GAP);
                    int sy = SKILL_START_Y + row * (SKILL_H + SKILL_GAP);
                    if (msg.x >= sx && msg.x <= sx + SKILL_W &&
                        msg.y >= sy && msg.y <= sy + SKILL_H) {
                        newHover = i;
                        break;
                    }
                }

                if (newHover != hoverSkillIdx) {
                    hoverSkillIdx = newHover;
                    Player& h_player = curPlayer(currentPlayerId);
                    DrawInterface(h_player, selectedPetIdx, buffRow, debuffRow,
                        skillSel[selectedPetIdx], skillCnt[selectedPetIdx], skillPage);

                    if (hoverSkillIdx >= 0 && h_tmpl) {
                        int skIdx = h_startIdx + hoverSkillIdx;
                        const Skill& sk = h_tmpl->skillPool[skIdx];

                        // 浮窗定位：技能区下方空白处
                        int tipX = 820;
                        int tipY = 570;
                        int tipW = 370;
                        int tipH = 80;

                        setfillcolor(RGB(255, 255, 220));
                        setlinecolor(RGB(200, 180, 60));
                        fillrectangle(tipX, tipY, tipX + tipW, tipY + tipH);
                        rectangle(tipX, tipY, tipX + tipW, tipY + tipH);

                        settextcolor(RGB(50, 50, 50));
                        settextstyle(17, 0, L"微软雅黑");
                        wstring skNameW = toWide(sk.name);
                        outtextxy(tipX + 10, tipY + 6, skNameW.c_str());

                        settextstyle(14, 0, L"微软雅黑");
                        settextcolor(RGB(80, 80, 80));
                        wstring descW = toWide(sk.description);
                        // 描述过长时截断显示
                        if (descW.length() > 30) {
                            descW = descW.substr(0, 28) + L"..";
                        }
                        outtextxy(tipX + 10, tipY + 30, descW.c_str());

                        wchar_t info[64];
                        swprintf_s(info, L"威力:%d  EP消耗:%d", sk.power, sk.epCost);
                        settextcolor(RGB(120, 120, 120));
                        outtextxy(tipX + 10, tipY + 52, info);
                    }
                }
            }

            if (msg.message == WM_LBUTTONDOWN) {
                int mx = msg.x, my = msg.y;
                Player& player = curPlayer(currentPlayerId);

                // ── 玩家切换 ──
                if (mx >= 450 && mx <= 550 && my >= 20 && my <= 70) {
                    if (currentPlayerId != 1) {
                        currentPlayerId = 1;
                        loadPlayerState(1, selectedPetIdx, buffRow, debuffRow,
                            skillSel, skillCnt, skillPage);
                        DrawInterface(curPlayer(1), selectedPetIdx, buffRow, debuffRow,
                            skillSel[selectedPetIdx], skillCnt[selectedPetIdx], skillPage);
                    }
                }
                if (mx >= 550 && mx <= 650 && my >= 20 && my <= 70) {
                    if (currentPlayerId != 2) {
                        currentPlayerId = 2;
                        loadPlayerState(2, selectedPetIdx, buffRow, debuffRow,
                            skillSel, skillCnt, skillPage);
                        DrawInterface(curPlayer(2), selectedPetIdx, buffRow, debuffRow,
                            skillSel[selectedPetIdx], skillCnt[selectedPetIdx], skillPage);
                    }
                }

                // ── 左侧宠物列表 ──
                for (int i = 0; i < 3; i++) {
                    int bx = PANEL_LEFT_X;
                    int by = 150 + i * 90;
                    if (mx >= bx && mx <= bx + PANEL_LEFT_W &&
                        my >= by && my <= by + PANEL_LEFT_H) {
                        selectedPetIdx = i;
                        const Pet& cp = player.playerPet[selectedPetIdx];
                        buffRow = natureToUiRow(cp.buffStat);
                        debuffRow = natureToUiRow(cp.debuffStat);
                        skillPage = 0;
                        DrawInterface(player, selectedPetIdx, buffRow, debuffRow,
                            skillSel[selectedPetIdx], skillCnt[selectedPetIdx], skillPage);
                        break;
                    }
                }

                // ── 性格列 ──
                int attrColLeft = colStartX;
                int attrColRight = colStartX + 100;
                for (int row = 0; row < 6; row++) {
                    int ay = colStartY + row * 30;

                    // 点击左侧列（buff）
                    if (mx >= attrColLeft && mx <= attrColLeft + 50 &&
                        my >= ay && my <= ay + 22) {

                        // ===== 判断：如果该行已经被右侧列选中（debuff），则不允许 =====
                        if (debuffRow == row) {
                            // 同行不同列，不能同时选中
                            break; // 或者弹出提示，或者不执行任何操作
                        }
                        if (buffRow == row) buffRow = -1;
                        else buffRow = row;
                        player.playerPet[selectedPetIdx].buffStat = (buffRow >= 0) ? uiRowToNature(buffRow) : NATURE_NONE;
                        DrawInterface(player, selectedPetIdx, buffRow, debuffRow,
                            skillSel[selectedPetIdx], skillCnt[selectedPetIdx], skillPage);
                        break;
                    }

                    // 点击右侧列（debuff）
                    if (mx >= attrColRight && mx <= attrColRight + 50 &&
                        my >= ay && my <= ay + 22) {

                        // ===== 判断：如果该行已经被左侧列选中（buff），则不允许 =====
                        if (buffRow == row) {
                            // 同行不同列，不能同时选中
                            break; // 或者弹出提示，或者不执行任何操作
                        }

                        // 正常切换 debuff 选中状态
                        if (debuffRow == row) debuffRow = -1;
                        else debuffRow = row;
                        player.playerPet[selectedPetIdx].debuffStat = (debuffRow >= 0) ? uiRowToNature(debuffRow) : NATURE_NONE;
                        DrawInterface(player, selectedPetIdx, buffRow, debuffRow,
                            skillSel[selectedPetIdx], skillCnt[selectedPetIdx], skillPage);
                        break;
                    }
                }

                // ── 技能选择 ──
                const PetTemplate* curTmpl = player.playerPet[selectedPetIdx].data;
                int poolSize = curTmpl ? (int)curTmpl->skillPool.size() : 0;
                int startIdx = skillPage * SKILL_COUNT;
                int petIdx = selectedPetIdx;
                for (int i = 0; i < SKILL_COUNT; i++) {
                    int idx = startIdx + i;
                    if (idx >= poolSize) break;
                    int col = i % 2;
                    int row = i / 2;
                    int sx = SKILL_START_X + col * (SKILL_W + SKILL_GAP);
                    int sy = SKILL_START_Y + row * (SKILL_H + SKILL_GAP);
                    if (mx >= sx && mx <= sx + SKILL_W &&
                        my >= sy && my <= sy + SKILL_H) {
                        int skId = curTmpl->skillPool[idx].id;
                        bool found = false;
                        int foundPos = -1;
                        for (int s = 0; s < skillCnt[petIdx]; s++) {
                            if (skillSel[petIdx][s] == skId) {
                                found = true; foundPos = s; break;
                            }
                        }
                        if (found) {
                            for (int s = foundPos; s < skillCnt[petIdx] - 1; s++)
                                skillSel[petIdx][s] = skillSel[petIdx][s + 1];
                            skillCnt[petIdx]--;
                        }
                        else {
                            if (skillCnt[petIdx] < 4)
                                skillSel[petIdx][skillCnt[petIdx]++] = skId;
                        }
                        DrawInterface(player, selectedPetIdx, buffRow, debuffRow,
                            skillSel[petIdx], skillCnt[petIdx], skillPage);
                        break;
                    }
                }

                // ── 翻页 ──
                {
                    int arrowX = SKILL_START_X + 2 * (SKILL_W + SKILL_GAP) + 15;
                    int arrowY = SKILL_START_Y + 3 * (SKILL_H + SKILL_GAP) + SKILL_H / 2;
                    int r = 20;
                    if ((mx - arrowX) * (mx - arrowX) + (my - arrowY) * (my - arrowY) <= r * r) {
                        if (curTmpl) {
                            int maxPage = (poolSize + SKILL_COUNT - 1) / SKILL_COUNT - 1;
                            skillPage = (skillPage + 1 > maxPage) ? 0 : skillPage + 1;
                        }
                        DrawInterface(player, selectedPetIdx, buffRow, debuffRow,
                            skillSel[petIdx], skillCnt[petIdx], skillPage);
                    }
                }

                // ── 底部按钮 ──
                int btnY = WIN_H - 80;
                // "显示技能信息" (850, btnY, 100, 40)
                if (mx >= 850 && mx <= 950 && my >= btnY && my <= btnY + 40) {
                    wchar_t infoText[512] = L"已选技能:\n";
                    wchar_t tmp[64];
                    for (int s = 0; s < skillCnt[petIdx]; s++) {
                        if (curTmpl) {
                            for (int k = 0; k < poolSize; k++) {
                                if (curTmpl->skillPool[k].id == skillSel[petIdx][s]) {
                                    const Skill& sk = curTmpl->skillPool[k];
                                    swprintf_s(tmp, L"  %s | 威力:%d  EP:%d  %s\n",
                                        toWide(sk.name).c_str(), sk.power, sk.epCost,
                                        sk.type == ATTACK ? L"攻击" : (sk.type == DEFENSE ? L"防御" : L"状态"));
                                    wcscat_s(infoText, tmp);
                                    break;
                                }
                            }
                        }
                    }
                    if (skillCnt[petIdx] == 0) wcscat_s(infoText, L"  (无)");
                    settextcolor(RGB(100, 100, 100));
                    settextstyle(14, 0, L"微软雅黑");
                    outtextxy(850, btnY - 60, infoText);
                    FlushBatchDraw();
                    Sleep(1500);
                    DrawInterface(player, selectedPetIdx, buffRow, debuffRow,
                        skillSel[petIdx], skillCnt[petIdx], skillPage);
                }
                // "保存" (980, btnY, 100, 40)
                if (mx >= 980 && mx <= 1080 && my >= btnY && my <= btnY + 40) {
                    Pet& pet = player.playerPet[petIdx];
                    for (int j = 0; j < 4; j++) {
                        if (skillCnt[petIdx] < 4)
                        {
                            break;
                        }
                        if (j < skillCnt[petIdx]) {
                            int targetId = skillSel[petIdx][j];
                            for (int k = 0; k < poolSize; k++) {
                                if (pet.data->skillPool[k].id == targetId) {
                                    pet.skills[j] = &pet.data->skillPool[k];
                                    break;
                                }
                            }
                        }
                        else {
                            pet.skills[j] = nullptr;
                        }
                    }
                    applyPlayerConfig(player);
                    savePlayerConfig(player);
                    settextcolor(RGB(0, 150, 0));
                    settextstyle(20, 0, L"微软雅黑");
                    if (debuffRow == -1 || buffRow == -1 && (buffRow != -1 && debuffRow == -1) || (debuffRow != -1 && buffRow == -1))
                    {
                        outtextxy(980, btnY - 50, L"请检查性格是否同时包含增减或无");
                    }
                    if (skillCnt[petIdx] != 4) {
                        outtextxy(980, btnY - 30, L"请选择完四个技能才能保存成功哦！");
                    }
                    if (skillCnt[petIdx] == 4 && (debuffRow != -1 && buffRow != -1) || (debuffRow == -1 && buffRow == -1))
                    {
                        outtextxy(980, btnY - 30, L"✓ 保存成功");
                    }
                    FlushBatchDraw();
                    Sleep(800);
                    DrawInterface(player, selectedPetIdx, buffRow, debuffRow,
                        skillSel[petIdx], skillCnt[petIdx], skillPage);
                }
                // "返回" (1110, btnY, 100, 40)
                if (mx >= 1110 && mx <= 1210 && my >= btnY && my <= btnY + 40) {
                    running = false;
                }
            }
        }
        Sleep(10);
    }

    closegraph();
}
