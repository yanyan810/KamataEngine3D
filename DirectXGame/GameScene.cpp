#include "GameScene.h"
// #include "PrimitiveDrawer.h"

using namespace KamataEngine;

// AL302_13の27ページから再開

void GameScene::Initialize() {
	// 初期化処理
	playerModel = Model::CreateFromOBJ("player", true);
	playerTextureHandle_ = KamataEngine::TextureManager::Load("sample.png");
	player_ = new Player();
	player_->Initialize(playerModel, playerTextureHandle_);

	
}

GameScene::~GameScene() {
	
	delete player_;
	delete playerModel;

}

void GameScene::Update() {

	player_->Updata(); 

}

void GameScene::Draw() { 
	
	player_->Draw(); 

}