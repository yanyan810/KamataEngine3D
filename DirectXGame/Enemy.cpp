//#include "Enemy.h"
//#define NOMINMAX
//#include "MapChipFiled.h"
//#include "WorldTransformClass.h"
//#include <algorithm>
//#include <cassert>
//#include <cmath>
//#include <numbers>
//
//void Enemy::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
//	// モデルの読み込み
//	model_ = model;
//
//	textureHandle_ = textureHandle;
//
//	// ワールドトランスフォームの初期化
//	worldTransform_.Initialize();
//
//		worldTransform_.translation_ = position;
//	worldTransform_.rotation_.y =-std::numbers::pi_v<float> / 2.0f;
//
//	// カメラの初期化
//	camera_ = camera;
//
//
//
//}
//
//void Enemy::Update() {
//	// ワールドトランスフォームの更新
//	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
//	worldTransform_.TransferMatrix();
//}
//
//void Enemy::Draw() {
//	// モデルの描画
//	if (model_) {
//		model_->Draw(worldTransform_, *camera_, textureHandle_);
//	}
//}