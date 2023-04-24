#include "UserPermissionTable.h"
#include <drogon/drogon.h>
#include <iostream>

using namespace std;
using namespace drogon;
using namespace drogon::orm;

const string USER_PERMISSION_TABLE_NAME = "user_permissions";

void UserPermissionTable::create() {
  try {
    auto sql = "CREATE TABLE IF NOT EXISTS public."+ USER_PERMISSION_TABLE_NAME +
               " ("
               "user_id UUID REFERENCES users(id),"
               "permission_id UUID REFERENCES permissions(id),"
               "created_at timestamp with time zone NOT NULL,"
               "updated_at timestamp with time zone"
               ")";
    if (client) {
      client->execSqlSync(sql);
      LOG_DEBUG << "Created table " << USER_PERMISSION_TABLE_NAME;
    } else {
      LOG_WARN << "Connection failed";
    }
  } catch (const std::exception& e) {
    LOG_ERROR
        << "Failed to create table "
        << USER_PERMISSION_TABLE_NAME << ": " << e.what();
  }
}


void UserPermissionTable::alter() {
  // TODO: Add ALTER TABLE query to modify table structure
}

void UserPermissionTable::_delete() {
  try {
    auto sql = "DROP TABLE IF EXISTS $1";
    client->execSqlSync(sql, USER_PERMISSION_TABLE_NAME);
    LOG_DEBUG << "Dropped table " << USER_PERMISSION_TABLE_NAME;
  } catch (const std::exception& e) {
    LOG_ERROR
        << "Failed to drop table "
        << USER_PERMISSION_TABLE_NAME << ": " << e.what();
  }
}
