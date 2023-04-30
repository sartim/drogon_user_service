#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class AuthFilter : public HttpFilter<AuthFilter>
{
  public:
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

