#pragma once

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
void resultView(int winnerID);
