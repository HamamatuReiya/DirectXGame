#include "RailCamera.h"
#include "ImGuiManager.h"
#include "MatrixMath.h"

void RailCamera::Initialize(const Vector3& worldPos, const Vector3& rotation) {
	//ワールドトランスフォームの初期化
	worldTransform_.translation_ = worldPos;
	worldTransform_.Initialize();
	worldTransform_.rotation_ = rotation;
	//ビュープロジェクションの初期化
	viewProjection_.farZ = 1200.0f;
	viewProjection_.Initialize();
}

void RailCamera::Update() {
	Vector3 move = {0.0f, 0.0f, -0.02f};
	worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	Vector3 rotate = {0.0f, 0.0f, 0.0f};
	worldTransform_.rotation_ = Add(worldTransform_.rotation_, rotate);

	worldTransform_.UpdateMatrix();
	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);
	//カメラ座標を画面表示する処理
	ImGui::Begin("Camera");
	ImGui::SliderFloat3("translate", &worldTransform_.translation_.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("rotate", &worldTransform_.rotation_.x, -10.0f, 10.0f);
	ImGui::End();
}