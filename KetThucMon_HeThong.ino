#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Blynk
#define BLYNK_TEMPLATE_ID "TMPL6mE5b87dK"   // ID template trên Blynk Cloud
#define BLYNK_TEMPLATE_NAME "Quickstart Template" 
#define BLYNK_AUTH_TOKEN "l1xcGDgfQyUh8PbLy5U8YrsL1R-XKa8L"  // Token để ESP32 kết nối Blynk

// WiFi
char ssid[] = "HaiDong";     
char pass[] = "244466666";   

// Chân kết nối cảm biến + buzzer
#define GAS_PIN    34  
#define FLAME_PIN  35   
#define BUZZER_PIN 26   

// Biến trạng thái
bool gasAlarm   = false;   // Cảnh báo khí gas
bool flameAlarm = false;   // Cảnh báo lửa
bool userMuted  = false;   // Người dùng tắt còi thủ công
unsigned long lastBeepTime = 0; // Thời gian để tạo tiếng beep beep
bool beepState = false;         // Trạng thái beep hiện tại

// FreeRTOS task handle
TaskHandle_t TaskSensor, TaskBuzzer, TaskBlynk;

// ---------------- 1: Đọc cảm biến ----------------
void taskSensor(void *pvParameters) {
  while (1) {
    int gasRaw     = analogRead(GAS_PIN);                  // Đọc giá trị ADC từ cảm biến khí
    int gasValue   = map(gasRaw, 0, 4095, 0, 1000);        // Chuẩn hóa về thang 0-1000
    int flameValue = digitalRead(FLAME_PIN);               // Đọc giá trị cảm biến lửa (0: có lửa, 1: an toàn)

    // Gửi dữ liệu cảm biến lên Blynk (theo Vpin)
    Blynk.virtualWrite(V0, gasValue);
    Blynk.virtualWrite(V1, flameValue);
    Blynk.virtualWrite(V2, (gasAlarm || flameAlarm));      // Trạng thái hệ thống

    // Kiểm tra điều kiện phát hiện
    bool gasDetected   = (gasValue > 400);   // Nếu nồng độ gas cao
    bool flameDetected = (flameValue == 0);  // Nếu phát hiện lửa

    if (flameDetected && !userMuted) {
      flameAlarm = true;
      gasAlarm   = false;
      digitalWrite(BUZZER_PIN, HIGH);  // Còi hú liên tục
      Blynk.virtualWrite(V3, 1);
      Blynk.logEvent("fire_alert", "🔥 Phát hiện NGỌN LỬA! Kiểm tra ngay!");  // Gửi thông báo Blynk
    }
    else if (gasDetected && !userMuted && !flameAlarm) {
      gasAlarm = true;
      Blynk.virtualWrite(V3, 1);
      Blynk.logEvent("fire_alert", "⚠️ Nồng độ KHÍ GAS cao bất thường!");
    }
    else if (!gasDetected && !flameDetected) {
      gasAlarm   = false;
      flameAlarm = false;
      userMuted  = false;
      digitalWrite(BUZZER_PIN, LOW);   // Tắt còi khi an toàn
      Blynk.virtualWrite(V3, 0);
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay 2s đọc lại
  }
}

// ----------------  2: Xử lý Buzzer ----------------
void taskBuzzer(void *pvParameters) {
  while (1) {
    if (gasAlarm && !flameAlarm && !userMuted) {
      unsigned long currentMillis = millis();
      if (currentMillis - lastBeepTime > 500) {   // Tạo tiếng beep mỗi 0.5s
        lastBeepTime = currentMillis;
        beepState = !beepState;
        digitalWrite(BUZZER_PIN, beepState ? HIGH : LOW);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Kiểm tra nhanh hơn (0.1s)
  }
}

// ---------------- 3: Blynk run ----------------
void taskBlynk(void *pvParameters) {
  while (1) {
    Blynk.run();   // Blynk cần chạy liên tục
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Tránh chiếm CPU
  }
}

// ---------------- Blynk Control ----------------
BLYNK_WRITE(V3) {  // Điều khiển còi từ app Blynk
  int buzzerState = param.asInt();
  if (buzzerState == 1) {
    userMuted  = false;
    flameAlarm = true;
    digitalWrite(BUZZER_PIN, HIGH);   // Bật còi
  } else {
    userMuted  = true;
    flameAlarm = false;
    gasAlarm   = false;
    digitalWrite(BUZZER_PIN, LOW);    // Tắt còi
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  pinMode(GAS_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);   // Kết nối Blynk + WiFi

  // Tạo task FreeRTOS chạy song song
  xTaskCreatePinnedToCore(taskSensor, "TaskSensor", 4096, NULL, 2, &TaskSensor, 1);
  xTaskCreatePinnedToCore(taskBuzzer, "TaskBuzzer", 2048, NULL, 1, &TaskBuzzer, 1);
  xTaskCreatePinnedToCore(taskBlynk,  "TaskBlynk",  4096, NULL, 3, &TaskBlynk,  1);

  Serial.println("✅ Hệ thống RTOS khởi động!");
}

void loop() {
  // loop trống vì đã chia task FreeRTOS
}
