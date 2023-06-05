#include "Player.h"
#include <cassert>
#include"MatrixMath.h"

void Player::Initialize(Model* model, uint32_t textureHandle) 
{ 
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	//x,y,z方向のスケーリングを設定
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// x,y,z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

}

void Player::Updete()
{
	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCaracterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCaracterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCaracterSpeed;
	}

	// 移動した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_UP)) {
		move.y += kCaracterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCaracterSpeed;
	}

	// 移動限界座標
	const float kMoveLimitX = 32;
	const float kMoveLimitY = 18;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	// 座標移動(ベクトルの加算)
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	// スケーリング行列を宣言
	Matrix4x4 matScale; // 4行4列

	matScale.m[0][0] = worldTransform_.scale_.x;
	matScale.m[1][1] = worldTransform_.scale_.y;
	matScale.m[2][2] = worldTransform_.scale_.z;
	matScale.m[3][3] = 1;

	// X軸回転行列を宣言
	Matrix4x4 matRotX;
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(worldTransform_.rotation_.x);
	matRotX.m[2][1] = -sinf(worldTransform_.rotation_.x);
	matRotX.m[1][2] = sinf(worldTransform_.rotation_.x);
	matRotX.m[2][2] = cosf(worldTransform_.rotation_.x);
	matRotX.m[3][3] = 1;

	// Y軸回転行列を宣言
	Matrix4x4 matRotY;
	matRotY.m[0][0] = cosf(worldTransform_.rotation_.y);
	matRotY.m[1][1] = 1;
	matRotY.m[0][2] = -sinf(worldTransform_.rotation_.y);
	matRotY.m[2][0] = sinf(worldTransform_.rotation_.y);
	matRotY.m[2][2] = cosf(worldTransform_.rotation_.y);
	matRotY.m[3][3] = 1;

	// Z軸回転行列を宣言
	Matrix4x4 matRotZ;
	matRotZ.m[0][0] = cosf(worldTransform_.rotation_.z);
	matRotZ.m[1][0] = sinf(worldTransform_.rotation_.z);
	matRotZ.m[0][1] = -sinf(worldTransform_.rotation_.z);
	matRotZ.m[1][1] = cosf(worldTransform_.rotation_.z);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	Matrix4x4 matRot = Multiply(matRotZ, Multiply(matRotX,matRotY));

	// 平行移動行列の宣言
	Matrix4x4 matTrans;

	matTrans.m[0][0] = 1;
	matTrans.m[1][1] = 1;
	matTrans.m[2][2] = 1;
	matTrans.m[3][3] = 1;
	matTrans.m[3][0] = worldTransform_.translation_.x;
	matTrans.m[3][1] = worldTransform_.translation_.y;
	matTrans.m[3][2] = worldTransform_.translation_.z;

	worldTransform_.matWorld_ =
		Multiply(matScale,Multiply(matRot ,matTrans));

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列の転送
	worldTransform_.TransferMatrix();

	//キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug");
	float playerPos[] = {
	    worldTransform_.translation_.x,
		worldTransform_.translation_.y,
	    worldTransform_.translation_.z
	};
	ImGui::SliderFloat3("PlayerPos", playerPos, -kMoveLimitX, kMoveLimitX);

	//↑処理のままだとSliderFloat3でplayerPosの値を変えているので実際の座標(translation)が
	//変わってないのでここで変更する
	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];
	ImGui::End();

	//キャラクター攻撃処理
	Attack();

	//弾更新
	if (bullet_) {
		bullet_->Update();
	}
}

void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y = worldTransform_.rotation_.y - kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y = worldTransform_.rotation_.y + kRotSpeed;
	}
}

void Player::Draw(ViewProjection& viewProjection) 
{
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	//弾描画
	if (bullet_) {
		bullet_->Drow(viewProjection);
	}
}

void Player::Attack() { 
	if (input_->TriggerKey(DIK_SPACE)) {

		//弾を生成し初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_);

		//弾を登録する
		bullet_ = newBullet;
	}
}
