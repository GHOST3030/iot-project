# Smart Fire Detection & Water Extinguishing System (ESP32 · PlatformIO)

MVP كامل، **simulation-first**، مبني بمعمارية وحدات (modules) نظيفة. يعمل في محاكي **Wokwi** ثم ينتقل إلى ESP32 حقيقي بتغيير أرقام GPIO فقط إن لزم.

> النسخة المعيارية الموصى بها للتعلّم. توجد نسخ `.ino` أبسط (Blynk / MQTT / Wokwi مبسّطة) في مجلدات `firmware/` بجوار هذا المجلد.

## البنية (Folder Structure)

```
platformio/
├── platformio.ini              # إعدادات البناء والمكتبات
├── wokwi.toml                  # ربط المحاكاة بالبناء
├── diagram.json                # دائرة Wokwi
├── src/
│   └── main.cpp                # المنسّق + آلة الحالات
├── lib/                        # كل وحدة بمسؤولية واحدة
│   ├── constants/constants.h   # المنافذ، التوقيت، القطبية، الـ enums
│   ├── config/                 # بيانات الاعتماد (config.h من config.example.h)
│   ├── flame_sensor/           # قراءة حساس اللهب
│   ├── led_controller/         # LED الإنذار/الحالة
│   ├── buzzer_controller/      # الإنذار الصوتي
│   ├── relay_controller/       # الريليه ← المضخة
│   ├── wifi_manager/           # اتصال Wi-Fi + طباعة IP
│   └── blynk_manager/          # لوحة التحكم Blynk (الوحيدة التي تضم مكتبة Blynk)
└── docs/
    ├── PIN_MAPPING.md          # جدول المنافذ
    ├── STATE_MACHINE.md        # مخطط آلة الحالات + مسار العمل
    └── TESTING_CHECKLIST.md    # قائمة اختبار تدريجية
```

## لماذا هذه المعمارية

- **مسؤولية واحدة لكل وحدة**: `main.cpp` لا يلمس العتاد مباشرة — ينادي الوحدات. سهل الفهم، الاختبار، والاستبدال.
- **لا أرقام سحرية**: كل الثوابت في `constants.h`. الانتقال للعتاد = تعديل ملف واحد.
- **enum للحالات**، **`millis()` بدل `delay()`**، دوال صغيرة ومركّزة.
- **الفصل الشبكي**: `wifi_manager` و`blynk_manager` منفصلان، والمنطق المحلي يعمل بدونهما.

## خطوات التشغيل

### 1. المتطلبات
- [PlatformIO](https://platformio.org/install) (إضافة VS Code أو `pip install platformio`).
- (اختياري للمحاكاة) إضافة **Wokwi for VS Code**.

### 2. بيانات الاعتماد
```bash
cd platformio
cp lib/config/config.example.h lib/config/config.h
# للمحاكاة: WIFI_SSID="Wokwi-GUEST", WIFI_PASSWORD=""
# وبيانات Blynk من blynk.cloud
```
`config.h` مستثنى من Git.

### 3. البناء
```bash
pio run
```

### 4. المحاكاة في Wokwi
- افتح المجلد في VS Code مع إضافة Wokwi، ثم شغّل `diagram.json`.
- أو ارفع `diagram.json` + الكود إلى [wokwi.com](https://wokwi.com).
- اضغط مطوّلاً زر **Flame** لمحاكاة الحريق.

### 5. رفع على عتاد حقيقي
```bash
pio run -t upload && pio device monitor
```

## الاختبار
اتبع [`docs/TESTING_CHECKLIST.md`](docs/TESTING_CHECKLIST.md) — اختبر كل وحدة قبل الانتقال للتالية.

## تحسينات مستقبلية
- حساس دخان (MQ-2) وحرارة (DHT22) ودمج القراءات (Sensor Fusion) لتقليل الإنذارات الكاذبة.
- كاميرا ESP32-CAM لإرسال صورة عند الإنذار.
- بطارية احتياطية لاستمرار العمل عند انقطاع الكهرباء.
- تعدّد حساسات اللهب لتغطية 360°.
- سجل أحداث في قاعدة بيانات (Firebase/Supabase).

## ⚠️ سلامة
نموذج تعليمي وليس بديلاً عن أنظمة إطفاء معتمدة. اختبر اللهب في بيئة آمنة، وافصل دائرة المضخة كهربائياً عن دائرة التحكم.
