# State Machine & Workflow — آلة الحالات ومسار العمل

## آلة الحالات

النظام له ثلاث حالات فقط، يديرها `updateStateMachine()` في `src/main.cpp`.

```
                 flame held ≥ FLAME_CONFIRM_MS (500ms)
      ┌─────────┐ ───────────────────────────────────►  ┌────────┐
      │ NORMAL  │                                        │  FIRE  │
      │ green ON│  ◄───────────────────────────────────  │ red ON │
      └─────────┘   calm ≥ 2×FLAME_CLEAR_MS              │ buzzer │
           ▲                                             │ pump ON│
           │                                             └────────┘
           │ (no flame)                                    │   ▲
           │                                no flame ≥     │   │ flame
           │                                FLAME_CLEAR_MS │   │ again
           │                                (5s)           ▼   │
      ┌────┴──────────────────────────────────────────────────┴─┐
      │                        COOLDOWN                          │
      │           outputs OFF, pump stopped, waiting             │
      └─────────────────────────────────────────────────────────┘
```

### شرح الانتقالات

| من | إلى | الشرط | لماذا |
|---|---|---|---|
| NORMAL | FIRE | لهب مستمر ≥ 500ms | Debounce يمنع الإنذارات الكاذبة من ومضة عابرة |
| FIRE | COOLDOWN | زوال اللهب ≥ 5s | نافذة أمان: نستمر بالرش بعد اختفاء اللهب لضمان الإخماد |
| COOLDOWN | FIRE | ظهور اللهب مجدداً | إعادة اشتعال → استجابة فورية |
| COOLDOWN | NORMAL | هدوء ≥ 10s | تأكد أن الخطر زال قبل العودة للوضع الطبيعي |

## مسار العمل (Workflow) في كل دورة `loop()`

```
loop():
  WifiManager.loop()      # صيانة Wi-Fi (غير حاجب)
  BlynkManager.loop()     # صيانة السحابة (غير حاجب)

  every SENSOR_POLL_MS (50ms):
      flame = FlameSensor.isFlameDetected()
      updateStateMachine(now, flame)      # المنطق المحلي
      if app manual override: pump ON      # تجاوز يدوي من التطبيق

  every STATUS_PUBLISH_MS (2s):
      print status to Serial
      BlynkManager.publishStatus(...)      # تحديث لوحة التحكم
```

**قاعدة التصميم:** المنطق المحلي (الاكتشاف + الإطفاء) يعمل دائماً، حتى بدون Wi-Fi أو Blynk. الشبكة للمراقبة والتحكم الإضافي فقط.
