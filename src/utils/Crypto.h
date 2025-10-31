#ifndef ION_CRYPTO_H
#define ION_CRYPTO_H

#include <Arduino.h>

namespace IonConnect {

/**
 * @brief Simple encryption utilities for IonConnect
 * 
 * Uses XOR cipher with device-unique key derived from MAC address.
 * Note: This is basic obfuscation, not cryptographically secure.
 * For production, consider using mbedTLS AES.
 */
class Crypto {
public:
    static void init();
    static String encrypt(const String& plaintext);
    static String decrypt(const String& ciphertext);
    static String generateToken(size_t length = 32);
    
private:
    static uint8_t key[16];
    static bool initialized;
    
    static void deriveKey();
    static String toHex(const uint8_t* data, size_t len);
    static void fromHex(const String& hex, uint8_t* data, size_t len);
};

} // namespace IonConnect

#endif // ION_CRYPTO_H

