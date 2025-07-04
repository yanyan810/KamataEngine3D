#include "TitleScene.h"

void TitleScene::Initialize() {
	// 初期化処理
	// ここではタイトルシーンの初期化を行います。
	// 例えば、背景画像の読み込みやタイトルテキストの設定などを行います。


	titleModel_ = KamataEngine::Model::CreateFromOBJ("title", true);
	playerModel_ = KamataEngine::Model::CreateFromOBJ("player", true);
	
	playerTextureHandle_ = KamataEngine::TextureManager::Load("sample.png"); // プレイヤー用のテクスチャハンドル

	titlePosition_ = {-14.0f, 12.0f, 0.0f}; // タイトルの位置

	// カメラの生成（←これが重要！）
	camera_ = new KamataEngine::Camera();
	camera_->Initialize(); // ← これが必須！

	worldTransform_.Initialize();

	title_ = new Title();

	title_->Initialize(titleModel_,  camera_, titlePosition_);

	player_ = new Player();
	player_->Initialize(playerModel_, playerTextureHandle_, camera_, playerPosition_);

}

TitleScene::~TitleScene() {
	// デストラクタでリソースを解放
	delete title_;
	delete player_;
	delete camera_; // カメラの解放
	if (titleModel_) {
		delete titleModel_;
	}
	if (playerModel_) {
		delete playerModel_;
	}
}

void TitleScene::Update() {
	
		title_->Update(); // ← これが必要
	

	
		player_->TitleUpdata(); // プレイヤーの更新処理
	
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_SPACE)) {
		    // スペースキーが押されたらシーンを終了
		    finished_ = true;
	    }

}

void TitleScene::Draw() {

  KamataEngine::DirectXCommon* dxCommon = KamataEngine::DirectXCommon::GetInstance();

	KamataEngine::Model::PreDraw(dxCommon->GetCommandList());

	// タイトルモデルの描画
	if (title_) {
		title_->Draw();
	}

	
		player_->Draw(); // プレイヤーの描画処理
	

	KamataEngine::Model::PostDraw();
}