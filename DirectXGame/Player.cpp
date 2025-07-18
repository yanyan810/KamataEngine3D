#define NOMINMAX
#include "Player.h"
#include "MapChipFiled.h"
#include "WorldTransformClass.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Player::Corner corner) {
	KamataEngine::Vector3 offsetTable[Player::Corner::kNumCenter] = {
	    {+Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0.0f}, // kRightBottom
	    {-Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0.0f}, // kLeftBottom
	    {+Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0.0f}, // kRightTop
	    {-Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0.0f}, // kLeftTop
	};

	KamataEngine::Vector3 offset = offsetTable[static_cast<uint32_t>(corner)];
	return {center.x + offset.x, center.y + offset.y, center.z + offset.z};
}

void Player::MapCllisionCheckUp(Player::ColisionMapInfo& info) {
	// 上昇あり？
	if (info.velosity_.y <= 0.0f) {
		return;
	}

	Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

	std::array<Vector3, Player::Corner::kNumCenter> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldPos, static_cast<Corner>(i));
	}

	MapChipField::MapChipType mapChipType;
	MapChipField::MapChipType mapChipTypeNext;
	// 真上のあたり判定を行う
	bool hit = false;
	// (自キャラの)左上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// 右上の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);

		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// float newTopY = positionsNew[kRightTop].y - kHeight / 2.0f;
			info.velosity_.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			// info.velosity.y = std::max(0.0f, rect.bottom - newTopY + kBlank);
			DebugText::GetInstance()->ConsolePrintf("info.move.y = %f", rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f));

			// 天井に当たったことを記録する
			info.celling = true;
		}
	}
}
void Player::MapCllisionCheckDown(Player::ColisionMapInfo& info) {

	std::array<Vector3, Player::Corner::kNumCenter> positionsNew;

	Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldPos, static_cast<Player::Corner>(i));
	}
	// 下降あり？

	if (info.velosity_.y > 0.0f) {
		return;
	}
	MapChipField::MapChipType mapChipType;
	MapChipField::MapChipType mapChipTypeNext;
	// 真上のあたり判定を行う
	bool hit = false;

	// 「移動後」のキャラ中心座標
	Vector3 moveLeftBottom = {positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y - kLandingAdjust, positionsNew[kLeftBottom].z};
	Vector3 moveRightBottom = {positionsNew[kRightBottom].x, positionsNew[kRightBottom].y - kLandingAdjust, positionsNew[kRightBottom].z};

	// 左下の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// 右下の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);

		if (indexSetNow.yIndex != indexSet.yIndex) {

			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.velosity_.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			// 地面に当たったことを記録する
			info.landing = true;
		}
	}
}

void Player::MapCllisionCheckRight(Player::ColisionMapInfo& info) {

	std::array<Vector3, Player::Corner::kNumCenter> positionsNew;

	Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldPos, static_cast<Player::Corner>(i));
	}

	// 右移動あり？
	if (info.velosity_.x <= 0.0f) {
		return;
	}

	MapChipField::MapChipType mapChipType;
	MapChipField::MapChipType mapChipTypeNext;

	bool hit = false;

	// 「移動後」のキャラ中心座標
	Vector3 moveRightTop = {positionsNew[kRightTop].x, positionsNew[kRightTop].y - kLandingAdjust, positionsNew[kRightTop].z};
	Vector3 moveRightBottom = {positionsNew[kRightBottom].x, positionsNew[kRightBottom].y - kLandingAdjust, positionsNew[kRightBottom].z};

	MapChipField::IndexSet indexSet;

	// 右上の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// 右下の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);
		//// めり込み先ブロックの範囲矩形
		//
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// info.move.x = std::min(0.0f, rect.left - (worldTransform_.translation_.x + (kWidth / 2.0f + kBlank)));
			info.velosity_.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			// 壁に当たったことを記録する
			info.isWall = true;
		}
	}
}

void Player::MapCllisionCheckLeft(Player::ColisionMapInfo& info) {

	std::array<Vector3, Player::Corner::kNumCenter> positionsNew;

	Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldPos, static_cast<Player::Corner>(i));
	}

	// 右移動あり？
	if (info.velosity_.x > 0.0f) {
		return;
	}

	MapChipField::MapChipType mapChipType;
	MapChipField::MapChipType mapChipTypeNext;

	bool hit = false;

	// 「移動後」のキャラ中心座標
	Vector3 moveLeftTop = {positionsNew[kLeftTop].x, positionsNew[kLeftTop].y - kLandingAdjust, positionsNew[kLeftTop].z};
	Vector3 moveLefttBottom = {positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y - kLandingAdjust, positionsNew[kLeftBottom].z};

	MapChipField::IndexSet indexSet;

	// 左上の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// 左下の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);

		MapChipField::IndexSet indexSetNow;

		indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);

		// めり込み先ブロックの範囲矩形
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.velosity_.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			// 壁に当たったことを記録する
			info.isWall = true;
		}
	}
}

void Player::OnSwichGround(const Player::ColisionMapInfo& info) {
	// 自キャラが接地状態？
	if (onGround_) {

		// ジャンプ開始
		if (velosity_.y > 0.0f) {
			onGround_ = false;
		}

		std::array<Vector3, Player::Corner::kNumCenter> positionsNew;
		Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldPos, static_cast<Player::Corner>(i));
		}
		MapChipField::MapChipType mapChipType;
		// 真上のあたり判定を行う
		bool hit = false;
		// 左下の判定
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipField::MapChipType::kBlock) {
			hit = true;
		}
		// 右下の判定
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipField::MapChipType::kBlock) {
			hit = true;
		}

		// 落下開始
		if (!hit) {
			// 空中状態に切り替える
			onGround_ = false;
		}

	} else {

		// 着地フラグ
		if (info.landing) {
			// 着地状態に切り替える(落下を止める)
			onGround_ = true;
			// 着地時にX座標を減衰
			velosity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velosity_.y = 0.0f;
		}
	}
}

void Player::IsWall(Player::ColisionMapInfo& info) {
	if (info.isWall) {
		info.velosity_.x *= (1.0f - kAttenuationWall);
	}
}

void MoveAccordingResult(WorldTransform& worldTransform, const Player::ColisionMapInfo& info) {

	worldTransform.translation_.x += info.velosity_.x;
	worldTransform.translation_.y += info.velosity_.y;
	worldTransform.translation_.z += info.velosity_.z;
}

void Player::IsCelling(Player::ColisionMapInfo& info) {

	if (info.landing) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		info.velosity_.y = 0;
	}
}

void Player::MapCollisionCheck(ColisionMapInfo& info) {
	MapCllisionCheckUp(info);
	MapCllisionCheckDown(info);
	MapCllisionCheckRight(info);
	MapCllisionCheckLeft(info);
}

KamataEngine::Vector3 Player::GetWorldPosition() {

	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() {

	AABB aabb;
	aabb.min = {worldTransform_.translation_.x - kWidth / 2.0f, worldTransform_.translation_.y - kHeight / 2.0f, worldTransform_.translation_.z - kWidth / 2.0f};
	aabb.max = {worldTransform_.translation_.x + kWidth / 2.0f, worldTransform_.translation_.y + kHeight / 2.0f, worldTransform_.translation_.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	// velosity_.x += 1.0f;
	// velosity_.y += 1.0f;
	// velosity_.z += 1.0f;

	if (IsAttack()) {
		return; // 無敵中なら何もしない
	}

	isDead_ = true;
}

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Model* attackModel, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	camera_ = camera;
	attackModel_ = attackModel;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = position;
	worldTransformAttack_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransformAttack_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 初期角度を90度に設定
}

void Player::PlayerMove() {

	if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT) || KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

		// 左右加速
		KamataEngine::Vector3 acceleration = {};
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {
			// 左移動中の右入力
			if (velosity_.x < 0.0f) {
				// 速度と逆方向の場合ブレーキ
				velosity_.x *= (1.0f - kAttenuation);
			}
			acceleration.x += kAcceleration;

			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnTimer_ = 1.0f;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
			}

		} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

			if (velosity_.x > 0.0f) {
				// 速度と逆方向の場合ブレーキ
				velosity_.x *= (1.0f - kAttenuation);
			}

			acceleration.x -= kAcceleration;

			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				turnTimer_ = 1.0f;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
			}
		}
		velosity_.x += acceleration.x;

		// 最大速度制限
		velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	} else {

		velosity_.x *= (1.0f - kAttenuation);
	}

	// 接地状態
	if (onGround_) {

		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ
			if (onGround_) {
				velosity_.y = kJumpAcceleration;
				// onGround_ = false;
			}
		}
		// 空中
	} else {

		// 着地

		velosity_.x += 0.0f;
		velosity_.y -= kGravityAcceleration;
		velosity_.z += 0.0f;
		// 落下速度制限
		velosity_.y = std::max(velosity_.y, -kLimitFallSpeed);
	}
}

void Player::TurnUpdata() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 30.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,

		};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 補間率
		float t = 1.0f - turnTimer_;
		t = std::clamp(t, 0.0f, 1.0f); // 念のため

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = worldTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
		;
	}
}

void Player::WorldPosUpdate(const ColisionMapInfo& info) {
	// 移動量を反映させる
	worldTransform_.translation_.x += info.velosity_.x;
	worldTransform_.translation_.y += info.velosity_.y;
	worldTransform_.translation_.z += info.velosity_.z;
}

void Player::BehaviorRootInitialize() { velosity_ = {0.0f, 0.0f, 0.0f}; }

void Player::BehaviorAttackInitialize() {
	// カウンター初期化
	attackParameter_ = 0;
	// 予備動作
	attackParameter_++;

	velosity_ = {0.0f, 0.0f, 0.0f};

	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// 攻撃フェーズをチャージ状態に設定
	attakFase_ = AttakFase::kChage;

	if (lrDirection_ == LRDirection::kRight) {

		correctionTransform.translation_.x = 2.0f;

	} else if (lrDirection_ == LRDirection::kLeft) {

		correctionTransform.translation_.x = -2.0f;
	}

	isAttack_ = true; // 攻撃開始時に無敵にする
}

void Player::ChageUpdate() {

	switch (attakFase_) {

	case Player::AttakFase::kChage:
	default: {

		float t = static_cast<float>(attackParameter_) / chageTime;
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 2.0f, t);
		// 前進動作へ移行
		if (attackParameter_ >= chageTime) {
			attakFase_ = Player::AttakFase::kAttack;
			attackParameter_ = 0; // カウンター初期化
		}

		break;
	}
	}
}

void Player::AttackUpdate() {

	switch (attakFase_) {

	case Player::AttakFase::kAttack:

		float t = static_cast<float>(attackParameter_) / kAttackTime;
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);
		// 攻撃動作終了
		if (attackParameter_ >= kAttackTime) {
			attakFase_ = Player::AttakFase::kAfterglow;
			attackParameter_ = 0; // カウンター初期化
		}

		break;
	}
}

void Player::AfterglowUpdate() {
	switch (attakFase_) {
	case Player::AttakFase::kAfterglow:

		// 余韻動作のスケール変化
		float t = static_cast<float>(attackParameter_) / kAfterglowTime;
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);

		break;
	}
}

void Player::BehaviorRootUpdate() {

	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 攻撃ビヘイビアをリクエスト
		behaviorRequest_ = Behavior::kAttak;
	}
}

void Player::BehaviorAttackUpdate() {

	attackParameter_++;

	Vector3 attackVelosity = {}; // 初期化

	// 攻撃フェーズごとの更新処理
	switch (attakFase_) {
	case Player::AttakFase::kChage:
		ChageUpdate();
		break;

	case Player::AttakFase::kAttack:
		AttackUpdate();

		// 攻撃中はX方向に突進
		if (lrDirection_ == LRDirection::kRight) {
			attackVelosity.x = attakVelosity;
			correctionTransform.translation_.x = 2.0f;

		} else if (lrDirection_ == LRDirection::kLeft) {
			attackVelosity.x = -attakVelosity;
			correctionTransform.translation_.x = -2.0f;
		}

		break;

	case Player::AttakFase::kAfterglow:
		AfterglowUpdate();

		// X方向は止まる（余韻）、Y方向には落下する
		attackVelosity.x = 0.0f;
		attackVelosity.y = velosity_.y - kGravityAcceleration;
		attackVelosity.y = std::max(attackVelosity.y, -kLimitFallSpeed);
		break;
	}
	velosity_ = attackVelosity;

	// フェーズがすべて終わったら元に戻す
	if (attackParameter_ > kAttackTime + kAfterglowTime) {
		behaviorRequest_ = Behavior::kRoot;
		velosity_.x = 0.0f;
		attackParameter_ = 0;
		attakFase_ = AttakFase::kUnKnown;

		isAttack_ = false;
	}

	// トランスフォームの値のコピー
	worldTransformAttack_.translation_ = worldTransform_.translation_;
	// worldTransformAttack_.translation_.x += correctionTransform.translation_.x;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
}

void Player::Updata() {

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
	worldTransformAttack_.matWorld_ = MakeAffineMatrix(worldTransformAttack_.scale_, worldTransformAttack_.rotation_, worldTransformAttack_.translation_);
	worldTransformAttack_.TransferMatrix();

	ColisionMapInfo collisionMapInfo;

	// 移動処理
	if (behavior_ == Behavior::kRoot) {
		PlayerMove(); // 通常時のみ移動
	}
	TurnUpdata();

	/*if (worldTransform_.translation_.y < 2.0f) {
	    worldTransform_.translation_.y = 2.0f;

	}*/

	if (behaviorRequest_ != Behavior::kUnKnown) {

		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Player::Behavior::kRoot:

			BehaviorRootInitialize();

			break;
		case Player::Behavior::kAttak:

			BehaviorAttackInitialize();

			break;
		}
		// 振る舞いのリクエストをクリア
		behaviorRequest_ = Behavior::kUnKnown;
	}

	switch (behavior_) {
	case Player::Behavior::kRoot:

		BehaviorRootUpdate();

		break;
	case Player::Behavior::kAttak:

		BehaviorAttackUpdate();

		break;
	}

	collisionMapInfo.velosity_ = velosity_;

	if (velosity_.y == 0) {
		onGround_ = true;
		collisionMapInfo.landing = true;
	}

	// マップ衝突判定
	MapCollisionCheck(collisionMapInfo);
	WorldPosUpdate(collisionMapInfo);
	OnSwichGround(collisionMapInfo);
	IsCelling(collisionMapInfo);
	IsWall(collisionMapInfo);
	// 判定結果を反映させて移動させる

	if (behavior_ == Behavior::kAttak) {
		worldTransformAttack_.translation_ = worldTransform_.translation_;

		// 進行方向へ少し前に出したい場合はここで補正
		//			worldTransformAttack_.translation_.x += correctionTransform.translation_.x;

		worldTransformAttack_.rotation_ = worldTransform_.rotation_;
	}

	WorldTrnasformUpdate(worldTransformAttack_);
	WorldTrnasformUpdate(worldTransform_);

	ImGui::Text("isWall  : %s", collisionMapInfo.isWall ? "true" : "false");
	ImGui::Text("isGround  : %s", collisionMapInfo.landing ? "true" : "false");
	ImGui::Text("onGround  : %s", onGround_ ? "true" : "false");
	ImGui::Text("playerY  : %f", worldTransform_.translation_.y);
	ImGui::Text("velsity.y  : %f", velosity_.y);
	ImGui::Text("velsity.x  : %f", velosity_.x);
	ImGui::Text("Player rotationY: %f", worldTransform_.rotation_.y);
	ImGui::Text("AttackModel rotationY: %f", worldTransformAttack_.rotation_.y);
	ImGui::Text("AttakModel worldTransform translation: %f, %f, %f", worldTransformAttack_.translation_.x, worldTransformAttack_.translation_.y, worldTransformAttack_.translation_.z);
}

void Player::TitleUpdata() {
	// タイトルシーンの更新処理
	// タイマーを増加（回転速度）
	rotateTimer_ += 1.0f;

	// Y軸に回転を適用
	worldTransform_.rotation_.y = rotateTimer_;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
	WorldTrnasformUpdate(worldTransform_);
}

void Player::Draw() {

	// 自キャラの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
	// 攻撃モデルの描画

	
	if (attakFase_ == AttakFase::kAttack) {
		attackModel_->Draw(worldTransformAttack_, *camera_);
	}
}

void Player::SetPosition(const KamataEngine::Vector3& position) { worldTransform_.translation_ = position; }

const KamataEngine::WorldTransform& Player::GetWoldTransform() const { return worldTransform_; }
