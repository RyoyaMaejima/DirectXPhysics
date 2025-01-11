# DirectXPhysics
![](https://github.com/RyoyaMaejima/DirectXPhysics/blob/main/ReadmeSrc/Title.png)

## 概要
- 長方形の面を操作して、落下する球体をリフティングする3D物理シミュレーション
- プレイヤーは面の位置と角度を変えることができる

## 操作方法
- 十字キー：移動
- WASD：回転
- Cキー：ゲーム開始
- Xキー：初期配置に戻す

## プレイ動画
プレイ動画は下記リンクに記載しています  

https://youtu.be/MEjydy8x1V0

## 開発目的
- DirectXを使った開発を行い、ゲームエンジンを使った開発では触れることが無い部分を実装することで、深い知識を得ること

## 開発環境・体制
- 個人開発
- 期間：１ヶ月
- 使用ツール：DirectX12

## ディレクトリ構成
Source内にソースコード一式を載せています  
以下が各ファイルの説明

- Application：DirectXにおける基本的な機能を管理するスクリプト
- BasicPixelShader：ピクセルシェーダー
- BasicType：頂点構造体
- BasicVertexShader：頂点シェーダー
- Face：面の挙動を管理するスクリプト
- main：メイン関数を含むスクリプト
- Sphere：球体の挙動管理や物理演算を行うスクリプト
- VertexData：頂点に関するデータを保持するスクリプト
- XMFLOAT3Math：ベクトルにおける演算を定義するスクリプト

## こだわった点
- 球体と面の当たり判定および衝突後の運動計算を自分で実装
- 画面右上に操作説明が記載されたテクスチャを表示
- 可変フレームレートへの対応
