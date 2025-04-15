#include "GameScene.h"
//#include "PrimitiveDrawer.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// 初期化処理

	textureHandle_ = TextureManager::Load("sample.png");

	//スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

	//3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//カメラの初期化
	camera_.Initialize();

	//サウンドデータの読み込み
	soundDataHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");

	//音声再生
	Audio::GetInstance()->PlayWave(soundDataHandle_);

	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);

	//ライン描画が参照するカメラを指定する(アドレス渡し)
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

}

GameScene::~GameScene() { 
	delete sprite_;
	delete model_;
	delete debugCamera_;
}

void GameScene::Update() {
	// 更新処理

	//スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	//座標を{2,1}移動
	position.x += 2.0f;
	position.y += 1.0f;

	//移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 音声再生
		Audio::GetInstance()->StopWave(soundDataHandle_);
	}

	//デモウィンドウの表示を有効化
	#ifdef _DEBUG
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	#endif

	debugCamera_->Update();

}

void GameScene::Draw() {
	// 描画処理

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	//sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

	// 3Dモデルの描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	//3Dモデル描画
	//model_->Draw(worldTransform_, camera_, textureHandle_);

	// 3Dモデルの描画後処理
	Model::PostDraw();

	//ラインを描画
	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

	

}
