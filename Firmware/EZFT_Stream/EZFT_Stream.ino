#include "esp_camera.h"
#include <WiFi.h>
#include "wifi_config_manager.h"


#define CAMERA_MODEL_TinyEspCam_S3//添加TinyEspCam_S3
#include "camera_pins.h"


WiFiConfigManager wifiManager;

void startCameraServer();
void setupLedFlash(int pin);
// void setupCameraParams();

// // 摄像头参数设置函数
// void setupCameraParams() {
//   sensor_t *s = esp_camera_sensor_get();
  
//   Serial.printf("Camera PID: 0x%04X\n", s->id.PID);
  
//   if (s->id.PID == OV3660_PID) {
//     Serial.println("Configuring OV3660");
//     s->set_vflip(s, 1);
//     s->set_hmirror(s, 0);
//     s->set_brightness(s, 0);
//     s->set_contrast(s, 0);
//     s->set_saturation(s, 0);
//     s->set_sharpness(s, 0);
//     s->set_whitebal(s, 1);
//     s->set_exposure_ctrl(s, 1);
//     s->set_gain_ctrl(s, 1);
//     s->set_quality(s, 10);
//     s->set_framesize(s, FRAMESIZE_VGA);
//   } else {
//     s->set_vflip(s, 1);
//     s->set_brightness(s, 0);
//     s->set_quality(s, 12);
//     s->set_framesize(s, FRAMESIZE_VGA);
//   }
// }

// 显示启动菜单
void showStartupMenu() {
  Serial.println("\n========================================");
  Serial.println("ESP32-S3 Camera Server");
  Serial.println("========================================");
  Serial.println();
  
  if (wifiManager.isWiFiConfigured()) {
    Serial.println("✓ WiFi configuration found");
    Serial.printf("  SSID: %s\n", wifiManager.getSSID().c_str());
    Serial.println("\nOptions:");
    Serial.println("  1. Connect to WiFi (auto in 3 seconds)");
    Serial.println("  2. Reconfigure WiFi");
    Serial.println("  3. Clear WiFi config");
    Serial.println("\nWaiting for command (3s timeout)...");
  } else {
    Serial.println("✗ No WiFi configuration found");
    Serial.println("\nOptions:");
    Serial.println("  1. Configure WiFi now");
    Serial.println("  2. Scan for networks");
    Serial.println("\nWaiting for command (3s timeout)...");
  }
  Serial.print("> ");
}

// 处理启动菜单输入
void handleStartupMenu() {
  unsigned long timeout = millis() + 3000;  // 3秒超时
  int selected = 0;
  
  while (millis() < timeout) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c >= '1' && c <= '3') {
        selected = c - '0';
        Serial.println(c);
        break;
      }
    }
    delay(10);
  }
  
  if (selected == 0) {
    // 超时或无效选择，如果有配置则尝试连接
    if (wifiManager.isWiFiConfigured()) {
      Serial.println("\nAuto-connecting to saved WiFi...");
    }
    return;
  }
  
  switch (selected) {
    case 1:  // 连接WiFi 或 配置WiFi
      if (wifiManager.isWiFiConfigured()) {
        // 已有配置，直接连接
        return;  // 继续执行后续的connectToWiFi
      } else {
        // 无配置，启动配置流程
        Serial.println("\n--- WiFi Configuration ---");
        Serial.print("Enter SSID: ");
        String ssid = readSerialLine();
        ssid.trim();
        
        if (ssid.length() > 0) {
          Serial.print("Enter Password: ");
          String password = readSerialLine();
          password.trim();
          
          wifiManager.saveConfig(ssid.c_str(), password.c_str());
          Serial.println("✓ WiFi configuration saved");
        } else {
          Serial.println("Configuration cancelled");
        }
      }
      break;
      
    case 2:  // 重新配置WiFi 或 扫描网络
      if (wifiManager.isWiFiConfigured()) {
        // 重新配置
        wifiManager.clearConfig();
        Serial.println("✓ WiFi config cleared");
        Serial.println("\n--- New WiFi Configuration ---");
        Serial.print("Enter SSID: ");
        String ssid = readSerialLine();
        ssid.trim();
        
        if (ssid.length() > 0) {
          Serial.print("Enter Password: ");
          String password = readSerialLine();
          password.trim();
          
          wifiManager.saveConfig(ssid.c_str(), password.c_str());
          Serial.println("✓ New WiFi configuration saved");
        }
      } else {
        // 扫描网络
        wifiManager.scanNetworks();
      }
      break;
      
    case 3:  // 清除配置
      if (wifiManager.isWiFiConfigured()) {
        wifiManager.clearConfig();
        Serial.println("✓ WiFi configuration cleared");
      }
      break;
  }
}

// 辅助函数：从串口读取一行（不回显密码时显示*号）
String readSerialLine() {
  String line = "";
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        Serial.println();
        break;
      } else if (c == '\b' || c == 127) {
        if (line.length() > 0) {
          line.remove(line.length() - 1);
          Serial.print("\b \b");
        }
      } else {
        line += c;
        Serial.print('*');
      }
    }
  }
  return line;
}

void setup() {
  // 初始化串口
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(1000);
  Serial.println("\n\n========================================");
  Serial.println("ESP32-S3 Camera Server Starting...");
  Serial.println("========================================\n");

  // 初始化WiFi配置管理器
  wifiManager.begin();
  
  // 显示启动菜单并等待用户输入
  showStartupMenu();
  handleStartupMenu();
  
  // 连接到WiFi
  if (wifiManager.isWiFiConfigured()) {
    Serial.println("\n--- Connecting to WiFi ---");
    if (!wifiManager.connectToWiFi(30)) {  // 30秒超时
      Serial.println("Failed to connect to WiFi!");
      Serial.println("You can configure WiFi via serial commands.");
    }
  } else {
    Serial.println("\nNo WiFi configuration found.");
    Serial.println("Use serial commands to configure WiFi.");
  }

  Serial.println("\n--- Initializing Camera ---");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 22000000;
  
  // 分辨率设置
  // FRAMESIZE_QQVGA (160x120)
  //FRAMESIZE_240X240 (240X240)
  // FRAMESIZE_QVGA (320x240)
  // FRAMESIZE_VGA (640x480)
  // FRAMESIZE_SVGA (800x600)
  // FRAMESIZE_XGA (1024x768)
  // FRAMESIZE_SXGA (1280x1024)
  // FRAMESIZE_UXGA (1600x1200)
  config.frame_size = FRAMESIZE_SVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 20;  // JPEG质量 (1-63，越小质量越高，越占带宽)
  config.fb_count = 2;  // 帧缓冲区数量（PSRAM越大可以设置越多）


  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      Serial.println("PSRAM found, using high quality settings");
      config.jpeg_quality = 20;
      config.fb_count = 3;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      Serial.println("No PSRAM found, using low quality settings");
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
      config.fb_count = 1;
    }
  } else {
    config.frame_size = FRAMESIZE_SVGA;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // 摄像头初始化
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    Serial.println("Please check camera module connection and power supply");
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  Serial.printf("Camera PID: 0x%04X\n", s->id.PID);

if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);          // 垂直翻转 (1=启用, 0=禁用)
    s->set_hmirror(s, 0);        // 水平镜像 (1=启用, 0=禁用)
    s->set_brightness(s, 0);     // 亮度: -2到2
    s->set_contrast(s, 2);       // 对比度: -2到2
    s->set_saturation(s, 0);    // 饱和度: -2到2
    s->set_sharpness(s, 1);      // 锐度: -2到2
      
    // 白平衡
    s->set_whitebal(s, 0);       // 自动白平衡: 0=禁用, 1=启用
    s->set_awb_gain(s, 0);       // AWB增益: 0=禁用, 1=启用
    s->set_wb_mode(s, 0);        // 白平衡模式: 0=自动,1=晴天,2=阴天,3=办公室,4=家庭
    
    // 曝光
    s->set_exposure_ctrl(s, 0);  // 自动曝光: 0=禁用, 1=启用
    s->set_aec2(s, 0);           // AEC2: 0=禁用, 1=启用
    s->set_ae_level(s, 0);       // AE等级: -2到2
    s->set_aec_value(s, 800);    // AEC值: 0-1200（OV3660建议800-1000）
    
    // 增益
    s->set_gain_ctrl(s, 0);      // 自动增益: 0=禁用, 1=启用
    s->set_agc_gain(s, 2);       // AGC增益: 0-30
    s->set_gainceiling(s, (gainceiling_t)6);  // 增益上限: 0-6
    
    // 图像质量
    s->set_quality(s, 20);       // JPEG质量: 0-63，越小质量越高
    s->set_colorbar(s, 0);       // 颜色条: 0=禁用, 1=启用（测试用）
    
    // 特殊效果
    s->set_special_effect(s, 2); // 特殊效果: 0=无,1=负片,2=灰度,3=红调,4=绿调,5=蓝调,6=复古

  } else if (s->id.PID == OV2640_PID) {
    s->set_brightness(s, 0);
    s->set_saturation(s, 0);
    s->set_contrast(s, 0);
    Serial.println("OV2640 sensor detected");
  } else if (s->id.PID == OV5640_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 0);
    Serial.println("OV5640 sensor detected");
  } else {
    Serial.println("Unknown camera sensor, using default settings");
  }

  // 降低帧尺寸以获得更高的初始帧率
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }


// 如果设置了LED引脚，则点亮LED
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
  ledcWrite(LED_GPIO_NUM, 255);  // 设置默认亮度
  Serial.printf("LED flash configured on GPIO %d\n", LED_GPIO_NUM);
  Serial.println("LED initialized with default brightness: 255/255");
#endif

  // 只有WiFi已连接时才启动服务器
  if (WiFi.status() == WL_CONNECTED) {
    startCameraServer();
    
    Serial.println("\n========================================");
    Serial.println("ESP32-S3 Camera Server Started!");
    Serial.println("========================================");
    Serial.print("Open in browser: http://");
    Serial.println(WiFi.localIP());
    Serial.println();
    Serial.println("Serial commands available: help");
    Serial.println("========================================\n");
  } else {
    Serial.println("\nCamera initialized but WiFi not connected.");
    Serial.println("Use serial commands to configure and connect WiFi.");
    Serial.println("\nAvailable commands: help, config, connect, list, status");
  }
}

void loop() {
  // 处理串口命令（任何时候都可以通过串口配置WiFi）
  wifiManager.processSerialInput();
  
  // 如果WiFi断开但已配置，尝试重新连接
  static unsigned long lastReconnectCheck = 0;
  if (wifiManager.isWiFiConfigured() && 
      WiFi.status() != WL_CONNECTED && 
      millis() - lastReconnectCheck > 30000) {  // 每30秒检查一次
    
    Serial.println("WiFi disconnected, attempting to reconnect...");
    wifiManager.connectToWiFi(10);
    lastReconnectCheck = millis();
  }
  
  delay(100);
}
