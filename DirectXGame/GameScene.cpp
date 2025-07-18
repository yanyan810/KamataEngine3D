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

	  viewProjection_.UpdateMatrix(); // ←これが必要！

	player_->Updata(); 

}

void GameScene::Draw() { 
	
	  DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// モデルの描画前処理（これが cmdList をセットする）
	Model::PreDraw(dxCommon->GetCommandList());

	// モデル描画
	player_->Draw(viewProjection_);

	// モデルの描画後処理
	Model::PostDraw();

}