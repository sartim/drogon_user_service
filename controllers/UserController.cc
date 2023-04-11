#include <drogon/orm/Mapper.h>
#include <drogon/HttpViewData.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpSimpleController.h>
#include <drogon/orm/DbClient.h>
#include "../models/User.h"
#include "UserController.h"

using namespace drogon;
using namespace drogon::orm;

void UserController::getUsers(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    LOG_DEBUG << "Received request: " << req->methodString() << " " << req->path();

    auto client = drogon::app().getDbClient();


    if (client) {
        try {
            Mapper<drogon_model::go_cms_db::User> mp(client);
        
            auto f = client->execSqlAsyncFuture("SELECT * FROM public.user");
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

            Json::Value response;
            response["users"] = usersJson;
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k200OK);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            callback(resp);

        } catch (const std::exception &e) {
            Json::Value error;
            error["error"] = e.what();
            auto resp=HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        }
    } else {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        auto resp=HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}










// void UserController::getUserById(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id)
// {
//     auto dbClientPtr = app().getDbClient("postgresql");
//     Mapper<User> mp(dbClientPtr);
//     auto user = mp.findByPrimaryKey(id);
//     if (user)
//     {
//         HttpViewData data;
//         data.insert("user", *user);
//         auto res = HttpResponse::newHttpViewResponse("user.csp", data);
//         callback(res);
//     }
//     else
//     {
//         auto res = HttpResponse::newHttpResponse();
//         res->setStatusCode(HttpStatusCode::k404NotFound);
//         callback(res);
//     }
// }

// void UserController::createUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
// {
//     auto dbClientPtr = app().getDbClient("postgresql");
//     Mapper<User> mp(dbClientPtr);

//     // Get the user data from the request body
//     auto jsonBody = req->getJsonObject();
//     User user;
//     user.setName(jsonBody["name"].asString());
//     user.setEmail(jsonBody["email"].asString());

//     // Save the new user to the database
//     auto result = mp.insert(user);
//     if (result.affectedRows() == 1)
//     {
//         auto res = HttpResponse::newHttpResponse();
//         res->setStatusCode(HttpStatusCode::k201Created);
//         res->addHeader("Location", "/users/" + std::to_string(result.insertId()));
//         callback(res);
//     }
//     else
//     {
//         auto res = HttpResponse::newHttpResponse();
//         res->setStatusCode(HttpStatusCode::k500InternalServerError);
//         callback(res);
//     }
// }

// void UserController::updateUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id)
// {
//     auto dbClientPtr = app().getDbClient("postgresql");
//     Mapper<User> mp(dbClientPtr);

//     // Get the user data from the request body
//     auto jsonBody = req->getJsonObject();
//     User user;
//     user.setId(id);
//     user.setName(jsonBody["name"].asString());
//     user.setEmail(jsonBody["email"].asString());

//     // Update the user in the database
//     auto result = mp.update(user);
//     if (result.affectedRows() == 1)
//     {
//         auto res = HttpResponse::newHttpResponse();
//         res->setStatusCode(HttpStatusCode::k204NoContent);
//         callback(res);
//     }
//     else
//     {
//         auto res = HttpResponse::newHttpResponse();
//         res->setStatusCode(HttpStatusCode::k500InternalServerError);
//         callback(res);
//     }
// }


// void UserController::deleteUserById(const HttpRequestPtr& req,
//                                 std::function<void(const HttpResponsePtr&)>&& callback,
//                                 int userId) {
//     auto client = app().getDbClient("postgresql");
//     auto result = client->execSqlSync("DELETE FROM users WHEREid = $1", userId);
//     if (result.first && result.second.affectedRows() > 0) {
//         auto res = HttpResponse::newRedirectionResponse("/users");
//         callback(res);
//     } else {
//         auto res = HttpResponse::newNotFoundResponse();
//         callback(res);
//     }
// }