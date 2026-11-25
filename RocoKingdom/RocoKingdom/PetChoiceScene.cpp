#include "View/PetChoiceScene.h"
#include "Button.h"
#include <graphics.h>


/*
 * 首发宠物选择界面
 * 负责人: kumaxi
 * 功能:
 *     1. 展示玩家三只宠物的信息（名称、属性、属性值）
 *     2. 检测键盘 1/2/3 或鼠标点击选择首发宠物
 *     3. 更新当前玩家的 nowPetID
 * 参数:
 *     player: 当前选择首发宠物的玩家（会修改 nowPetID）
 */

void petChoiceView(Player& player) {
    initgraph(1427, 830, EX_SHOWCONSOLE);
    // 把string转为wstring宽字符, 不然的话会显示中文乱码
    auto kumaxi_stringToWstring = [](const std::string& str) -> std::wstring {
        if (str.empty()) return L"";
        int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
        if (len == 0) {
            len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
            std::wstring wstr(len, 0);
            MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
            return wstr;
        }
        std::wstring wstr(len, 0);
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], len);
        return wstr;
        };

    // 创建 3 个卡片button对象
    Button kumaxi_PetCard1(173, 140, 300, 520, L"");
    Button kumaxi_PetCard2(543, 140, 300, 520, L"");
    Button kumaxi_PetCard3(913, 140, 300, 520, L"");

    IMAGE kumaxi_bgImg;



    // 放图片
    ////////////////////////////
    loadimage(&kumaxi_bgImg, _T("figure//3.png"), 1427, 830);
    ////////////////////////////




     // 默认选中第一个未死亡的宠物
    int kumaxi_selectIndex = 0;
    for (int i = 0; i < 3; i++) {
        if (player.playerPet[i].currentHp > 0) {
            kumaxi_selectIndex = i;
            break;
        }
    }
    // 接收系统消息
    ExMessage kumaxi_msg;

    // 防止画面闪烁
    BeginBatchDraw();

    while (true) {

        // 绘制背景 
        putimage(0, 0, &kumaxi_bgImg);

        // 绘制顶部标题
        setbkmode(TRANSPARENT);
        settextcolor(RGB(255, 215, 0));
        settextstyle(50, 0, _T("微软雅黑"));

        wchar_t kumaxi_titleText[64];
        swprintf_s(kumaxi_titleText, _T("玩家 %d 请选择出战宠物"), player.playerID);
        outtextxy(500, 50, kumaxi_titleText);

        // 统一绘制 3 张卡片框体与唯一的高亮黄色边框
        for (int i = 0; i < 3; i++) {
            int kumaxi_cardX = 173 + i * 370;

            // 只有当索引等于唯一的 selectIndex 时才画亮黄色加厚边框
            if (player.playerPet[i].currentHp == 0) {
                setfillcolor(RGB(169, 169, 169)); // 宠物阵亡灰色背景
                setlinecolor(RGB(150, 150, 150)); // 普通灰色边框
                setlinestyle(PS_SOLID, 2);
            }
            else if (i == kumaxi_selectIndex) {
                setfillcolor(RGB(160, 200, 240)); // 选中卡片微亮背景
                setlinecolor(RGB(255, 215, 0));   // 亮黄色高亮边框
                setlinestyle(PS_SOLID, 4);
            }
            else {
                setfillcolor(RGB(220, 235, 250)); // 未选中卡片普通背景
                setlinecolor(RGB(150, 150, 150)); // 普通灰色边框
                setlinestyle(PS_SOLID, 2);
            }

            // 画卡片底层背景框
            fillroundrect(kumaxi_cardX, 140, kumaxi_cardX + 300, 660, 10, 10);
        }
        setlinestyle(PS_SOLID, 1);

        // 绘制卡片上的宠物详细面板信息
        wchar_t kumaxi_infoBuf[128];

        for (int i = 0; i < 3; i++) {

            int kumaxi_cardX = 173 + i * 370;
            int kumaxi_cardY = 140;

            // 1. 宠物名称
            settextcolor(RGB(30, 30, 30));
            settextstyle(36, 0, _T("微软雅黑"));
            std::wstring kumaxi_wName = kumaxi_stringToWstring(player.playerPet[i].data->m_name);
            outtextxy(kumaxi_cardX + 25, kumaxi_cardY + 25, kumaxi_wName.c_str());

            // 2. 详细面板属性
            settextcolor(RGB(60, 60, 60));
            settextstyle(22, 0, _T("微软雅黑"));
            int kumaxi_dataY = kumaxi_cardY + 100;
            int kumaxi_stepY = 38;

            swprintf_s(kumaxi_infoBuf, _T("生命 (maxHp):  %d"), player.playerPet[i].maxHp);
            outtextxy(kumaxi_cardX + 25, kumaxi_dataY + kumaxi_stepY * 0, kumaxi_infoBuf);

            swprintf_s(kumaxi_infoBuf, _T("物攻 (atk): %d"), player.playerPet[i].atk);
            outtextxy(kumaxi_cardX + 25, kumaxi_dataY + kumaxi_stepY * 1, kumaxi_infoBuf);

            swprintf_s(kumaxi_infoBuf, _T("物防 (def): %d"), player.playerPet[i].def);
            outtextxy(kumaxi_cardX + 25, kumaxi_dataY + kumaxi_stepY * 2, kumaxi_infoBuf);

            swprintf_s(kumaxi_infoBuf, _T("魔攻 (spAtk): %d"), player.playerPet[i].spAtk);
            outtextxy(kumaxi_cardX + 25, kumaxi_dataY + kumaxi_stepY * 3, kumaxi_infoBuf);

            swprintf_s(kumaxi_infoBuf, _T("魔防 (spDef): %d"), player.playerPet[i].spDef);
            outtextxy(kumaxi_cardX + 25, kumaxi_dataY + kumaxi_stepY * 4, kumaxi_infoBuf);

            swprintf_s(kumaxi_infoBuf, _T("速度 (speed): %d"), player.playerPet[i].speed);
            outtextxy(kumaxi_cardX + 25, kumaxi_dataY + kumaxi_stepY * 5, kumaxi_infoBuf);

            if (player.playerPet[i].currentHp == 0) {
                outtextxy(kumaxi_cardX + 25, kumaxi_dataY + kumaxi_stepY * 8, _T("宠物已阵亡"));
            }
        }

        // 绘制底部提示信息
        settextcolor(RGB(0, 0, 0));
        settextstyle(20, 0, _T("微软雅黑"));
        outtextxy(485, 720, _T("提示：按 1/2/3 切换高亮，按 回车 确认；或直接鼠标点击卡片选择"));

        // 刷新画面
        FlushBatchDraw();

        // 事件轮询处理循环
        while (peekmessage(&kumaxi_msg, EX_MOUSE | EX_KEY)) {

            // 1. 键盘切换高亮与确认
            if (kumaxi_msg.message == WM_KEYDOWN) {
                if (kumaxi_msg.vkcode == '1' && player.playerPet[0].currentHp != 0) kumaxi_selectIndex = 0;
                if (kumaxi_msg.vkcode == '2' && player.playerPet[1].currentHp != 0) kumaxi_selectIndex = 1;
                if (kumaxi_msg.vkcode == '3' && player.playerPet[2].currentHp != 0) kumaxi_selectIndex = 2;

                // 按回车键确认选择
                if (kumaxi_msg.vkcode == VK_RETURN && player.playerPet[kumaxi_selectIndex].currentHp > 0) {
                    player.nowPetID = kumaxi_selectIndex;
                    EndBatchDraw();
                    return;
                }
            }

            // 2. 鼠标移动时，同步把高亮索引修改为鼠标指向的卡片
            if (kumaxi_msg.message == WM_MOUSEMOVE) {
                if (kumaxi_msg.y >= 140 && kumaxi_msg.y <= 660) {
                    if (kumaxi_msg.x >= 173 && kumaxi_msg.x <= 473 && player.playerPet[0].currentHp != 0) kumaxi_selectIndex = 0;
                    if (kumaxi_msg.x >= 543 && kumaxi_msg.x <= 843 && player.playerPet[1].currentHp != 0) kumaxi_selectIndex = 1;
                    if (kumaxi_msg.x >= 913 && kumaxi_msg.x <= 1213 && player.playerPet[2].currentHp != 0) kumaxi_selectIndex = 2;
                }
            }

            // 3. 鼠标左键点击直接确认当前高亮的卡片
            if (kumaxi_PetCard1.handleEvent(kumaxi_msg) && player.playerPet[0].currentHp != 0) { player.nowPetID = 0; EndBatchDraw(); return; }
            if (kumaxi_PetCard2.handleEvent(kumaxi_msg) && player.playerPet[1].currentHp != 0) { player.nowPetID = 1; EndBatchDraw(); return; }
            if (kumaxi_PetCard3.handleEvent(kumaxi_msg) && player.playerPet[2].currentHp != 0) { player.nowPetID = 2; EndBatchDraw(); return; }
        }

        Sleep(16);
    }
}

void checkAlive() {
    if (player1.playerPet[player1.nowPetID].currentHp == 0) {
        petChoiceView(player1);
    }
    if (player2.playerPet[player2.nowPetID].currentHp == 0) {
        petChoiceView(player2);
    }
}
