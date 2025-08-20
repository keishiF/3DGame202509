#pragma once

class HPGauge
{
public:
	HPGauge();
	~HPGauge();

	void Draw();

private:
	void DrawPlayerHP();
	void DrawBossHP();
	void DrawMageHP();
	void DrawMinionHP();
};

