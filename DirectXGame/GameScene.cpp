#define _HAS_CXX20 0
#include "GameScene.h"
#include "Collider.h"
#include "Stage.h"
#include <algorithm>
#include <memory>

using namespace KamataEngine;

void GameScene::Initialize() {

	bullets_.clear();
	rocks_.clear();
	aquatics_.clear();

	stage_.Initialize();

	// ゴールの再初期化
	goal_.Initialize({1180, 300, 0}, {1240, 420, 0}); // 元の初期化と同じ座標にしておく
	
	// 状態リセット
	elapsedTimeSec_ = 0.0f;
	goalSpawned_ = false;
	stopRockSpawn_ = false;

	// 経過時間などもリセット
	elapsedTimeSec_ = 0.0f;
	goalSpawned_ = false;
	stopRockSpawn_ = false;

	// phase_ = GamePhase::Title;

	// パラメータ（必要ならここで上書き可能）
	gParams_.playerMaxHP = 3;
	gParams_.maxAmmo = 1000;

	// 例：ロード（あなたの実装に合わせて）
	auto* mdlPlayer = KamataEngine::Model::CreateFromOBJ("player");
	auto* mdlBullet = KamataEngine::Model::CreateFromOBJ("ikura");
	auto* mdlRock = KamataEngine::Model::CreateFromOBJ("player");
	auto* mdlAquatic = KamataEngine::Model::CreateFromOBJ("enemy");
	auto* mdlGoal = KamataEngine::Model::CreateFromOBJ("Goal");

	// カメラ（既存の camera_ or debugCamera_->GetCamera() を使用）
	KamataEngine::Camera* cam = /* 例 */ &camera_; // or debugCamera_->GetCamera();

	camera_.Initialize();
	// camera_.translation_ = {-0.0f, -0.0f, 0.0f}; // 初期位置（必要なら調整）
	camera_.rotation_ = {-0.2f, -0.0f, 0.0f}; // 初期回転（必要なら調整）
	camera_.TransferMatrix();                 // または camera_.UpdateMatrix(); ←実装次第

	Player::SetGraphics(mdlPlayer, cam);
	Bullet::SetGraphics(mdlBullet, cam);
	Rock::SetGraphics(mdlRock, cam);
	Aquatic::SetGraphics(mdlAquatic, cam);
	Goal::SetGraphics(mdlGoal, cam);

	player_.Initialize({0, 0, -35}, gParams_);

	player_.SetOnFireCallback([this](const Vector3& pos, const Vector3& vel) {
		auto bullet = std::make_unique<Bullet>();
		bullet->Initialize(pos, vel);
		bullets_.push_back(std::move(bullet));
	});

	// 岩の生成
	Vector3 min, max;

	// 岩のランダムサイズと出現位置をここで生成
	// ランダムなX/YサイズとZ出現位置
	float x = static_cast<float>(rand() % 20 - 10);
	float y = static_cast<float>(rand() % 15 - 10);
	float w = 1.5f;
	float h = 1.5f;
	float d = static_cast<float>(rand() % 3 + 1);

	float z = 500.0f; // Zの出現位置（奥）

	min = {x, y, z};
	max = {x + w, y + h, z + d};

	auto r = std::make_unique<Rock>();
	r->Initialize(min, max); // ← これが **絶対に必要！！**
	rocks_.push_back(std::move(r));
	
	// 水生生物
	Aquatic a1;
	a1.Initialize({900, 260, 0}, {-120, 0, 0}, gParams_.enemyRadius);
	Aquatic a2;
	a2.Initialize({1100, 480, 0}, {-130, 0, 0}, gParams_.enemyRadius);
	{
		auto e = std::make_unique<Aquatic>();
		e->Initialize({900, 260, 0}, {-120, 0, 0}, gParams_.enemyRadius);
		aquatics_.push_back(std::move(e));
	}
	{
		auto e = std::make_unique<Aquatic>();
		e->Initialize({1100, 480, 0}, {-130, 0, 0}, gParams_.enemyRadius);
		aquatics_.push_back(std::move(e));
	}

	goal_.Initialize({1180, 300, 0}, {1240, 420, 0});

	rockSpawnCount_ = 0;
	rockSpawnTimer_ = 0;

	elapsedTimeSec_ = 0.0f;
	goalSpawned_ = false;
	stopRockSpawn_ = false;
	goalSpawnTimeSec_ = 60.0f; // 例：60秒でゴール出現

	// goal_.Initialize();

	fadeSprite_ = std::unique_ptr<Sprite>(KamataEngine::Sprite::Create(0, {0, 0})); // テクスチャハンドルは0でもOK（真っ黒）
	fadeSprite_->SetSize({1280, 720});                                              // 画面サイズ
	fadeSprite_->SetColor({0, 0, 0, 0});                                            // 最初は透明

	// 最初だけ Title、それ以外は変更しない
	/*if (phase_ == GamePhase::Title || phase_ == GamePhase::Over || phase_ == GamePhase::Clear) {
		phase_ = GamePhase::Title;
	}*/



	stage_.Initialize();

	skydomeModel_ = KamataEngine::Model::CreateFromOBJ("skydome");

	skydome_.Initialize(skydomeModel_, &camera_);

	uint32_t texWaterOverlay = TextureManager::Load("Images/water.png");
	waterOverlaySprite_ = std::unique_ptr<Sprite>(Sprite::Create(texWaterOverlay, {0, 0}));
	waterOverlaySprite_->SetSize({1280, 720});
	waterOverlaySprite_->SetColor({1, 1, 1, 0.4f}); // 半透明

	int texFull = TextureManager::Load("Images/FullFish.png");
	int texDmg1 = TextureManager::Load("Images/damegeFish1.png");
	int texDmg2 = TextureManager::Load("Images/damegeFish2.png");

	fullFishSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texFull, {0, 0}));
	damageFish1Sprite_ = std::unique_ptr<Sprite>(Sprite::Create(texDmg1, {0, 0}));
	damageFish2Sprite_ = std::unique_ptr<Sprite>(Sprite::Create(texDmg2, {0, 0}));

	fullFishSprite_->SetSize({128, 128});
	damageFish1Sprite_->SetSize({128, 128});
	damageFish2Sprite_->SetSize({128, 128});

	// イクラアイコン
	int texAmmo = TextureManager::Load("Images/ikura.png");
	ammoSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texAmmo, {98, 32}));
	ammoSprite_->SetSize({64, 64});

	for (int i = 0; i < 10; i++) {
		std::string path = "Images/" + std::to_string(i) + ".png";
		numberTex_[i] = TextureManager::Load(path.c_str());

		numberSprites_[i] = std::unique_ptr<Sprite>(Sprite::Create(numberTex_[i], {0, 0}));
		numberSprites_[i]->SetSize({64, 64});
	}

	// 各桁分のスプライトを初期化（最初は全部0）
	for (int i = 0; i < 4; i++) {
		digitSprites_[i] = std::unique_ptr<Sprite>(Sprite::Create(numberTex_[0], {0, 0}));
		digitSprites_[i]->SetSize({64, 64});
	}

	for (int i = 0; i < 2; i++) {
		countdownDigitSprites_[i] = std::unique_ptr<Sprite>(Sprite::Create(numberTex_[0], {0, 0}) // 仮の位置・0番画像
		);
		countdownDigitSprites_[i]->SetSize({128, 128}); // 大きく表示
	}

	int texStart = TextureManager::Load("Images/start.png");
	startSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texStart, {640 - 128, 200}) // 画面中央に近い位置に表示
	);
	startSprite_->SetSize({256, 256}); // サイズ調整（画像サイズに応じて）

	int texControl = TextureManager::Load("Images/UI.png");
	controlUiSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texControl, {20, 48})); // 左上に表示
	controlUiSprite_->SetSize({256, 256});                                            // 画像サイズに合わせて調整

	titleRockSpawnTimer_ = 0;
	titleSpawnLeftSide_ = true;

	int texOverHigh = TextureManager::Load("Images/IkuraDonEnd.png"); // 500以上用
	int texOverLow = TextureManager::Load("Images/sushiEnd.png");   // 500未満用
	int texClear = TextureManager::Load("/Images/GameClear.png");


	gameOverSpriteHigh_ = std::unique_ptr<Sprite>(Sprite::Create(texOverHigh, {0, 0}));
	gameOverSpriteHigh_->SetSize({1280, 720});

	gameOverSpriteLow_ = std::unique_ptr<Sprite>(Sprite::Create(texOverLow, {0, 0}));
	gameOverSpriteLow_->SetSize({1280, 720});

	gameClearSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texClear, {0, 0}));
	gameClearSprite_->SetSize({1280, 720});

	// --- タイトル画面用 ---
	int texTitle = TextureManager::Load("Images/Title.png");
	int texPressESC = TextureManager::Load("Images/QuitButton.png");               // 「Press ESC to Quit」
	int texPressSpaceSta = TextureManager::Load("Images/StartButton.png"); // 「Press SPACE to Start」

	titleSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texTitle, {0, 0}));
	pressESC_ = std::unique_ptr<Sprite>(Sprite::Create(texPressESC, {0, 0}));
	pressSpaceStart_ = std::unique_ptr<Sprite>(Sprite::Create(texPressSpaceSta, {0, 0}));

	// 画面にフィットさせる例（1280x720想定）
	titleSprite_->SetSize({1280, 720});

	// 位置はお好みで調整
	pressSpaceStart_->SetPosition({640 - 256, 400}); // 中央下あたり
	pressSpaceStart_->SetSize({512, 256});

	pressESC_->SetPosition({640 - 220, 520}); // さらに下
	pressESC_->SetSize({440, 256});

	// --- クリア/オーバーで共通の「Press SPACE」 ---
	int texPressSpace = TextureManager::Load("Images/PressSpace.png");
	pressSpace_ = std::unique_ptr<Sprite>(Sprite::Create(texPressSpace, {0, 0}));
	pressSpace_->SetPosition({640 - 220, 480});
	pressSpace_->SetSize({440, 256});

	// 地面モデルをロード
	
	// 地面モデルをロード（既存）
	groundModel_.reset(KamataEngine::Model::CreateFromOBJ("ground", true));

	// 2枚のトランスフォーム
	groundA_.Initialize();
	groundB_.Initialize();
	groundA_.scale_ = {10.0f, 1.0f, 1.0f};
	groundB_.scale_ = groundA_.scale_;

	// Aを手前、Bを奥（Z正）に「ちょうど1枚分」だけ離して置く
	groundA_.translation_ = {0.0f, -5.0f, 0.0f};
	groundB_.translation_ = {0.0f, -5.0f, groundTileLen_}; // ← ここがポイント
	groundA_.TransferMatrix();
	groundB_.TransferMatrix();

	int texPauseGame = TextureManager::Load("Images/returnGame.png");   // ゲームに戻る
	int texPauseTitle = TextureManager::Load("Images/goTitle.png"); // タイトルに戻る

	pauseReturnGameSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texPauseGame, {0, 0}));
	pauseReturnGameSprite_->SetSize({1280, 720});

	pauseReturnTitleSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texPauseTitle, {0, 0}));
	pauseReturnTitleSprite_->SetSize({1280, 720});

	if (isHardMode_) {
		// 黒1x1テクスチャを用意しておく（無ければ適当に黒画像をリソースに追加）
		uint32_t texBlack = TextureManager::Load("Images/black.png");
		hardDarkOverlay_ = std::unique_ptr<Sprite>(Sprite::Create(texBlack, {0, 0}));
		hardDarkOverlay_->SetSize({1280, 720});
		hardDarkOverlay_->SetColor({0, 0, 0, 0.9f}); // 暗さ（0.6〜0.85くらい好みで）
	}

	// === 難易度選択UI ===
	int texDiffTitle = TextureManager::Load("Images/DiffTitle.png"); // 例: 「SELECT DIFFICULTY」
	int texDiffNormal = TextureManager::Load("Images/NormalButton.png");
	int texDiffHard = TextureManager::Load("Images/HardButton.png");
	int texDiffBack = TextureManager::Load("Images/BackButton.png");

	diffTitleSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texDiffTitle, {0, 0}));
	diffNormalSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texDiffNormal, {0, 0}));
	diffHardSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texDiffHard, {0, 0}));
	diffBackSprite_ = std::unique_ptr<Sprite>(Sprite::Create(texDiffBack, {0, 0}));

	// レイアウト（1280x720想定）
	diffTitleSprite_->SetSize({1280, 720});
	diffTitleSprite_->SetPosition({0, 0});

	diffNormalSprite_->SetSize({1280, 720});
	diffHardSprite_->SetSize({1280, 720});
	diffBackSprite_->SetSize({1280, 720});

	diffNormalSprite_->SetPosition({0 , 0});
	diffHardSprite_->SetPosition({0, 0});
	diffBackSprite_->SetPosition({0, 0});

	// デフォルト：タイトルから来たときに先頭を選んでおく
	diffSelectIndex_ = 0;



}

void GameScene::CheckCollisions() {

	// プレイヤーと岩
	KamataEngine::Vector3 playerPos = player_.GetPosition();
	float playerRadius = player_.GetRadius();

	for (auto& rock : rocks_) {
		const AABB& aabb = rock->GetAABB();
		// 例：視界外はスキップ（値はお好みで）
		if (aabb.max.z < -80.0f || aabb.min.z > 900.0f)
			continue;

		// ==== 球 vs AABB 判定 ====
		KamataEngine::Vector3 nearest{
		    std::max<float>(aabb.min.x, std::min<float>(playerPos.x, aabb.max.x)),
		    std::max<float>(aabb.min.y, std::min<float>(playerPos.y, aabb.max.y)),
		    std::max<float>(aabb.min.z, std::min<float>(playerPos.z, aabb.max.z)),
		};

		KamataEngine::Vector3 diff = playerPos - nearest;
		float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		if (distSq <= playerRadius * playerRadius) {
			// ★ プレイヤーが無敵中（特に回転無敵）の場合は何もしない
			if (player_.IsRotating()) {
				// 当たっても無敵中は何も処理しない（岩も残す）
				continue;
			}

			// 通常当たり処理（岩を消す・ダメージを受ける）
			player_.Oncollision();
			rock->OnCollision();
		}
	}

	// ==== プレイヤー vs ゴール ====
	{
		const AABB& goalAABB = goal_.GetAABB();

		KamataEngine::Vector3 nearest{
		    std::max<float>(goalAABB.min.x, std::min<float>(playerPos.x, goalAABB.max.x)),
		    std::max<float>(goalAABB.min.y, std::min<float>(playerPos.y, goalAABB.max.y)),
		    std::max<float>(goalAABB.min.z, std::min<float>(playerPos.z, goalAABB.max.z)),
		};

		KamataEngine::Vector3 diff = playerPos - nearest;
		float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		if (distSq <= playerRadius * playerRadius) {
			// ★ ゴール到達！
			phase_ = GamePhase::FadeOutToClear;
			fadeTimer_ = 0.0f; // ← 念のためリセット
		}
	}
}

void GameScene::CheckCollisionsTitle() {
	KamataEngine::Vector3 playerPos = player_.GetPosition();
	float playerRadius = player_.GetRadius();

	for (auto& rock : rocks_) {
		const AABB& aabb = rock->GetAABB();

		KamataEngine::Vector3 nearest{
		    std::max<float>(aabb.min.x, std::min<float>(playerPos.x, aabb.max.x)),
		    std::max<float>(aabb.min.y, std::min<float>(playerPos.y, aabb.max.y)),
		    std::max<float>(aabb.min.z, std::min<float>(playerPos.z, aabb.max.z)),
		};
		KamataEngine::Vector3 diff = playerPos - nearest;
		float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		if (distSq <= playerRadius * playerRadius) {
			// タイトル時：HP/Ammoは減らさない。弾ばら撒き＋無敵だけ付与
			player_.TitleHit();
			rock->OnCollision(); // 岩は消す（見た目のため）
		}
	}
}

void GameScene::UpdateCountdown() {
	countdownTimer_ -= 1.0f / 60.0f;

	if (countdownTimer_ <= 0.0f && !showStart_) {
		// START表示に切り替え
		showStart_ = true;
		startDisplayTimer_ = 2.0f; // 2秒間表示する
	}

	if (showStart_) {
		startDisplayTimer_ -= 1.0f / 60.0f;
		if (startDisplayTimer_ <= 0.0f) {
			showStart_ = false;
			phase_ = GamePhase::Play; // ゲームスタート！
		}
	}
}

void GameScene::UpdatePlay() {
	camera_.UpdateMatrix();

	// 入力：発射（※あとで実際のInputに接続）
	// if (input_->TriggerKey(DIK_SPACE)) {
	//     if (player_.TryFire()) SpawnBulletFromPlayer();
	// }

	// --- ゴールの出現判定 ---
	if (!goalSpawned_ && elapsedTimeSec_ >= goalSpawnTimeSec_) {
		KamataEngine::Vector3 goalPos{0.0f, 0.0f, 800.0f}; // 画面奥あたり
		goal_.Spawn(goalPos);

		// ★プレイヤー側（Z-方向）に近づける。速度は好みで調整（単位/秒）
		goal_.SetVelocity({0.0f, 0.0f, -150.0f}); // ← 例：毎秒 150 移動

		goalSpawned_ = true;
		stopRockSpawn_ = true;
	}

	float dt = 1.0f / 60.0f;

	elapsedTimeSec_ += 1.0f / 60.0f; // 毎フレーム加算（60FPS前提）

	player_.Update(dt);

	// 弾の更新
	if (player_.TryFire()) {
		SpawnBulletFromPlayer();
	}

	for (auto& b : bullets_)
		b->Update(dt);
	for (auto& r : rocks_)
		r->Update(dt);
	for (auto& e : aquatics_)
		e->Update(dt);
	goal_.Update(dt);

	if (!stopRockSpawn_) {
		if (--rockSpawnTimer_ <= 0 && rocks_.size() < kMaxRocks) {
			// --- 1. 左右交互に1個出す ---
			float x2 = spawnLeftSide_ ? -5.0f : +5.0f;
			spawnLeftSide_ = !spawnLeftSide_;

			float y2 = static_cast<float>(rand() % 8 - 5);
			float w2 = 1.5f;
			float h2 = 1.5f;
			float d2 = static_cast<float>(rand() % 1 + 1);
			float z = 800.0f;

			Vector3 min = {x2, y2, z};
			Vector3 max = {x2 + w2, y2 + h2, z + d2};

			auto r1 = std::make_unique<Rock>();
			r1->Initialize(min, max);
			rocks_.push_back(std::move(r1));

			// --- 2. 60%の確率で中央にも「もう1個」岩を出す ---
			if (rand() % 10 < 6) {                             // 50%
				float cx = static_cast<float>(rand() % 6 - 3); // 中央付近に散らす
				float cy = static_cast<float>(rand() % 10 - 8);
				float cz = 800.0f;
				Vector3 cmin = {cx, cy, cz};
				Vector3 cmax = {cx + w2, cy + h2, cz + d2};

				auto r2 = std::make_unique<Rock>();
				r2->Initialize(cmin, cmax);
				rocks_.push_back(std::move(r2));
			}

			// --- スポーン間隔調整 ---
			float baseInterval = 40.0f;
			float minInterval = 10.0f;
			float difficultyRatio = std::min<float>(elapsedTimeSec_ / 60.0f, 1.0f);
			float interval = baseInterval - (baseInterval - minInterval) * difficultyRatio;
			rockSpawnTimer_ = static_cast<int>(interval);
		}
	}

	CheckCollisions();

	//===================
	// 特定の条件で削除
	//==================
	bullets_.erase(
	    std::remove_if(
	        bullets_.begin(), bullets_.end(),
	        [](const std::unique_ptr<Bullet>& b) {
		        if (!b)
			        return true; // 念のため
		        const auto& p = b->GetPosition();
		        return b->IsDead() || p.x < -50.0f || p.x > 100.0f || p.y < -50.0f || p.y > 100.0f;
	        }),
	    bullets_.end());

	rocks_.erase(
	    std::remove_if(
	        rocks_.begin(), rocks_.end(),
	        [](const std::unique_ptr<Rock>& r) {
		        return r->IsDead() || r->GetAABB().max.z < -40.0f; // ★ 死亡 or 画面外
	        }),
	    rocks_.end());

	if (!player_.IsAlive()) {
		phase_ = GamePhase::FadeOutToOver; // 👈 フェードへ遷移
		fadeTimer_ = 0.0f;                 // タイマーリセット
	}
}

void GameScene::UpdateClear() {
	// 今の GamePhase::Clear の処理内容

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			fadeFrom_ = EndScreen::Clear; // ★ 追加
			phase_ = GamePhase::FadeOutToTitle;
			fadeTimer_ = 0.0f;
		}
}

void GameScene::UpdateOver() {



if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			fadeFrom_ = EndScreen::Over; // ★ 追加
			phase_ = GamePhase::FadeOutToTitle;
			fadeTimer_ = 0.0f;
		}
}

void GameScene::UpdateTitleDemo() {
	float dt = 1.0f / 60.0f;

	// ランダム泳ぎ
	player_.UpdateDemo(dt);

	for (auto& b : bullets_)
		b->Update(dt);

	// 岩の更新
	for (auto& r : rocks_)
		r->Update(dt);

	// === ここから追加：タイトルでも岩を定期スポーン ===
	if (--titleRockSpawnTimer_ <= 0 && rocks_.size() < 20) {
		// 左右交互に 1 個
		float x2 = titleSpawnLeftSide_ ? -5.0f : +5.0f;
		titleSpawnLeftSide_ = !titleSpawnLeftSide_;

		float y2 = static_cast<float>(rand() % 8 - 5);
		float w2 = 1.5f;
		float h2 = 1.5f;
		float d2 = static_cast<float>(rand() % 1 + 1);
		float z = 800.0f;

		KamataEngine::Vector3 min = {x2, y2, z};
		KamataEngine::Vector3 max = {x2 + w2, y2 + h2, z + d2};

		auto r1 = std::make_unique<Rock>();
		r1->Initialize(min, max);
		rocks_.push_back(std::move(r1));

		// ときどき中央にも 1 個（見栄え用）
		if (rand() % 10 < 2) {
			float cx = static_cast<float>(rand() % 6 - 3);
			float cy = static_cast<float>(rand() % 15 - 10);
			float cz = 800.0f;

			KamataEngine::Vector3 cmin = {cx, cy, cz};
			KamataEngine::Vector3 cmax = {cx + w2, cy + h2, cz + d2};

			auto r2 = std::make_unique<Rock>();
			r2->Initialize(cmin, cmax);
			rocks_.push_back(std::move(r2));
		}

		// タイトルは少しゆったり
		titleRockSpawnTimer_ = 50 + rand() % 40; // 50〜89フレーム
	}
	// === 追加ここまで ===

	// 画面外＆死亡の岩を掃除（既存）
	rocks_.erase(std::remove_if(rocks_.begin(), rocks_.end(), [](const std::unique_ptr<Rock>& r) { return r->IsDead() || r->GetAABB().max.z < -100.0f; }), rocks_.end());

	// タイトル用当たり（HPは触らない）
	CheckCollisionsTitle();
}

void GameScene::UpdateTitle() {
	// デモ更新はそのまま
	UpdateTitleDemo();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		phase_ = GamePhase::DifficultySelect; // ★ 変更：難易度選択へ
		diffSelectIndex_ = 0;
	}
}

void GameScene::UpdateGroundScroll(float dt) {
	// 手前（Zマイナス）に流す
	groundA_.translation_.z -= groundSpeed_ * dt;
	groundB_.translation_.z -= groundSpeed_ * dt;

	auto wrap = [this](KamataEngine::WorldTransform& t) {
		if (t.translation_.z <= -groundTileLen_) {
			t.translation_.z += groundTileLen_ * 2.0f; // ← 2枚ぶんだけ戻す
		}
		t.TransferMatrix();
	};
	wrap(groundA_);
	wrap(groundB_);
}

// ★ 追加：難易度選択
static void SetButtonColor(KamataEngine::Sprite* s, bool active) {
	if (!s)
		return;
	if (active)
		s->SetColor({1, 1, 1, 1});
	else
		s->SetColor({0.5f, 0.5f, 0.5f, 1});
}

void GameScene::UpdateDifficultySelect() {
	auto* input = Input::GetInstance();

	// 上下で 0..2 を移動
	if (input->TriggerKey(DIK_W) || input->TriggerKey(DIK_UP)) {
		diffSelectIndex_ = (diffSelectIndex_ + 3 - 1) % 3;
	}
	if (input->TriggerKey(DIK_S) || input->TriggerKey(DIK_DOWN)) {
		diffSelectIndex_ = (diffSelectIndex_ + 1) % 3;
	}

	// ハイライト色
	SetButtonColor(diffNormalSprite_.get(), diffSelectIndex_ == 0);
	SetButtonColor(diffHardSprite_.get(), diffSelectIndex_ == 1);
	SetButtonColor(diffBackSprite_.get(), diffSelectIndex_ == 2);

	// 決定
	if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE)) {
		if (diffSelectIndex_ == 0) {
			// Normal
			isHardMode_ = false;
			appearZ_ = 99999.0f;               // 実質フィルタ無し
			phase_ = GamePhase::FadeOutToPlay; // フェード経由でゲームへ
			fadeTimer_ = 0.0f;
		} else if (diffSelectIndex_ == 1) {
			// Hard
			isHardMode_ = true;
			appearZ_ = 100.0f; // 100 or 200 お好みで
			phase_ = GamePhase::FadeOutToPlay;
			fadeTimer_ = 0.0f;
		} else {
			// Back to Title
			phase_ = GamePhase::Title;
		}
	}

	// Esc でもタイトルに戻る
	if (input->TriggerKey(DIK_ESCAPE)) {
		phase_ = GamePhase::Title;
	}
}



void GameScene::Update() {

	skydome_.Update();

	// ★ ここに移動（switchの前）
	if (phase_ == GamePhase::Play && Input::GetInstance()->TriggerKey(DIK_TAB)) {
		if (subPhase_ == GameSubPhase::None) {
			subPhase_ = GameSubPhase::PauseMenu;
			pauseSelection_ = 0; // ★ ここで初期化
			if (fadeSprite_)
				fadeSprite_->SetColor({0, 0, 0, 0.4f});
		} else {
			subPhase_ = GameSubPhase::None;
			if (fadeSprite_)
				fadeSprite_->SetColor({0, 0, 0, 0.0f});
		}
	}

	switch (phase_) {
	case GamePhase::Title:
		UpdateTitle();

		 UpdateGroundScroll(1.0f / 60.0f);

		stage_.Update();
		break;
	case GamePhase::Play: {
		if (subPhase_ == GameSubPhase::None) {
			UpdatePlay();
			stage_.Update();

			 UpdateGroundScroll(1.0f / 60.0f);

			stage_.Update();
		} else {
			// ★ PauseMenu 中の入力処理
			auto* input = Input::GetInstance();

			// 0/1 トグル（W/↑ で 0、S/↓ で 1 に動かす。両方押したら先に反応した方を優先）
			if (input->TriggerKey(DIK_W) || input->TriggerKey(DIK_UP)) {
				pauseSelection_ = 0;
			}
			if (input->TriggerKey(DIK_S) || input->TriggerKey(DIK_DOWN)) {
				pauseSelection_ = 1;
			}

			// 決定（Enter or Space）
			if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE)) {
				if (pauseSelection_ == 0) {
					// 0: ゲームに戻る
					subPhase_ = GameSubPhase::None;
					if (fadeSprite_)
						fadeSprite_->SetColor({0, 0, 0, 0.0f});
				} else {
					// 1: タイトルへ
					phase_ = GamePhase::Title;
					if (fadeSprite_)
						fadeSprite_->SetColor({0, 0, 0, 0.0f});
					Initialize(); // 必要ならリセット
				}
			}
		}

		// 既存の TAB トグルはこの下にあるならそのままでもOK（重複に注意）
		// ...
		break;
	}

						case GamePhase::DifficultySelect:
		// タイトル背景などを動かしたいなら（任意）
		UpdateTitleDemo();        // 背景の流れ・岩などを演出として継続
		UpdateDifficultySelect(); // ★ 追加関数
		break;


	case GamePhase::Clear:
		UpdateClear();
		break;
	case GamePhase::Over:
		UpdateOver();
		waterOverlaySprite_->SetPosition({0, -200});

		break;

	case GamePhase::FadeOutToPlay: {
		// ★ フェードアウト中もタイトルのデモを動かす
		UpdateTitleDemo(); // ← 追加
		stage_.Update();   // ← 追加（タイトルと同等に床など更新）

		 UpdateGroundScroll(1.0f / 60.0f);

		fadeTimer_ += 1.0f / 60.0f;
		float t = std::clamp(fadeTimer_ / fadeDuration_, 0.0f, 1.0f);
		fadeSprite_->SetColor({0, 0, 0, t});

		if (t >= 1.0f) {
			// 真っ黒になったこの瞬間にゲーム側を初期化してからフェードインへ
			Initialize(); // ← 既存
			phase_ = GamePhase::FadeInFromTitle;
			fadeTimer_ = 0.0f;
		}
		break;
	}

	case GamePhase::FadeInFromTitle: {
		fadeTimer_ += 1.0f / 60.0f;
		float t = std::clamp(fadeTimer_ / fadeDuration_, 0.0f, 1.0f);
		fadeSprite_->SetColor({0, 0, 0, 1.0f - t});

		if (t >= 1.0f) {
			phase_ = GamePhase::Countdown;
			countdownTimer_ = 4.0f;
		}
		break;
	}
	case GamePhase::FadeOutToOver: {
		fadeTimer_ += 1.0f / 60.0f;
		float t = std::clamp(fadeTimer_ / fadeDuration_, 0.0f, 1.0f);
		fadeSprite_->SetColor({0, 0, 0, t}); // 黒フェード

		if (t >= 1.0f) {
			// ★ フェード終わったら透明に戻す
			fadeSprite_->SetColor({0, 0, 0, 0}); // 👈 これを追加！
			phase_ = GamePhase::Over;
		}
		break;
	}

	case GamePhase::FadeOutToClear: {
		// 黒へフェードアウト
		fadeTimer_ += 1.0f / 60.0f;
		float t = std::clamp(fadeTimer_ / fadeDuration_, 0.0f, 1.0f);
		fadeSprite_->SetColor({0, 0, 0, t});
		if (t >= 1.0f) {
			// 真っ黒になったら次はフェードインへ
			phase_ = GamePhase::FadeInFromClear;
			fadeTimer_ = 0.0f;
		}
		break;
	}

	case GamePhase::FadeInFromClear: {
		// Clear 画面へ遷移しつつ黒からフェードイン
		// Clear 用のUI等を出したいなら、この時点で一度だけ初期化してもOK
		if (fadeTimer_ == 0.0f) {
			// 必要ならここでClear画面のセットアップ
			// （今は特に不要なので何もしない）
		}
		fadeTimer_ += 1.0f / 60.0f;
		float t = std::clamp(fadeTimer_ / fadeDuration_, 0.0f, 1.0f);
		fadeSprite_->SetColor({0, 0, 0, 1.0f - t});
		if (t >= 1.0f) {
			phase_ = GamePhase::Clear; // 完全に明るくなったらClear状態に
		}
		break;
	}

	case GamePhase::Countdown:
		stage_.Update();
		UpdateCountdown();
		break;


		case GamePhase::FadeOutToTitle: {
		// 黒へフェードアウト（中身は今の画面＝Over/Clearのまま）
		fadeTimer_ += 1.0f / 60.0f;
		float t = std::clamp(fadeTimer_ / fadeDuration_, 0.0f, 1.0f);
		if (fadeSprite_)
			fadeSprite_->SetColor({0, 0, 0, t});

		if (t >= 1.0f) {
			// 真っ黒になったらタイトル準備してフェードインへ
			Initialize();                      // タイトル用の各種初期化（デモなども）
			phase_ = GamePhase::FadeInToTitle; // ★ 次のフェーズへ
			fadeTimer_ = 0.0f;
		}
		break;
	}

	case GamePhase::FadeInToTitle: {
		// タイトルのデモは動かしながら黒からフェードイン
		UpdateTitleDemo();
		stage_.Update();

		fadeTimer_ += 1.0f / 60.0f;
		float t = std::clamp(fadeTimer_ / fadeDuration_, 0.0f, 1.0f);
		if (fadeSprite_)
			fadeSprite_->SetColor({0, 0, 0, 1.0f - t});

		if (t >= 1.0f) {
			phase_ = GamePhase::Title; // 完了
		}
		break;
	}


	}



}

void GameScene::Draw() {

	switch (phase_) {
	case GamePhase::Title: {
		auto* dxCommon = DirectXCommon::GetInstance();

		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			skydome_.Draw();

			   // ★ 地面モデル描画
			if (groundModel_) {
				groundModel_->Draw(groundA_, camera_);
				groundModel_->Draw(groundB_, camera_);
			}

			// ★ 岩も描画（デモ演出）
			for (auto& r : rocks_)
				r->Draw();
			for (auto& b : bullets_)
				b->Draw();

			player_.Draw();
		}
		KamataEngine::Model::PostDraw();
		break;
	}

	case GamePhase::Play: {
		auto* dxCommon = DirectXCommon::GetInstance();
		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			skydome_.Draw();
			if (groundModel_) {
				groundModel_->Draw(groundA_, camera_);
				groundModel_->Draw(groundB_, camera_);
			}

			// ★ ハードモード時は距離フィルタを掛ける
			if (isHardMode_) {
				float pz = player_.GetPosition().z;
				for (auto& r : rocks_) {
					float ez = (r->GetAABB().min.z + r->GetAABB().max.z) * 0.5f;
					if (fabsf(ez - pz) <= appearZ_) {
						r->Draw();
					}
				}
				for (auto& e : aquatics_) {
					float ez = e->GetPosition().z;
					if (fabsf(ez - pz) <= appearZ_) {
						e->Draw();
					}
				}
			} else {
				// ノーマル時は全部描画
				for (auto& r : rocks_)
					r->Draw();
				for (auto& e : aquatics_)
					e->Draw();
			}

			for (auto& b : bullets_)
				b->Draw();
			goal_.Draw();
			player_.Draw();
		}
		KamataEngine::Model::PostDraw();
		break;
	}

	case GamePhase::Over: {
		auto* dxCommon = DirectXCommon::GetInstance();

		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			skydome_.Draw();
			player_.Draw(); // 👈 仰向けプレイヤーの描画
		}
		KamataEngine::Model::PostDraw();

			KamataEngine::Sprite::PreDraw(dxCommon->GetCommandList());

		// ★ 残弾でイラスト切り替え
		int ammo = player_.GetAmmo();
		if (ammo >= 500) {
			if (gameOverSpriteHigh_)
				pressSpace_->SetPosition({850, 300});
				gameOverSpriteHigh_->Draw();
		} else {
			if (gameOverSpriteLow_)
				pressSpace_->SetPosition({400, 450});
				gameOverSpriteLow_->Draw();
		}

			KamataEngine::Sprite::PostDraw();

		break;
	}

						case GamePhase::DifficultySelect: {
		auto* dxCommon = DirectXCommon::GetInstance();

		// 3D（背景/床/プレイヤー等）はタイトルと同じでOK
		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			skydome_.Draw();
			if (groundModel_) {
				groundModel_->Draw(groundA_, camera_);
				groundModel_->Draw(groundB_, camera_);
			}
			for (auto& r : rocks_)
				r->Draw(); // デモ演出
			for (auto& b : bullets_)
				b->Draw();
			player_.Draw();
		}
		KamataEngine::Model::PostDraw();

		// 2D UI
		KamataEngine::Sprite::PreDraw(dxCommon->GetCommandList());
		{
			if (waterOverlaySprite_)
				waterOverlaySprite_->Draw();

			if (diffTitleSprite_)
				diffTitleSprite_->Draw();
			if (diffNormalSprite_)
				diffNormalSprite_->Draw();
			if (diffHardSprite_)
				diffHardSprite_->Draw();
			if (diffBackSprite_)
				diffBackSprite_->Draw();
		}
		// フェードは最後に（必要なら）
		// fadeSprite_->Draw();

		KamataEngine::Sprite::PostDraw();
		break;
	}


	case GamePhase::Clear: {

		auto* dxCommon = DirectXCommon::GetInstance();

		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			skydome_.Draw();
		}

			KamataEngine::Sprite::PreDraw(dxCommon->GetCommandList());

			gameClearSprite_->Draw();

				KamataEngine::Sprite::PostDraw();

		break;
	}
	case GamePhase::Countdown: {
		auto* dxCommon = DirectXCommon::GetInstance();
		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			skydome_.Draw();
			stage_.Draw(&camera_);

			if (groundModel_) {
				groundModel_->Draw(groundA_, camera_);
				groundModel_->Draw(groundB_, camera_);
			}

			player_.Draw();
		}
		break;
	}

	case GamePhase::FadeOutToPlay: { // ★ 追加
		auto* dxCommon = DirectXCommon::GetInstance();
		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			skydome_.Draw();
			stage_.Draw(&camera_);
			if (groundModel_) {
				groundModel_->Draw(groundA_, camera_);
				groundModel_->Draw(groundB_, camera_);
			}
			for (auto& r : rocks_)
				r->Draw();
			for (auto& b : bullets_)
				b->Draw();
			player_.Draw();
		}
		KamataEngine::Model::PostDraw();
		break;
	}

	case GamePhase::FadeOutToTitle: {
		auto* dxCommon = DirectXCommon::GetInstance();
		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			// 背景など（必要なら Over/Clear と同じ描画内容に）
			skydome_.Draw();
			// Over時はプレイヤーを仰向けで出しているならそれも
			player_.Draw();
		}
		KamataEngine::Model::PostDraw();

		// --- スプライト層 ---
		KamataEngine::Sprite::PreDraw(dxCommon->GetCommandList());
		{
			// ★ フェード元に応じて画像を出す
			if (fadeFrom_ == EndScreen::Over) {
				// 残弾で切替（>=500 / <500）
				int ammo = player_.GetAmmo();
				if (ammo >= 500) {
					if (gameOverSpriteHigh_)
						gameOverSpriteHigh_->Draw();
				} else {
					if (gameOverSpriteLow_)
						gameOverSpriteLow_->Draw();
				}
				// もし「Press SPACE」なども乗せたいならここで Draw()

			} else if (fadeFrom_ == EndScreen::Clear) {
				if (gameClearSprite_)
					gameClearSprite_->Draw();
				// 「Press SPACE」なども必要ならここで Draw()
			}

			// 最後に黒フェード（既存）
			if (fadeSprite_)
				fadeSprite_->Draw();
		}
		KamataEngine::Sprite::PostDraw();
		break;
	}


	case GamePhase::FadeInToTitle: {
		auto* dxCommon = DirectXCommon::GetInstance();
		KamataEngine::Model::PreDraw(dxCommon->GetCommandList());
		{
			skydome_.Draw();
			stage_.Draw(&camera_);
			if (groundModel_) {
				groundModel_->Draw(groundA_, camera_);
				groundModel_->Draw(groundB_, camera_);
			}
			for (auto& r : rocks_)
				r->Draw();
			for (auto& b : bullets_)
				b->Draw();
			player_.Draw();
		}
		KamataEngine::Model::PostDraw();
		break;
	}

	}

	auto* dxCommon = DirectXCommon::GetInstance();


	KamataEngine::Sprite::PreDraw(dxCommon->GetCommandList());

	if (phase_ == GamePhase::FadeInToTitle||phase_ == GamePhase::Title || phase_ == GamePhase::Play || phase_ == GamePhase::Countdown || phase_ == GamePhase::FadeInFromTitle ||
	    phase_ == GamePhase::FadeOutToPlay) {
		if (waterOverlaySprite_) {
			waterOverlaySprite_->Draw();
		}

			// ★ ハードモードなら暗幕を最後に描く
		if (isHardMode_ && hardDarkOverlay_) {
			hardDarkOverlay_->Draw();
		}
		
	}


	
	// ===== タイトル =====
	if (phase_ == GamePhase::Title ||phase_== GamePhase::FadeInToTitle) {
		if (titleSprite_)
			titleSprite_->Draw();
		if (pressSpaceStart_)
			pressSpaceStart_->Draw();
		if (pressESC_)
			pressESC_->Draw();
	}

	// ===== クリア／オーバー =====
	if (phase_ == GamePhase::Over || phase_ == GamePhase::Clear) {
		if (pressSpace_)
			pressSpace_->Draw(); // 「Press SPACE」
	}

	if(phase_ == GamePhase::Play || phase_ == GamePhase::Countdown){

		// 🎯 HP表示（重ね順に注意）
		if (fullFishSprite_)
			fullFishSprite_->Draw();

	}

	if (phase_ == GamePhase::Play) {
		int hp = player_.GetHP(); // プレイヤーの現在HP

		if (hp <= 2 && damageFish1Sprite_)
			damageFish1Sprite_->Draw();
		if (hp <= 1 && damageFish2Sprite_)
			damageFish2Sprite_->Draw();
	}

	if (phase_ == GamePhase::Play || phase_ == GamePhase::Countdown) {
		if (ammoSprite_) {
			ammoSprite_->Draw(); // イクラアイコン
		}

		if (phase_ == GamePhase::Play || phase_ == GamePhase::Countdown) {
			int ammo = player_.GetAmmo();

			// 文字列に変換
			std::string ammoStr = std::to_string(ammo);

			// 4桁になるまで先頭に '0' を足す
			while (ammoStr.length() < 4) {
				ammoStr = "0" + ammoStr;
			}

			float startX = 175.0f;
			float y = 32.0f;
			float digitWidth = 32.0f;

			for (int i = 0; i < 4; i++) {
				int digit = ammoStr[i] - '0';
				digitSprites_[i]->SetTextureHandle(numberTex_[digit]);
				digitSprites_[i]->SetPosition({startX + i * digitWidth, y});
				digitSprites_[i]->Draw();
			}
		}
	}

	if (showStart_) {
		startSprite_->Draw();
	} else if (phase_ == GamePhase::Countdown) {
		int countNum = static_cast<int>(countdownTimer_);

		if (countNum > 0) {
			std::string countStr = std::to_string(countNum);

			// カウントダウンの中央位置
			float centerX = 640.0f; // 画面幅の中心（仮）
			float y = 300.0f;
			float digitWidth = 128.0f;

			size_t len = countStr.size();
			float totalWidth = len * digitWidth;
			float startX = centerX - totalWidth / 2.0f;

			for (size_t i = 0; i < len; i++) {
				int digit = countStr[i] - '0';
				countdownDigitSprites_[i]->SetTextureHandle(numberTex_[digit]);
				countdownDigitSprites_[i]->SetPosition({startX + i * digitWidth, y});
				countdownDigitSprites_[i]->Draw();
			}
		}
	}

	if (phase_ == GamePhase::Play || phase_ == GamePhase::Countdown) {
		if (controlUiSprite_) {
			controlUiSprite_->Draw();
		}
	}

	if (subPhase_ == GameSubPhase::PauseMenu) {
		if (pauseReturnGameSprite_) {
			if (pauseSelection_ == 0)
				pauseReturnGameSprite_->SetColor({1, 1, 1, 1}); // 白
			else
				pauseReturnGameSprite_->SetColor({0.5f, 0.5f, 0.5f, 1}); // グレー
			pauseReturnGameSprite_->Draw();
		}
		if (pauseReturnTitleSprite_) {
			if (pauseSelection_ == 1)
				pauseReturnTitleSprite_->SetColor({1, 1, 1, 1});
			else
				pauseReturnTitleSprite_->SetColor({0.5f, 0.5f, 0.5f, 1});
			pauseReturnTitleSprite_->Draw();
		}
	}


	// フェードスプライトは常に最後
	fadeSprite_->Draw();

	KamataEngine::Sprite::PostDraw();
}

void GameScene::SpawnBulletFromPlayer() {
	auto b = std::make_unique<Bullet>();

	// プレイヤーの後ろに出す（Z-方向）
	// ベクトル: {0, 0, -1} * 速度
	b->Initialize(
	    player_.GetPosition() + KamataEngine::Vector3{0, 0, -1.0f}, // 後ろ側に出す
	    KamataEngine::Vector3{0, 0, -100.0f},                       // Zマイナス方向へ
	    2.0f);
	bullets_.push_back(std::move(b));
}
