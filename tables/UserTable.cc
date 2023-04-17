#include <drogon/drogon.h>
#include <iostream>
#include "UserTable.h"

using namespace drogon;
using namespace drogon::orm;

const std::string USER_TABLE_NAME = "user";

void UserTable::create(drogon::orm::DbClientPtr client) {
    try {
        client->execSqlSync(
            "CREATE TABLE IF NOT EXISTS " + USER_TABLE_NAME + " ("
            "id UUID PRIMARY KEY NOT NULL,"
            "first_name VARCHAR(50) NOT NULL,"
            "last_name VARCHAR(50) NOT NULL,"
            "email VARCHAR(50) UNIQUE NOT NULL,"
            "password VARCHAR(50) UNIQUE NOT NULL,"
            "is_deleted BOOL,"
            "created_at timestamp with time zone NOT NULL,"
            "updated_at timestamp with time zone,"
            "deleted_at timestamp with time zone"
            ")"
        );
        LOG_DEBUG << "Created table " << USER_TABLE_NAME;
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to create table " << USER_TABLE_NAME << ": " << e.what();
    }
}

void UserTable::alter(drogon::orm::DbClientPtr client) {
    // TODO: Add ALTER TABLE query to modify table structure
}

void UserTable::_delete(drogon::orm::DbClientPtr client) {
    try {
        client->execSqlSync("DROP TABLE IF EXISTS " + USER_TABLE_NAME);
        LOG_DEBUG << "Dropped table " << USER_TABLE_NAME;
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to drop table " << USER_TABLE_NAME << ": " << e.what();
    }
}
