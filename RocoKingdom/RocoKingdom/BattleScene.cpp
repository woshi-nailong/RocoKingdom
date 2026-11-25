#include "View/BattleScene.h"
#include <graphics.h>

PetMaskImageCache g_petMaskImages;
PetImageCache g_petImages;
AvatarCache g_avatars;

// ═══════════════════════════════════
// 窗口初始化
// ═══════════════════════════════════
void initBattleWindow() {
	closegraph();
	initgraph(WIN_W, WIN_H);
	g_petMaskImages.load();
	g_petImages.load();
	g_avatars.load();
}

// ═══════════════════════════════════
// 逃跑确认对话框
// ═══════════════════════════════════
bool escapeConfirmView() {
	setfillcolor(RGB(0, 0, 0));
	solidrectangle(0, 0, WIN_W, WIN_H);

	constexpr int DLG_W = 480, DLG_H = 220;
	constexpr int DLG_X = (WIN_W - DLG_W) / 2;
	constexpr int DLG_Y = (WIN_H - DLG_H) / 2;

	BeginBatchDraw();
	while (true) {
		setfillcolor(RGB(0, 0, 0));
		solidrectangle(0, 0, WIN_W, WIN_H);

		setfillcolor(RGB(30, 35, 45));
		solidrectangle(DLG_X, DLG_Y, DLG_X + DLG_W, DLG_Y + DLG_H);
		setlinecolor(RGB(200, 100, 80));
		rectangle(DLG_X, DLG_Y, DLG_X + DLG_W, DLG_Y + DLG_H);

		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 200, 150));
		settextstyle(28, 0, _T("微软雅黑"));
		outtextxy(DLG_X + 140, DLG_Y + 30, L"⚠ 确定逃跑？");

		settextstyle(18, 0, _T("微软雅黑"));
		settextcolor(RGB(180, 180, 200));
		outtextxy(DLG_X + 80, DLG_Y + 80, L"逃跑将直接判负，本场战斗结束");

		int btn1X = DLG_X + 80, btnW = 140, btnH = 55;
		int btn2X = DLG_X + 260;
		int btnY = DLG_Y + 130;

		setfillcolor(RGB(180, 60, 50));
		solidrectangle(btn1X, btnY, btn1X + btnW, btnY + btnH);
		setlinecolor(RGB(220, 100, 80));
		rectangle(btn1X, btnY, btn1X + btnW, btnY + btnH);
		settextcolor(RGB(255, 255, 255));
		settextstyle(22, 0, _T("微软雅黑"));
		outtextxy(btn1X + 25, btnY + 14, L"确认逃跑");

		setfillcolor(RGB(50, 55, 60));
		solidrectangle(btn2X, btnY, btn2X + btnW, btnY + btnH);
		setlinecolor(RGB(100, 120, 140));
		rectangle(btn2X, btnY, btn2X + btnW, btnY + btnH);
		settextcolor(RGB(200, 200, 200));
		outtextxy(btn2X + 32, btnY + 14, L"再想想");

		FlushBatchDraw();

		ExMessage msg;
		while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
			if (msg.message == WM_LBUTTONDOWN) {
				if (msg.x >= btn1X && msg.x <= btn1X + btnW && msg.y >= btnY && msg.y <= btnY + btnH)
				{
					return true;
				}
				if (msg.x >= btn2X && msg.x <= btn2X + btnW && msg.y >= btnY && msg.y <= btnY + btnH)
				{
					return false;
				}
			}
			if (msg.message == WM_KEYDOWN) {
				if (msg.vkcode == VK_RETURN || msg.vkcode == 'Y' || msg.vkcode == 'y')
				{
					return true;
				}
				if (msg.vkcode == VK_ESCAPE || msg.vkcode == 'N' || msg.vkcode == 'n')
				{
					return false;
				}
			}
		}
		Sleep(16);
	}
	EndBatchDraw();
	return false;
}

// ═══════════════════════════════════
// battleView
// ═══════════════════════════════════
Action battleView(const Player& self, const Player& opponent, int /*round*/) {
	const Pet& selfPet = self.playerPet[self.nowPetID];
	const Pet& oppPet = opponent.playerPet[opponent.nowPetID];

	int selfPetIdx = -1, oppPetIdx = -1;
	for (int i = 0; i < 3; i++) {
		if (selfPet.data == &petDatabase[i]) selfPetIdx = i;
		if (oppPet.data == &petDatabase[i]) oppPetIdx = i;
	}

	int backupPets[2] = { -1, -1 };
	int backupCount = 0;
	for (int i = 0; i < 3; i++)
		if (i != self.nowPetID && self.playerPet[i].currentHp > 0)
			backupPets[backupCount++] = i;

	Button skillBtns[4];
	for (int i = 0; i < 4; i++) {
		wchar_t btnText[128];
		bool disabled = false;
		if (selfPet.skills[i] != nullptr) {
			const Skill& sk = *selfPet.skills[i];
			const wchar_t* icon = (sk.type == DEFENSE) ? L"🛡"
				: (sk.type == STATUS) ? L"✨"
				: (sk.attr == SP_ATK || sk.attr == SP_DEF) ? L"🪄" : L"⚔";
			wchar_t skName[64]; toWide(sk.name, skName, 64);
			swprintf(btnText, 128, L"%s%s(%d)", icon, skName, sk.epCost);
			if (selfPet.currentEp < sk.epCost) disabled = true;
			if (sk.type == DEFENSE && self.defenseCooldown) disabled = true;
		}
		else {
			swprintf(btnText, 128, L"--空--");
			disabled = true;
		}
		skillBtns[i] = Button(BTN_SKILL_XS[i], BTN_SKILL_Y, BTN_SKILL_W, BTN_SKILL_H, btnText);
		skillBtns[i].setDisabled(disabled);
	}

	if (self.playerID == 2) {
		setfillcolor(RGB(20, 25, 35));
		solidrectangle(0, 0, WIN_W, WIN_H);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 220, 100));
		settextstyle(36, 0, _T("微软雅黑"));
		outtextxy(WIN_W / 2 - 100, WIN_H / 2 - 60, L"请玩家 2 准备");
		settextstyle(20, 0, _T("微软雅黑"));
		settextcolor(RGB(180, 180, 200));
		outtextxy(WIN_W / 2 - 90, WIN_H / 2, L"按任意键或点击鼠标开始");
		FlushBatchDraw();
		while (true) {
			ExMessage wm;
			if (peekmessage(&wm, EX_KEY | EX_MOUSE)) {
				if (wm.message == WM_KEYDOWN || wm.message == WM_LBUTTONDOWN) break;
			}
			Sleep(50);
		}
	}

	IMAGE imgbk;
    loadimage(&imgbk,L"..\\image\\battlebk.jpeg");
	BeginBatchDraw();
	while (true) {
		cleardevice();
		putimage(0,0,&imgbk);

		// 绘制宠物精灵（面对面站立）
		if (self.playerID == 1)
		{
			if (oppPetIdx >= 0)
			    putMaskimageFlipX(PET_OPP_X, PET_OPP_Y, &g_petMaskImages.imgs[oppPetIdx]);
				putimageFlipX(PET_OPP_X, PET_OPP_Y, &g_petImages.imgs[oppPetIdx]);
			drawPetInfo(oppPet, INFO_OPP_X, INFO_OPP_Y,
				oppPetIdx >= 0 ? &g_avatars.imgs[oppPetIdx] : nullptr, true);
			drawTeamInfo(opponent, INFO_OPP_X, INFO_OPP_Y + 85);

			if (selfPetIdx >= 0)
			    putMaskimageA(PET_SELF_X, PET_SELF_Y, &g_petMaskImages.imgs[selfPetIdx]);
				putimageA(PET_SELF_X, PET_SELF_Y, &g_petImages.imgs[selfPetIdx]);
			drawPetInfo(selfPet, INFO_SELF_X, INFO_SELF_Y,
				selfPetIdx >= 0 ? &g_avatars.imgs[selfPetIdx] : nullptr, false);
			drawTeamInfo(self, INFO_SELF_X, INFO_SELF_Y + 85);
		}
		else
		{
			if (oppPetIdx >= 0)
			    putMaskimageA(PET_SELF_X, PET_SELF_Y, &g_petMaskImages.imgs[oppPetIdx]);
				putimageA(PET_SELF_X, PET_SELF_Y, &g_petImages.imgs[oppPetIdx]);
			drawPetInfo(oppPet, INFO_SELF_X, INFO_SELF_Y,
				oppPetIdx >= 0 ? &g_avatars.imgs[oppPetIdx] : nullptr, false);
			drawTeamInfo(opponent, INFO_SELF_X, INFO_SELF_Y + 85);

			if (selfPetIdx >= 0)
			    putMaskimageFlipX(PET_OPP_X, PET_OPP_Y, &g_petMaskImages.imgs[selfPetIdx]);
				putimageFlipX(PET_OPP_X, PET_OPP_Y, &g_petImages.imgs[selfPetIdx]);
			drawPetInfo(selfPet, INFO_OPP_X, INFO_OPP_Y,
				selfPetIdx >= 0 ? &g_avatars.imgs[selfPetIdx] : nullptr, true);
			drawTeamInfo(self, INFO_OPP_X, INFO_OPP_Y + 85);
		}

		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 240, 180));
		settextstyle(20, 0, _T("微软雅黑"));
		wchar_t turnText[64];
		swprintf(turnText, 64, L"▸ 玩家 %d 的回合", self.playerID);
		outtextxy(WIN_W / 2 - 80, 10, turnText);

		if (self.defenseCooldown) {
			settextcolor(RGB(255, 140, 80));
			settextstyle(17, 0, _T("微软雅黑"));
			outtextxy(BTN_SKILL1_X, BTN_SKILL_Y - 24, L"⚠ 防御冷却中");
		}

		drawCircleBtn(BTN_GATHER_CX, BTN_GATHER_CY, BTN_GATHER_R, L"⚡聚能",
			RGB(60, 80, 160), RGB(100, 140, 240), false);
		drawCircleBtn(BTN_ESCAPE_CX, BTN_ESCAPE_CY, BTN_ESCAPE_R, L"逃跑",
			RGB(160, 60, 50), RGB(220, 100, 80), false);
		if (backupPets[0] >= 0) {
			const Pet& bp = self.playerPet[backupPets[0]];
			wchar_t bname[32]; toWide(bp.data->m_name, bname, 32);
			drawCircleBtn(BTN_PET1_CX, BTN_PET1_CY, BTN_PET_R, bname,
				RGB(50, 120, 60), RGB(80, 200, 100), false);
		}
		if (backupPets[1] >= 0) {
			const Pet& bp = self.playerPet[backupPets[1]];
			wchar_t bname[32]; toWide(bp.data->m_name, bname, 32);
			drawCircleBtn(BTN_PET2_CX, BTN_PET2_CY, BTN_PET_R, bname,
				RGB(50, 120, 60), RGB(80, 200, 100), false);
		}
		for (int i = 0; i < 4; i++) skillBtns[i].draw();

		// ── 禁用按钮灰色遮罩 ──
		for (int i = 0; i < 4; i++) {
			if (skillBtns[i].isDisabled()) {
				setfillcolor(RGB(180, 180, 180));
				solidroundrect(BTN_SKILL_XS[i], BTN_SKILL_Y,
					BTN_SKILL_XS[i] + BTN_SKILL_W, BTN_SKILL_Y + BTN_SKILL_H, 10, 10);
				settextcolor(RGB(120, 120, 120));
				settextstyle(18, 0, L"微软雅黑");
				int txtW = textwidth(L"不可用");
				outtextxy(BTN_SKILL_XS[i] + (BTN_SKILL_W - txtW) / 2,
					BTN_SKILL_Y + BTN_SKILL_H / 2 - 12, L"不可用");
			}
		}

		settextcolor(RGB(140, 160, 180));
		settextstyle(25, 0, _T("微软雅黑"));
		outtextxy(WIN_W / 2 - 350, WIN_H - 52,
			L"1/2/3/4=技能 | G=聚能 | R=换宠（左侧宠物） | T=换宠（右侧宠物） | ESC=逃跑");

		FlushBatchDraw();

		ExMessage msg;
		while (peekmessage(&msg, EX_MOUSE | EX_KEY)) {
			if (msg.message == WM_LBUTTONDOWN) {
				if (inCircle(BTN_GATHER_CX, BTN_GATHER_CY, BTN_GATHER_R, msg.x, msg.y))
				{
					EndBatchDraw(); return { ACTION_GATHER, -1 };
				}
				if (inCircle(BTN_ESCAPE_CX, BTN_ESCAPE_CY, BTN_ESCAPE_R, msg.x, msg.y))
				{
					if (escapeConfirmView()) { EndBatchDraw(); return { ACTION_ESCAPE, -1 }; }
				}
				if (backupPets[0] >= 0 && inCircle(BTN_PET1_CX, BTN_PET1_CY, BTN_PET_R, msg.x, msg.y))
				{
					EndBatchDraw(); return { ACTION_CHANGEPET, backupPets[0] };
				}
				if (backupPets[1] >= 0 && inCircle(BTN_PET2_CX, BTN_PET2_CY, BTN_PET_R, msg.x, msg.y))
				{
					EndBatchDraw(); return { ACTION_CHANGEPET, backupPets[1] };
				}
			}
			// 技能按钮需要接收全部鼠标消息（移动/按下/松开）
			for (int i = 0; i < 4; i++)
				if (!skillBtns[i].isDisabled() && skillBtns[i].handleEvent(msg)) { EndBatchDraw(); return { ACTION_SKILL, i }; }
			if (msg.message == WM_KEYDOWN) {
				switch (msg.vkcode) {
				case '1': case VK_NUMPAD1:
					if (!skillBtns[0].isDisabled()) { EndBatchDraw(); return { ACTION_SKILL, 0 }; } break;
				case '2': case VK_NUMPAD2:
					if (!skillBtns[1].isDisabled()) { EndBatchDraw(); return { ACTION_SKILL, 1 }; } break;
				case '3': case VK_NUMPAD3:
					if (!skillBtns[2].isDisabled()) { EndBatchDraw(); return { ACTION_SKILL, 2 }; } break;
				case '4': case VK_NUMPAD4:
					if (!skillBtns[3].isDisabled()) { EndBatchDraw(); return { ACTION_SKILL, 3 }; } break;
				case 'G': case 'g':
					EndBatchDraw(); return { ACTION_GATHER, -1 };
				case 'R': case 'r': {
					if (backupPets[0] >= 0)
					{
						EndBatchDraw(); return { ACTION_CHANGEPET, backupPets[0] };
					}
					break;
				}
				case 'T': case 't': {
					if (backupPets[1] >= 0)
					{
						EndBatchDraw(); return { ACTION_CHANGEPET, backupPets[1] };
					}
				}
				case VK_ESCAPE:
					if (escapeConfirmView()) { EndBatchDraw(); return { ACTION_ESCAPE, -1 }; }
					break;
				}
			}
		}
		Sleep(16);
	}
	EndBatchDraw();
	return { ACTION_GATHER, -1 };
}

/*
 * 回合结算
 */
void updateBattleView(const Player& p1, const Player& p2, int /*round*/) {
	const Pet& pet1 = p1.playerPet[p1.nowPetID];
	const Pet& pet2 = p2.playerPet[p2.nowPetID];

	int idx1 = -1, idx2 = -1;
	for (int i = 0; i < 3; i++) {
		if (pet1.data == &petDatabase[i]) idx1 = i;
		if (pet2.data == &petDatabase[i]) idx2 = i;
	}

	cleardevice();
	setfillcolor(RGB(0, 0, 0));
	solidrectangle(0, 0, WIN_W, WIN_H);

	if (idx1 >= 0) putimageA(PET_SELF_X, PET_SELF_Y, &g_petImages.imgs[idx1]);
	drawPetInfo(pet1, INFO_SELF_X, INFO_SELF_Y,
		idx1 >= 0 ? &g_avatars.imgs[idx1] : nullptr, false);
	drawTeamInfo(p1, INFO_SELF_X, INFO_SELF_Y + 85);

	if (idx2 >= 0) putimageFlipX(PET_OPP_X, PET_OPP_Y, &g_petImages.imgs[idx2]);
	drawPetInfo(pet2, INFO_OPP_X, INFO_OPP_Y,
		idx2 >= 0 ? &g_avatars.imgs[idx2] : nullptr, true);
	drawTeamInfo(p2, INFO_OPP_X, INFO_OPP_Y + 85);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 255, 120));
	settextstyle(24, 0, _T("微软雅黑"));
	outtextxy(WIN_W / 2 - 60, WIN_H / 2 - 100, L" 回合结算");

	settextstyle(17, 0, _T("微软雅黑"));
	int hintY = WIN_H / 2 + 20;
	if (p1.defenseCooldown) { settextcolor(RGB(255, 140, 80)); outtextxy(WIN_W / 2 - 100, hintY, L"P1 防御冷却中"); hintY += 22; }
	if (p2.defenseCooldown) { settextcolor(RGB(255, 140, 80)); outtextxy(WIN_W / 2 - 100, hintY, L"P2 防御冷却中"); }

	FlushBatchDraw();
	Sleep(2000);
}
