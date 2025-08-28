#include "SceneManager.h"

SceneManager::~SceneManager() {
	if (currentScene_) {
		delete currentScene_;
		currentScene_ = nullptr;
	}
}

SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::ChangeScene(Scene* newScene) {
	if (currentScene_) {
		delete currentScene_;
	}
	currentScene_ = newScene;
	currentScene_->Initialize();
}

void SceneManager::Update() {
	if (currentScene_) {
		currentScene_->Update();
	}
}

void SceneManager::Draw() {
	if (currentScene_) {
		currentScene_->Draw();
	}
}
