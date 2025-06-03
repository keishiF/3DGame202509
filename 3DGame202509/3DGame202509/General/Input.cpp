#include "Input.h"
#include "DxLib.h"

Input::Input()
{
	m_inputActionMap["A"]     = { {InputType::keyboard, KEY_INPUT_8},
							      {InputType::pad,      PAD_INPUT_2}};

	m_inputActionMap["B"]     =	{ {InputType::keyboard, KEY_INPUT_LSHIFT},
								  {InputType::pad,      PAD_INPUT_1} };

	m_inputActionMap["X"]     = { {InputType::keyboard, KEY_INPUT_9},
								  {InputType::pad,      PAD_INPUT_4} };

	m_inputActionMap["OK"]    = { {InputType::keyboard, KEY_INPUT_RETURN},
							      {InputType::pad,      PAD_INPUT_2} };

	m_inputActionMap["UP"]    = { {InputType::keyboard, KEY_INPUT_W},
							      {InputType::pad,      PAD_INPUT_UP} };

	m_inputActionMap["DOWN"]  = { {InputType::keyboard, KEY_INPUT_S},
							      {InputType::pad,      PAD_INPUT_DOWN} };

	m_inputActionMap["LEFT"]  = { {InputType::keyboard, KEY_INPUT_A},
							      {InputType::pad,      PAD_INPUT_LEFT} };

	m_inputActionMap["RIGHT"] = { {InputType::keyboard, KEY_INPUT_D},
							      {InputType::pad,      PAD_INPUT_RIGHT} };

	m_inputActionMap["LB"]    = { {InputType::keyboard, KEY_INPUT_1},
								  {InputType::pad,      PAD_INPUT_5} };

	m_inputActionMap["RB"]    = { {InputType::keyboard, KEY_INPUT_2},
								  {InputType::pad,      PAD_INPUT_6} };

	m_inputActionMap["LPush"] = { {InputType::keyboard, KEY_INPUT_LCONTROL},
								  {InputType::pad,      PAD_INPUT_9} };
}

void Input::Update()
{
	m_lastInput = m_currentInput;

	char keyboardInput[256] = {};
	int padInput = 0;
	GetHitKeyStateAll(keyboardInput);
	padInput = GetJoypadInputState(DX_INPUT_PAD1);

	for (const auto& keyInfo : m_inputActionMap)
	{
		const auto& key = keyInfo.first;
		m_currentInput[key] = false;
		for (const auto& inputInfo : keyInfo.second)
		{
			if (inputInfo.type == InputType::pad)
			{
				m_currentInput[key] = inputInfo.buttonId & padInput;
			}
			else if (inputInfo.type == InputType::keyboard)
			{
				m_currentInput[key] = keyboardInput[inputInfo.buttonId];
			}

			if (m_currentInput[key])
			{
				break;
			}
		}
	}
}

bool Input::IsPress(const char* key) const
{
	if (!m_currentInput.contains(key))
	{
		return false;
	}

	return m_currentInput.at(key);
}

bool Input::IsTrigger(const char* key) const
{
	if (!m_currentInput.contains(key))
	{
		return false;
	}

	return (m_currentInput.at(key) && !m_lastInput.at(key));
}
