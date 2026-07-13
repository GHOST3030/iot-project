# Testing Checklist — قائمة الاختبار التدريجي

اختبر كل ميزة في Wokwi قبل الانتقال للتالية. علّم ✅ عند نجاح كل بند.

## المرحلة 0 — البناء
- [ ] `pio run` يبني بدون أخطاء.
- [ ] بدء المحاكاة يفتح Serial Monitor ويطبع `=== Smart Fire Detection System — starting ===`.
- [ ] يُطبع `All modules initialized. Monitoring for fire...`.

## المرحلة 1 — LED الحالة (led_controller)
- [ ] عند الإقلاع، الـ LED الأخضر مضيء (حالة NORMAL).
- [ ] الـ LED الأحمر مطفأ.

## المرحلة 2 — حساس اللهب (flame_sensor)
- [ ] الضغط على زر Flame يجعل Serial يطبع `flame=YES` (بعد التحديث التالي).
- [ ] رفع الزر يعيد `flame=no`.

## المرحلة 3 — الإنذار (led + buzzer)
- [ ] الضغط المطوّل على Flame ≥ نصف ثانية: يطفأ الأخضر، يضيء الأحمر، ويصدر الـ Buzzer صوتاً.
- [ ] يُطبع `>>> FIRE DETECTED — alarm + pump ON`.
- [ ] ضغطة خاطفة (< 500ms) لا تُطلق الإنذار (اختبار الـ Debounce).

## المرحلة 4 — المضخة (relay_controller)
- [ ] عند الحريق تعمل وحدة الريليه (يظهر مؤشرها / تسمع الطقطقة).
- [ ] بعد رفع الزر، تستمر المضخة ~5 ثوانٍ ثم تتوقف (`pump OFF, cooling down`).
- [ ] الضغط على الزر أثناء التهدئة يعيد `Re-ignition!` فوراً.

## المرحلة 5 — Wi-Fi (wifi_manager)
- [ ] في `config.h`: `WIFI_SSID="Wokwi-GUEST"`, `WIFI_PASSWORD=""`.
- [ ] Serial يطبع `Wi-Fi: connected. IP address: ...` مع عنوان IP.
- [ ] فصل الشبكة (توقيف المحاكاة/تغيير SSID) لا يوقف اكتشاف الحريق.

## المرحلة 6 — لوحة Blynk (blynk_manager)
- [ ] بعد ملء بيانات Blynk، يظهر الجهاز Online في التطبيق.
- [ ] عند الحريق: مؤشر Fire Status (V0) ومؤشر Pump (V1) يضيئان في التطبيق.
- [ ] وصول إشعار Push عند الحريق (حدث `fire_alert`).
- [ ] زر التحكم اليدوي (V3) يشغّل المضخة يدوياً.

## المرحلة 7 — التكامل الكامل
- [ ] دورة كاملة: NORMAL → FIRE → COOLDOWN → NORMAL تعمل كما في مخطط الحالات.
- [ ] لا استخدام لـ `delay()` داخل `loop()` (استجابة فورية دائماً).
