#include "Stage.h"
#include "KamataEngine.h"
#include "WorldTransformClass.h"

using namespace KamataEngine;

void Stage::Initialize() {
	// 水面
	auto water = std::make_unique<StageObject>();
	water->model = Model::CreateFromOBJ("water");
	water->transform.translation_ = {0.0f, 0.0f, 0.0f};
	water->transform.scale_ = {50.0f, 1.0f, 50.0f};
	water->transform.rotation_ = {0.0f, 4.55f, 0.0f};
	water->transform.Initialize();
	water->name = "Water";
	objects_.push_back(std::move(water));

	// 地面
	auto ground = std::make_unique<StageObject>();
	ground->model = Model::CreateFromOBJ("mud");
	ground->transform.translation_ = {0.0f, -2.0f, 0.0f};
	ground->transform.scale_ = {50.0f, 1.0f, 50.0f};
	ground->transform.Initialize();
	ground->name = "Ground";
	objects_.push_back(std::move(ground));
}

void Stage::Update() {
	// 今は処理なしでOK
}

void Stage::Draw(KamataEngine::Camera* camera) {
	for (const auto& obj : objects_) {
		if (obj && obj->model) {
			obj->model->Draw(obj->transform, *camera, obj->textureHandle);
		}
	}
}

void Stage::DebugDrawUI() {
#ifdef _DEBUG
	ImGui::Begin("ステージデバッグ");
	for (const auto& obj : objects_) {
		std::string label = obj->name + " Rotation";
		ImGui::DragFloat3(label.c_str(), &obj->transform.rotation_.x, 0.01f);
		obj->transform.matWorld_ = MakeAffineMatrix(obj->transform.scale_, obj->transform.rotation_, obj->transform.translation_);
		obj->transform.TransferMatrix();
	}
	ImGui::End();
#endif
}
