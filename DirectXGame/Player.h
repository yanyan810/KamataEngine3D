#pragma once
#include "KamataEngine.h"
#include <cassert>
#include "WorldTransformClass.h"
#include "Matrix4x4_.h"
/// <summary>
/// 自キャラ
/// </summary>
class Player {

	enum class LRDirection {
		kRight,
		kLeft,
	};

	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="model">モデル</param>
		/// <param name="textureHandle">テクスチャハンドル</param>
	    void Initialize(KamataEngine::Model* model, uint32_t textureHandle,KamataEngine::Camera* camera,const KamataEngine::Vector3& position);

		/// <summary>
		/// 更新
		/// </summary>
		void Updata();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

		const KamataEngine::WorldTransform& GetWoldTransform() const;

		void SetPosition(const KamataEngine::Vector3& position);

		const KamataEngine::Vector3& GetVelosity() const { return velosity_; }

		const KamataEngine::Vector3& GetPosition() const { return worldTransform_.translation_; }

		private:
	    KamataEngine::WorldTransform worldTransform_;

		KamataEngine::Model* model_ = nullptr;

		KamataEngine::Camera* camera_ = nullptr;

		uint32_t textureHandle_ = 0u;

		KamataEngine::Vector3 velosity_ = {};
	    
		static inline const float kAcceleration = 0.1f;

		static inline const float kAttenuation = 0.05f;

		static inline const float kLimitRunSpeed = 1.0f;

		LRDirection lrDirection_ = LRDirection::kRight;

		//旋回時の角度
	    float turnFirstRotationY_ = 0.0f;
		//旋回タイマー
	    float turnTimer_ = 0.0f;
		//旋回時間<秒>
	    static inline const float kTurnTime = 0.3f;

		//設置状態フラグ
	    bool onGround_ = true;

		//重力加速度
	    static inline const float kGravityAcceleration = 0.1f;
		//最大落下速度
	    static inline const float kLimitFallSpeed = 1.0f;
		//ジャンプ初速
	    static inline const float kJumpAcceleration = 1.0f;
};

