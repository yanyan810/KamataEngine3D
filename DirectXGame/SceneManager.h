#pragma once
#include "Scene.h"

class SceneManager {
public:
	static SceneManager* GetInstance();

	void ChangeScene(Scene* newScene);
	void Update();
	void Draw();

private:
	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	Scene* currentScene_ = nullptr;
};
