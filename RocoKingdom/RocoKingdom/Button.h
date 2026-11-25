#pragma once
#include <string>
#include <graphics.h>

class Button
{
public:
	Button();
	Button(int x, int y, int w, int h, std::wstring text);
	~Button();

	void draw();                                  // 绘制按钮
	bool handleEvent(const ExMessage& msg);       // 处理鼠标事件，返回当前帧是否被点击

	void setPosition(int x, int y);
	void setSize(int w, int h);
	void setText(const std::wstring& text);

	//ai新加两个函数
	void setDisabled(bool d) { disabled = d; }
	bool isDisabled()const { return disabled; }

private:
	int x, y, w, h;
	std::wstring text;
	bool isHover;     // 鼠标悬停
	bool isPressed;   // 鼠标按下
	bool clicked;     // 本帧刚点击（一次性消费）

	//ai加一个变量
	bool disabled;
};

