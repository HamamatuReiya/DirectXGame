#include"Enemy.h"
#include <cassert>
#include"Player.h"

// bullet_の開放
Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy::Initialize(Model* model, uint32_t textureHandle) {
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	// x,y,z方向のスケーリングを設定
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// x,y,z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.translation_ = {10.0f, 0.0f, 20.0f};

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 行列の転送
	worldTransform_.UpdateMatrix();

	//接近フェーズ初期化
	InitializeApproach();

}

void Enemy::Update() {

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCaracterSpeed = 0.05f;

	move.z -= kCaracterSpeed;

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);

	switch (phase_) {
	case Phase::Approach:
	default:
		//移動(ベクトルを加算)
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
		//既定の位置に到達したら離脱
		if (worldTransform_.translation_.z < 0.0f) {
			phase_ = Phase::Leave;
		}
		break;
	case Phase::Leave:
		// 移動(ベクトルを加算)
		move.x -= kCaracterSpeed;
		move.y -= kCaracterSpeed;
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
		break;
	}

	// 発射タイマーカウントダウン
	fireTimer -= 1;
	// 指定時間に達した
	if (fireTimer <= 0) {
		// 弾を発射
		Fire();
		// 発射タイマーを初期化
		fireTimer = kFireInterval;
	}

	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 行列の転送
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Enemy::Fire() {

	// 速度ベクトルを自機の向きに合わせて回転させる
	// velocity = TransformNormal(velocity, worldTransform_.matWorld_);

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

	// 弾を登録する
	bullets_.push_back(newBullet);
}

void Enemy::InitializeApproach() {
	
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
