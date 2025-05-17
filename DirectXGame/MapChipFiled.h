#pragma once
#include <cassert>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "KamataEngine.h" 

using namespace KamataEngine;

// マップチップの種類
enum class MapChipType {
	kBlank, // 空白
	kBlock  // ブロック
};

// マップチップのデータ構造体
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
public:
	// 定数（ブロックサイズと個数）
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	// マップチップデータのリセット
	void ResetMapChipData();

	// マップチップCSVの読み込み
	void LoadMapChipCsv(const std::string& filePath);
	
	 MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	// ブロック数取得
	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }

private:
	// マップチップのデータ
	MapChipData mapChipData_;
};