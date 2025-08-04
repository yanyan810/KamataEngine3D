#pragma once
#define NOMINMAX
#include "algorithm"
#include "KamataEngine.h"
#include "Operator.h"

inline KamataEngine::Vector3 Normalize(const KamataEngine::Vector3& v) {
	float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (len == 0.0f)
		return {0, 0, 0};
	return {v.x / len, v.y / len, v.z / len};
}


// 線形補間
inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

// ✅ ドット積（Slerpに必要）
inline float Dot(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

// 球面線形補間
inline KamataEngine::Vector3 Slerp(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b, float t) {
	float dot = Dot(a, b);
	dot = std::clamp(dot, -1.0f, 1.0f); // 精度対策
	float theta = std::acos(dot) * t;

	KamataEngine::Vector3 relative = Normalize(b - a * dot);
	return a * std::cos(theta) + relative * std::sin(theta);
}



// EaseOut（2乗曲線）
inline float EaseOut(float x, float y, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	float easedT = 1.0f - (1.0f - t) * (1.0f - t);
	return x + (y - x) * easedT;
}
// EaseIn（2乗曲線）
inline float EaseIn(float x, float y, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	float easedT = t * t;
	return x + (y - x) * easedT;
}