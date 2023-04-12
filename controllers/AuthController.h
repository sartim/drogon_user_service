#include <drogon/HttpSimpleController.h>

using namespace drogon;

class AuthController:public drogon::HttpSimpleController<AuthController>
{
public:
    virtual void asyncHandleHttpRequest(
        const HttpRequestPtr &req, 
        std::function<void (const HttpResponsePtr &)> &&callback
        )override;
    PATH_LIST_BEGIN
        PATH_ADD("/api/v1/auth/generate-jwt", Post);
    PATH_LIST_END
};