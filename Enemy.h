#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include"MatrixMath.h"
#include"EnemyBullet.h"
#include <list>

class Player;
//GameSceneの前方宣言(苦肉の策)
class GameScene;

class Enemy {
public:

	void Initialize(Model* model, Vector3& position, const Vector3& velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void Fire();

	// 発射間隔
	static const int kFireInterval = 60;
	
	// 自キャラ
	Player* player_ = nullptr;

	void SetPlayer(Player* player) { player_ = player; }

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();
	
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead() const { return isDead_; }

	float radius = 3;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//行動フェーズ
	enum class Phase {
		Approach,//接近する
		Leave,//離脱する
	};
	//フェーズ
	Phase phase_ = Phase::Approach;
	// 弾
	EnemyBullet* bullet_ = nullptr;
	// 速度
	Vector3 velocity_;
	//発射タイマー
	int32_t fireTimer_ = 0;
	// デスフラグ
	bool isDead_ = false;
	
};