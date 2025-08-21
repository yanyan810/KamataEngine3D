#include "GameScene.h"
// #include "PrimitiveDrawer.h"

// AL302_13の27ページから再開


using namespace KamataEngine;


void GameScene::Initialize() {
	// 初期化処理
	

	viewProjection_.Initialize();
	worldTransform_.Initialize();

	playerModel = Model::Create();
	playerTextureHandle_ = TextureManager::Load("sample.png");
	player_ = new Player();
	player_->Initialize(playerModel, playerTextureHandle_);

	enemyModel_ = Model::Create();
	enemyTextureHandle_ = TextureManager::Load("uvChecker.png");
	enemy_ = new Enemy();

	Vector3 position = {20.0f, 0.0f, 150.0f};

	enemy_->Initialize(enemyModel_, position, enemyTextureHandle_);

		enemy_->SetPlayer(player_); // プレイヤーを敵に設定


	debugCamera_ = new DebugCamera(1280, 720);
	input_ = Input::GetInstance();
	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetCamera(&viewProjection_);
}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	Vector3 posA = colliderA->GetWorldPosition();
	Vector3 posB = colliderB->GetWorldPosition();

	// 位置の差を計算
	float dx = posA.x - posB.x;
	float dy = posA.y - posB.y;
	float dz = posA.z - posB.z;

	// 距離の2乗
	float distSq = dx * dx + dy * dy + dz * dz;

	float radiusSum = colliderA->GetRadius() + colliderB->GetRadius();

	if (distSq <= radiusSum * radiusSum) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}


GameScene::~GameScene() {
	
	delete player_;
	delete playerModel;
	delete debugCamera_;
}

void GameScene::ChackAllCollisions() {
	// 自弾リストと敵弾リストを取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の衝突判定
	for (EnemyBullet* bullet : enemyBullets) {
		if (!bullet)
			continue;
		CheckCollisionPair(player_, bullet);
	}
#pragma endregion

#pragma region 自弾と敵キャラの衝突判定
	for (PlayerBullet* bullet : playerBullets) {
		if (!bullet)
			continue;
		CheckCollisionPair(bullet, enemy_);
	}
#pragma endregion

#pragma region 自弾と敵弾の衝突判定
	for (PlayerBullet* playerBullet : playerBullets) {
		if (!playerBullet)
			continue;
		for (EnemyBullet* enemyBullet : enemyBullets) {
			if (!enemyBullet)
				continue;
			CheckCollisionPair(playerBullet, enemyBullet);
		}
	}
#pragma endregion
}


void GameScene::Update() {
	
	

	  viewProjection_.UpdateMatrix(); // ←これが必要！



#ifdef _DEBUG

	


if (input_->TriggerKey(DIK_TAB)) {
		  isDebugCameraActive_ = !isDebugCameraActive_;
	  }


	#endif
	

	if (isDebugCameraActive_) {
	
		debugCamera_->Update();

		viewProjection_.matView =debugCamera_->GetCamera().matView;
		viewProjection_.matProjection = debugCamera_->GetCamera().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		//ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}

		player_->Updata(); 

		if (enemy_) {

		    enemy_->Update();
	    }

		ChackAllCollisions();

	ImGui::Text("Space: %s", input_->PushKey(DIK_SPACE) ? "Held" : "Not held");
	    ImGui::Text("Space Trigger: %s", input_->TriggerKey(DIK_SPACE) ? "Triggered" : "Not triggered");
	    ImGui::Text("DebugCamera Active: %s", isDebugCameraActive_ ? "True" : "False");
}

void GameScene::Draw() { 
	
	  DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// モデルの描画前処理（これが cmdList をセットする）
	Model::PreDraw(dxCommon->GetCommandList());

	// モデル描画
	player_->Draw(viewProjection_);

	
		if (enemy_) {

		enemy_->Draw(viewProjection_);
	}

	// モデルの描画後処理
	Model::PostDraw();

}