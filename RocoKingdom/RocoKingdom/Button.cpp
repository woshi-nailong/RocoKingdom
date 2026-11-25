#include "Button.h"

// 默认构造
Button::Button()
	: x(0), y(0), w(100), h(50), text(L"Button"),
	isHover(false), isPressed(false), clicked(false) {
}

// 带参构造
Button::Button(int x, int y, int w, int h, std::wstring text)
	: x(x), y(y), w(w), h(h), text(text),
	isHover(false), isPressed(false), clicked(false) {
}

Button::~Button() {}

// ========== 绘制 ==========
void Button::draw()
{
	// 根据状态选颜色
	if (isPressed)
		setfillcolor(RGB(100, 160, 220));   // 按下：蓝
	else if (isHover)
		setfillcolor(RGB(160, 200, 240));   // 悬停：浅蓝
	else
		setfillcolor(RGB(220, 235, 250));   // 默认：白蓝

	setlinecolor(RGB(60, 120, 180));
	fillroundrect(x, y, x + w, y + h, 10, 10);
	roundrect(x, y, x + w, y + h, 10, 10);   // 圆角边框

	// 文字（居中）
	setbkmode(TRANSPARENT);
	settextcolor(RGB(30, 30, 30));
	settextstyle(24, 0, _T("微软雅黑"));

	int tx = x + (w - textwidth(text.c_str())) / 2;
	int ty = y + (h - textheight(text.c_str())) / 2;
	outtextxy(tx, ty, text.c_str());
}

// ========== 事件处理 ==========
bool Button::handleEvent(const ExMessage& msg)
{
	clicked = false;   // 每帧重置

	switch (msg.message)
	{
	case WM_MOUSEMOVE:
		isHover = (msg.x >= x && msg.x <= x + w &&
			msg.y >= y && msg.y <= y + h);
		break;

	case WM_LBUTTONDOWN:
		if (isHover)
			isPressed = true;
		break;

	case WM_LBUTTONUP:
		if (isPressed && isHover)
			clicked = true;          // 有效点击
		isPressed = false;
		break;
	}
	return clicked;
}

// ========== Setter ==========
void Button::setPosition(int x, int y) { this->x = x; this->y = y; }
void Button::setSize(int w, int h) { this->w = w; this->h = h; }
void Button::setText(const std::wstring& text) { this->text = text; }
