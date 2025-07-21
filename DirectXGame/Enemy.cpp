#include "Enemy.h"
#define NOMINMAX
#include "MapChipFiled.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#define _USE_MATH_DEFINES
#include <numbers>

using namespace KamataEngine;

void Enemy::Initialize(Model* model,const Vector3& position,uint32_t textureHandle) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	
}

void Enemy::Update() {

	const float kMoveSpeed = 0.1f;

	Vector3 velocity = {0, 0, kMoveSpeed};

	worldTransform_.translation_ -= velocity;


	WorldTrnasformUpdate(worldTransform_);

}

void Enemy::Draw(const Camera& camera) {

	model_->Draw(worldTransform_, camera, textureHandle_);



}