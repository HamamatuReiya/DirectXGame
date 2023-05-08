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
	//worldTransform_.translation_ += move;
}

void Player::Draw(ViewProjection& viewProjection) 
{
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
