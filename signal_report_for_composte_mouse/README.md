# Signal report for composte gaming mouse

Для Arduino Leonardo R3 с Host SHield:
    signal_report_for_composte_mouse  - 

Этот Arduino скетч захватывает и выводит в Serial сырые HID-отчеты от USB-мышей (классических и игровых) через USB Host Shield.

Основные возможности:
    Поддерживает классические HID-мыши (proto=2) и игровые мыши (iface=1).
    Парсит все входящие HID-отчеты (движение, клики, колесико).
    Выводит в Serial: длина отчета, наличие Report ID, hex-дамп данных.

How-To:
    Подключите USB Host Shield + USB-мышь
    Откройте Serial Monitor (115200 baud)
    Двигайте/кликайте мышью что бы увидить сырые данные