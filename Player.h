#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "PlayerBullet.h"
#include <list>
#include"MatrixMath.h"

///<summary>
///自キャラ
///</summary>
class Player {
public:
	///< summary>
	/// 初期化
	///</summary>
	///<param name="model">モデル</param> 
	///<param name="textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model,uint32_t textureHandle);

	///< summary>
	/// 更新
	///</summary>
	void Update();
	void Rotate();

	///< summary>
	/// 描画
	///</summary>
	void Draw(ViewProjection& viewProjection);

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//キーボード入力
	Input* input_ = nullptr;
	//ImGuiで値を入力する変数
	float inputFloat3[3] = {0, 0, 0};
	//弾
	std::list<PlayerBullet*> bullets_;
};