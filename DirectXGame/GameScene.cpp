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

GameScene::~GameScene() {
	
	delete player_;
	delete playerModel;
	delete debugCamera_;
}

void GameScene::ChackAllCollisions() {

	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の衝突判定

	posA = player_->GetPosition();
	float playerRadius = player_->GetRadius();

	for (EnemyBullet* bullet : enemyBullets) {
		posB = bullet->GetPosition();
		float bulletRadius = bullet->GetRadius();

		// 距離計算
		Vector3 diff = posA - posB;
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		float collisionDist = playerRadius + bulletRadius;

		if (distanceSq <= collisionDist * collisionDist) {
			player_->OnCollision();
			bullet->OnCollision();
		}
	}


	#pragma endregion


	#pragma region 自弾と敵キャラの衝突判定

	
	Vector3 enemyPos = enemy_->GetPosition();
	float enemyRadius = enemy_->GetRadius();

	for (PlayerBullet* bullet : playerBullets) {
		Vector3 bulletPos = bullet->GetPositon();
		float bulletRadius = bullet->GetRadius();

		Vector3 diff = bulletPos - enemyPos;
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		float collisionDist = bulletRadius + enemyRadius;

		if (distanceSq <= collisionDist * collisionDist) {
			bullet->OnCollision();
			enemy_->OnCollision();
		}
	}

	#pragma endregion


	#pragma region 自弾と敵弾の衝突判定

	//これがまだできていない

for (PlayerBullet* playerBullet : playerBullets) {
		Vector3 playerPos = playerBullet->GetPositon(); // ← posA → playerPos に変更
		float radiusA = playerBullet->GetRadius();

		for (EnemyBullet* enemyBullet : enemyBullets) {
			Vector3 enemyPosition = enemyBullet->GetPosition();
			float radiusB = enemyBullet->GetRadius();

			Vector3 diff = playerPos - enemyPos;
			float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
			float collisionDist = radiusA + radiusB;

			if (distanceSq <= collisionDist * collisionDist) {
				playerBullet->OnCollision();
				enemyBullet->OnCollision();
			}
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