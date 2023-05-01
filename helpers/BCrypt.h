//
// Created by Tim on 01/05/2023.
//

#ifndef DROGONCORE_USER_SERVICE_BCRYPT_H
#define DROGONCORE_USER_SERVICE_BCRYPT_H

class BCrypt {
    public:
        BCrypt() = default;
    public:
      static std::string hashPassword(const std::string& password);
        bool verifyPassword(const std::string& password, const std::string& salt_key_b64);
};

#endif // DROGONCORE_USER_SERVICE_BCRYPT_H
