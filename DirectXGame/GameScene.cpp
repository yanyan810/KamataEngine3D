#include "GameScene.h"
// #include "PrimitiveDrawer.h"

using namespace KamataEngine;

// using KamataEngine::Matrix4x4_;

void GameScene::Initialize() {
	// 初期化処理

	textureHandle_ = TextureManager::Load("/cube/cube.jpg");

	playerTextureHandle_ = TextureManager::Load("/sample.png");

	// スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

	// 3Dモデルの生成
	playerModel_ = Model::CreateFromOBJ("player", true);
	//敵
	enemyModel_ = Model::CreateFromOBJ("enemy", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.farZ = 1000.0f; // カメラの奥行きの最大値を設定
	camera_.Initialize();
	
	// サウンドデータの読み込み
	//soundDataHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");

	// 音声再生
	//Audio::GetInstance()->PlayWave(soundDataHandle_);

	//voiceHandle_ = Audio::GetInstance()->PlayWave(soundDataHandle_, true);

	// ライン描画が参照するカメラを指定する(アドレス渡し)
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);

	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	
	modelBlock_ = Model::Create();

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	//=================
	//プレイヤー
	//=================

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	KamataEngine::Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 17);

	// 座標をマップチップ番号で指定
	player_->Initialize(playerModel_, playerTextureHandle_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	cameraController_ = new CameraController();
	// CameraControllerの初期化
	cameraController_->Initialize(&camera_); // 自前のCameraを使うなら引数なしでOK

	// プレイヤーを追従対象としてセット
	cameraController_->SetTarget(player_);

	// カメラ位置を即時合わせる
	cameraController_->Reset();

	//=======
	//敵
	//=======

	// 敵の生成
	enemy_ = new Enemy();
	// 敵の初期化
	KamataEngine::Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(10, 17);
	//座標をマップチップで指定
	enemy_->Initialize(enemyModel_, enemyHandle_, &camera_, enemyPosition);

}



GameScene::~GameScene() {
	delete sprite_;
	delete playerModel_;
	delete enemyModel_;
	delete debugCamera_;
	delete player_;
	delete modelBlock_;
	delete modelSkydome_;
	delete skydome_;
	delete cameraController_;
	delete mapChipField_;
	delete enemy_;
	// ワールドトランスフォームの解放
	for (std::vector<KamataEngine::WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	// ブロック1個分の横幅

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);//縦方向
	for (uint32_t i = 0; i < numBlockVirtical; i++) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipField::MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_= mapChipField_->GetMapChipPositionByIndex(j,i);
			}
		}
	}
}

void GameScene::Update() {
	// 更新処理

	// スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	// 座標を{2,1}移動
	position.x += 2.0f;
	position.y += 1.0f;

	// 移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 音声再生
		//	Audio::GetInstance()->StopWave(soundDataHandle_);
	}

// デモウィンドウの表示を有効化
#ifdef _DEBUG
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
#endif

	debugCamera_->Update();

	player_->Updata();

	//ポインタがnullではないときだけ行う
//	if (enemy_!=nullptr) {

		enemy_->Update();
	//}
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			// 定数バッファを転送する
			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		isDebugCameraActive_ = true;
	}
#endif

	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の更新と転送
		camera_.TransferMatrix();
	} else {

		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

	// 天球の処理
	skydome_->Update();

	cameraController_->Update();

}

void GameScene::Draw() {
	// 描画処理

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

	// 3Dモデルの描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	//// 3Dモデル描画
	// model_->Draw(worldTransform_, camera_, textureHandle_);

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_, textureHandle_);
		}
	}
	// 自キャラの描画
	player_->Draw();

	//敵の描画
	//if (enemy_ != nullptr) {
		enemy_->Draw();
	//}
	// === Skydome描画（背景） ===
	skydome_->Draw();



	// 3Dモデルの描画後処理
	Model::PostDraw();

	// ラインを描画
	// PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
}
