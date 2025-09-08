#pragma once
#include <memory>
#include <vector>

class GameTimer;
class HPGauge;
class SpecialGauge;
class StaminaGauge;
class UIManager
{
public:
	UIManager();
	~UIManager();
	void Draw();

private:
	std::shared_ptr<GameTimer> m_gameTimer;
	std::shared_ptr<HPGauge> m_hpGauge;
	std::shared_ptr<SpecialGauge> m_specialGauge;
	std::shared_ptr<StaminaGauge> m_staminaGauge;
};

