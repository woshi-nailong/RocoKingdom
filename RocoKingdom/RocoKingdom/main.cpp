#include "Model/type.h"
#include "Model/Skill.h"
#include "Model/PetTemplate.h"
#include "Model/Pet.h"
#include "Model/Player.h"
#include "Model/Action.h"
#include "Service/DataService.h"
#include "Service/BattleService.h"
#include "Service/GameData.h"
#include "View/MainMenuScene.h"
#include "View/TeamConfigScene.h"
#include "View/PetChoiceScene.h"
#include "View/BattleScene.h"
#include "View/ResultScene.h"
#include"View/GameIntroduce.h"



// ── 游戏主流程 ──

/*
 * 游戏主循环
 * 负责人:
 * 功能:
 *     1. 初始化游戏数据（调用 initGameData）
 *     2. 加载玩家配置
 *     3. 进入主菜单循环，根据玩家选择跳转到对应功能
 *     4. 主菜单 → 开始对战 / 队伍配置 / 游戏说明 / 退出
 *     5. 对战流程：选首发 → 回合循环 → 胜利结算 → 回主菜单
 */
int main() {
	// 1. 初始化宠物和技能数据（硬编码）
	initGameData();

	// 2. 加载玩家配置
	loadPlayerConfig(player1);
	loadPlayerConfig(player2);
	applyPlayerConfig(player1);
	applyPlayerConfig(player2);

	while (1) {

		// 初始化开始数据
		initBattleStart(player1, player2);


		// 3.1 显示主菜单，获取玩家选择
		int choice = menuView();

		if (choice == 0) {

			kumaxi_playSceneBGM(L"..\\music\\游戏对战声音.mp3");
			// ── 开始对战 ──

			// 选首发宠物
			petChoiceView(player1);
			petChoiceView(player2);


			// 初始化战斗状态：满血、10能量
			// 初始化宠物能量值为10
			initBattleWindow();

			int round = 1;
			while (1) {
				// 玩家1选择操作
				Action a1 = battleView(player1, player2, round);

				//如果玩家1选择逃跑，直接显示获胜界面，break.为了防止两个玩家同时逃跑
				if (a1.type == ACTION_ESCAPE) {
					resultView(2);
					break;
				}

				// 玩家2选择操作（界面遮盖）
				Action a2 = battleView(player2, player1, round);

				// 回合结算
				executeRound(&player1, &player2, a1, a2);


				// 更新界面展示
				updateBattleView(player1, player2, round);

				// 检查胜负
				int winner = checkWin(player1, player2);
				if (winner != 0) {
					resultView(winner);
					break;
				}

				//判断是否有宠物阵亡，需要换宠物
				checkAlive();

				round++;
			}
		}
		else if (choice == 1) {
			// ── 队伍配置（内部可通过按钮切换玩家1/玩家2） ──
			teamConfigView();
		}
		else if (choice == 2) {
			// ── 游戏说明 ──
			GameIntroduceView();
		}
		else if (choice == 3) {
			// ── 退出游戏 ──
			break;
		}
	}

	return 0;
}
