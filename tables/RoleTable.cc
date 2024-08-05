#include "RoleTable.h"
#include <drogon/drogon.h>
#include <iostream>
#include <pqxx/pqxx>

using namespace pqxx;
using namespace std;
using namespace drogon;
using namespace drogon::orm;

const string ROLE_TABLE_NAME = "roles";

void RoleTable::create(const string &connectionString) {
  try {
    auto sql = "CREATE TABLE IF NOT EXISTS public." + ROLE_TABLE_NAME +
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
    connection client{connectionString};
    work txn{client};
    txn.exec(sql);
    txn.commit();
    LOG_DEBUG << "Created table " << ROLE_TABLE_NAME;
  } catch (const std::exception &e) {
    LOG_ERROR << "Failed to create table " << ROLE_TABLE_NAME << ": "
              << e.what();
  }
}

void RoleTable::alter(const string &connectionString) {
  // TODO: Add ALTER TABLE query to modify table structure
}

void RoleTable::_delete(const string &connectionString) {
    try {
      auto sql = "DROP TABLE IF EXISTS $1";
      connection client{connectionString};
      work txn{client};
      txn.exec(sql);
      txn.commit();
      LOG_DEBUG << "Dropped table " << ROLE_TABLE_NAME;
    } catch (const std::exception &e) {
      LOG_ERROR << "Failed to drop table " << ROLE_TABLE_NAME << ": " << e.what();
    }
}
