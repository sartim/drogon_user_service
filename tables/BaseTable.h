#pragma once

#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

class BaseTable
{
    public:
        drogon::orm::DbClientPtr client;
        void connect(){
            if (client == nullptr) {
                client = drogon::app().getDbClient();
            }
        }
};

