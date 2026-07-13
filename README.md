# 🔥 نظام ذكي لاكتشاف الحرائق وإطفائها — IoT Fire Detection & Suppression System

نظام يعتمد على **ESP32** لمراقبة البيئة عبر حساس اللهب. عند اكتشاف حريق يقوم النظام تلقائياً بتشغيل إنذار (LED + Buzzer) وتفعيل مضخة المياه عبر الريليه، مع إرسال حالة النظام إلى تطبيق الهاتف (Blynk) عبر Wi-Fi، ما يتيح المراقبة والتحكم بالمضخة عن بُعد.

> 📋 راجع [`PLAN.md`](PLAN.md) للخطة الكاملة: المكونات، مخطط التوصيل، مراحل التنفيذ، واعتبارات السلامة.
>
> 🖥️ **لا تملك المكونات؟** جرّب المشروع كاملاً في المتصفح بدون شراء أي قطعة عبر محاكي Wokwi — انظر [`firmware/fire_detection_wokwi/`](firmware/fire_detection_wokwi/).

---

## هيكل المشروع

يتوفر الـ firmware بنسختين — نفس منطق الاكتشاف والإطفاء، تختلفان فقط في طبقة الاتصال. ارفع **واحدة فقط** على اللوحة:

```
firmware/
├── fire_detection/            # النسخة 1: Blynk (الأسرع للنموذج الأولي)
│   ├── fire_detection.ino     # الكود الرئيسي (آلة الحالات + Blynk)
│   ├── config.h               # أرقام المنافذ (Pins) وإعدادات التوقيت
│   └── secrets.h.example      # قالب بيانات Wi-Fi وBlynk
├── fire_detection_mqtt/       # النسخة 2: MQTT (معيار صناعي، يعمل محلياً بدون إنترنت)
│   ├── fire_detection_mqtt.ino
│   ├── config.h               # المنافذ + أسماء الـ Topics
│   └── secrets.h.example      # قالب بيانات Wi-Fi والـ Broker
└── fire_detection_wokwi/      # النسخة 3: محاكاة في المتصفح (بدون مكونات فعلية)
    ├── sketch.ino             # نسخة قائمة بذاتها بدون شبكة
    ├── diagram.json           # دائرة Wokwi جاهزة
    └── README.md              # خطوات التشغيل على wokwi.com
```

| | نسخة Blynk | نسخة MQTT |
|---|---|---|
| تطبيق الهاتف | Blynk (جاهز) | أي تطبيق MQTT Dashboard (مثل IoT MQTT Panel) أو Home Assistant |
| المكتبة المطلوبة | Blynk | PubSubClient |
| يحتاج إنترنت للمراقبة | نعم (سحابة Blynk) | لا، مع Broker محلي على شبكتك |

## خطوات التشغيل

### 1. تجهيز بيئة العمل
1. ثبّت [Arduino IDE](https://www.arduino.cc/en/software).
2. أضف دعم ESP32: من **File → Preferences → Additional Board URLs** أضف:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   ثم من **Boards Manager** ثبّت `esp32`.
3. من **Library Manager** ثبّت مكتبة **Blynk** (by Volodymyr Shymanskyy).

### 2. إعداد Blynk
1. أنشئ حساباً على [blynk.cloud](https://blynk.cloud) وأنشئ Template باسم `Fire Detection System`.
2. أضف **Datastreams** (Virtual Pins):
   - `V0` — حالة النظام (String)
   - `V1` — شدة اللهب (Integer, 0–4095)
   - `V2` — حالة المضخة (Integer, 0–1)
   - `V3` — مفتاح المضخة اليدوي (Integer, 0–1)
   - `V4` — الوضع: تلقائي/يدوي (Integer, 0–1)
3. أنشئ **Event** باسم `fire_alert` وفعّل له إشعارات Push.
4. أنشئ Device من الـ Template وانسخ الـ Auth Token.
5. في تطبيق Blynk على الهاتف: أضف Widgets (Label لـ V0، Gauge لـ V1، LED لـ V2، Switch لـ V3 وV4).

### 3. إعداد بيانات الاعتماد
```bash
cd firmware/fire_detection
cp secrets.h.example secrets.h
# ثم عدّل secrets.h وضع بيانات Wi-Fi وBlynk الحقيقية
```
⚠️ ملف `secrets.h` مستثنى من Git — لا ترفع بياناتك أبداً.

### 4. الرفع على اللوحة
1. افتح `fire_detection.ino` في Arduino IDE.
2. اختر اللوحة: **ESP32 Dev Module** والمنفذ الصحيح.
3. اضغط **Upload**، ثم افتح **Serial Monitor** على 115200 لمتابعة السجل.

---

## البديل: نسخة MQTT

إن كنت تفضّل MQTT بدل Blynk (تحكم كامل، يعمل على شبكتك المحلية بدون إنترنت):

### 1. جهّز الـ Broker
- **محلي (موصى به):** ثبّت [Mosquitto](https://mosquitto.org/download/) على جهاز كمبيوتر أو Raspberry Pi على نفس الشبكة، وسجّل عنوان IP الخاص به.
- **سحابي للتجربة السريعة:** استخدم `broker.hivemq.com` بالمنفذ 1883 (عام وغير مشفّر — للتجارب فقط).

### 2. جهّز الكود
1. من **Library Manager** ثبّت مكتبة **PubSubClient** (by Nick O'Leary).
2. ```bash
   cd firmware/fire_detection_mqtt
   cp secrets.h.example secrets.h
   # عدّل secrets.h: بيانات Wi-Fi وعنوان الـ Broker
   ```
3. افتح `fire_detection_mqtt.ino` وارفعه على اللوحة.

### 3. جهّز تطبيق الهاتف
ثبّت تطبيق **IoT MQTT Panel** (أندرويد) أو أي MQTT Dashboard، واربطه بنفس الـ Broker، ثم أضف عناصر على هذه الـ Topics:

| Topic | الاتجاه | القيم | عنصر مقترح |
|---|---|---|---|
| `firesystem/state` | من الجهاز | NORMAL / FIRE / COOLDOWN | Text |
| `firesystem/flame` | من الجهاز | 0–4095 | Gauge |
| `firesystem/pump` | من الجهاز | ON / OFF | LED |
| `firesystem/availability` | من الجهاز | online / offline | LED |
| `firesystem/alert` | من الجهاز | رسالة الإنذار | Notification |
| `firesystem/cmd/mode` | إلى الجهاز | AUTO / MANUAL | Switch |
| `firesystem/cmd/pump` | إلى الجهاز | ON / OFF | Switch (يعمل في الوضع اليدوي) |

للتجربة من سطر الأوامر بدون تطبيق:
```bash
mosquitto_sub -h <BROKER_IP> -t "firesystem/#" -v          # مراقبة كل شيء
mosquitto_pub -h <BROKER_IP> -t "firesystem/cmd/mode" -m MANUAL
mosquitto_pub -h <BROKER_IP> -t "firesystem/cmd/pump" -m ON
```

## كيف يعمل النظام

- **آلة حالات محلية:** `NORMAL → FIRE → COOLDOWN → NORMAL`
  - يجب أن يستمر اللهب **500ms** قبل إطلاق الإنذار (تقليل الإنذارات الكاذبة).
  - بعد زوال اللهب تستمر المضخة **5 ثوانٍ** كنافذة أمان قبل التوقف.
  - إعادة الاشتعال أثناء التهدئة تعيد النظام فوراً لحالة الحريق.
- **الوضع التلقائي/اليدوي:** من التطبيق يمكن التبديل للوضع اليدوي والتحكم بالمضخة مباشرة. حتى في الوضع اليدوي، الإنذار المحلي (LED + Buzzer) يعمل دائماً عند اكتشاف لهب.
- **يعمل بدون إنترنت:** منطق الأمان يعمل بالكامل على ESP32؛ انقطاع Wi-Fi أو السحابة لا يعطّل الاكتشاف والإطفاء التلقائي.

## ⚠️ تنبيه سلامة

هذا **نموذج تعليمي** وليس بديلاً عن أنظمة إطفاء معتمدة. اختبر اللهب في بيئة آمنة ومفتوحة وبوجود طفاية يدوية، وافصل دائرة المضخة (الجهد الأعلى) كهربائياً عن دائرة التحكم.
