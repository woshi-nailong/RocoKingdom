#include "Service/DataService.h"
#include <json/json.hpp>
#include <iostream>
#include <fstream>
using json = nlohmann::json;

bool savePlayerConfig(const Player& player) {
	json j;

	std::ifstream ifile("player_save.json");
	if (!ifile.is_open()) {
		std::cerr << "无法打开文件 player_save.json !" << std::endl;
		return false;
	}
	else {
		ifile >> j;
		ifile.close();
	}

	int targetPlayerID = player.playerID;
	auto& playerArr = j["playerList"];
	bool isFixed = false;

	for (auto& it : playerArr) {
		int savePlayerID = it["playerID"].get<int>();
		if (savePlayerID == targetPlayerID) {
			for (int i = 0; i < 3; i++) {
				auto& pet = it["playerPet"][i];
				auto& ppet = player.playerPet[i];
				pet["buffStart"] = static_cast<int>(ppet.buffStat);
				pet["debuffStart"] = static_cast<int>(ppet.debuffStat);

				pet["skill"] = { ppet.skills[0]->id,
					ppet.skills[1]->id,
					ppet.skills[2]->id,
					ppet.skills[3]->id
				};
			}

			isFixed = true;
			break;
		}
	}

	std::ofstream ofile("player_save.json");
	if (!ofile.is_open()) {
		std::cerr << "无法打开文件 player_save.json !" << std::endl;
		return false;
	}
	ofile << j.dump(4);
	ofile.close();

	return isFixed;
}


bool loadPlayerConfig(Player& player) {
	json j;

	std::ifstream ifile("player_save.json");
	if (!ifile.is_open()) {
		std::cerr << "无法打开文件 player_save.json !" << std::endl;
		return false;
	}
	else {
		ifile >> j;
		ifile.close();
	}

	int targetPlayerID = player.playerID;
	auto& playerArr = j["playerList"];
	bool isFixed = false;

	for (auto& it : playerArr) {
		int savePlayerID = it["playerID"].get<int>();
		if (savePlayerID == targetPlayerID) {
			for (int i = 0; i < 3; i++) {
				auto& pet = it["playerPet"][i];
				auto& ppet = player.playerPet[i];
				int buffVal = pet["buffStart"].get<int>();
				int debuffVal = pet["debuffStart"].get<int>();
				ppet.buffStat = static_cast<NatureStat>(buffVal);
				ppet.debuffStat = static_cast<NatureStat>(debuffVal);

				auto& saveSkills = pet["skill"];
				for (int j = 0; j < 4; j++) {
					for (int k = 0; k < player.playerPet[i].data->skillPool.size();k++) {
						if (player.playerPet[i].data->skillPool[k].id == saveSkills[j].get<int>()) {
							player.playerPet[i].skills[j] = &player.playerPet[i].data->skillPool[k];
							break;
						}
					}
				}
			}

			isFixed = true;
			break;
		}
	}

	return isFixed;
}


void applyPlayerConfig(Player& player) {
	for (int i = 0; i < 3; i++) {
		Pet& pet = player.playerPet[i];
		PetTemplate* temp = pet.data;

		// 先把六个属性设为基础值
		pet.maxHp  = temp->m_maxHp;
		pet.atk    = temp->m_atk;
		pet.def    = temp->m_def;
		pet.spAtk  = temp->m_spAtk;
		pet.spDef  = temp->m_spDef;
		pet.speed  = temp->m_speed;

		// 性格加成（从基础值 × 110%）
		switch (pet.buffStat) {
		case NATURE_HP:    pet.maxHp  = temp->m_maxHp  * 1.1;  break;
		case NATURE_ATK:   pet.atk    = temp->m_atk    * 1.1;  break;
		case NATURE_DEF:   pet.def    = temp->m_def    * 1.1;  break;
		case NATURE_SP_ATK:pet.spAtk  = temp->m_spAtk  * 1.1;  break;
		case NATURE_SP_DEF:pet.spDef  = temp->m_spDef  * 1.1;  break;
		case NATURE_SPEED: pet.speed  = temp->m_speed  * 1.1;  break;
		}

		// 性格减成（从基础值 × 90%）
		switch (pet.debuffStat) {
		case NATURE_HP:    pet.maxHp  = temp->m_maxHp  * 0.9;  break;
		case NATURE_ATK:   pet.atk    = temp->m_atk    * 0.9;  break;
		case NATURE_DEF:   pet.def    = temp->m_def    * 0.9;  break;
		case NATURE_SP_ATK:pet.spAtk  = temp->m_spAtk  * 0.9;  break;
		case NATURE_SP_DEF:pet.spDef  = temp->m_spDef  * 0.9;  break;
		case NATURE_SPEED: pet.speed  = temp->m_speed  * 0.9;  break;
		}

		// 绑定技能指针,去掉了
	}
}