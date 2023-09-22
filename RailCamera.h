#pragma once
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class RailCamera {
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(const Vector3& worldPos, const Vector3& position);
	///< summary>
	/// 更新
	///</summary>
	void Update();

	ViewProjection& GetViewProjection() { return viewProjection_; }
	WorldTransform& GetWorldTransform() { return worldTransform_; };

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
};