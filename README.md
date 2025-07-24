# **Hiểu rõ về giao thức truyền tin trong mạng** #
Giao thức truyền thông là tập hợp các quy tắc để hai thiết bị (máy tính, điện thoại, IoT, v.v.) có thể gửi và nhận dữ liệu qua mạng một cách chính xác và đáng tin cậy.

## 1. Socket - Cửa số giao tiếp giữa hai thiết bị ##
Socket là đối tượng đại diện cho *kênh truyền tải* trong mạng. Nó cho phép giao tiếp giữa các ứng dụng và hệ thống sử dụng các giao thức như TCP hay UDP. Socket đóng vai trong là cầu nối giữa ứng dụng và mạng.

* Socket giống như một cổng ảo để giao tiếp giữa hai thiết bị trong mạng
* Nó giống như cái "cửa" qua đó dữ liệu đi vào và đi ra 
* Lập trình socket thường dùng trong các ngôn ngữ như C, Python, Java, Node.js,...

→ Socket hoạt động dựa trên TCP hoặc UDP.

## 2. Giao thức TCP (Transmission Control Protocol) - Giao thức truyền tin đáng tin cậy ##
* Kết nối hướng kết nối (connection-oriented) -> phải "bắt tay" (handshake) trước khi truyền tải dữ liệu, hay còn được biết đến với cái tên Three-way handshake. 
* Đảm bảo dữ liệu đến đúng thứ tự, không mất mát
* Tự động kiểm tra lỗi và gửi lại nếu cần
* Phù hợp với: Web (http, https), Email (SMTP), MQTT (mặc định), Truyền file.

🟢 Ưu điểm: Đảm bảo toàn vẹn dữ liệu.

🔴 Nhược điểm: Tốc độ hơi chậm hơn do phải xác nhận.

## 3. Giao thức UDP (Uset Datagram Protocol) - Giao thức không kết nối ##
* Không có bước handshake, gửi là gửi luôn 
* Không đảm bảo thứ tự, không đảm bảo đến nơi
* Phù hợp với: Video stream, Game,...giao tiếp trong mạng nội bộ cần tốc độ cao

🟢 Ưu điểm: Rất nhanh, ít trễ.

🔴 Nhược điểm: Dễ mất gói, không đáng tin cậy.

## 4. MQTT trên TCP ##
* MQTT là giao thức truyền tin chạy trên nền TCP 
* MQTT rất nhẹ, lý tưởng cho thiết bị IoT 
* Khi dùng MQTT (ví dụ như HiveMQ hay Mosquitto,...), bản chất bên dưới là: 
  * Dùng socket TCP để mở kết nối
  * Truyền dữ liệu bằng khung tin MQTT


# **Tổng quan về các thuật ngữ trong giao thức MQTT** #
1. MQTT là gì ?
* MQTT (Messege Queuing Telemetry Transport) là giao thức truyền thông nhẹ hoạt động trên nền TCP/IP
* Dùng mô hình pub/sub(publish/subcribe): thiết bị gửi dữ liệu (publish) lên một topic, và các thiết bị khác cần dữ liệu đó có thể subcribe để nhận dữ liệu

2. Broker là gì ?
* Broker trong MQTT là trung tâm điều phối việc nhận và gửi dữ liệu giữa các thiết bị 
* Hình dung đơn giản, Esp32 gửi dữ liệu -> Broker, các client khác subcribe vào topic -> Broker chuyển dữ liệu đến họ
* Ví dụ tên một vài broker: Mosquitto (Nhẹ phổ biến, dễ chạy), HiveMQ (Hỗ trợ cả WebSocket), EMQX, AWS IoT Core (Cloud),...

3. Websocket là gì ? 
* Websocket là một giao thức truyền thông 2 chiều real-time giữa trình duyệt (hoặc thiết bị) và server bằng cách sử dụng một TCP socket để tạo ra một kết nối hiệu quả và ít tốn kém.
> MQTT mặc định sử dụng TCP (port 1883), nhưng MQTT over Websocket cho phép chạy qua WebSocket (port 9001,..)
* MQTT over Websocket là khi giao thức MQTT được bọc trong Websocket -> thuận tiện cho: <br>
 - Trình duyệt: vì trình duyệt không thể dùng TCP thô, chỉ dùng Websocket
 - Debug trên Web: như HiveMQ Web Client 
Ví dụ:
```text 
ws://broker.hivemq.com:8000/mqtt      (WebSocket)
mqtt://broker.hivemq.com:1883         (TCP truyền thống)
``` 
4. Kiến trúc tổng quan của MQTT
|Vai trò | Thiết bị | Mô tả |
|--------|----------|-------|
|Publisher| Esp32 |Gửi dữ liệu lên broker |
|Broker | Mosquitto/EMQX/HiveMQ | Trung gian nhận và phân phối dữ liệu |
|Subcriber| Web app/Python Backend | Nhận dữ liệu để xử lý hoặc hiển thị waveform |

5. Lý do dùng MQTT với Esp32 
| Ưu điểm | Mô tả |
|---------|-------|
|Giao thức nhẹ | Rất phù hợp cho vi điều khiển Esp32 |
|Thời gian thực| Dữ liệu được đẩy lên ngay lập tức |
|Hỗ trợ nhiều client | Nhiều thiết bị có thể cùng đọc/ghi vào 1 topic |
|Bảo mật tốt (nếu dùng TLS) | Hỗ trợ xác thực username/password và mã hóa TLS |

6. Quy trình kết nối cơ bản 
* Esp32 kết nối Wifi
* Esp32 kết nối tới một MQTT broker (ví dụ: `test.mosquitto.com` hoặc `broker.hivemq.com`)
* Esp32 publish dữ liệu (theo thời gian thực) lên một topic, ví dụ `esp32/data`
* Server của bạn hoặc client (Python/web) subcribe vào topic đó để nhận và xử lý 

7. Tổng quan mối quan hệ 
|Thành phần| Vai trò chính |
|----------|---------------|
|MQTT broker| Trung tâm tiếp nhận và phân phối dữ liệu |
|Publisher |Thiết bị gửi dữ liệu lên broker (Esp32 gửi dữ liệu lên chẳng hạn) |
|Subcriber | Thiết bị nhận dữ liệu từ broker (ứng dụng web, socket) |
|Websocket | Giao thức dùng để chạy MQTT trong trình duyệt (hoặc qua firewall dễ hơn) | 
