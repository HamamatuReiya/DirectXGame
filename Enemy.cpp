#include"Enemy.h"
#include"Player.h"
#include "GameScene.h"

void Enemy::Initialize(Model* model, Vector3& position, const Vector3& velocity) {
	assert(model);

	model_ = model;
	textureHandle_ = TextureManager::Load("creeper.png");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// x,y,z方向の回転を設定
	worldTransform_.translation_ = position;

	// X,Y,Z方向の平行移動を設定
	velocity_ = velocity;
}

void Enemy::Update() {

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCaracterSpeed = 0.1f;

	move.z -= kCaracterSpeed;

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	switch (phase_) {
	case Phase::Approach:
	default:
		//移動(ベクトルを加算)
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
		//既定の位置に到達したら離脱
		//if (worldTransform_.translation_.z < 0.0f) {
		//	phase_ = Phase::Leave;
		//}
		break;
	case Phase::Leave:
		// 移動(ベクトルを加算)
		move.x -= kCaracterSpeed;
		move.y -= kCaracterSpeed;
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
		break;
	}

	// 発射タイマーカウントダウン
	fireTimer_--;
	// 指定時間に達した
	if (fireTimer_ <= 0) {
		// 弾の発射
		Fire();
		// 発射タイマーを初期化
		fireTimer_ = kFireInterval;
	}

	// 行列の転送
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	
}

void Enemy::Fire() {

	assert(player_);

	// 弾の速度
	const float kBulletSpeed = 1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	Vector3 vector = Subtract(player_->GetWorldPosition(), GetWorldPosition());
	Vector3 normal = Normalize(vector);
	velocity = VectorMultiply(kBulletSpeed, normal);

	//弾を生成し、初期化
	EnemyBullet* newBullet =  new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_,velocity);
	gameScene_->AddEnemyBullet(newBullet);
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::OnCollision() { 
	isDead_ = true;
}
