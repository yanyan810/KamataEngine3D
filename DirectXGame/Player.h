#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "Matrix4x4_.h"
#include "WorldTransformClass.h"
#include <cassert>
#include "Easing.h"
#include "Operator.h"
#include "PlayerBullet.h"
#include "Collider.h"
#include "CollisionConfig.h"
/// <summary>
/// 自キャラ
/// </summary>
class Player : public Collider {

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();
	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Updata();

	
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(KamataEngine::Camera& viewProjection);

	/// <summary>
	/// 回転
	/// </summary>
	void Rotate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	KamataEngine::Vector3 GetPosition();

	// 衝突を検知したら曜日出されるコールバック関数
	//void OnCollision();

		// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	float GetRadius() const { return 1.0f; } // 自キャラの半径（仮値）

	 Vector3 GetWorldPosition() const override { return worldTransform_.translation_; }

	void OnCollision() override {
		// プレイヤーの衝突時処理
	}

	   // ゴール
	void SetGoalX(float x) { goalX_ = x; }  // ゴール位置を設定
	bool IsGoal() const { return isGoal_; } // ゴール済みフラグを返す


	//カメラ関係
	void EnableLockX(float minX, float maxX) {
		lockXEnabled_ = true;
		lockMinX_ = minX;
		lockMaxX_ = maxX;
	}
	void DisableLockX() { lockXEnabled_ = false; }

private:

		// 弾
	std::list<PlayerBullet*> bullets_;

	KamataEngine::Input* input_ = nullptr;
	
	KamataEngine::WorldTransform worldTransform_; // ワールド変形
	KamataEngine::Model* model_ = nullptr;        // モデル
	uint32_t textureHandle_ = 0;                  // テクスチャハンドル

	KamataEngine::Model* playerBulletModel = nullptr; // プレイヤーのモデル

	  // ゴール用
	float goalX_ = 0.0f;  // ゴール位置
	bool isGoal_ = false; // ゴール達成フラグ

	//カメラ追従関係
	bool lockXEnabled_ = false;
	float lockMinX_ = -FLT_MAX;
	float lockMaxX_ = FLT_MAX;

};
