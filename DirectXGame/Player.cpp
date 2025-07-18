#define NOMINMAX
#include "Player.h"

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();

}

void Player::Updata() {

}

void Player::Draw() {

}