#include"PlayerBullet.h"
#include<cassert>

void PlayerBullet::Initialize(Model* model, const Vector3& position) {
	//NULLポインタチェック
	assert(model);

	model_ = model;
	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("white1x1.png");

	worldTransform_.Initialize();
	//引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

void PlayerBullet::Update() {

	 worldTransform_.UpdateMatrix();

}


void PlayerBullet::Drow(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_, viewProjection, textureHandle_);

}
