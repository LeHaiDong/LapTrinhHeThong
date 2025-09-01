# ESP32 - Hệ thống cảnh báo cháy thông minh (Gas + Flame + Blynk + FreeRTOS)

## Giới thiệu
Dự án này sử dụng ESP32 kết hợp cảm biến khí gas MQ-2, cảm biến lửa và buzzer để xây dựng hệ thống cảnh báo cháy. Dữ liệu cảm biến được gửi lên Blynk Cloud để giám sát từ xa. Khi phát hiện khí gas hoặc ngọn lửa, hệ thống sẽ phát còi cảnh báo và đồng thời gửi thông báo đến ứng dụng Blynk. 

Cơ chế hoạt động:
- Nếu phát hiện lửa, buzzer sẽ hú liên tục cho đến khi người dùng tắt bằng ứng dụng Blynk.  
- Nếu phát hiện khí gas vượt ngưỡng, buzzer sẽ kêu theo chu kỳ beep beep.  
- Blynk sẽ nhận thông tin cảm biến và hiển thị trạng thái cảnh báo.  

Hệ thống được xây dựng dựa trên FreeRTOS để chia thành ba tác vụ chạy song song: đọc dữ liệu cảm biến, điều khiển buzzer, và duy trì kết nối với Blynk.

## Phần cứng sử dụng
- ESP32 Devkit
- Cảm biến khí Gas MQ-2 (chân ADC34)
- Cảm biến Lửa (chân D35)
- Buzzer (chân D26)

## Thư viện cần cài đặt
- Blynk (blynkkk/blynk-library)
- WiFi.h (có sẵn trong Arduino core cho ESP32)
- FreeRTOS (tích hợp sẵn trong ESP32 Arduino core)

## Cách triển khai
1. Clone code từ GitHub:
   ```bash
   git clone https://github.com/<your-username>/<your-repo>.git
Mở project bằng Arduino IDE hoặc PlatformIO.

Cài đặt thư viện Blynk nếu chưa có.

Chỉnh sửa thông tin trong code để phù hợp với mạng WiFi và tài khoản Blynk:

cpp
Sao chép mã
#define BLYNK_TEMPLATE_ID "xxx"
#define BLYNK_AUTH_TOKEN  "xxx"
char ssid[] = "Your-WiFi-SSID";
char pass[] = "Your-WiFi-PASS";
Nạp code vào ESP32 và theo dõi kết quả trên Serial Monitor hoặc ứng dụng Blynk.

Cấu hình trên Blynk
Datastreams:

V0: Giá trị khí gas (Integer)

V1: Giá trị cảm biến lửa (Integer)

V2: Trạng thái hệ thống (Integer)

V3: Điều khiển buzzer (Integer)

Events:

fire_alert: thông báo cảnh báo cháy

Tính năng chính
Đọc dữ liệu từ cảm biến khí gas và cảm biến lửa theo thời gian thực.

Buzzer hoạt động theo hai chế độ: cảnh báo khí gas bằng tiếng beep beep, cảnh báo lửa bằng tiếng hú liên tục.

Điều khiển bật tắt buzzer từ ứng dụng Blynk.

Gửi thông báo sự kiện cảnh báo cháy đến người dùng qua Blynk.

Hệ thống được tổ chức dựa trên FreeRTOS với nhiều tác vụ chạy song song để đảm bảo hoạt động ổn định.
