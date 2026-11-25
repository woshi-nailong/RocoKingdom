#pragma once
#include "../Model/Player.h"
#include "../Model/Action.h"
#include "../Button.h"
#include <graphics.h>
#include <string>
#include <cstdio>
#include <cwchar>

extern PetTemplate petDatabase[3];

inline void toWide(const std::string& src, wchar_t* dst, int dstSize) {
	if (MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, dst, dstSize) == 0)
		MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, dst, dstSize);
}

// ═══════════════════════════════════════════
// ★ 可调参数
// ═══════════════════════════════════════════
constexpr int WIN_W = 1427;
constexpr int WIN_H = 830;

// ── 左侧圆形按钮 ──
constexpr int BTN_GATHER_CX = 100, BTN_GATHER_CY = 665, BTN_GATHER_R = 55; // 聚能

// ── 右侧圆形按钮（逃跑 + 备选宠横排）──
constexpr int BTN_ESCAPE_CX = 1380, BTN_ESCAPE_CY = 600, BTN_ESCAPE_R = 40; // 逃跑
constexpr int BTN_PET1_CX = 1290, BTN_PET1_CY = 700;                    // 备选宠A
constexpr int BTN_PET2_CX = 1385, BTN_PET2_CY = 700;                    // 备选宠B
constexpr int BTN_PET_R = 38;                                             // 备选宠半径

// ── 矩形技能按钮 ──
constexpr int BTN_SKILL_W = 200;
constexpr int BTN_SKILL_H = 75;
constexpr int BTN_SKILL_Y = 690;
constexpr int BTN_SKILL1_X = 284;
constexpr int BTN_SKILL2_X = 504;
constexpr int BTN_SKILL3_X = 724;
constexpr int BTN_SKILL4_X = 944;
constexpr int BTN_SKILL_XS[4] = { BTN_SKILL1_X, BTN_SKILL2_X, BTN_SKILL3_X, BTN_SKILL4_X };

constexpr int BAR_HP_H = 35;
constexpr int BAR_EP_H = 20;
constexpr int BAR_W = 400;
//双方宠物位置
constexpr int PET_SELF_X = 75;
constexpr int PET_SELF_Y = 125;
constexpr int PET_OPP_X = 900;
constexpr int PET_OPP_Y = 125;
constexpr int PET_AVATAR_SIZE = 80;  // 血条旁头像尺寸

constexpr int INFO_SELF_X = 100;
constexpr int INFO_SELF_Y = 10;
constexpr int INFO_OPP_X = 950;
constexpr int INFO_OPP_Y = 10;

// 宠物掩码图路径 
constexpr const wchar_t* Mask_PET_IMAGE_PATHS[3] = {
	L"..\\image\\maskPet\\1.png",
    L"..\\image\\maskPet\\2.png",
    L"..\\image\\maskPet\\3.png",
};

// 宠物原图路径（每只一张03.png）
constexpr const wchar_t* PET_IMAGE_PATHS[3] = {
	L"..\\image\\1_402\\03.png",
	L"..\\image\\1_403\\03.png",
	L"..\\image\\1_428\\03.png"
};

// 头像图片路径
constexpr const wchar_t* AVATAR_DIRS[3] = {
	L"..\\image\\avatar_0.png",
	L"..\\image\\avatar_1.png",
	L"..\\image\\avatar_2.png"
};

// ═══════════════════════════════════
// 宠物图片缓存
// ═══════════════════════════════════
//掩码图缓存
struct PetMaskImageCache {
	bool loaded = false;
	IMAGE imgs[3];
	void load() {
		if (loaded) return;
		for (int p = 0; p < 3; p++) {
			loadimage(&imgs[p], Mask_PET_IMAGE_PATHS[p]);
		}
		loaded = true;
	}
};
extern PetMaskImageCache g_petMaskImages;

//原图缓存
struct PetImageCache {
	bool loaded = false;
	IMAGE imgs[3];
	void load() {
		if (loaded) return;
		for (int p = 0; p < 3; p++) {
			loadimage(&imgs[p], PET_IMAGE_PATHS[p]);
		}
		loaded = true;
	}
};
extern PetImageCache g_petImages;

// 头像缓存
struct AvatarCache {
	bool loaded = false;
	IMAGE imgs[3];
	void load() {
		if (loaded) return;
		for (int p = 0; p < 3; p++) {
			loadimage(&imgs[p], AVATAR_DIRS[p], PET_AVATAR_SIZE, PET_AVATAR_SIZE);
		}
		loaded = true;
	}
};
extern AvatarCache g_avatars;

// ═══════════════════════════════════
// 工具函数
// ═══════════════════════════════════
inline bool inCircle(int cx, int cy, int r, int px, int py) {
	int dx = px - cx, dy = py - cy;
	return dx * dx + dy * dy <= r * r;
}

inline void drawCircleBtn(int cx, int cy, int r, const wchar_t* text,
	COLORREF fill, COLORREF border, bool disabled) {
	if (disabled) { fill = RGB(60, 60, 60); border = RGB(100, 100, 100); }
	setfillcolor(fill);
	solidcircle(cx, cy, r);
	setlinecolor(border);
	circle(cx, cy, r);
	setbkmode(TRANSPARENT);
	settextcolor(disabled ? RGB(130, 130, 130) : RGB(255, 255, 255));
	settextstyle(20, 0, _T("微软雅黑"));
	int tw = textwidth(text), th = textheight(text);
	outtextxy(cx - tw / 2, cy - th / 2, text);
}

inline void drawHPBar(int x, int y, int w, int h, int cur, int max) {
	setfillcolor(RGB(40, 40, 40));
	solidrectangle(x, y, x + w, y + h);
	if (max <= 0 || cur <= 0) return;
	int fillW = (int)((long long)cur * w / max);
	COLORREF c = cur * 4 > max * 3 ? RGB(60, 220, 60) :
		cur * 4 > max ? RGB(220, 220, 40) : RGB(220, 50, 50);
	setfillcolor(c);
	solidrectangle(x, y, x + fillW, y + h);
	setlinecolor(RGB(180, 180, 180));
	rectangle(x, y, x + w, y + h);
}

inline void drawEPBar(int x, int y, int w, int h, int cur, int max) {
	setfillcolor(RGB(40, 40, 40));
	solidrectangle(x, y, x + w, y + h);
	if (max <= 0 || cur <= 0) return;
	int fillW = (int)((long long)cur * w / max);
	setfillcolor(RGB(60, 140, 220));
	solidrectangle(x, y, x + fillW, y + h);
	setlinecolor(RGB(180, 180, 180));
	rectangle(x, y, x + w, y + h);
}

inline void drawTeamInfo(const Player& p, int x, int y) {
	settextstyle(16, 0, _T("微软雅黑"));
	for (int i = 0; i < 3; i++) {
		int yy = y + i * 20;
		wchar_t buf[64];
		const Pet& tp = p.playerPet[i];
		wchar_t tName[32]; toWide(tp.data->m_name, tName, 32);
		if (i == p.nowPetID)
			swprintf(buf, 64, L"● %s HP:%d/%d		EP:%d/%d", tName, tp.currentHp, tp.maxHp, tp.currentEp, 10);
		else if (tp.currentHp <= 0)
			swprintf(buf, 64, L"○ %s 阵亡", tName);
		else
			swprintf(buf, 64, L"○ %s HP:%d/%d		EP:%d/%d", tName, tp.currentHp, tp.maxHp, tp.currentEp, 10);

		COLORREF tc = (i == p.nowPetID) ? RGB(200, 220, 255) :
			(tp.currentHp <= 0) ? RGB(150, 100, 100) : RGB(140, 155, 180);
		settextcolor(tc);
		outtextxy(x, yy, buf);
	}
}

// 精灵缩放倍率
constexpr double PET_SCALE = 1.5;

//掩码图绘制（缩放 + 3参数，位置 = 左上角）
inline void putMaskimageA(int x, int y, IMAGE* img) {
	int fw = img->getwidth(), fh = img->getheight();
	if (fw <= 0 || fh <= 0) return;
	int dw = (int)(fw * PET_SCALE);
	int dh = (int)(fh * PET_SCALE);
	IMAGE tmp(dw, dh);
	DWORD* src = GetImageBuffer(img);
	DWORD* dst = GetImageBuffer(&tmp);
	for (int iy = 0; iy < dh; iy++) {
		int sy = iy * fh / dh;
		for (int ix = 0; ix < dw; ix++) {
			int sx = ix * fw / dw;
			dst[iy * dw + ix] = src[sy * fw + sx];
		}
	}
	putimage(x, y, &tmp,SRCAND);
}

// 原图绘制（缩放 + 3参数，位置 = 左上角）
inline void putimageA(int x, int y, IMAGE* img) {
	int fw = img->getwidth(), fh = img->getheight();
	if (fw <= 0 || fh <= 0) return;
	int dw = (int)(fw * PET_SCALE);
	int dh = (int)(fh * PET_SCALE);
	IMAGE tmp(dw, dh);
	DWORD* src = GetImageBuffer(img);
	DWORD* dst = GetImageBuffer(&tmp);
	for (int iy = 0; iy < dh; iy++) {
		int sy = iy * fh / dh;
		for (int ix = 0; ix < dw; ix++) {
			int sx = ix * fw / dw;
			dst[iy * dw + ix] = src[sy * fw + sx];
		}
	}
	putimage(x, y, &tmp,SRCPAINT);
}

// 掩码图水平反转 + 缩放绘制
inline void putMaskimageFlipX(int x, int y, IMAGE* img) {
	int fw = img->getwidth(), fh = img->getheight();
	if (fw <= 0 || fh <= 0) return;
	int dw = (int)(fw * PET_SCALE);
	int dh = (int)(fh * PET_SCALE);
	IMAGE tmp(dw, dh);
	DWORD* src = GetImageBuffer(img);
	DWORD* dst = GetImageBuffer(&tmp);
	for (int iy = 0; iy < dh; iy++) {
		int sy = iy * fh / dh;
		for (int ix = 0; ix < dw; ix++) {
			int sx = (dw - 1 - ix) * fw / dw;
			dst[iy * dw + ix] = src[sy * fw + sx];
		}
	}
	putimage(x, y, &tmp, SRCAND);
}

// 原图水平翻转 + 缩放绘制
inline void putimageFlipX(int x, int y, IMAGE* img) {
	int fw = img->getwidth(), fh = img->getheight();
	if (fw <= 0 || fh <= 0) return;
	int dw = (int)(fw * PET_SCALE);
	int dh = (int)(fh * PET_SCALE);
	IMAGE tmp(dw, dh);
	DWORD* src = GetImageBuffer(img);
	DWORD* dst = GetImageBuffer(&tmp);
	for (int iy = 0; iy < dh; iy++) {
		int sy = iy * fh / dh;
		for (int ix = 0; ix < dw; ix++) {
			int sx = (dw - 1 - ix) * fw / dw;
			dst[iy * dw + ix] = src[sy * fw + sx];
		}
	}
	putimage(x, y, &tmp,SRCPAINT);
}

inline void drawPetInfo(const Pet& pet, int px, int py,
	IMAGE* avatar = nullptr, bool flipAvatar = false) {
	setbkmode(TRANSPARENT);

	// ── 头像（血条旁边）──
	if (avatar && avatar->getwidth() > 0) {
		if (flipAvatar) {
			// 右侧：像素级翻转
			int ax = px + BAR_W + 10;
			int as = PET_AVATAR_SIZE;
			IMAGE tmp(as, as);
			DWORD* src = GetImageBuffer(avatar);
			DWORD* dst = GetImageBuffer(&tmp);
			for (int iy = 0; iy < as; iy++)
				for (int ix = 0; ix < as; ix++)
					dst[iy * as + ix] = src[iy * as + (as - 1 - ix)];
			putimage(ax, py + 10, &tmp);
		}
		else {
			// 左侧：正常
			int ax = px - PET_AVATAR_SIZE - 15;
			putimage(ax, py + 10, avatar);
		}
	}

	wchar_t nameBuf[64];
	toWide(pet.data->m_name, nameBuf, 64);
	settextcolor(RGB(255, 255, 200));
	settextstyle(22, 0, _T("微软雅黑"));
	outtextxy(px, py, nameBuf);

	const wchar_t* elemStr = L"无";
	COLORREF elemColor = RGB(180, 180, 180);
	switch (pet.data->m_element) {
	case FIRE:  elemStr = L"火";  elemColor = RGB(255, 100, 60);  break;
	case WATER: elemStr = L"水";  elemColor = RGB(60, 140, 255);   break;
	case GRASS: elemStr = L"草";  elemColor = RGB(80, 210, 80);    break;
	}
	settextcolor(elemColor);
	settextstyle(18, 0, _T("微软雅黑"));
	outtextxy(px + textwidth(nameBuf) + 10, py + 3, elemStr);

	drawHPBar(px, py + 30, BAR_W, BAR_HP_H, pet.currentHp, pet.maxHp);
	drawEPBar(px, py + 65, BAR_W, BAR_EP_H, pet.currentEp, 10);

	if (!pet.statusList.empty()) {
		int sx = px, sy = py + 90;
		for (const auto& st : pet.statusList) {
			if (st.type == STATE_BURN) {
				settextcolor(RGB(255, 130, 40));
				settextstyle(14, 0, _T("微软雅黑"));
				wchar_t buf[32];
				swprintf(buf, 32, L"灼烧x%d", st.stack);
				outtextxy(sx, sy, buf);
				sx += textwidth(buf) + 8;
			}
		}
	}
}

// ═══════════════════════════════════
// 界面函数声明
// ═══════════════════════════════════
bool escapeConfirmView();
void initBattleWindow();
Action battleView(const Player& self, const Player& opponent, int round = 0);
void updateBattleView(const Player& p1, const Player& p2, int round = 0);
