#pragma once

#include <drogon/HttpSimpleController.h>

#include <utility>

using namespace std;
using namespace drogon;

class BaseController
{
    public:
        static void getHeaders(
            const HttpRequestPtr& req,
            function<void(const HttpResponsePtr&)>&& callback);
        static void getByIdHeaders(
            const HttpRequestPtr& req,
            function<void(const HttpResponsePtr&)>&& callback,
            const string& userId);
        static shared_ptr<HttpResponse> handleResponse(
            const Json::Value& responseData, HttpStatusCode statusCode)
        {
            auto resp= HttpResponse::newHttpJsonResponse(responseData);
            resp->setStatusCode(statusCode);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            resp->addHeader("Access-Control-Allow-Origin", "*");
            return resp;
        }
};
