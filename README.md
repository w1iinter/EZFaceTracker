# EZFaceTracker
![enter image description here](https://cdn.phototourl.com/member/2026-04-26-5184d4bd-2160-49c2-b761-6a0f66db14c1.jpg)
![enter image description here](https://cdn.phototourl.com/free/2026-04-26-7873d0e5-c441-46f2-be72-2523bbf368a4.jpg)

- [EZFaceTracker](#ezfacetracker)
	- [项目简介/Project introduction](#项目简介project-introduction)
	- [支持的设备/Supported headset](#支持的设备supported-headset)
	- [光生物安全/Photobiological safety](#光生物安全photobiological-safety)
	- [开发进度/Development progress](#开发进度development-progress)
	- [自己复刻/DIY](#自己复刻diy)
	- [许可协议/License](#许可协议license)

## 项目简介/Project introduction
为VR头显开发的外置全脸识别追踪器。采用轻量化、模块化、与头显融合设计思路。目标适配主流头显设备。支持无线和有线传输模式，兼容Project Babble、EyeTrackVR等软件。**此项目正在积极开发中。**

External Eye & Face Tracker for VR Headsets.Designed with a lightweight, modular, and headset-integrated approach. Aims to be compatible with major VR headsets. Supports both wireless and wired transmission, and works with software such as Project Babble and EyeTrackVR. **The project is currently under active development.**

## 支持的设备/Supported headset
- PICO 4
- PICO 4 ultra
- Meta Quest 3  **尚未开始适配**
- Meta Quest 3s  **尚未开始适配**
- 更多头显


## 光生物安全/Photobiological safety
眼部光源使用了4颗型号为XL-1608HIRC-850的IR-LED,电源路径串联了一颗270Ω的电阻，单眼总电流约5毫安。
## 开发进度/Development progress
**现在**正在优化面部追踪模块

**2026年5月** 再次修改面部追踪模块的设计，虽然能完整的捕捉脸部画面，但是识别效果还是比较差，我想应该只能增加支架了。
![输入图片说明](https://cdn.phototourl.com/member/2026-05-24-9dedeb95-d04b-44ec-8d42-33cfdd951997.jpg)

**2026年3月** 优化电源模块，体积更小，重量更轻。无线模式时，电池可提供约1小时续航。

![enter image description here](https://cdn.phototourl.com/member/2026-04-27-13c232cd-602b-4202-8d3b-bde2af82660b.jpg)

**2026年2月** 优化面部追踪模块，设计了专门的面部追踪摄像头主控板，移除独立的电源降压模块并集成到面部追踪模块。

![enter image description here](https://cdn.phototourl.com/member/2026-04-26-b9c9f21c-3310-448f-8f7b-4607c6b97bbb.jpg)

**2026年1月** 优化眼部追踪模块，封闭式结构，补光灯和摄像头不再完全裸露。摄像头由OV2640 160°更换为OV3660 120°，虽然摄像头FOV减小，但是镜头距离眼球更远，这使摄像头可视区域有较大提升。

![enter image description here](https://cdn.phototourl.com/member/2026-04-26-6ba09dc4-0170-4b5c-8e8f-2bebe534db27.jpg)

**2025年12月** 优化摄像头主控板，尺寸缩减20%，优化WIFI天线性能，优化电源设计以解决摄像头画面干扰问题。

![enter image description here](https://cdn.phototourl.com/member/2026-04-26-b5831c3c-80cd-46e7-927a-e6465cf98430.jpg)

**2025年10月** 在VRChat中测试整体追踪效果，虽然效果较差，但已验证项目的可行性，后续将进行优化。

![enter image description here](https://cdn.phototourl.com/member/2026-04-26-f2b08e1c-d8df-41b4-9bce-c7b6577a2592.jpg)

**2025年9月** 修改眼部追踪模块的支架以适配近视镜片，完成面部追踪模块的设计。

![enter image description here](https://cdn.phototourl.com/member/2026-04-26-e48bb8c6-d550-4c46-b79d-2d1a6021b6f4.jpg)

**2025年8月** 完成眼部追踪模块支架和电源模块外壳的设计，调整摄像头的角度和位置以获得最佳的追踪性能。

![enter image description here](https://cdn.phototourl.com/member/2026-04-26-a1b80ebf-3aa6-4fe0-882e-130f658ea2b0.jpg)

**2025年7月** 完成首个摄像头主控板的设计，使用Arduino IDE的WebCam示例程序进行测试，摄像头成功运行。

![enter image description here](https://cdn.phototourl.com/member/2026-04-26-32f26be9-3ee8-4128-90c2-d2cd0bea6364.jpg)
## 自己复刻/DIY
- 正在完善
## 许可协议/License
所有文档采用 Creative Commons CC BY-NC-SA 4.0许可协议
