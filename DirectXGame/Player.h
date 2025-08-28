#pragma once
#include "GameParams.h"
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include "Operator.h"
#include <functional>
class Player {
public:
	// 既存API
	void Initialize(const KamataEngine::Vector3& pos, const GameParams& params);
	void Update(float dt);
	void Draw();

	bool TryFire();
	void OnHit();

	// ★ 追加: モデル＆カメラ注入
	static void SetGraphics(KamataEngine::Model* model, KamataEngine::Camera* camera);

	// Getter
	const KamataEngine::Vector3& GetPosition() const { return pos_; }
	float GetRadius() const { return params_.playerRadius; }
	int GetHP() const { return hp_; }
	int GetAmmo() const { return ammo_; }
	bool IsAlive() const { return hp_ > 0; }

	void Oncollision();

	bool IsRotating() const { return rotationTime_ > 0.0f; }

	void SetDeadPose(); // 👈 追加


	// 弾を出すためのコールバック（GameSceneから渡す）
	using FireCallback = std::function<void(const KamataEngine::Vector3& pos, const KamataEngine::Vector3& vel)>;
	void SetOnFireCallback(const FireCallback& cb) { onFireCallback_ = cb; }

	 void UpdateDemo(float dt);

	void FireRadialBullets(); // 回転時に呼ぶ

	// タイトル中の当たり判定（HP関連の副作用なし）
	void CheckCollisionsTitle();

	// タイトルでのヒット演出（HP/Ammoを減らさない）
	void TitleHit();


private:
	KamataEngine::Vector3 pos_{};
	KamataEngine::Vector3 vel_{};
	int hp_ = 20;
	int ammo_ = 1000;
	float invincibleTimer_ = 0.0f;
	GameParams params_{};

	// ★ 追加: 描画用
	KamataEngine::WorldTransform world_{};

	// ★ 追加: 静的共有リソース
	static inline KamataEngine::Model* s_model_ = nullptr;
	static inline KamataEngine::Camera* s_camera_ = nullptr;
	float rotationTime_ = 0.0f; // 回転演出用タイマー
	float rotationDuration_ = 0.0f; // 全体の時間（補間計算用）
	float rotationStartZ_ = 0.0f; // ← 回転前の角度
	 float moveSpeed_ = 0.3f;

	KamataEngine::ObjectColor objectColor_; // オブジェクトカラー管理

	 KamataEngine::Vector4 color_;

	 FireCallback onFireCallback_ = nullptr;

};
