#pragma once
#include "KamataEngine.h"

#include <string>

struct StageObject {
	KamataEngine::WorldTransform transform; // ワールド変換
	KamataEngine::Model* model = nullptr;   // モデルへのポインタ（共有）
	uint32_t textureHandle = 0;             // テクスチャ（必要であれば）
	std::string name;                       // 識別用の名前（ImGuiなどで使用）

	// コピー禁止（Model*はコピーできないので）
	StageObject(const StageObject&) = delete;
	StageObject& operator=(const StageObject&) = delete;

	// ムーブは許可
	StageObject() = default;
	StageObject(StageObject&&) = default;
	StageObject& operator=(StageObject&&) = default;
};
