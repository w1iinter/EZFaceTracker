
// TinyEspCam_S3
#if defined(CAMERA_MODEL_TinyEspCam_S3)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    4
#define SIOD_GPIO_NUM    48
#define SIOC_GPIO_NUM    47

#define Y9_GPIO_NUM      13
#define Y8_GPIO_NUM      5
#define Y7_GPIO_NUM      6
#define Y6_GPIO_NUM      15
#define Y5_GPIO_NUM      17
#define Y4_GPIO_NUM      8
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM      16
#define VSYNC_GPIO_NUM   21
#define HREF_GPIO_NUM    14
#define PCLK_GPIO_NUM    7

// 闪光灯引脚
#define LED_GPIO_NUM     1

#else
#error "Camera model not selected"
#endif
