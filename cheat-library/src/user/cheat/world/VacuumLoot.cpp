#include "pch-il2cpp.h"
#include "VacuumLoot.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>

namespace cheat::feature
{
	VacuumLoot::VacuumLoot() : Feature(),
		NF(f_Enabled, "Vacuum Loot", "VacuumLoot", false)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(VacuumLoot::OnGameUpdate);
    }
	const FeatureGUIInfo& VacuumLoot::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "Misc", "World", true };
		return info;
	}
  
	void VacuumLoot::DrawMain()
	{
		if (ImGui::CollapsingHeader("Vacuum Loot", false))
		{
			ConfigWidget("Enabled", f_Enabled, "Vacuum Loot drops"); ImGui::SameLine(); ImGui::SetNextItemWidth(100.0f);
			ConfigWidget("Delay Time (ms)", f_DelayTime, 1, 0, 1000, "Delay (in ms) between loot vacuum.");
			ConfigWidget("Radius (m)", f_Radius, 0.1f, 5.0f, 100.0f, "Radius of loot vacuum.");
			ConfigWidget("Distance (m)", f_Distance, 0.1f, 1.0f, 10.0f, "Distance between the player and the loot.\n"
				"Values under 1.5 may be too intruding.");
			if (ImGui::TreeNode(this, "Loot Types"))
			{
				for (auto& [section, filters] : m_Sections)
				{
					ImGui::PushID(section.c_str());
					DrawSection(section, filters);
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
		}
	}

	bool VacuumLoot::NeedStatusDraw() const
	{
		return f_Enabled;
	}

	void VacuumLoot::DrawStatus()
	{
		ImGui::Text ("VacuumLoot");
	}

	VacuumLoot& VacuumLoot::GetInstance()
	{
		static VacuumLoot instance;
		return instance;
	}

	bool VacuumLoot::IsEntityForVac(game::Entity* entity)
	{
		auto& manager = game::EntityManager::instance();
		auto distance = manager.avatar()->distance(entity);
		float radius = 100.0f;

		// TODO: Add more on the filter list in the future
		static std::vector<std::string> dropList
		{
			"SceneObj_DropItem",
			"SceneObj_Ore_Drop",
			"_Thundercrystaldrop",
			"Meat",
			"Fishmeat",
			"Equip_Sword",
			"Equip_Pole",
			"Equip_Bow",
			"Equip_Catalyst",
			"Equip_Claymore",
			"Eff_Animal"
		};

		for (auto& dropListNames : dropList)
			if (entity->name().find(dropListNames) != std::string::npos)
				return distance <= radius;
		
		return false;
	}

    void VacuumLoot::OnGameUpdate()
    {
        if (!f_Enabled)
            return;

		auto currentTime = util::GetCurrentTimeMillisec();
		if (currentTime < nextTime)
			return;

        auto& manager = game::EntityManager::instance();
		auto avatarEntity = manager.avatar();
        for (const auto& entity : manager.entities())
        {
            if (!IsEntityForVac(entity))
                continue;

            entity->setRelativePosition(avatarEntity->relativePosition());
        }
		nextTime = currentTime + 1000;
    }
	
}