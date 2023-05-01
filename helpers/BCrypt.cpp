#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <cstring>
#include "BCrypt.h"

// Hashes a password using OpenSSL and PBKDF2
// Returns a string containing the salt and the derived key (in base64 encoding)
std::string BCrypt::hashPassword(const std::string& password) {
    // Generate a random salt
    unsigned char salt[16];
    RAND_bytes(salt, sizeof(salt));

    // Derive a key using PBKDF2
    unsigned char key[32];
    int rc = PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.length(),
                                    salt, sizeof(salt), 100000, sizeof(key), key);
    if (rc != 1) {
      std::cerr << "Error: PKCS5_PBKDF2_HMAC_SHA1 returned " << rc << std::endl;
      return "";
    }

    // Encode the salt and derived key in base64 and return as a string
    std::string salt_key_b64 = "";
    salt_key_b64 += "bcrypt:"; // prefix to indicate the use of PBKDF2
    salt_key_b64 += std::string(reinterpret_cast<char*>(salt), sizeof(salt));
    salt_key_b64 += std::string(reinterpret_cast<char*>(key), sizeof(key));
    return salt_key_b64;
}

// Verifies a password against a stored hash (in base64 encoding)
bool BCrypt::verifyPassword(const std::string& password, const std::string& salt_key_b64) {
    // Decode the salt and derived key from the stored hash
    if (salt_key_b64.substr(0, 7) != "bcrypt:") {
      std::cerr << "Error: Invalid hash format" << std::endl;
      return false;
    }
    std::string salt_key = salt_key_b64.substr(7);
    if (salt_key.length() != 16 + 32) {
      std::cerr << "Error: Invalid hash length" << std::endl;
      return false;
    }
    unsigned char salt[16];
    unsigned char key[32];
    std::memcpy(salt, salt_key.c_str(), 16);
    std::memcpy(key, salt_key.c_str() + 16, 32);

    // Derive a key using PBKDF2 with the input password and the stored salt
    unsigned char key2[32];
    int rc = PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.length(),
                                    salt, sizeof(salt), 100000, sizeof(key2), key2);
    if (rc != 1) {
      std::cerr << "Error: PKCS5_PBKDF2_HMAC_SHA1 returned " << rc << std::endl;
      return false;
    }

    // Compare the derived key with the stored key
    return std::memcmp(key, key2, sizeof(key)) == 0;
}