#include "TitleScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "SceneManager.h"

using namespace KamataEngine;

void TitleScene::Initialize() {
	// ここでBGM再生やSpriteのロードなど必要なら書く
}

void TitleScene::Update() {
	Input* input = Input::GetInstance();

	if (input->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene(new GameScene());
	}
}

void TitleScene::Draw() {

}
