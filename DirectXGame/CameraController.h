#pragma once
#include "KamataEngine.h"

class Player;

// 矩形
struct Rect {
	float left = 0.0f;
	float right = 1.0f;
	float bottom = 0.0f;
	float top = 1.0f;
};

class CameraController {
public:
	enum class Mode { Follow, AutoScroll, SideScrollByPlayer };

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	void SetMovableArea(Rect area) { movableArea_ = area; }

	void SetMode(Mode m) { mode_ = m; }
	// 画面中心から見た「窓」の左右オフセット（例：左=-2, 右=+6）
	void SetSideWindow(float left, float right) {
		windowLeft_ = left;
		windowRight_ = right;
	}
	// プレイヤーが画面からはみ出さない帯の半幅（画面中央±half）
	void SetScrollWindowHalf(float half) { playHalfWidth_ = half; }
	// 左方向への“戻りスクロール”を許可するか
	void AllowBackScroll(bool allow) { allowBackScroll_ = allow; }

	void SetScrollFrozen(bool f);

	void SetFrozenPlayerHalf(float half) { frozenPlayerHalf_ = half; } // 停止中だけ使う可動半幅

private:
	Mode mode_ = Mode::Follow;

	// ビュープロジェクション
	KamataEngine::Camera* camera_ = nullptr;
	Player* target_ = nullptr;
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};
	Rect movableArea_ = {0, 100, 0, 100};
	KamataEngine::Vector3 goalPosition_ = {0, 0, 0};
	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f;
	// 速度掛け率
	static inline const float kVelosityBias = 5.0f;

	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect marginArea_ = {-3.0f, 3.0f, -2.0f, 2.0f};

	// === SideScrollByPlayer 用 ===
	float windowLeft_ = -2.0f;     // カメラ中心から見た左の閾値
	float windowRight_ = 6.0f;     // カメラ中心から見た右の閾値
	float playHalfWidth_ = 8.0f;   // プレイヤーの画面内拘束帯の半幅
	bool allowBackScroll_ = false; // 既定は“戻らない”（横スクロールっぽく）

	//スクロール
	bool scrollFrozen_ = false;
	float frozenCamX_ = 0.0f;
	float frozenPlayerHalf_ = 12.0f; // ★デフォルト。画面半分くらいに調整（お好みで）


};