# Project Sekai Slidar
プロジェクトセカイ　カラフルステージ！をアーケード風にプレイするためのコントローラーです。  
https://www.youtube.com/watch?v=6ZV3q7aEVVU

# 使用機器
- Arduino Pro Micro 5V x1
- Adafruit MPR121(静電容量センサ) x1
- WS2812B(LEDテープ) x1
- 3mm乳白色アクリル板 x1
- 3mm透明アクリル板 x1
- 静電容量式センサ用透明フィルム ３枚セット  x1
- その他
    - ACアダプタ(LEDテープの電源用)
    - DCジャック
    - Si8640BT(デジタルアイソレータ)
      - Arduinoの制御電源とLEDテープの電源を分けるため
    - 配線
    - ユニバーサル基板
 
 # 回路図
 ![pjsekai-slidar](https://user-images.githubusercontent.com/16555696/119915029-8c9f8500-bf9c-11eb-86b0-d8bb7d930f85.png)

 
 # ビルド環境
 Platform IOでビルド
 
 # 仕組み
 Androidに接続すると、USB HID マルチタッチデバイスとして認識される。  
 
