#include "bcrypt.h"
#include <drogon/orm/Mapper.h>
#include <drogon/HttpViewData.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpSimpleController.h>
#include "../models/User.h"
#include "UserController.h"

using namespace drogon;
using namespace drogon::orm;

void UserController::connect()
{
    if (client == nullptr) {
        client = drogon::app().getDbClient();
    }
}

void UserController::getUsers(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    LOG_DEBUG << "Received request: " << req->methodString() << " " << req->path();
    
    connect();

    if (client) {
        try {
            Mapper<drogon_model::drogon_user_service::User> mp(client);
        
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

void UserController::getUserById(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string id)
{
    if (client) {
        auto sql = "SELECT * FROM public.user WHERE id = $1";
        auto f = client->execSqlAsyncFuture(sql, id);
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
        response["user"] = usersJson;
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

void UserController::createUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{   
    auto _client = drogon::app().getDbClient();
    Mapper<drogon_model::drogon_user_service::User> mp(_client);

    auto jsonBody = req->getJsonObject();
    drogon_model::drogon_user_service::User user;
    user.setFirstName((*jsonBody)["first_name"].asString());
    user.setLastName((*jsonBody)["last_name"].asString());
    user.setEmail((*jsonBody)["email"].asString());
    user.setIsDeleted(true);
    std::string password = (*jsonBody)["password"].asString();
    char salt[BCRYPT_HASHSIZE];
    char hash[BCRYPT_HASHSIZE];
    // Generate a salt with a work factor of 12
    bcrypt_gensalt(12, salt);
    // Hash the password using the generated salt
    if (bcrypt_hashpw(password.c_str(), salt, hash) == 0) {
        std::cout << "Password hashed successfully: " << hash << std::endl;
        user.setPassword(hash);
    } else {
        std::cerr << "Failed to hash password" << std::endl;
    }
    auto currDate = trantor::Date::now();
    std::cout << currDate.toDbString() << std::endl;
    user.setCreatedAt(currDate);

    try {
        // Save the new user to the database
        auto result = mp.insertFuture(user);
        if (result.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
            auto r = result.get();
            Json::Value response;
            response["user"] = r.toJson();
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k200OK);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            callback(resp);
        } else {
            std::cerr << "Error: future not ready" << std::endl;
        }
    }
    catch (const std::exception& e) {
        // Code to handle the exception
        std::cerr << "Exception caught: " << typeid(e).name() << " - " << e.what() << std::endl;
    }

    Json::Value response;
    response["user"] = "result";
    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}

// void UserController::updateUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id)
// {
//     auto dbclientPtr = app().getDbClient("postgresql");
//     Mapper<User> mp(dbclientPtr);

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