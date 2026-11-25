#include "View/ResultScene.h"
#include "Button.h"
#include <graphics.h>
#include "Service/GameData.h"
/*
 * 胜利/结束界面
 * 负责人: kumaxi
 * 功能:
 *     1. 显示获胜方信息（背景、文字提示）
 *     2. 提示"按回车键返回主菜单"
 *     3. 检测回车键输入，返回主菜单
 * 参数:
 *     winnerID: 获胜方编号
 *         1: 玩家1胜利
 *         2: 玩家2胜利
 */
void resultView(int winnerID) {


    kumaxi_playSceneBGM(L"..\\music\\游戏对战声音.mp3");


    initgraph(1427, 830, EX_SHOWCONSOLE);




    // 根据传入的 winnerID 生成顶部标题文本
    LPCTSTR kumaxi_winnerText;
    if (winnerID == 1) {
        kumaxi_winnerText = _T("玩家1胜利！！");
    }
    else {
        kumaxi_winnerText = _T("玩家2胜利！！");
    }

    IMAGE kumaxi_bgImg;





    //放图片
    ////////////////////////////
    loadimage(&kumaxi_bgImg, _T("figure//2.png"), 1427, 830);
    ////////////////////////////




    // 防止画面闪烁
    BeginBatchDraw();

    // 绘制背景
    putimage(0, 0, &kumaxi_bgImg);

    // 绘制顶部胜利大字
    setbkmode(TRANSPARENT);                 // 设置文字背景为透明
    settextcolor(RGB(255, 255, 0));         // 亮黄色
    settextstyle(100, 0, _T("微软雅黑"));   // 设置字号为 100，字体为微软雅黑
    outtextxy(460, 250, kumaxi_winnerText);

    // 绘制底部提示文字
    settextstyle(40, 0, _T("微软雅黑"));
    outtextxy(553, 600, _T("按回车回到主菜单"));

    // 将缓冲区画面刷新到屏幕上 
    FlushBatchDraw();

    ExMessage kumaxi_msg;
    while (true) {
        // 监听键盘 EX_KEY 消息
        getmessage(&kumaxi_msg, EX_KEY);

        // 检测到按键按下且是回车键时退出
        if (kumaxi_msg.message == WM_KEYDOWN && kumaxi_msg.vkcode == VK_RETURN) {
            EndBatchDraw();
            return;
        }
    }
}