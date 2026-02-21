# USB Mouse to HID Bridge

Arduino-скетч для преобразования сигналов физической USB-мыши в HID-события с возможностью управления через Serial.

Устройство подключается к USB-мыши через USB Host Shield, парсит HID-отчёты и эмулирует движения/клики через библиотеку HID-Project. Дополнительно поддерживает управление по Serial.

## Зависимости
- [USB_Host_Shield_2.0](https://github.com/felis/USB_Host_Shield_2.0)
- [HID-Project](https://github.com/NicoHood/HID)

## Подключение
| Компонент | Пин Arduino |
|-----------|-------------|
| USB Host Shield | SPI (11,12,13) + SS (10) |
| Питание | 5V / GND |

## Конфигурация
```cpp
#define RAWHID_TX_INTERVAL 0      // Интервал отправки RAW HID
#define REPORT_LEN     6          // Длина отчёта мыши
#define BUTTON_OFFSET  0          // Смещение байта кнопок
#define WHEEL_OFFSET   5          // Смещение колеса прокрутки
#define DX_OFFSET      1          // Смещение оси X (16-bit LE)
#define DY_OFFSET      3          // Смещение оси Y (16-bit LE)
```


## Serial-протокол
Формат: `x,y,click\n`
- `x`: смещение по X (-127..127)
- `y`: смещение по Y (-127..127)  
- `click`: ненулевое значение = левый клик

Пример: `10,-5,0\n` → движение, `0,0,1\n` → клик

## Особенности
- Координаты мыши clamped к int8_t для совместимости с BootMouse
- Поддержка кнопок: Left, Right, Middle, X1 (Back), X2 (Forward)
- Принудительный Boot Protocol для совместимости
- Отключение Idle-режима для минимизации задержек

## Сборка
1. Установите зависимости через Library Manager
2. Выберите плату (Arduino Leonardo R3)
3. Загрузите скетч
4. Подключите мышь к USB Host Shield