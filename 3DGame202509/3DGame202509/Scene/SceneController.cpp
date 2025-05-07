#include "SceneController.h"
#include "TitleScene.h"
#include "GameScene.h"

SceneController::SceneController()
{
	ChangeScene(std::make_shared<TitleScene>(*this));
}

void SceneController::Update(Input& input)
{
	m_scenes.back()->Update(input);
}

void SceneController::Draw()
{
	for (auto& scene : m_scenes)
	{
		scene->Draw();
	}
}

void SceneController::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	if (m_scenes.empty())
	{
		m_scenes.push_back(scene);
	}
	else
	{
		m_scenes.back() = scene;
	}
}

void SceneController::PushScene(std::shared_ptr<SceneBase> scene)
{
	m_scenes.push_back(scene);
}

void SceneController::PopScene()
{
	if (m_scenes.size() == 1)
	{
		// Às‚·‚×‚«ƒV[ƒ“‚ª‚È‚­‚È‚é‚½‚ß‹–‰Â‚µ‚È‚¢
		return;
	}
	// ––”ö‚ğæ‚èœ‚­
	m_scenes.pop_back();
}
