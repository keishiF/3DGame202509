#pragma once
#include <memory>
#include <list>

class SceneBase;
class Input;

class SceneController
{
public:
	SceneController();

	void Update(Input& input);
	void Draw();

	void ChangeScene(std::shared_ptr<SceneBase> scene);
	void PushScene(std::shared_ptr<SceneBase> scene);
	void PopScene();

private:
	using SceneStack_t = std::list<std::shared_ptr<SceneBase>>;
	SceneStack_t m_scenes;
};

