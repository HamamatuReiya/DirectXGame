#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"AxisIndicator.h"

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete player_;
	delete debugCamera_;
	delete enemy_;
	delete modelSkydome_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("Steve.jpg");
	model_ = Model::Create();
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	player_->Initialize(model_,textureHandle_);

	// ファイル名を指定してテクスチャを読み込む
	enemyTextureHandle_ = TextureManager::Load("creeper.png");
	enemyModel_ = Model::Create();
	// 敵キャラの生成
	enemy_ = new Enemy();
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
	// 敵キャラの初期化
	enemy_->Initialize(enemyModel_, enemyTextureHandle_);

	//3Dモデルの生成
	skydome_ = new Skydome();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(modelSkydome_);

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	
	
}

void GameScene::Update() {
	//自キャラの更新
	player_->Update();
	// 自キャラの更新
	enemy_->Update();
	// デバッグカメラの更新
	debugCamera_->Update();
	//衝突判定の更新
	GameScene::CheckAllCollisions();
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = true;
	}

	if (input_->TriggerKey(DIK_B)) {
		isDebugCameraActive_ = false;
	}
	
#endif
	//カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		//ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		//ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	//自キャラの描画
	player_->Draw(viewProjection_);
	/// </summary>

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	// 自キャラの描画
	enemy_->Draw(viewProjection_);
	/// </summary>

	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB, posC, posD;
	// 2間点の距離(自キャラと敵弾の当たり判定)
	float posAB;
	// 自キャラの半径
	float playerRadius = 1.0f;
	// 敵弾の半径
	float enemyBulletRadius = 1.0f;

	//自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定
	//自キャラの座標
	posA = player_->GetWorldPosition();
	
	//自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
	//敵弾の座標
		posB = bullet->GetWorldPosition();

		posAB = (posB.x - posA.x) * (posB.x - posA.x) + 
				(posB.y - posA.y) * (posB.y - posA.y) +
				(posB.z - posA.z) * (posB.z - posA.z);
		//球と球の交差判定
		if (posAB <= (playerRadius + enemyBulletRadius) * (playerRadius + enemyBulletRadius)) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定
	// 自弾の半径
	float playerBulletRadius = 1.0f;
	// 敵キャラの半径
	float enemyRadius = 1.0f;

	// 敵キャラのワールド座標
	posC = enemy_->GetWorldPosition();

	// 2間点の距離(自弾と敵キャラの当たり判定)
	float posCD;

	// 自弾と敵キャラの当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		// 自弾の座標
		posD = bullet->GetWorldPosition();
		// AとBの距離を求める
		posCD = (posC.x - posD.x) * (posC.x - posD.x) + 
				(posC.y - posD.y) * (posC.y - posD.y) +
		        (posC.z - posD.z) * (posC.z - posD.z);

		// 球と球の当たり判定
		if (posCD <= (playerBulletRadius + enemyRadius) * (playerBulletRadius + enemyRadius)) {
			// 自弾の衝突時コールバックを呼び出す
			enemy_->OnCollision();
			// 敵キャラの衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

	#pragma region 自弾と敵弾の当たり判定
	float posBD;
	for (PlayerBullet* playerbullet : playerBullets) {
		posD = playerbullet->GetWorldPosition();
		for (EnemyBullet* enemybullet : enemyBullets) {
			posB = enemybullet->GetWorldPosition();
			posBD = (posB.x - posD.x) * (posB.x - posD.x) + 
					(posB.y - posD.y) * (posB.y - posD.y) +
			        (posB.z - posD.z) * (posB.z - posD.z);
			// 球と球の当たり判定
			if (posBD <= (playerBulletRadius + enemyBulletRadius) *
			                 (playerBulletRadius + enemyBulletRadius)) {
				// 自弾の衝突時コールバックを呼び出す
				enemybullet->OnCollision();
				// 敵キャラの衝突時コールバックを呼び出す
				playerbullet->OnCollision();
			}
		}
	}
#pragma endregion
}
