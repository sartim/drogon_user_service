#include "UserTable.h"
#include <iostream>
#include <pqxx/pqxx>

using namespace std;

const string USER_TABLE_NAME = "users_test_4";

void UserTable::create(const string &connectionString) {
  try {
    auto sql = "CREATE TABLE IF NOT EXISTS public." + USER_TABLE_NAME +
               " ("
               "id UUID DEFAULT uuid_generate_v4() PRIMARY KEY,"
               "first_name VARCHAR(255) NOT NULL,"
               "last_name VARCHAR(255) NOT NULL,"
               "email VARCHAR(255) UNIQUE NOT NULL,"
               "password VARCHAR(255) UNIQUE NOT NULL,"
               "is_deleted BOOL,"
               "created_at timestamp with time zone NOT NULL,"
               "updated_at timestamp with time zone,"
               "deleted_at timestamp with time zone"
               ")";
    pqxx::connection client{connectionString};
    pqxx::work txn{client};
    txn.exec(sql);
    txn.commit();
    LOG_DEBUG << "Created table " << USER_TABLE_NAME;
  } catch (const exception &e) {
    LOG_ERROR << "Failed to create table " << USER_TABLE_NAME << ": "
              << e.what();
  }
}

void UserTable::alter(const string &connectionString) {
  // TODO: Add ALTER TABLE query to modify table structure
}

void UserTable::_delete(const string &connectionString) {
  try {
    auto sql = "DROP TABLE IF EXISTS $1";
    pqxx::connection client{connectionString};
    pqxx::work txn{client};
    txn.exec(sql);
    txn.commit();
    LOG_DEBUG << "Dropped table " << USER_TABLE_NAME;
  } catch (const exception &e) {
    LOG_ERROR << "Failed to drop table " << USER_TABLE_NAME << ": " << e.what();
  }
}
