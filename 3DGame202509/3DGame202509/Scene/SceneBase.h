#pragma once

class Input;
class SceneController;

// ゲームシーン基底クラス
class SceneBase
{
public:
	SceneBase(SceneController& controller);

	// 更新、描画
	virtual void Update(Input& input) abstract;
	virtual void Draw() = 0;

protected:
	SceneController& m_controller;
};

