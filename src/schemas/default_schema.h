#ifndef DEFAULT_SCHEMA_H
#define DEFAULT_SCHEMA_H

namespace IonConnect {

// Default configuration schema embedded in library
const char DEFAULT_SCHEMA[] PROGMEM = R"json({
  "version": "1.0",
  "namespace": "ionconnect",
  "fields": [
    {
      "id": "wifi_ssid",
      "label": "WiFi Network",
      "type": "text",
      "required": true,
      "placeholder": "Enter SSID",
      "maxLength": 32
    },
    {
      "id": "wifi_pass",
      "label": "WiFi Password",
      "type": "password",
      "required": false,
      "encrypted": true,
      "minLength": 8,
      "placeholder": "Enter password"
    },
    {
      "id": "device_name",
      "label": "Device Name",
      "type": "text",
      "required": false,
      "default": "IonConnect Device",
      "maxLength": 32
    }
  ]
})json";

} // namespace IonConnect

#endif // DEFAULT_SCHEMA_H

