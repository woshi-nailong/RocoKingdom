#include "Service/GameData.h"
#include "View/MainMenuScene.h"
#include "Button.h"
#include <graphics.h>
#include <windows.h>
#include <mmeapi.h>
#include <algorithm>
#pragma comment(lib, "winmm.lib")

/*
 * 主菜单界面
 * 负责人: kumaxi
 * 功能:
 *     1. 绘制游戏主菜单背景图
 *     2. 显示四个功能按钮（开始对战 / 队伍配置 / 游戏说明 / 退出游戏）
 *     3. 检测鼠标点击（EasyX ExMessage），返回玩家选择的选项编号
 * 返回值:
 *     玩家点击的按钮编号:
 *     0: 开始对战
 *     1: 队伍配置
 *     2: 游戏说明
 *     3: 退出游戏
 *     -1: 未选择（无操作）
 */
int menuView() {
    initgraph(1427, 830, EX_SHOWCONSOLE);







    // 放音频并使用全局音量（函数内部会自动先关掉上一个场景的声音）
    // 放 MP3 音频地址
    //////////////////////////////////
    kumaxi_playSceneBGM(L"..\\music\\背景音乐.mp3");
    //////////////////////////////////





    // 创建 4 个按钮对象
    Button kumaxi_StartBattle(508, 330, 412, 50, L"");
    Button kumaxi_TeamConfig(508, 410, 412, 50, L"");
    Button kumaxi_GameRule(508, 490, 412, 50, L"");
    Button kumaxi_ExitGame(508, 570, 412, 50, L"");

    IMAGE kumaxi_bgImg;

    //放图片
    ////////////////////////////
    loadimage(&kumaxi_bgImg, _T("figure//1.png"), 1427, 830);
    ////////////////////////////

    // 按钮对应的文本标题数组
    const wchar_t* kumaxi_btnNames[4] = { _T("开始对战"), _T("队伍配置"), _T("游戏说明"), _T("退出游戏") };

    // 定义状态记录变量
    // 用来记住当前键盘/鼠标光标指向第几个选项（0对战，1配置，2说明，3退出）
    int kumaxi_selectIndex = 0;

    // 用来接收系统传来的真实键盘与鼠标消息
    ExMessage kumaxi_msg;

    // 防止画面刷屏时闪烁
    BeginBatchDraw();

    // 逻辑循环
    while (true) {

        //// 绘制背景
        //setfillcolor(RGB(135, 206, 235));       // 设置画刷颜色
        //solidrectangle(0, 0, 1024, 768);        // 背景大小为 1024 * 768

        putimage(0, 0, &kumaxi_bgImg);

        // 绘制标题 
        setbkmode(TRANSPARENT);                 // 设置文字背景为透明
        settextcolor(RGB(255, 215, 0));         // 设置文字颜色金色
        settextstyle(80, 0, _T("微软雅黑"));    // 设置字号为 80，字体为微软雅黑
        outtextxy(585, 110, _T("洛克王国"));    // 绘制大标题

        // 统一绘制 4 个按钮框体与文字
        for (int i = 0; i < 4; i++) {
            int kumaxi_btnY = 330 + i * 80;

            // 只有当索引等于唯一的 kumaxi_selectIndex 时才画高亮背景与亮黄色加厚边框
            if (i == kumaxi_selectIndex) {
                setfillcolor(RGB(160, 200, 240)); // 选中按钮微亮背景
                setlinecolor(RGB(255, 215, 0));   // 亮黄色高亮边框
                setlinestyle(PS_SOLID, 4);        // 4 像素加厚边框
            }
            else {
                setfillcolor(RGB(220, 235, 250)); // 未选中按钮普通背景
                setlinecolor(RGB(150, 150, 150)); // 普通灰色边框
                setlinestyle(PS_SOLID, 2);        // 普通 2 像素边框
            }

            // 画按钮底层背景框
            fillroundrect(508, kumaxi_btnY, 508 + 412, kumaxi_btnY + 50, 10, 10);

            // 绘制按钮中心文字
            settextcolor(RGB(30, 30, 30));
            settextstyle(28, 0, _T("微软雅黑"));
            outtextxy(508 + 160, kumaxi_btnY + 10, kumaxi_btnNames[i]);
        }
        setlinestyle(PS_SOLID, 1);

        // 绘制 ---> 提示箭头
        int kumaxi_cursorY = 336 + kumaxi_selectIndex * 80;
        settextcolor(RGB(255, 69, 0));                  // 设置箭头颜色红色
        settextstyle(36, 0, _T("微软雅黑"));            // 设置字号大小 36
        outtextxy(428, kumaxi_cursorY, _T("--->"));     // 绘制箭头

        // 绘制底部操作提示文字
        settextcolor(RGB(0, 0, 0));           // 设置提示文字黑色
        settextstyle(20, 0, _T("微软雅黑"));  // 设置字号大小为 20
        outtextxy(575, 720, _T("提示：按 W/S 键上下选择，按 回车 确认")); // 绘制底部提示

        // 绘制右下角音量控制按键 (静音按钮、减音量按钮、当前音量大小、加音量按钮)
        setfillcolor(kumaxi_isMuted ? RGB(220, 80, 80) : RGB(100, 200, 120));
        fillroundrect(1180, 740, 1255, 780, 8, 8);
        settextcolor(RGB(255, 255, 255));
        settextstyle(18, 0, _T("微软雅黑"));
        outtextxy(1198, 750, kumaxi_isMuted ? L"静音" : L"声音");

        setfillcolor(RGB(220, 235, 250));
        fillroundrect(1265, 740, 1300, 780, 8, 8);
        settextcolor(RGB(30, 30, 30));
        settextstyle(22, 0, _T("微软雅黑"));
        outtextxy(1276, 748, L"-");

        wchar_t kumaxi_volBuf[16];
        swprintf_s(kumaxi_volBuf, L"%d%%", kumaxi_isMuted ? 0 : (kumaxi_volume / 10));
        settextcolor(RGB(0, 0, 0));
        settextstyle(18, 0, _T("微软雅黑"));
        outtextxy(1306, 750, kumaxi_volBuf);

        setfillcolor(RGB(220, 235, 250));
        fillroundrect(1345, 740, 1380, 780, 8, 8);
        settextcolor(RGB(30, 30, 30));
        settextstyle(22, 0, _T("微软雅黑"));
        outtextxy(1355, 748, L"+");

        // 将缓冲区画面刷新到屏幕上 
        FlushBatchDraw();

        // 事件轮询处理循环（同时监听鼠标 EX_MOUSE 和键盘 EX_KEY 消息）
        while (peekmessage(&kumaxi_msg, EX_MOUSE | EX_KEY)) {

            // 1. 键盘按键逻辑分支
            if (kumaxi_msg.message == WM_KEYDOWN) {
                // 如果按下了 W 键
                if (kumaxi_msg.vkcode == 'W' || kumaxi_msg.vkcode == 'w') {
                    if (kumaxi_selectIndex == 0) {
                        kumaxi_selectIndex = 3; // 在最上面时再按向上，循环跳到最下面
                    }
                    else {
                        kumaxi_selectIndex = kumaxi_selectIndex - 1; // 序号减 1
                    }
                }
                // 如果按下了 S 键
                else if (kumaxi_msg.vkcode == 'S' || kumaxi_msg.vkcode == 's') {
                    if (kumaxi_selectIndex == 3) {
                        kumaxi_selectIndex = 0; // 在最下面时再按向下，循环跳到最上面
                    }
                    else {
                        kumaxi_selectIndex = kumaxi_selectIndex + 1; // 序号加 1
                    }
                }
                // 如果按下了 回车键
                else if (kumaxi_msg.vkcode == VK_RETURN) {
                    EndBatchDraw();            // 关闭缓冲
                    return kumaxi_selectIndex; // 返回当前选中的编号
                }
            }

            // 2. 鼠标划过逻辑分支，鼠标移到哪个按钮上方，就把 selectIndex 切换到哪个
            if (kumaxi_msg.message == WM_MOUSEMOVE) {
                // 先判断 X 坐标是否处于按钮区域横坐标范围内 (300 ~ 712)
                if (kumaxi_msg.x >= 508 && kumaxi_msg.x <= 920) {
                    if (kumaxi_msg.y >= 330 && kumaxi_msg.y <= 380) kumaxi_selectIndex = 0; // 鼠标在第 1 按钮上
                    if (kumaxi_msg.y >= 410 && kumaxi_msg.y <= 460) kumaxi_selectIndex = 1; // 鼠标在第 2 按钮上
                    if (kumaxi_msg.y >= 490 && kumaxi_msg.y <= 540) kumaxi_selectIndex = 2; // 鼠标在第 3 按钮上
                    if (kumaxi_msg.y >= 570 && kumaxi_msg.y <= 620) kumaxi_selectIndex = 3; // 鼠标在第 4 按钮上
                }
            }

            // 3. 鼠标点击右下角音量调节控制功能
            if (kumaxi_msg.message == WM_LBUTTONDOWN) {
                int mx = kumaxi_msg.x, my = kumaxi_msg.y;
                bool needUpdateVol = false; // 标记是否需要刷新音量

                // 点击静音/开音
                if (mx >= 1180 && mx <= 1255 && my >= 740 && my <= 780) {
                    kumaxi_isMuted = !kumaxi_isMuted;
                    needUpdateVol = true;
                }
                // 点击减音量 
                if (mx >= 1265 && mx <= 1300 && my >= 740 && my <= 780) {
                    kumaxi_volume = (std::max)(0, kumaxi_volume - 100);
                    needUpdateVol = true;
                }
                // 点击加音量 
                if (mx >= 1345 && mx <= 1380 && my >= 740 && my <= 780) {
                    kumaxi_volume = (std::min)(1000, kumaxi_volume + 100);
                    kumaxi_isMuted = false;
                    needUpdateVol = true;
                }

                // 实时修改音量
                if (needUpdateVol) {
                    wchar_t volCmd[128];
                    swprintf_s(volCmd, L"setaudio bgm volume to %d", kumaxi_isMuted ? 0 : kumaxi_volume);
                    mciSendString(volCmd, NULL, 0, NULL);

                    // 如果是取消静音，重新发一遍播放指令
                    if (!kumaxi_isMuted) {
                        mciSendString(L"play bgm repeat", NULL, 0, NULL);
                    }
                }
            }

            // 4. 鼠标点击逻辑分支，点击哪个按钮关闭缓冲并返回对应编号
            if (kumaxi_StartBattle.handleEvent(kumaxi_msg)) { EndBatchDraw(); return 0; }
            if (kumaxi_TeamConfig.handleEvent(kumaxi_msg)) { EndBatchDraw(); return 1; }
            if (kumaxi_GameRule.handleEvent(kumaxi_msg)) { EndBatchDraw(); return 2; }
            if (kumaxi_ExitGame.handleEvent(kumaxi_msg)) { EndBatchDraw(); return 3; }
        }

        Sleep(16);
    }

    // 退出
    EndBatchDraw();
    return -1;
}