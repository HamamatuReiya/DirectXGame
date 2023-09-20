#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include"MatrixMath.h"
#include"EnemyBullet.h"
#include <list>

class Player;

class Enemy {
public:

	void Initialize(Model* model, uint32_t textureHandle);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void Fire();

	~Enemy();

	// 発射間隔
	static const int kFireInterval = 60;

	//接近フェーズの初期化
	void InitializeApproach();
	
	// 自キャラ
	Player* player_ = nullptr;

	void SetPlayer(Player* player) { player_ = player; }

	// ワールド座標を取得
	Vector3 GetWorldPosition();

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
	std::list<EnemyBullet*> bullets_;
	//発射タイマー
	int32_t fireTimer = 0;
};