#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model, uint32_t textureHandle) 
{ 
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	//x,y,z方向のスケーリングを設定
	worldTransform_.scale_ = {5.0f, 1.0f, 1.0f};

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Player::Updete()
{
	//スケーリング行列を宣言
	Matrix4x4 matScale; // 4行4列

	matScale.m[0][0] = worldTransform_.scale_.x;
	matScale.m[1][1] = worldTransform_.scale_.y;
	matScale.m[2][2] = worldTransform_.scale_.z;
	matScale.m[3][3] = 1;

	//x,y,z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//x軸回転行列を宣言
	Matrix4x4 matRotX;
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(worldTransform_.rotation_.x);
	matRotX.m[2][1] = -sinf(worldTransform_.rotation_.x);
	matRotX.m[1][2] = sinf(worldTransform_.rotation_.x);
	matRotX.m[2][2] = cosf(worldTransform_.rotation_.x);
	matRotX.m[3][3] = 1;

	// 行列の転送 行列の計算後に行う
	worldTransform_.TransferMatrix();

	worldTransform_.matWorld_;

	worldTransform_.matWorld_ =
	    worldTransform_.scale_ * worldTransform_.rotation_ * worldTransform_.matWorld_;

	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	//キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	//キャラクターの移動速さ
	const float kCaracterSpeed = 0.2f;

	//押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCaracterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCaracterSpeed;
	}

	//移動した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_UP)) {
		move.y -= kCaracterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y += kCaracterSpeed;
	}

	//座標移動(ベクトルの加算)
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	
}

void Player::Draw(ViewProjection& viewProjection) 
{
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
