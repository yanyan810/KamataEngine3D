#include "GameScene.h"
// #include "PrimitiveDrawer.h"

// AL302_13の27ページから再開


using namespace KamataEngine;

// ====== 共通ユーティリティ ======
static inline bool IntersectAABB(const AABB& a, const AABB& b) {
	return (a.min.x <= b.max.x && a.max.x >= b.min.x) && (a.min.y <= b.max.y && a.max.y >= b.min.y) && (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

static inline bool IntersectSphereSphere(const Vector3& c1, float r1, const Vector3& c2, float r2) {
	const Vector3 d = c1 - c2;
	const float dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
	const float rr = (r1 + r2) * (r1 + r2);
	return dist2 <= rr;
}

void GameScene::Initialize() {
	// 初期化処理
	

	viewProjection_.Initialize();
	worldTransform_.Initialize();

	playerModel = Model::CreateFromOBJ("player",true);
	playerTextureHandle_ = TextureManager::Load("sample.png");
	player_ = new Player();
	player_->Initialize(playerModel, playerTextureHandle_);

	//近接攻撃用
	player_->LoadMell();

	enemyModel_ = Model::CreateFromOBJ("enemy",true);
	enemyTextureHandle_ = TextureManager::Load("uvChecker.png");
	//enemies_ = new Enemy();

	Vector3 position = {20.0f, 0.0f, 150.0f};


	//enemy_->Initialize(enemyModel_, position, enemyTextureHandle_);

		//enemy_->SetPlayer(player_); // プレイヤーを敵に設定

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
	viewProjection_.translation_.z = -25.0f*1.0f;

	EnemyBullet::LoadModel();
	PlayerBullet::LoadModel(); 

	cameraController = new CameraController();
	cameraController->Initialize(&viewProjection_);
	cameraController->SetTarget(player_);

	// === ステージ全体の可動範囲 ===
	cameraController->SetMovableArea({-10.0f, 10000.0f, -50.0f, 50.0f});

	// === 横スクロール設定 ===
	cameraController->SetMode(CameraController::Mode::SideScrollByPlayer);

	// プレイヤーが画面のこの位置まで来たらカメラが動く（左/右の“デッドゾーン”）
	cameraController->SetSideWindow(-2.0f, +6.5f);

	// プレイヤーが画面外に出ないようにする帯（画面中央±half）
	cameraController->SetScrollWindowHalf(20.0f);

	// 左方向への戻りスクロールを禁止（横スク風）
	cameraController->AllowBackScroll(false);

	// 可動範囲を保存（ロックエリア復帰などに使う想定なら）
	prevCamArea_ = {-10.0f, 10000.0f, -50.0f, 50.0f};

	// フェード生成
	fade_ = new Fade();
	fade_->Initialize();

	// プレイ開始はフェードなし（必要なら FadeIn してもOK）
	// fade_->Start(Fade::Status::FadeIn, 1.0f);

	phase_ = GamePhase::kPlay;
	finished_ = false;

	// ライン式ゴール（X=1000）
	goal_.InitializeLineX(150.0f);
	goal_.SetOneShot(true);

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

void GameScene::ResolveHits() {
	// --- 近接 vs 敵 ---
	if (player_->IsMeleeActive()) {
		AABB meleeAabb = player_->GetMeleeAABB();

		for (auto& up : enemies_) {
			Enemy* e = up.get();
			if (!e || !e->IsAlive())
				continue;

			// AABB vs AABB（敵は半径rのAABBで近似）
			if (IntersectAABB(meleeAabb, e->GetAABB())) {
				// ★ ヒット
				e->OnCollision();
				// 必要なら多段防止で近接を終了させるAPIを用意して呼ぶ
				// player_->ForceEndMelee();
			}
		}
	}
}


GameScene::~GameScene() {
	
	delete player_;
	delete playerModel;
	delete debugCamera_;
	delete collisionManager_;
	collisionManager_ = nullptr;
	delete skydome_;
	delete cameraController;
	cameraController = nullptr;
	delete modelSkydome_;
	delete fade_;
	fade_ = nullptr;
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
	for (auto& e : enemies_) {
		if (!e || !e->IsAlive())
			continue;
		collisionManager_->AddCollider(e.get());
		for (EnemyBullet* bullet : e->GetBullets()) {
			if (bullet)
				collisionManager_->AddCollider(bullet);
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

	// ゴール未達成の通常プレイ中だけプレイヤー更新
	if (phase_ == GamePhase::kPlay) {
		player_->Updata();

		for (auto& e : enemies_) {
			e->Update();
		}

		ResolveHits();

		cameraController->Update();
	}

	OutputDebugStringA(std::format("[GameScene] enemies_.size(before)={}\n", enemies_.size()).c_str());

	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](const std::unique_ptr<Enemy>& e) { return !e || !e->IsAlive(); }), enemies_.end());
	// 敵が全滅したら解除
	if (lockActive_ && !AnyEnemyAlive()) {
		lockActive_ = false;

		player_->DisableLockX();
		cameraController->SetMovableArea({prevCamArea_.left, prevCamArea_.right, prevCamArea_.bottom, prevCamArea_.top});

		// ★ スクロール凍結を解除（これ必須）
		cameraController->SetScrollFrozen(false);

		// （任意）次のロックに備えて
		// lockEngaged_ = false;
	}

	// ===== ロック発火条件 =====
	// 例）プレイヤーがロック区間に入った & まだエンゲージしてない → ロック開始＆敵を出すなど
	const float px = player_->GetPosition().x;
	if (!lockEngaged_ && px >= lockMinX_ && px <= lockMaxX_) {
		lockEngaged_ = true;
		lockActive_ = true;

		// プレイヤーも区間にロック
		player_->EnableLockX(lockMinX_, lockMaxX_);

		// カメラの可動範囲も区間に縛る（既存）
		cameraController->SetMovableArea({lockMinX_, lockMaxX_, prevCamArea_.bottom, prevCamArea_.top});

		// ★ スクロール凍結 ON（カメラXを固定）
		cameraController->SetScrollFrozen(true);

		// ここで「停止中の可動半幅」をセット（= 画面端まで動かしたい幅）
		// 値の目安: 通常 playHalfWidth_ より大きく。ステージスケールにより 10〜16 あたりから調整
		cameraController->SetFrozenPlayerHalf(16.0f); // ←お好みで微調整

		// 敵スポーン
		SpawnWaveInLockArea(5);
	}

	// ===== ロック維持中：敵がいる限りロック =====

	// ゴール到達でフェードアウト開始（1.0秒はお好みで）
	if (phase_ == GamePhase::kPlay && goal_.IsReached()) {
		if (fade_)
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		phase_ = GamePhase::kFadeOutOnGoal;
		// ここでプレイヤー操作停止にしたいなら、フラグで Updata() を止める運用にしてOK
	}

	// フェード更新
	if (fade_)
		fade_->Update();

	// フェードアウト完了でシーン終了
	if (phase_ == GamePhase::kFadeOutOnGoal && fade_ && fade_->IsFinished()) {
		finished_ = true;
	}

	// ゴール判定（Goal クラスを使用）
	goal_.Update(*player_);
	if (goal_.IsReached()) {
		// ゴール時の処理：今はログだけ
		OutputDebugStringA("[GameScene] ゴール！\n");

		// ここで “プレイヤーの操作停止・フェード開始・シーン遷移” などを行う
		// 例：
		// fade_->Start(Fade::Status::FadeOut, 1.0f);
		// phase_ = GamePhase::kFadeOutOnGoal;
	}
	

	/*	if (enemy_) {

		    enemy_->Update();
	    }*/

		collisionManager_->Clear(); // 登録を毎フレームリセット

	    // 登録
	    if (player_) {
		    collisionManager_->AddCollider(player_);
		    for (auto* bullet : player_->GetBullets()) {
			    if (bullet)
				    collisionManager_->AddCollider(bullet);
		    }
	    }

	    for (auto& e : enemies_) {
		    if (!e || !e->IsAlive())
			    continue;
		    collisionManager_->AddCollider(e.get());
		    for (auto* bullet : e->GetBullets()) {
			    if (bullet)
				    collisionManager_->AddCollider(bullet);
		    }
	    }

		CheckAllCollisions();

		skydome_->Update();

	/*ImGui::Text("Space: %s", input_->PushKey(DIK_SPACE) ? "Held" : "Not held");
	    ImGui::Text("Space Trigger: %s", input_->TriggerKey(DIK_SPACE) ? "Triggered" : "Not triggered");
	    ImGui::Text("DebugCamera Active: %s", isDebugCameraActive_ ? "True" : "False");*/
}

void GameScene::Draw() { 
	
	  DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// モデルの描画前処理（これが cmdList をセットする）
	Model::PreDraw(dxCommon->GetCommandList());

	// モデル描画
	player_->Draw(viewProjection_);

	
for (auto& e : enemies_) {
		e->Draw(viewProjection_);
	}

		// 天球の描画
	skydome_->Draw();

	goal_.DrawDebug3D();

	// モデルの描画後処理
	Model::PostDraw();

		if (fade_)
		fade_->Draw();

}

void GameScene::SpawnWaveInLockArea(int count) {
	const float camX = viewProjection_.translation_.x; // カメラX
	const float screenHalf = 20.0f;                    // SetScrollWindowHalf と同じ値
	const float marginBase = 4.0f;                     // 画面外余白
	const float eps = 0.25f;
	const float lockWidth = (lockMaxX_ - lockMinX_);

	// 画面外に出す“理想距離”をロック幅に合わせて安全に丸める
	float desiredEdge = screenHalf + marginBase;      // 右の画面外までの理想距離
	float maxEdgeWithinLock = 0.5f * lockWidth - eps; // ロック内で取れる最大距離
	float edge = std::min<float>(desiredEdge, std::max<float>(maxEdgeWithinLock, 0.0f));
	if (edge < 1.0f)
		edge = 1.0f; // 極端に狭い時の保険

	// Z=0 は避ける（プレイヤーZと重なりを防ぐ）
	const float zOffsets[] = {-3.5f, -2.0f, +2.0f, +3.5f};

	const Vector3 pc = player_->GetWorldPosition();
	const float minGapX = 2.0f; // プレイヤーから最低X離隔（右側へ）

	for (int i = 0; i < count; ++i) {
		// ★ 全部右から
		float x = camX + edge; // まず“右の画面外”へ
		x = std::clamp(x, lockMinX_ + eps, lockMaxX_ - eps);

		// ★ 必ずプレイヤーの右側に最低距離を確保
		if (x <= pc.x + minGapX) {
			x = pc.x + minGapX;
			x = std::clamp(x, lockMinX_ + eps, lockMaxX_ - eps);
		}

		float z = zOffsets[i % (int)std::size(zOffsets)];
		// ついでにZが近すぎれば少し逃がす（任意）
		if (std::abs(z - pc.z) < 0.5f)
			z += (z <= pc.z ? -0.5f : +0.5f);

		auto e = std::make_unique<Enemy>();
		e->Initialize(enemyModel_, {x, 0.0f, z}, enemyTextureHandle_);
		e->SetPlayer(player_);
		enemies_.push_back(std::move(e));
	}
}



bool GameScene::AnyEnemyAlive() const {
	for (auto& e : enemies_) {
		if (e && e->IsAlive())
			return true;
	}
	return false;
}