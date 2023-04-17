#include <drogon/drogon.h>
#include <iostream>
#include "UserTable.h"

using namespace drogon;
using namespace drogon::orm;

const std::string USER_TABLE_NAME = "user";

void UserTable::connect()
{
    if (client == nullptr) {
        client = drogon::app().getDbClient();
    }
}

void UserTable::create() {
    connect();

    try {
        auto sql = "CREATE TABLE IF NOT EXISTS public."+ USER_TABLE_NAME +" ("
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
        if (client) {
            client->execSqlSync(sql);
            LOG_DEBUG << "Created table " << USER_TABLE_NAME;
        } else {
            LOG_WARN << "Connection failed";
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to create table " << USER_TABLE_NAME << ": " << e.what();
    }
}


void UserTable::alter() {
    // TODO: Add ALTER TABLE query to modify table structure
}

void UserTable::_delete() {
    try {
        auto sql = "DROP TABLE IF EXISTS $1";
        client->execSqlSync(sql, USER_TABLE_NAME);
        LOG_DEBUG << "Dropped table " << USER_TABLE_NAME;
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to drop table " << USER_TABLE_NAME << ": " << e.what();
    }
}
