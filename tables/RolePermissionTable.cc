#include "RolePermissionTable.h"
#include <drogon/drogon.h>
#include <iostream>

using namespace std;
using namespace drogon;
using namespace drogon::orm;

const string ROLE_PERMISSION_TABLE_NAME = "role_permissions";

void RolePermissionTable::create(const string &connectionString) {
  try {
    auto sql = "CREATE TABLE IF NOT EXISTS public." +
               ROLE_PERMISSION_TABLE_NAME +
               " ("
               "user_id UUID REFERENCES users(id),"
               "role_id UUID REFERENCES roles(id),"
               "created_at timestamp with time zone NOT NULL,"
               "updated_at timestamp with time zone,"
               "PRIMARY KEY (user_id, role_id)"
               ")";
    pqxx::connection client{connectionString};
    pqxx::work txn{client};
    txn.exec(sql);
    txn.commit();
    LOG_DEBUG << "Created table " << ROLE_PERMISSION_TABLE_NAME;
  } catch (const std::exception &e) {
    LOG_ERROR << "Failed to create table " << ROLE_PERMISSION_TABLE_NAME << ": "
              << e.what();
  }
}

void RolePermissionTable::alter(const string &connectionString) {
  // TODO: Add ALTER TABLE query to modify table structure
}

void RolePermissionTable::_delete(const string &connectionString) {
  try {
    auto sql = "DROP TABLE IF EXISTS $1";
    pqxx::connection client{connectionString};
    pqxx::work txn{client};
    txn.exec(sql);
    txn.commit();
    LOG_DEBUG << "Dropped table " << ROLE_PERMISSION_TABLE_NAME;
  } catch (const std::exception &e) {
    LOG_ERROR << "Failed to drop table " << ROLE_PERMISSION_TABLE_NAME << ": "
              << e.what();
  }
}
