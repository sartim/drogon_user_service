#pragma once

#include <drogon/HttpSimpleController.h>

using namespace std;
using namespace drogon;

class BaseController
{
    public:
        void getHeaders(const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback);
        void getByIdHeaders(const HttpRequestPtr& req, function<void(const HttpResponsePtr&)>&& callback, string id);
};
