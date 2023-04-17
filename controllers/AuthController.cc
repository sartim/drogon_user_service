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
        const std::string secretKey = drogon::app().getCustomConfig()["secret_key"].asString();
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
