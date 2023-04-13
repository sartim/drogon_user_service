#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/hmac.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <drogon/HttpController.h>
#include <drogon/orm/Mapper.h>
#include <drogon/HttpViewData.h>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>
#include "AuthController.h"
#include "../models/User.h"

using namespace drogon;
using namespace drogon::orm;


std::string generateJWT(const std::string& secretKey, const std::string& payload)
{
    // Get current time
    std::time_t currentTime = std::time(nullptr);

    // Encode the payload as base64
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, payload.c_str(), payload.length());
    BIO_flush(b64);
    char* encodedPayload;
    long encodedPayloadSize = BIO_get_mem_data(bmem, &encodedPayload);
    std::string encodedPayloadStr(encodedPayload, encodedPayloadSize - 1);

    // Generate the header
    std::string header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
    std::string encodedHeader;
    b64 = BIO_new(BIO_f_base64());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, header.c_str(), header.length());
    BIO_flush(b64);
    char* encodedHeaderCharPtr;
    long encodedHeaderSize = BIO_get_mem_data(bmem, &encodedHeaderCharPtr);
    encodedHeader = std::string(encodedHeaderCharPtr, encodedHeaderSize - 1);

    // Generate the signature
    std::stringstream ss;
    ss << encodedHeader << "." << encodedPayloadStr;
    std::string message = ss.str();
    unsigned int hmacResultLen = EVP_MAX_MD_SIZE;
    unsigned char* hmacResult = (unsigned char*)malloc(hmacResultLen);
    HMAC(EVP_sha256(), secretKey.c_str(), secretKey.length(),
        (unsigned char*)message.c_str(), message.length(),
        hmacResult, &hmacResultLen);
    std::string signature;
    for (unsigned int i = 0; i < hmacResultLen; i++) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(2) << (int)hmacResult[i];
        signature += ss.str();
    }

    // Free memory allocated for hmacResult
    free(hmacResult);

    // Concatenate the encoded header, payload and signature to form the JWT
    std::stringstream jwtSS;
    jwtSS << encodedHeader << "." << encodedPayloadStr << "." << signature;
    return jwtSS.str();
}


bool verifyJWT(const std::string& secretKey, const std::string& jwt)
{
    // Split the JWT into header, payload and signature
    std::stringstream ss(jwt);
    std::string header, payload, signature;
    std::getline(ss, header, '.');
    std::getline(ss, payload, '.');
    std::getline(ss, signature);

    // Decode the payload
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bmem = BIO_new_mem_buf(payload.c_str(), payload.length());
    bmem = BIO_push(b64, bmem);
    char* decodedPayload = new char[payload.length() * 3 / 4 + 1];
    int decodedPayloadLen = BIO_read(bmem, decodedPayload, payload.length() * 3 / 4 + 1);
    decodedPayload[decodedPayloadLen] = '\0';
    std::string decodedPayloadStr(decodedPayload, decodedPayloadLen);
    delete[] decodedPayload;
    // Generate the header
    std::string header2 = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
    std::string encodedHeader;
    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, header2.c_str(), header2.length());
    BIO_flush(b64);
    bmem = BIO_push(b64, bmem);
    char* encodedHeaderCharPtr;
    long encodedHeaderSize = BIO_get_mem_data(bmem, &encodedHeaderCharPtr);
    encodedHeader = std::string(encodedHeaderCharPtr, encodedHeaderSize - 1);

    // Generate the signature
    std::stringstream ss2;
    ss2 << encodedHeader << "." << decodedPayloadStr;
    std::string message = ss2.str();
    unsigned char* hmacResult = nullptr;
    unsigned int hmacResultLen = 0;
    hmacResult = HMAC(EVP_sha256(), secretKey.c_str(), secretKey.length(),
        (unsigned char*)message.c_str(), message.length(),
        hmacResult, &hmacResultLen);
    std::string calculatedSignature;
    for (unsigned int i = 0; i < hmacResultLen; i++) {
        std::stringstream ss2;
        ss2 << std::hex << std::setfill('0') << std::setw(2) << (int)hmacResult[i];
        calculatedSignature += ss2.str();
    }

    // Compare the calculated signature with the signature in the JWT
    return calculatedSignature == signature;
}

void AuthController::asyncHandleHttpRequest(const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto email = req->getParameter("email");
    auto password = req->getParameter("password");

    auto client = drogon::app().getDbClient();

    if (client) {
        auto sql = "SELECT * FROM public.user WHERE email = $1";
        auto f = client->execSqlAsyncFuture(sql, email);
        auto result = f.get();

        Json::Value usersJson(Json::arrayValue);
        for (const auto& row : result)
        {
            Json::Value userJson;
            userJson["id"] = row["id"].as<std::string>();
            userJson["first_name"] = row["first_name"].as<std::string>();
            userJson["last_name"] = row["last_name"].as<std::string>();
            userJson["email"] = row["email"].as<std::string>();
            usersJson.append(userJson);
        }

        // Generate token for user found here and return as json
        std::string secretKey = "mysecretkey";
        std::string payload = "{\"sub\":\"1234567890\",\"name\":\"" + email +"\",\"iat\":1516239022}";
        std::string jwt = generateJWT(secretKey, payload);

        Json::Value response;
        response["token"] = jwt;
        auto resp=HttpResponse::newHttpJsonResponse(response);
        callback(resp);

    } else {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        auto resp=HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }

}
