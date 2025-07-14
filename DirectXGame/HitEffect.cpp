#include "HitEffect.h"

// 静的メンバ変数の実態
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(KamataEngine::Vector3& spawnPosition) {
	HitEffect* instance = new HitEffect();
	// newの失敗を検出
	assert(instance);
	// インスタンスの初期化
	instance->Initialize(spawnPosition);
	//初期化したインスタンスを返す
	return instance;
}

void HitEffect::Initialize(KamataEngine::Vector3& spawnPosition) {
	// ワールドトランスフォームの初期化   
	circleWorldTransform.Initialize();
	// 初期位置を設定
	circleWorldTransform.translation_ = spawnPosition;
	// スケールを設定
	circleWorldTransform.scale_ = {1.0f, 1.0f, 1.0f};
	// 回転を設定
	circleWorldTransform.rotation_ = {0.0f, 0.0f, 0.0f};
}

void HitEffect::Update() {
	// エフェクトの更新処理

	circleWorldTransform.matWorld_ = MakeAffineMatrix(circleWorldTransform.scale_, circleWorldTransform.rotation_, circleWorldTransform.translation_);
	circleWorldTransform.TransferMatrix();		
	WorldTrnasformUpdate(circleWorldTransform);

}

void HitEffect::Draw() {
	// モデルが設定されている場合のみ描画


	if (model_) {
		model_->Draw(circleWorldTransform, *camera_);
	}
}