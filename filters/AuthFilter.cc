#include <jwt-cpp/jwt.h>
#include <drogon/drogon.h>
#include "AuthFilter.h"

using namespace drogon;

bool verifyJWT(const std::string& secretKey, const std::string& token)
{
    if(token != "") {
      auto decoded_token = jwt::decode(token);

      auto verifier = jwt::verify()
                          .allow_algorithm(jwt::algorithm::hs256{ secretKey })
                          .with_issuer("auth0");

      try {
        verifier.verify(decoded_token);
        return true;
      }  catch (const std::exception& e) {
        LOG_ERROR << "Failed to verify token " << e.what();
        return false;
      }
    } else {
      LOG_ERROR << "Token missing " ;
      return false;
    }
}


void AuthFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{  
    const std::string token = req->getHeader("Authorization");
    const std::string secretKey = drogon::app().getCustomConfig()["secret_key"].asString();
    bool isVerified = verifyJWT(secretKey, token);
    if (isVerified) {
        Json::Value error;
        error["error"] = "Unauthorized";
        auto response = drogon::HttpResponse::newHttpJsonResponse(error);
        response->setStatusCode(k401Unauthorized);
        fcb(response);
    } else if (!isVerified){
        Json::Value error;
        error["error"] = "Missing Authorization Header";
        auto response = drogon::HttpResponse::newHttpJsonResponse(error);
        response->setStatusCode(k401Unauthorized);
        fcb(response);
    }
    fccb();
}

