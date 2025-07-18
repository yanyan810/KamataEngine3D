#include "GameScene.h"
// #include "PrimitiveDrawer.h"

// AL302_13の27ページから再開


using namespace KamataEngine;


void GameScene::Initialize() {
	// 初期化処理
	
	viewProjection_.Initialize();
	worldTransform_.Initialize();

	playerModel = Model::Create();
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
	
	player_->Draw(viewProjection_); 

}