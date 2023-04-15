#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>

using namespace drogon;
using namespace drogon::orm;

class UserController : public drogon::HttpController<UserController, false>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(UserController::getUsers, "/api/v1/user", Get, "AuthFilter");
    METHOD_ADD(UserController::getUserById, "/api/v1/user/{id}", Get, "AuthFilter");
    METHOD_ADD(UserController::createUser, "/api/v1/user", Post, "AuthFilter");
    METHOD_ADD(UserController::updateUserById, "/api/v1/user/{id}", Put, "AuthFilter");
    METHOD_ADD(UserController::deleteUserById, "/api/v1/user/{id}", Delete, "AuthFilter");
    METHOD_LIST_END
  
  public:
    UserController()
    {
        LOG_DEBUG << "UserController constructor!";
    }
  
  public:
    // Get a list of all users
    void getUsers(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // Get a user by their ID
    void getUserById(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string id);

    // Create a new user
    void createUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // Update a user by their ID
    void updateUserById(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string id);

    // Delete a user by their ID
    void deleteUserById(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string id);

  private:
    std::shared_ptr<DbClient> client_;

    // Connect to the database
    void connect();

};
