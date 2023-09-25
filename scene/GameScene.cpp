#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"AxisIndicator.h"
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete player_;
	delete debugCamera_;
	delete modelSkydome_;
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	for (Enemy* enemy : enemys_) {
		delete enemy;
	}
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
	//自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());
	Vector3 playerPosition(0,0,30);
	//自キャラの初期化
	player_->Initialize(model_,textureHandle_,playerPosition);

	//3Dモデルの生成
	skydome_ = new Skydome();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(modelSkydome_);


	//レールカメラの生成
	railCamera_ = new RailCamera();
	railCamera_->Initialize(worldPos_, rotation_);

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	
	player_->SetParent(&railCamera_->GetWorldTransform());

	LoadEnemyPopData();

} 

void GameScene::Update() {
	//自キャラの更新
	player_->Update();
	// デバッグカメラの更新
	debugCamera_->Update();
	//レールカメラの更新
	railCamera_->Update();
	//衝突判定の更新
	CheckAllCollisions();

	UpdeteEnemyPopCommand();

	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}

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
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		//ビュープロジェクション行列の更新と転送
		viewProjection_.TransferMatrix();
	}

	// デスフラグが立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// デスフラグの立った敵を削除
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
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

	// 敵の描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}

	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	// 自キャラの描画
	//enemy_->Draw(viewProjection_);
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

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();

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

	#pragma region 自キャラと敵弾の当たり判定
	//自キャラの座標
	posA = player_->GetWorldPosition();
	
	//自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets_) {
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

	// 2間点の距離(自弾と敵キャラの当たり判定)
	float posCD;

	// 自弾と敵キャラの当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		for (Enemy* enemy : enemys_) {
			// 敵キャラのワールド座標
			posC = enemy->GetWorldPosition(); // リストになってるので範囲for
			                                  // 自弾の座標
			posD = bullet->GetWorldPosition();
			// AとBの距離を求める
			posCD = (posC.x - posD.x) * (posC.x - posD.x) + (posC.y - posD.y) * (posC.y - posD.y) +
			        (posC.z - posD.z) * (posC.z - posD.z);

			// 球と球の当たり判定
			if (posCD <= (playerBulletRadius + enemyRadius) * (playerBulletRadius + enemyRadius)) {
				// 自弾の衝突時コールバックを呼び出す
				enemy->OnCollision(); // 範囲forで直る
				// 敵キャラの衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
		
		}
	}
#pragma endregion

	#pragma region 自弾と敵弾の当たり判定
	float posBD;
	for (PlayerBullet* playerbullet : playerBullets) {
		posD = playerbullet->GetWorldPosition();
		for (EnemyBullet* enemybullet : enemyBullets_) {
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

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	//リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdeteEnemyPopCommand() {

	// 待機処理
	if (standFlag) {
		standTime--;
		if (standTime <= 0) {
			// 待機完了
			standFlag = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		// １行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		//"//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			EnemyOccurrence(Vector3(x, y, z), {0, 0, 0});

		} // WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機時間
			standFlag = true;
			standTime = waitTime;

			// コマンドループを抜ける
			break; // 重要。なぜ重要か考えてみよう
		}
	}
}

void GameScene::EnemyOccurrence(Vector3 position, Vector3 velocity) {
	Enemy* enemy = new Enemy();

	// 初期化
	enemy->Initialize(model_, position, velocity);
	// 敵キャラに自キャラのアドレスを渡す
	enemy->SetPlayer(player_);
	// 敵キャラにゲームシーン渡す
	enemy->SetGameScene(this);
	// リストに登録
	enemys_.push_back(enemy);
}
