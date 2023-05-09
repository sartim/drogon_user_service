#include <drogon/HttpResponse.h>
#include <drogon/HttpSimpleController.h>
#include <drogon/HttpViewData.h>
#include <drogon/orm/Mapper.h>
#include "RoleController.h"
#include "models/Roles.h"
#include "helpers/BCrypt.h"

using namespace std;
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::drogon_user_service;

void RoleController::connect()
{
    if (client == nullptr) {
        client = drogon::app().getDbClient();
    }
}

void RoleController::getRoles(
    const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback)
{
    string method = req->methodString();
    string reqPath = req->path();
    LOG_DEBUG << "Received request: " << method << " " << req->path();

    connect(); // connect to db

    if (client) {
        try {
            // Get query parameters
            auto query = req->getParameters();
            int page = 1;
            auto page_it = query.find("page");
            if (page_it != query.end()) {
                page = stoi(page_it->second);
            }
            int page_size = 25;
            auto page_size_it = query.find("page_size");
            if (page_size_it != query.end()) {
                page_size = stoi(page_size_it->second);
            }

            // Calculate offset and limit
            int offset = (page - 1) * page_size;
            int limit = page_size;

            Mapper<Roles> mp(client);

            auto Roles = mp.orderBy(Roles::Cols::_created_at)
                           .limit(limit)
                           .offset(offset)
                           .findAll();
            Json::Value RolesJson(Json::arrayValue);
            for (const auto& role : Roles) {
                Json::Value roleJson;
                roleJson["id"] = role.getValueOfId();
                roleJson["name"] = role.getValueOfName();
                roleJson["description"] =  role.getValueOfDescription();
                roleJson["created_at"] = role.getValueOfCreatedAt().toDbString();
                roleJson["updated_at"] = role.getValueOfUpdatedAt().toDbString();
                RolesJson.append(roleJson);
            }
            Json::Value roleResults;
            roleResults["results"] = RolesJson;
            shared_ptr<HttpResponse> response = handleResponse(
                RolesJson, k200OK);
            callback(response);
        } catch (const exception &e) {
            Json::Value error;
            error["error"] = e.what();
            shared_ptr<HttpResponse> response = handleResponse(
                error, k500InternalServerError);
            callback(response);
        }
    } else {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        shared_ptr<HttpResponse> response = handleResponse(
            error, k500InternalServerError);
        callback(response);
    }
}

void RoleController::getRoleById(
    const HttpRequestPtr& req,
    function<void(const HttpResponsePtr&)>&& callback, string id)
{
    string method = req->methodString();
    string reqPath = req->path();
    LOG_DEBUG << "Received request: " << method << " " << req->path();

    connect(); // connect to db

    if (client) {
        Mapper<Roles> mp(client);
        auto role = mp.findByPrimaryKey(id);

        Json::Value RolesJson(Json::arrayValue);
        Json::Value roleJson;
        roleJson["id"] = role.getValueOfId();
        roleJson["name"] = role.getValueOfName();
        roleJson["description"] =  role.getValueOfDescription();
        roleJson["created_at"] = role.getValueOfCreatedAt().toDbString();
        roleJson["updated_at"] = role.getValueOfUpdatedAt().toDbString();
        RolesJson.append(roleJson);

        shared_ptr<HttpResponse> response = handleResponse(
            RolesJson, k200OK);
        callback(response);
    } else {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        shared_ptr<HttpResponse> response = handleResponse(
            error, k500InternalServerError);
        callback(response);
    }
}

void RoleController::createRole(
    const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback)
{
    string method = req->methodString();
    string reqPath = req->path();
    LOG_DEBUG << "Received request: " << method << " " << req->path();

    connect(); // connect to db

    if (client) {
        Mapper<Roles> mp(client);

        auto jsonBody = req->getJsonObject();
        Roles role;
        role.setName((*jsonBody)["name"].asString());
        role.setDescription((*jsonBody)["description"].asString());
        role.setIsDeleted(true);


        auto currDate = trantor::Date::now();
        role.setCreatedAt(currDate);

        try
        {
            auto result = mp.insertFuture(role);
            auto r = result.get();
            auto resp = handleResponse(r.toJson(), k201Created);
            callback(resp);
        }
        catch (const exception& e)
        {
            cerr 
            << "Exception caught: " 
            << typeid(e).name() << " - " << e.what() << endl;
        }

        Json::Value response;
        response["role"] = "result";
        auto resp= HttpResponse::newHttpJsonResponse(response);
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_APPLICATION_JSON);
        resp->addHeader("Access-Control-Allow-Origin", "*");
        callback(resp);
    } else {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        shared_ptr<HttpResponse> response = handleResponse(
            error, k500InternalServerError);
        callback(response);
    }

}

void RoleController::updateRoleById(
    const HttpRequestPtr& req,
    function<void(const HttpResponsePtr&)>&& callback, const string& roleId)
{
    string method = req->methodString();
    string reqPath = req->path();
    LOG_DEBUG << "Received request: " << method << " " << req->path();

    connect(); // connect to db

    if (client)
    {
        Mapper<Roles> mp(client);

        auto jsonBody = req->getJsonObject();
        string name = jsonBody->get("name", "").asString();
        string description = jsonBody->get("description", "").asString();

        Roles role;

        role.setId(roleId);
        role.setName(name);
        role.setDescription(description);

        // Update the role in the database
        auto result = mp.update(role);
        shared_ptr<HttpResponse> response = handleResponse(
            role.toJson(), k200OK);
        callback(response);
    }
    else
    {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        shared_ptr<HttpResponse> response = handleResponse(
            error, k500InternalServerError);
        callback(response);
    }

}


void RoleController::deleteRoleById(
    const HttpRequestPtr& req,
    function<void(const HttpResponsePtr&)>&& callback, const string& roleId)
{
    string method = req->methodString();
    string reqPath = req->path();
    LOG_DEBUG << "Received request: " << method << " " << req->path();

    connect(); // connect to db

    if (client)
    {
        Mapper<Roles> mp(client);
        auto role = mp.deleteByPrimaryKey(roleId);

        if (role)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k204NoContent);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            resp->addHeader("Access-Control-Allow-Origin", "*");
            callback(resp);
        }
        else {
            Json::Value error;
            error["error"] = "Record not found";
            shared_ptr<HttpResponse> response = handleResponse(
                error, k400BadRequest);
            callback(response);
        }
    }
    else
    {
        Json::Value error;
        error["error"] = "Unable to connect to database";
        shared_ptr<HttpResponse> response = handleResponse(
            error, k500InternalServerError);
        callback(response);
    }
}