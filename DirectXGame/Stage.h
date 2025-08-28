#pragma once
#include "KamataEngine.h"
#include "StageObject.h"

#include <memory>
#include <vector>

/// <summary>
/// ステージクラス（地面・水などをまとめる）
/// </summary>
class Stage {
public:
	/// 初期化
	void Initialize();

	/// 更新（今は何もなくてもOK）
	void Update();

	/// 描画
	void Draw(KamataEngine::Camera* camera);

	/// デバッグUI
	void DebugDrawUI();

private:
	std::vector<std::unique_ptr<StageObject>> objects_;
	KamataEngine::Camera* camera_ = nullptr;
};
