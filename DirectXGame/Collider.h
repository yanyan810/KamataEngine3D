#pragma once
#include "AABB.h" // 既存のAABB {Vector3 min,max} を想定
#include "KamataEngine.h"

inline bool CheckCollision_CircleCircle(const KamataEngine::Vector3& a, float ra, const KamataEngine::Vector3& b, float rb) {
	KamataEngine::Vector3 d = {a.x - b.x, a.y - b.y, 0};
	float dist2 = d.x * d.x + d.y * d.y;
	float r = ra + rb;
	return dist2 <= r * r;
}

inline bool CheckCollision_CircleAABB(const KamataEngine::Vector3& c, float r, const AABB& box) {
	float nx = std::max<float>(box.min.x, std::min<float>(c.x, box.max.x));
	float ny = std::max<float>(box.min.y, std::min<float>(c.y, box.max.y));
	float dx = c.x - nx, dy = c.y - ny;
	return (dx * dx + dy * dy) <= r * r;
}
