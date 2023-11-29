#include <drogon/drogon.h>
#include <jwt-cpp/jwt.h>
#include "AuthFilter.h"
#include "../helpers/AuthToken.h"
#include "../helpers/Env.h"

using namespace drogon;



void AuthFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb,
                          FilterChainCallback &&fccb) {
  const std::string token = req->getHeader("Authorization");
  const std::string secretKey = getEnv("SECRET_KEY");
  bool isVerified = verifyJWT(secretKey, token);
  if (!isVerified) {
    Json::Value error;
    error["error"] = "Missing Authorization Header";
    auto response = drogon::HttpResponse::newHttpJsonResponse(error);
    response->setStatusCode(k401Unauthorized);
    fcb(response);
  }
  fccb();
}
