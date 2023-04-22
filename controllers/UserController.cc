#include "bcrypt.h"
#include <drogon/orm/Mapper.h>
#include <drogon/HttpViewData.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpSimpleController.h>
#include "../models/Users.h"
#include "UserController.h"

using namespace std;
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::drogon_user_service;

void UserController::connect()
{
    if (client == nullptr) {
        client = drogon::app().getDbClient();
    }
}

void UserController::getHeaders(const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback) {
    Json::Value response;
    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
    resp->addHeader("Access-Control-Allow-Methods", "OPTIONS,POST,GET");
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}

void UserController::getByIdHeaders(const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback, string id) {
    Json::Value response;
    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
    resp->addHeader("Access-Control-Allow-Methods", "OPTIONS,DELETE,PUT,GET");
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    callback(resp);
}

void UserController::getUsers(const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback)
{
    LOG_DEBUG << "Received request: " << req->methodString() << " " << req->path();

    connect();

    if (client) {
        try {
            // Get query parameters
            auto query = req->getParameters();
            int page = 1;
            auto page_it = query.find("page");
            if (page_it != query.end()) {
                page =stoi(page_it->second);
            }
            int page_size = 25;
            auto page_size_it = query.find("page_size");
            if (page_size_it != query.end()) {
                page_size =stoi(page_size_it->second);
            }

            // Calculate offset and limit
            int offset = (page - 1) * page_size;
            int limit = page_size;

            Mapper<Users> mp(client);

            auto users = mp.orderBy(Users::Cols::_created_at)
                           .limit(limit)
                           .offset(offset)
                           .findAll();
            Json::Value response;
            Json::Value usersJson(Json::arrayValue);
            for (const auto& user : users) {
                Json::Value userJson;
                userJson["id"] = user.getValueOfId();
                userJson["first_name"] = user.getValueOfFirstName();
                userJson["last_name"] =  user.getValueOfLastName();
                userJson["email"] = user.getValueOfEmail();
                userJson["created_at"] = user.getValueOfCreatedAt().toDbString();
                userJson["updated_at"] = user.getValueOfUpdatedAt().toDbString();
                usersJson.append(userJson);
            }
            response["results"] = usersJson;
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k200OK);
            resp->addHeader("Access-Control-Allow-Origin", "*");
            callback(resp);


        } catch (const exception &e) {
            Json::Value error;
            error["error"] = e.what();
            auto resp=HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(k500InternalServerError);
            resp->addHeader("Access-Control-Allow-Origin", "*");
            callback(resp);
        }
    } else {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        auto resp=HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        resp->addHeader("Access-Control-Allow-Origin", "*");
        callback(resp);
    }
}



void UserController::getUserById(const HttpRequestPtr& req,function<void(const HttpResponsePtr&)>&& callback, string id)
{
    if (client) {
        Mapper<Users> mp(client);
        auto user = mp.findByPrimaryKey(id);

        Json::Value usersJson(Json::arrayValue);
        Json::Value userJson;
        userJson["id"] = user.getValueOfId();
        userJson["first_name"] = user.getValueOfFirstName();
        userJson["last_name"] = user.getValueOfLastName();
        userJson["email"] = user.getValueOfEmail();
        usersJson.append(userJson);

        auto resp=HttpResponse::newHttpJsonResponse(usersJson);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->addHeader("Access-Control-Allow-Origin", "*");
        callback(resp);
    } else {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        auto resp=HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k500InternalServerError);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->addHeader("Access-Control-Allow-Origin", "*");
        callback(resp);
    }
}

void UserController::createUser(const HttpRequestPtr& req,function<void(const HttpResponsePtr&)>&& callback)
{   
    auto _client = drogon::app().getDbClient();
    Mapper<Users> mp(_client);

    auto jsonBody = req->getJsonObject();
    Users user;
    user.setFirstName((*jsonBody)["first_name"].asString());
    user.setLastName((*jsonBody)["last_name"].asString());
    user.setEmail((*jsonBody)["email"].asString());
    user.setIsDeleted(true);
    string password = (*jsonBody)["password"].asString();
    char salt[BCRYPT_HASHSIZE];
    char hash[BCRYPT_HASHSIZE];
    // Generate a salt with a work factor of 12
    bcrypt_gensalt(12, salt);
    // Hash the password using the generated salt
    if (bcrypt_hashpw(password.c_str(), salt, hash) == 0) {
       cout << "Password hashed successfully: " << hash <<endl;
        user.setPassword(hash);
    } else {
       cerr << "Failed to hash password" <<endl;
    }
    auto currDate = trantor::Date::now();
    cout << currDate.toDbString() <<endl;
    user.setCreatedAt(currDate);

    try {
        // Save the new user to the database
        auto result = mp.insertFuture(user);
        if (result.wait_for(std::chrono::seconds(1)) ==future_status::ready) {
            auto r = result.get();
            Json::Value response;
            response["user"] = r.toJson();
            auto resp = HttpResponse::newHttpJsonResponse(response);
            resp->setStatusCode(k201Created);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            callback(resp);
        } else {
           cerr << "Error: future not ready" <<endl;
        }
    }
    catch (const exception& e) {
        // Code to handle the exception
       cerr << "Exception caught: " << typeid(e).name() << " - " << e.what() <<endl;
    }

    Json::Value response;
    response["user"] = "result";
    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    resp->addHeader("Access-Control-Allow-Origin", "*");
    callback(resp);
}

void UserController::updateUserById(const HttpRequestPtr& req,function<void(const HttpResponsePtr&)>&& callback, string id)
{
    char salt[BCRYPT_HASHSIZE];
    char hash[BCRYPT_HASHSIZE];

    // Generate a salt with a work factor of 12
    bcrypt_gensalt(12, salt);

    Mapper<Users> mp(client);

    auto jsonBody = req->getJsonObject();
    string firstName = jsonBody->get("first_name", "").asString();
    string lastName = jsonBody->get("last_name", "").asString();
    string email = jsonBody->get("email", "").asString();
    string password = jsonBody->get("password", "").asString();

    Users user;

    user.setId(id);
    user.setFirstName(firstName);
    user.setLastName(lastName);
    user.setEmail(email);
    user.setEmail(email);

    // Hash the password using the generated salt
    if (bcrypt_hashpw(password.c_str(), salt, hash) == 0) {
        cout << "Password hashed successfully: " << hash <<endl;
        user.setPassword(hash);
    } else {
        cerr << "Failed to hash password" <<endl;
    }

    // Update the user in the database
    auto result = mp.update(user);
    
    Json::Value response;
    response["user"] = user.toJson();
    auto resp = HttpResponse::newHttpJsonResponse(response);
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_APPLICATION_JSON);
    resp->addHeader("Access-Control-Allow-Origin", "*");
    callback(resp);
}


void UserController::deleteUserById(const HttpRequestPtr& req,
                               function<void(const HttpResponsePtr&)>&& callback,
                               string userId) {
     Mapper<Users> mp(client);

    auto user = mp.deleteByPrimaryKey(userId);
    
    if (user) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k204NoContent);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->addHeader("Access-Control-Allow-Origin", "*");
        callback(resp);
    } else {
        Json::Value response;
        response["error"] = "Record not found";
        auto resp = HttpResponse::newHttpJsonResponse(response);
        resp->setStatusCode(k400BadRequest);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->addHeader("Access-Control-Allow-Origin", "*");
        callback(resp);
    }
}