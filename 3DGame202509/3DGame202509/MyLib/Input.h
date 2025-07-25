#pragma once
#include <map>
#include <string>
#include <vector>

class Input
{
public:
	void Update();

	bool IsPress(const char* key) const;

	bool IsTrigger(const char* key) const;

	static Input& GetInstance();

private:
	Input();

	enum class InputType
	{
		keyboard,
		pad
	};

	struct InputInfo
	{
		InputType type;
		int buttonId;
	};

	std::map<std::string, std::vector<InputInfo>> m_inputActionMap;

	std::map<std::string, bool> m_currentInput;
	std::map<std::string, bool> m_lastInput;

	Input(const Input&) = delete;
	Input& operator =(const Input&) = delete;
};

