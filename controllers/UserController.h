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
        // CRUD Operations
        METHOD_ADD(UserController::getUsers, "/user", Get);
        METHOD_ADD(UserController::getUserById, "/user/{id}", Get);
        METHOD_ADD(UserController::createUser, "/user", Post);
        METHOD_ADD(UserController::updateUserById, "/user/{id}", Put);
        METHOD_ADD(UserController::deleteUserById, "/user/{id}", Delete);
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
    void getUserById(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

    // Create a new user
    void createUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

    // Update a user by their ID
    void updateUserById(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

    // Delete a user by their ID
    void deleteUserById(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, int id);

  private:
    std::shared_ptr<DbClient> client_;

    // Connect to the database
    void connect();

};
