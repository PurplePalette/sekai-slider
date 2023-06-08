# Project Sekai Slider
Arcade Style Controller For Project Sekai Colorful Stage!  
プロジェクトセカイ カラフルステージ！をアーケード風にプレイするためのコントローラー  

<img width="360" alt="picture" src="https://user-images.githubusercontent.com/16555696/175815366-ba8c9e83-4401-4b38-926a-aee3e0e676c5.png">

## List of Hardware/部品
|Part|Qty||
|:----|:----|:----|
|Arduino Pro Micro 5V|1|Microcontroller|
|Adafruit MPR121|1|Capacitance sensor|
|WS2812B|1|LED Tape|
|3mm Milky white Acrylic |1| |
|3mm Transparent Acrylic|1| |
|[Transparent film for capacitive sensors](https://www.amazon.co.jp/dp/B015R4RYL6)|1| |
|AC Adaptor|1|For power supply of LED tape|
|Power connector| |For AC adaptor to LED Tape|
|Si8640BT|1|Digital Isolator to isolate the LED power supply from the control power supply. This is optinal.|
|Wire| | |
|Stripboard or Breadboard| |Recommend stripboard|
 
## Schematic/回路図
![pjsekai-slidar](https://user-images.githubusercontent.com/16555696/175814429-3f9bb644-7874-4d54-a7de-323d1facfc80.png)
 
## Build/ビルド
Platform IOでビルド  
Build with Platform IO
 
## How it works/仕組み
Androidに接続すると、USB HID マルチタッチデバイスとして認識される。  
When connected to Android, it is recognized as a USB HID multi-touch device.  
 
