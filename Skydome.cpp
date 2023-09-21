#include "Skydome.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include "ImGuiManager.h"
#include "MatrixMath.h"
#include <math.h>

void Skydome::Initialize(Model* model) {

	assert(model);

	model_ = model;

	// x,y,z方向のスケーリングを設定
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// x,y,z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransform_.Initialize();
}

void Skydome::Update() { 
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) {
	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);

}
