#include <drogon/drogon.h>
#include <iostream>
#include "PermissionTable.h"

using namespace std;
using namespace drogon;
using namespace drogon::orm;

const string PERMISSION_TABLE_NAME = "permissions";

void PermissionTable::create() {
  try {
    auto sql = "CREATE TABLE IF NOT EXISTS public."+ PERMISSION_TABLE_NAME +
               " ("
               "id UUID DEFAULT uuid_generate_v4() PRIMARY KEY,"
               "name VARCHAR(255) NOT NULL,"
               "description VARCHAR(255),"
               "meta json,"
               "is_deleted BOOL,"
               "created_at timestamp with time zone NOT NULL,"
               "updated_at timestamp with time zone,"
               "deleted_at timestamp with time zone"
               ")";
    if (client) {
      client->execSqlSync(sql);
      LOG_DEBUG << "Created table " << PERMISSION_TABLE_NAME;
    } else {
      LOG_WARN << "Connection failed";
    }
  } catch (const std::exception& e) {
    LOG_ERROR << "Failed to create table " << PERMISSION_TABLE_NAME << ": " << e.what();
  }
}


void PermissionTable::alter() {
  // TODO: Add ALTER TABLE query to modify table structure
}

void PermissionTable::_delete() {
  try {
    auto sql = "DROP TABLE IF EXISTS $1";
    client->execSqlSync(sql, PERMISSION_TABLE_NAME);
    LOG_DEBUG << "Dropped table " << PERMISSION_TABLE_NAME;
  } catch (const std::exception& e) {
    LOG_ERROR << "Failed to drop table " << PERMISSION_TABLE_NAME << ": " << e.what();
  }
}
