#pragma once
#include "KamataEngine.h"
#include "Matrix4x4_.h"
#include "WorldTransformClass.h"
#include <cassert>

class MapChipField;

/// <summary>
/// 自キャラ
/// </summary>
class Player {

	

public:

	enum class LRDirection {
		kRight,
		kLeft,
	};

	enum Corner {
		kRightBottom,//右下
		kLeftBottom,//左下
		kRightTop,//右上
		kLeftTop,//左上

		kNumCenter // 要素数
	};

	// マップとの当たり判定情報
	struct ColisionMapInfo {
		bool celling = false;            // 天井
		bool landing = false;           // 床
		bool isWall = false;             // 壁
		KamataEngine::Vector3 velosity_; // 移動量
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Updata();

	void PlayerMove();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	const KamataEngine::WorldTransform& GetWoldTransform() const;

	void SetPosition(const KamataEngine::Vector3& position);

	const KamataEngine::Vector3& GetVelosity() const { return velosity_; }

	const KamataEngine::Vector3& GetPosition() const { return worldTransform_.translation_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void MapCollisionCheck(ColisionMapInfo& info);

	void MapCllisionCheckUp(Player::ColisionMapInfo& info);

	void MapCllisionCheckDown(Player::ColisionMapInfo& info);

	void MapCllisionCheckLeft(Player::ColisionMapInfo& info);

	void MapCllisionCheckRight(Player::ColisionMapInfo& info);

	void OnSwichGround(const Player::ColisionMapInfo& info);

	void IsCelling(Player::ColisionMapInfo& info);

	void IsWall(Player::ColisionMapInfo& info);

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;
	KamataEngine::WorldTransform worldTransform_;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	void MoveAccordingResult(const ColisionMapInfo& info);

	void WorldPosUpdate(const ColisionMapInfo& info);

private:

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	uint32_t textureHandle_ = 0u;

	KamataEngine::Vector3 velosity_ = {};

	static inline const float kAcceleration = 0.05f;

	static inline const float kAttenuation = 0.05f;

	static inline const float kLimitRunSpeed = 0.7f;

	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTurnTime = 0.3f;

	// 設置状態フラグ
	bool onGround_ = true;

	// 重力加速度
	static inline const float kGravityAcceleration = 0.1f;
	// 最大落下速度
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速
	static inline const float kJumpAcceleration = 1.0f;
	static inline const float kBlank = 2.0f;
	static inline const float kLandingAdjust = 0.0001f; // 着地調整量
	static inline const float kAttenuationLanding = 0.1f;
	static inline const float smallNum = 0.05f;
	static inline const float kAttenuationWall = 0.01f;
	const float kEpsilon = 0.01f;


};
