#include "Crypto.h"
#include "../core/IonTypes.h"

#if ION_PLATFORM_ESP32
    #include <WiFi.h>
#elif ION_PLATFORM_ESP8266
    #include <ESP8266WiFi.h>
#endif

namespace IonConnect {

uint8_t Crypto::key[16] = {0};
bool Crypto::initialized = false;

void Crypto::init() {
    if (initialized) return;
    
    deriveKey();
    initialized = true;
}

void Crypto::deriveKey() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    
    #if ION_PLATFORM_ESP32
        uint32_t chipId = ESP.getEfuseMac();
    #else
        uint32_t chipId = ESP.getChipId();
    #endif
    
    // Derive 128-bit key from MAC + chip ID
    for (int i = 0; i < 16; i++) {
        key[i] = mac[i % 6] ^ ((chipId >> ((i % 4) * 8)) & 0xFF) ^ (i * 31);
    }
}

String Crypto::encrypt(const String& plaintext) {
    if (!initialized) init();
    
    size_t len = plaintext.length();
    uint8_t* output = new uint8_t[len];
    
    // Simple XOR cipher with key
    for (size_t i = 0; i < len; i++) {
        output[i] = plaintext[i] ^ key[i % 16];
    }
    
    String result = toHex(output, len);
    delete[] output;
    
    return result;
}

String Crypto::decrypt(const String& ciphertext) {
    if (!initialized) init();
    
    size_t len = ciphertext.length() / 2;
    uint8_t* data = new uint8_t[len];
    fromHex(ciphertext, data, len);
    
    // XOR again to decrypt
    String result = "";
    for (size_t i = 0; i < len; i++) {
        result += (char)(data[i] ^ key[i % 16]);
    }
    
    delete[] data;
    return result;
}

String Crypto::generateToken(size_t length) {
    String token = "";
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    
    for (size_t i = 0; i < length; i++) {
        token += charset[random(0, sizeof(charset) - 1)];
    }
    
    return token;
}

String Crypto::toHex(const uint8_t* data, size_t len) {
    String hex = "";
    for (size_t i = 0; i < len; i++) {
        if (data[i] < 16) hex += "0";
        hex += String(data[i], HEX);
    }
    return hex;
}

void Crypto::fromHex(const String& hex, uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        String byteString = hex.substring(i * 2, i * 2 + 2);
        data[i] = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
    }
}

} // namespace IonConnect

