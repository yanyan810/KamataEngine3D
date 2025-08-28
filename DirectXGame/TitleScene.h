#pragma once
#include "Scene.h"

class TitleScene : public Scene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};
