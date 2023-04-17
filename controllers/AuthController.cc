#include <jwt-cpp/jwt.h>
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
    auto token = jwt::create()
    .set_issuer("auth0")
    .set_type("JWS")
    .set_payload_claim("sample", jwt::claim(std::string("test")))
    .sign(jwt::algorithm::hs256{"secret"});
    return token;
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
