#ifndef WIFI_CONFIG_MANAGER_H
#define WIFI_CONFIG_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

#define WIFI_CONFIG_NAMESPACE "wifi_config"
#define WIFI_SSID_KEY "ssid"
#define WIFI_PASS_KEY "pass"
#define WIFI_CONFIG_FLAG "configured"
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64

class WiFiConfigManager {
private:
  Preferences preferences;
  String current_ssid;
  String current_password;
  bool is_configured;
  
  // 串口命令缓冲区
  static const int BUFFER_SIZE = 128;
  char serial_buffer[BUFFER_SIZE];
  int buffer_index = 0;
  
public:
  WiFiConfigManager() {
    is_configured = false;
    buffer_index = 0;
    memset(serial_buffer, 0, BUFFER_SIZE);
  }
  
  // 初始化，从Preferences加载配置
  void begin() {
    preferences.begin(WIFI_CONFIG_NAMESPACE, false);
    is_configured = preferences.getBool(WIFI_CONFIG_FLAG, false);
    
    if (is_configured) {
      current_ssid = preferences.getString(WIFI_SSID_KEY, "");
      current_password = preferences.getString(WIFI_PASS_KEY, "");
      Serial.println("Loaded WiFi config from flash:");
      Serial.printf("  SSID: %s\n", current_ssid.c_str());
      Serial.printf("  Password: %s\n", current_password.c_str());
    } else {
      Serial.println("No WiFi config found in flash");
    }
  }
  
  // 保存WiFi配置到Flash
  bool saveConfig(const char* ssid, const char* password) {
    if (strlen(ssid) == 0 || strlen(ssid) > MAX_SSID_LEN) {
      Serial.println("Invalid SSID length");
      return false;
    }
    
    if (strlen(password) > MAX_PASS_LEN) {
      Serial.println("Invalid password length");
      return false;
    }
    
    preferences.putString(WIFI_SSID_KEY, ssid);
    preferences.putString(WIFI_PASS_KEY, password);
    preferences.putBool(WIFI_CONFIG_FLAG, true);
    
    current_ssid = String(ssid);
    current_password = String(password);
    is_configured = true;
    
    Serial.println("WiFi config saved to flash");
    return true;
  }
  
  // 清除保存的WiFi配置
  void clearConfig() {
    preferences.remove(WIFI_SSID_KEY);
    preferences.remove(WIFI_PASS_KEY);
    preferences.remove(WIFI_CONFIG_FLAG);
    is_configured = false;
    current_ssid = "";
    current_password = "";
    Serial.println("WiFi config cleared from flash");
  }
  
  // 获取保存的SSID
  String getSSID() {
    return current_ssid;
  }
  
  // 获取保存的密码
  String getPassword() {
    return current_password;
  }
  
  // 检查是否已配置
  bool isWiFiConfigured() {
    return is_configured;
  }
  
  // 连接到WiFi
  bool connectToWiFi(int timeout_seconds = 20) {
    if (!is_configured) {
      Serial.println("No WiFi configuration available");
      return false;
    }
    
    Serial.printf("Connecting to WiFi: %s\n", current_ssid.c_str());
    WiFi.begin(current_ssid.c_str(), current_password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < timeout_seconds * 2) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected successfully!");
      Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
      return true;
    } else {
      Serial.println("WiFi connection failed!");
      return false;
    }
  }
  
  // 处理串口输入
  void processSerialInput() {
    while (Serial.available()) {
      char c = Serial.read();
      
      // 回车键表示命令结束
      if (c == '\n' || c == '\r') {
        if (buffer_index > 0) {
          serial_buffer[buffer_index] = '\0';
          processCommand(serial_buffer);
          buffer_index = 0;
          memset(serial_buffer, 0, BUFFER_SIZE);
        }
      } 
      // 退格键
      else if (c == '\b' || c == 127) {
        if (buffer_index > 0) {
          buffer_index--;
          Serial.print("\b \b");
        }
      }
      // 正常字符
      else if (buffer_index < BUFFER_SIZE - 1) {
        serial_buffer[buffer_index++] = c;
        Serial.print(c);
      }
    }
  }
  
  // 显示帮助信息
  void printHelp() {
    Serial.println("\n=== WiFi Configuration Commands ===");
    Serial.println("help        - Show this help");
    Serial.println("config      - Configure WiFi (you will be prompted for SSID and password)");
    Serial.println("connect     - Connect to WiFi using saved config");
    Serial.println("disconnect  - Disconnect from WiFi");
    Serial.println("status      - Show WiFi connection status and IP");
    Serial.println("list        - Scan and show available WiFi networks");
    Serial.println("clear       - Clear saved WiFi config");
    Serial.println("ip          - Show current IP address");
    Serial.println("====================================\n");
  }
  
  // 显示当前状态
  void printStatus() {
    Serial.println("\n=== WiFi Status ===");
    Serial.printf("Configured: %s\n", is_configured ? "Yes" : "No");
    if (is_configured) {
      Serial.printf("SSID: %s\n", current_ssid.c_str());
    }
    Serial.printf("Connected: %s\n", (WiFi.status() == WL_CONNECTED) ? "Yes" : "No");
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
      Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
      Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());
    }
    Serial.println("===================\n");
  }
  
  // 扫描WiFi网络
  void scanNetworks() {
    Serial.println("Scanning for WiFi networks...");
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
      Serial.println("No networks found!");
    } else {
      Serial.printf("Found %d networks:\n", n);
      Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
      Serial.println("---|-----------------------------------|------|----|------------");
      
      for (int i = 0; i < n; i++) {
        // 显示信号强度指示
        String strength = "";
        int rssi = WiFi.RSSI(i);
        if (rssi > -50) strength = "*****";
        else if (rssi > -60) strength = "**** ";
        else if (rssi > -70) strength = "***  ";
        else if (rssi > -80) strength = "**   ";
        else if (rssi > -90) strength = "*    ";
        else strength = "     ";
        
        // 格式化输出
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "%-2d | %-33.33s | %4d | %2d | %s %s", 
                 i+1, 
                 WiFi.SSID(i).c_str(), 
                 rssi, 
                 WiFi.channel(i),
                 (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secure",
                 strength.c_str());
        Serial.println(buffer);
      }
    }
    WiFi.scanDelete();
  }
  
private:
  // 处理配置命令
  void processConfigCommand() {
    Serial.println("\n--- WiFi Configuration ---");
    
    // 输入SSID
    Serial.print("Enter SSID: ");
    String ssid = readSerialLine();
    ssid.trim();
    
    if (ssid.length() == 0) {
      Serial.println("Configuration cancelled");
      return;
    }
    
    // 输入密码
    Serial.print("Enter Password: ");
    String password = readSerialLine();
    password.trim();
    
    // 保存配置
    if (saveConfig(ssid.c_str(), password.c_str())) {
      Serial.println("WiFi configuration saved successfully!");
    } else {
      Serial.println("Failed to save WiFi configuration!");
    }
  }
  
  // 从串口读取一行
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
          Serial.print('*');  // 不回显密码，显示*号
        }
      }
    }
    return line;
  }
  
  // 处理命令
  void processCommand(char* cmd) {
    String command = String(cmd);
    command.trim();
    command.toLowerCase();
    
    Serial.println();  // 换行
    
    if (command == "help" || command == "?") {
      printHelp();
    }
    else if (command == "config" || command == "configure") {
      processConfigCommand();
    }
    else if (command == "connect") {
      if (connectToWiFi()) {
        Serial.println("Connection successful!");
      } else {
        Serial.println("Connection failed!");
      }
    }
    else if (command == "disconnect") {
      WiFi.disconnect();
      Serial.println("Disconnected from WiFi");
    }
    else if (command == "status") {
      printStatus();
    }
    else if (command == "list" || command == "scan") {
      scanNetworks();
    }
    else if (command == "clear") {
      clearConfig();
    }
    else if (command == "ip") {
      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
      } else {
        Serial.println("Not connected to WiFi");
      }
    }
    else if (command.length() > 0) {
      Serial.printf("Unknown command: %s\n", cmd);
      Serial.println("Type 'help' for available commands");
    }
  }
};

#endif