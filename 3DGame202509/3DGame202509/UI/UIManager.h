#pragma once
#include <memory>
#include <vector>

class HPGauge;
class SpecialGauge;
class StaminaGauge;
class UIManager
{
public:
	UIManager();
	~UIManager();
	void Draw(std::shared_ptr<Player> player,
		std::shared_ptr<EnemyBoss> boss,
		std::vector<std::shared_ptr<EnemyMinion>> minions,
		std::vector<std::shared_ptr<EnemyMage>> mages);

private:
	std::shared_ptr<HPGauge> m_hpGauge;
	std::shared_ptr<SpecialGauge> m_specialGauge;
	std::shared_ptr<StaminaGauge> m_staminaGauge;
};

