#pragma once

#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

class UserTable
{
  public:
    UserTable() {
      connect();
    };

  public:
    void create();
    void alter();
    void _delete();
  
  private:
    drogon::orm::DbClientPtr client;
    void connect();
};

