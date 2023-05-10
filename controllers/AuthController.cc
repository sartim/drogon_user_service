#include <jwt-cpp/jwt.h>
#include <drogon/HttpController.h>
#include <drogon/orm/Mapper.h>
#include <drogon/HttpResponse.h>
#include "AuthController.h"
#include "../models/Users.h"
#include "helpers/BCrypt.h"

using namespace std;
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::drogon_user_service;


string generateJWT(const string& secretKey, const string& email)
{
    auto token = jwt::create()
    .set_issuer("auth0")
    .set_type("JWS")
    .set_payload_claim("identity", jwt::claim(email))
    .set_issued_at(chrono::system_clock::now())
    .set_expires_at(chrono::system_clock::now() + chrono::seconds{3600})
    .sign(jwt::algorithm::hs256{secretKey});
    return token;
}

void AuthController::asyncHandleHttpRequest(const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback)
{
    string method = req->methodString();
    string reqPath = req->path();
    LOG_DEBUG << "Received request: " << method << " " << req->path();

    auto client = drogon::app().getDbClient();
    if (client)
    {
      auto jsonBody = req->getJsonObject();
      auto email = (*jsonBody)["email"].asString();
      auto password = (*jsonBody)["password"].asString();

      Mapper<Users> mp(client);
      auto user = mp.findFutureBy(Criteria(Users::Cols::_email, email));
      auto result = user.get();
      Json::Value usersJson(Json::arrayValue);

      // Check to handle invalid credentials
      Json::Value error;
      error["error"] = "Invalid credentials";

      if (!result.empty())
      {
        for (const auto& user : result)
        {
          string hashedPassword = user.getValueOfPassword();
          bool isVerified = BCrypt::verifyPassword(password, hashedPassword);
          if (!isVerified) {
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k401Unauthorized);
            callback(resp);
            return;
          }
          Json::Value userJson;
          userJson["id"] = user.getValueOfId();
          userJson["first_name"] = user.getValueOfFirstName();
          userJson["last_name"] = user.getValueOfLastName();
          userJson["email"] = user.getValueOfEmail();
          usersJson.append(userJson);
        }
      } else {
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k401Unauthorized);
        callback(resp);
        return;
      }

      // Generate token for user found here and return as json
      auto config = drogon::app().getCustomConfig()["secret_key"];
      const string secretKey = config.asString();
      string jwt = generateJWT(secretKey, email);

      Json::Value response;
      response["token"] = jwt;
      response["user"] = usersJson;
      auto resp = HttpResponse::newHttpJsonResponse(response);
      callback(resp);
    }
    else
    {
      Json::Value error;
      error["error"] = "Unable to connect to the database";
      auto resp = HttpResponse::newHttpJsonResponse(error);
      resp->setStatusCode(k500InternalServerError);
      callback(resp);
    }
}

