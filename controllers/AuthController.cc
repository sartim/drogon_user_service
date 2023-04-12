
#include <drogon/HttpController.h>
#include <drogon/orm/Mapper.h>
#include <drogon/HttpViewData.h>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>
#include "AuthController.h"
#include "../models/User.h"

using namespace drogon;
using namespace drogon::orm;



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
        

    } else {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        auto resp=HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }

}
