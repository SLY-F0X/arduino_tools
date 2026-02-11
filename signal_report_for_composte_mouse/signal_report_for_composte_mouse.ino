#include <SPI.h>
#include <Usb.h>
#include <usbhub.h>
#include <hidcomposite.h>

USB Usb;
USBHub Hub(&Usb); 

class MyHID : public HIDComposite {
public:
  MyHID(USB* p) : HIDComposite(p) {}

protected:
  // proto: 0=none, 1=keyboard, 2=mouse
  // Игровые мыши proto=0, но iface==1
  bool SelectInterface(uint8_t iface, uint8_t proto) override {
    // Классическая HID mouse
    if (proto == 2) return true;

    // Игровая
    if (iface == 1) return true;

    return false;
  }
};

MyHID Hid(&Usb);

class RawHIDParser : public HIDReportParser {
public:
  void Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* buf) override {
    Serial.print("len=");
    Serial.print(len);
    Serial.print(" rptIdPresent=");
    Serial.print(is_rpt_id ? "yes" : "no");
    Serial.print(" data: ");
    for (uint8_t i = 0; i < len; i++) {
      if (buf[i] < 16) Serial.print("0");
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
};

RawHIDParser rawParser;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (Usb.Init() == -1) {
    Serial.println("USB Host Shield init failed");
    while (1) {}
  }

  Hid.SetReportParser(0, &rawParser);
  Hid.SetReportParser(1, &rawParser);

  Serial.println("Ready. Move/click the mouse...");
}

void loop() {
  Usb.Task();
}
