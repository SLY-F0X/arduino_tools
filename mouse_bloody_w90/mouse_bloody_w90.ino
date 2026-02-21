#define RAWHID_TX_INTERVAL 0

#include <SPI.h>
#include <Usb.h>
#include <usbhub.h> 
#include <hidcomposite.h>
#include <HID-Project.h>

USB Usb;
USBHub Hub(&Usb); 

// Параметры парсинга
#define REPORT_LEN     6
#define BUTTON_OFFSET  0
#define WHEEL_OFFSET   5
#define DX_OFFSET      1
#define DY_OFFSET      3
#define BUTTON_X1 0x08
#define BUTTON_X2 0x10

// Буфер для Serial команд
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

static inline int16_t read_i16_le(const uint8_t *p) {
  return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}
static inline int16_t read_dx(uint8_t *buf) { return read_i16_le(&buf[DX_OFFSET]); }
static inline int16_t read_dy(uint8_t *buf) { return read_i16_le(&buf[DY_OFFSET]); }

static inline int8_t clamp_i16_to_i8(int16_t v) {
  if (v > 127) return 127;
  if (v < -127) return -127;
  return (int8_t)v;
}

class MyHID : public HIDComposite {
public:
  MyHID(USB* p) : HIDComposite(p) {}
protected:
  bool SelectInterface(uint8_t iface, uint8_t proto) override {
    if (proto == 2) return true;
    if (iface == 0 || iface == 1) return true; 
    return false;
  }

  uint8_t OnInitSuccessful() override {
    // Принудительно Boot Protocol (0) для интерфейса 0
    SetProtocol(0, 0);
    // Отключаем режим ожидания (Idle)
    SetIdle(0, 0, 0); 
    return 0;
  }
};

MyHID Hid(&Usb);

// Парсер физической мыши
class MouseParser : public HIDReportParser {
  void Parse(USBHID* hid, bool isRptID, uint8_t len, uint8_t *buf) override {
    if (len < REPORT_LEN) return;

    uint8_t buttons = buf[BUTTON_OFFSET];
    int8_t dx = clamp_i16_to_i8(read_dx(buf));
    int8_t dy = clamp_i16_to_i8(read_dy(buf));
    int8_t wheel = (int8_t)buf[WHEEL_OFFSET];

    // Движение
    if (dx != 0 || dy != 0 || wheel != 0) {
      BootMouse.move(dx, dy, wheel);
    }

    // Защита от залипания
    if (buttons & 0x01) BootMouse.press(MOUSE_LEFT);   else BootMouse.release(MOUSE_LEFT);
    if (buttons & 0x02) BootMouse.press(MOUSE_RIGHT);  else BootMouse.release(MOUSE_RIGHT);
    if (buttons & 0x04) BootMouse.press(MOUSE_MIDDLE); else BootMouse.release(MOUSE_MIDDLE);
    if (buttons & BUTTON_X1) BootMouse.press(MOUSE_PREV); else BootMouse.release(MOUSE_PREV);
    if (buttons & BUTTON_X2) BootMouse.press(MOUSE_NEXT); else BootMouse.release(MOUSE_NEXT);
  }
} mouseParser;

void setup() {
  Serial.begin(115200);
  
  if (Usb.Init() == -1) {
    while (1); 
  }

  delay(200);

  BootMouse.begin();
  
  Hid.SetReportParser(0, &mouseParser);
}

// Функция чтения Serial
void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker && rc != '\r') {
      if (ndx < numChars - 1) {
        receivedChars[ndx] = rc;
        ndx++;
      }
    }
    else if (rc == endMarker) {
      receivedChars[ndx] = '\0'; 
      ndx = 0;
      newData = true;
    }
  }
}

void processNewData() {
  if (newData == true) {
    char * ptr;
    
    ptr = strtok(receivedChars, ",");
    int x = (ptr) ? atoi(ptr) : 0;
    
    ptr = strtok(NULL, ",");
    int y = (ptr) ? atoi(ptr) : 0;
    
    ptr = strtok(NULL, ",");
    int m = (ptr) ? atoi(ptr) : 0;

    if (x != 0 || y != 0) {
      BootMouse.move((int8_t)x, (int8_t)y);
    }

    if (m != 0) {
      BootMouse.click(MOUSE_LEFT); 
    }

    newData = false;
  }
}

void loop() {
  Usb.Task();
  recvWithEndMarker();
  processNewData();
}
