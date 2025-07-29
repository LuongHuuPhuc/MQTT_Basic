## Tình huống cụ thể mà tôi muốn làm: ##
**Tôi muốn xây dựng một mô hình mà Esp32 gửi dữ liệu lên MQTT broker như HiveMQ. Tôi muốn sử dụng 1 trang web riêng mà tôi tự tạo (Không phải dùng giao diện của HiveMQ hay bên thứ ba) để hiển thị dữ liệu này, ngoài ra tôi có thể gửi đường link này cho người khác xem được** <br>
## ✅ Giải pháp: ##
Vẫn sử dụng public MQTT broker như HiveMQ cloud (cho tiện), nhưng xây dưng web client riêng kết nối tới broker đó

# Vì sao trang web muốn kết nối tới HiveMQ cloud thì lại phải dùng tới Websocket (WS) ? #
1. Trình duyệt không thể dùng TCP thô như ESP32
Khi bạn viết code trên ESP32 (như `esp_mqtt_client`), nó dùng giao thức MQTT qua TCP hoặc MQTT qua TLS (MQTTS). Nhưng: 
  * Trình duyệt không được phép mở kết nối TCP/SSL(Secure Sockets Layer) một cách tùy ý ở bất kỳ server nào 
  * Trình duyệt chỉ hỗ trợ các giao thức web như: 
    - HTTPS, HTTP
    - WebSocket(`ws://` và `wss://`)
    - WebRTC
→ Vì vậy, muốn dùng MQTT từ trình duyệt, chỉ còn cách là dùng WebSocket (MQTT over WebSocket)

# Vấn đề bảo mật khi truyền dữ liệu # 
MQTT thường truyền dữ liệu qua internet. Nếu không có biện pháp bảo vệ, dữ liệu có thể bị nghe lén hoặc bị chỉnh sửa trong quá trình truyền. Ví dụ: 
* Username/password bạn nhập -> Dễ bị đánh cắp
* Dữ liệu đo từ cảm biến -> Có thể bị giả mạo
* Kẻ tấn công có thể gửi tin giả đến topic của bạn

**Từ đó sinh ra giao thức mã hóa giúp bảo vệ dữ liệu trong quá trình truyền**
### SSL/TLS là gì ? ###
- Đây là giao thức mã hóa giúp bảo vệ dữ liệu trong quá trình truyền tin. SSL và TLS có cùng chức năng:
 * SSL (Secure Sockets Layer): Phiên bản cũ - không còn được dùng phổ biến 
 * TLS (Transport Layer Security): phiên bản nâng cấp - hiện đang là tiêu chuẩn hiện tại

Nói đơn giản: TLS/SSL giống như một cái hộp khóa an toàn để đựng dữ liệu của bạn trước khi gửi qua internet 

### MQTT với TLS/SSL ###
MQTT hỗ trợ cả 2 chế độ: 
 * `mqtt://` -> Không bảo mật (Plain text)
 * `mqtts://` hoặc `tls://` -> Có bảo mật qua TLS (Mã hóa dữ liệu + xác thực server)

Ví dụ: 
| Giao thức                   | Port thường dùng | Bảo mật           |
| --------------------------- | ---------------- | ----------------- |
| `mqtt://broker.hivemq.com`  | 1883             | ❌ Không có mã hóa |
| `mqtts://broker.hivemq.com` | 8883             | ✅ Có mã hóa TLS   |

### MQTT sử dụng TLS như thế nào ###
Khi kết nối MQTT với TLS, quá trình sẽ như sau: 
 1. Client (ESP32, STM32, PC,...) yêu cầu kết nối bảo mật tới Broker
 2. Broker gửi certificate (chứng chỉ) để chứng minh nó là ai 
 3. Client kiểm tra chứng chỉ (so sánh với CA - Certificate Authority)
 4. Nếu chứng chỉ đó OK -> Cả 2 bên thoả thuận mã hóa -> Bắt đầu truyền dữ liệu một cách an toàn 

### MẸO DỄ NHỚ ###
*MQTT + TLS = MQTTs* (giống như HTTP + TLS = HTTPS)

# URL Web # 
https://mqtt-basic.vercel.app/
