
#pragma once
#include "KamataEngine.h"
using KamataEngine::Vector3;

class Collider {
private:
	float radius_ = 1.0f; // デフォルト半径

public:
	float GetRadius() const { return radius_; }
	void SetRadius(float radius) { radius_ = radius; }
	virtual Vector3 GetWorldPosition() const = 0; // 純粋仮想関数
	virtual void OnCollision() = 0;               // 純粋仮想関数
};