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

		skydome_ = new Skydome();
	    modelSkydome_ = Model::CreateFromOBJ("skydome",true);
	    skydome_->Initialize(modelSkydome_,&viewProjection_);

	debugCamera_ = new DebugCamera(1280, 720);
	input_ = Input::GetInstance();
	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetCamera(&viewProjection_);

	collisionManager_ = new CollisionManager();

	viewProjection_.farZ = 10000.0f; // ← 天球がしっかり入るように大きくする
	viewProjection_.UpdateMatrix();  // farZを変えたら必ず再計算



}
//void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
//	// 衝突フィルタリング
//	if ((colliderA->GetCollisionMask() & colliderB->GetCollisionAttribute()) == 0 || (colliderB->GetCollisionMask() & colliderA->GetCollisionAttribute()) == 0) {
//		return; // どちらかが当たらない設定になっている場合、スキップ
//	}
//
//	// 実際の当たり判定
//	Vector3 posA = colliderA->GetWorldPosition();
//	Vector3 posB = colliderB->GetWorldPosition();
//
//	float dx = posA.x - posB.x;
//	float dy = posA.y - posB.y;
//	float dz = posA.z - posB.z;
//
//	float distSq = dx * dx + dy * dy + dz * dz;
//	float radiusSum = colliderA->GetRadius() + colliderB->GetRadius();
//
//	if (distSq <= radiusSum * radiusSum) {
//		colliderA->OnCollision();
//		colliderB->OnCollision();
//	}
//}
//

GameScene::~GameScene() {
	
	delete player_;
	delete playerModel;
	delete debugCamera_;
	delete collisionManager_;
	collisionManager_ = nullptr;
	delete skydome_;
	delete modelSkydome_;
}


void GameScene::CheckAllCollisions() {
	// コリジョンマネージャのリストを一度クリア
	collisionManager_->Clear();

	// プレイヤーと自弾
	if (player_) {
		collisionManager_->AddCollider(player_);
		for (PlayerBullet* bullet : player_->GetBullets()) {
			if (bullet) {
				collisionManager_->AddCollider(bullet);
			}
		}
	}

	// 敵と敵弾
	if (enemy_) {
		collisionManager_->AddCollider(enemy_);
		for (EnemyBullet* bullet : enemy_->GetBullets()) {
			if (bullet) {
				collisionManager_->AddCollider(bullet);
			}
		}
	}

	// 総当たりチェックはマネージャに任せる
	collisionManager_->CheckAllCollisions();
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

		collisionManager_->Clear(); // 登録を毎フレームリセット

	    // 登録
	    if (player_) {
		    collisionManager_->AddCollider(player_);
		    for (auto* bullet : player_->GetBullets()) {
			    if (bullet)
				    collisionManager_->AddCollider(bullet);
		    }
	    }
	    if (enemy_) {
		    collisionManager_->AddCollider(enemy_);
		    for (auto* bullet : enemy_->GetBullets()) {
			    if (bullet)
				    collisionManager_->AddCollider(bullet);
		    }
	    }

		CheckAllCollisions();

		skydome_->Update();

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

		// 天球の描画
	skydome_->Draw();

	// モデルの描画後処理
	Model::PostDraw();

}