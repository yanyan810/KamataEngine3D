#pragma once
#include "Enemy.h"
#include "GameParams.h"
#include "Goal.h"
#include "KamataEngine.h"
#include "Player.h" // あなたの既存
#include "Rock.h"
#include "Scene.h"
#include "Skydome.h"
#include "Stage.h"
#include "playerBullet.h"

#include <memory>
enum class GamePhase {
	Title,
	DifficultySelect,
	FadeOutToPlay,
	FadeInFromTitle,
	Countdown,
	Play,
	FadeOutToOver, // 👈 追加！
	Over,
	FadeOutToClear,
	FadeInFromClear,
	Clear,
	FadeOutToTitle,
	FadeInToTitle
};

enum class GameSubPhase {
	None,     // 通常プレイ中
	PauseMenu // メニュー中
};

// フェードアウトの元（どこからTitleへ戻るか）
enum class EndScreen { None, Over, Clear };



class GameScene : public Scene {
public:
	void Initialize();
	void Update();
	void Draw();

	void UpdatePlay();
	void UpdateClear();
	void UpdateOver();
	void UpdateTitle();

	void CheckCollisions();

	void UpdateCountdown();

	void UpdateTitleDemo();

	void CheckCollisionsTitle();

	void UpdateGroundScroll(float dt); // 内部ヘルパ

	void UpdateDifficultySelect();

private:
	GameParams gParams_;
	GamePhase phase_ = GamePhase::Title;

	// KamataEngine::Camera* camera_ = nullptr; // 既存のカメラを使用
	KamataEngine::Camera camera_;
	Player player_;
	std::vector<std::unique_ptr<Bullet>> bullets_;
	std::vector<std::unique_ptr<Rock>> rocks_;
	std::vector<std::unique_ptr<Aquatic>> aquatics_;
	Goal goal_;

	void SpawnBulletFromPlayer(); // 弾生成

	int rockSpawnTimer_ = rand() % 30 + 30; // ロック生成タイマー

	int rockSpawnCount_ = 0;      // 今まで生成した数
	const int kMaxRocks = 45;     // 最大数
	float elapsedTimeSec_ = 0.0f; // 経過時間（秒単位）

	bool goalSpawned_ = false;       // ゴール出現したか
	bool stopRockSpawn_ = false;     // 岩生成を止めるか
	float goalSpawnTimeSec_ = 45.0f; // 何秒後にゴールを出すか（調整用）

	bool spawnLeftSide_ = true; // ← 左右交互に出す用

	float fadeTimer_ = 0.0f;
	float fadeDuration_ = 1.0f;
	bool isFading_ = false;

	// スプライト
	std::unique_ptr<KamataEngine::Sprite> fadeSprite_;

	Stage stage_;
	KamataEngine::Model* skydomeModel_ = nullptr;
	Skydome skydome_;
	float countdownTimer_ = 4.0f;

	std::unique_ptr<KamataEngine::Sprite> waterOverlaySprite_;
	std::unique_ptr<KamataEngine::Sprite> fullFishSprite_;
	std::unique_ptr<KamataEngine::Sprite> damageFish1Sprite_;
	std::unique_ptr<KamataEngine::Sprite> damageFish2Sprite_;

	// イクラ残弾UI
	std::unique_ptr<KamataEngine::Sprite> ammoSprite_;
	// 数字スプライト
	std::unique_ptr<KamataEngine::Sprite> numberSprites_[10];

	int numberTex_[10];
	std::unique_ptr<KamataEngine::Sprite> digitSprites_[4];

	// カウントダウン表示用のスプライト（最大2桁まで表示対応）
	std::unique_ptr<KamataEngine::Sprite> countdownDigitSprites_[2];

	std::unique_ptr<KamataEngine::Sprite> startSprite_;
	bool showStart_ = false;         // START表示フラグ
	float startDisplayTimer_ = 0.0f; // 表示時間カウント

	std::unique_ptr<KamataEngine::Sprite> controlUiSprite_;

	int titleRockSpawnTimer_ = 0;
	bool titleSpawnLeftSide_ = true;

	// ポーズメニュー選択値（0=ゲームに戻る, 1=タイトルへ）
	int pauseSelection_ = 0;

	GameSubPhase subPhase_ = GameSubPhase::None;

	// GameScene.h
	//ゲームオーバー画面
	std::unique_ptr<KamataEngine::Sprite> gameOverSpriteHigh_; // ammo >= 500 の時
	std::unique_ptr<KamataEngine::Sprite> gameOverSpriteLow_;  // ammo < 500 の時
	//ゲームクリア画面
	std::unique_ptr<KamataEngine::Sprite> gameClearSprite_;

	//タイトル画面
	std::unique_ptr<KamataEngine::Sprite> titleSprite_;
	std::unique_ptr<KamataEngine::Sprite> pressESC_;
	std::unique_ptr<KamataEngine::Sprite> pressSpaceStart_;

	//文字
	std::unique_ptr<KamataEngine::Sprite> pressSpace_; //ゲームクリアとゲームオーバーに表示
	
	std::unique_ptr<KamataEngine::Model> groundModel_;
	KamataEngine::WorldTransform groundTransform_;

	KamataEngine::WorldTransform groundA_;
	KamataEngine::WorldTransform groundB_;
	// GameScene.h の初期値（テスト用の見やすい設定）
	float groundSpeed_ = 600.0f;   // 速め
	float groundTileLen_ = 200.0f; // 短め（巻き替えが頻繁に起きて視認しやすい）

	// ポーズメニュー画像
	std::unique_ptr<KamataEngine::Sprite> pauseReturnGameSprite_;
	std::unique_ptr<KamataEngine::Sprite> pauseReturnTitleSprite_;

	EndScreen fadeFrom_ = EndScreen::None;

	private:
	// …既存の最後の方に追加
	bool isHardMode_ = false; // ハードモードON/OFF
	float appearZ_ = 20.0f; // 表示開始Z距離（100や200に調整可）

	std::unique_ptr<KamataEngine::Sprite> hardDarkOverlay_; // 暗幕スプライト


	 int diffSelectIndex_ = 0; // 0:Normal, 1:Hard, 2:Back to Title

	// 難易度選択画面のボタン画像
	std::unique_ptr<KamataEngine::Sprite> diffTitleSprite_;
	std::unique_ptr<KamataEngine::Sprite> diffNormalSprite_;
	std::unique_ptr<KamataEngine::Sprite> diffHardSprite_;
	std::unique_ptr<KamataEngine::Sprite> diffBackSprite_;

};
