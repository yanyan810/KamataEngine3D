#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include "Easing.h"
#include "Operator.h"
#include "EnemyState.h"
#include "EnemyBullet.h"
#include "TimedeCall.h"
#include "Collider.h"
//#include "Player.h"

class Player;

class Enemy: public Collider {
public:

	~Enemy();

	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, uint32_t textureHandle);
	void Update();
	void Draw(const KamataEngine::Camera& camera);

	void SetState(EnemyState* newState);
	void Move(const KamataEngine::Vector3& velocity);
	const KamataEngine::Vector3& GetPosition() const;

	void Fire();

	//発射感覚
	static const int kFIreInterval = 60;

	void ApproachInitialize();

	void DecrementFireTimer();       // タイマーを1減らす
	bool IsFireTimerExpired() const; // 0以下になったか

	/// <summary>
	/// 球を発射しリセットする関数
	/// </summary>
	void FireReset();

	void SetPlayer(Player* player) { player_ = player; }

	KamataEngine::Vector3 GetWorldPosition();

	//衝突を検知したら曜日出されるコールバック関数
	void OnCollision();

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }
	float GetRadius() const { return 1.0f; } // 敵の半径（仮値）

	KamataEngine::Vector3 GetWorldPosition() const override { return worldTransform_.translation_; }


private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	EnemyState* state_ = nullptr;

	std::list<EnemyBullet*> bullets_;

	KamataEngine::Input* input_ = nullptr;

	//発射タイマー
	int32_t fireTimer = 0;

	std::list<TimedeCall> timedCalls_;

	Player* player_ = nullptr;

	
};