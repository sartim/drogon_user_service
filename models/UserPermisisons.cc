/**
 *
 *  UserPermisisons.cc
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#include "UserPermisisons.h"
#include <drogon/utils/Utilities.h>
#include <string>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::drogon_user_service;

const std::string UserPermisisons::primaryKeyName = "";
const bool UserPermisisons::hasPrimaryKey = false;
const std::string UserPermisisons::tableName = "user_permisisons";

const std::vector<typename UserPermisisons::MetaData> UserPermisisons::metaData_={
};
const std::string &UserPermisisons::getColumnName(size_t index) noexcept(false)
{
    assert(index < metaData_.size());
    return metaData_[index].colName_;
}
UserPermisisons::UserPermisisons(const Row &r, const ssize_t indexOffset) noexcept
{
    if(indexOffset < 0)
    {
    }
    else
    {
        size_t offset = (size_t)indexOffset;
        if(offset + 0 > r.size())
        {
            LOG_FATAL << "Invalid SQL result for this model";
            return;
        }
        size_t index;
    }

}

UserPermisisons::UserPermisisons(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 0)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
}

UserPermisisons::UserPermisisons(const Json::Value &pJson) noexcept(false)
{
}

void UserPermisisons::updateByMasqueradedJson(const Json::Value &pJson,
                                            const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 0)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
}

void UserPermisisons::updateByJson(const Json::Value &pJson) noexcept(false)
{
}

void UserPermisisons::updateId(const uint64_t id)
{
}

const std::vector<std::string> &UserPermisisons::insertColumns() noexcept
{
    static const std::vector<std::string> inCols={
    };
    return inCols;
}

void UserPermisisons::outputArgs(drogon::orm::internal::SqlBinder &binder) const
{
}

const std::vector<std::string> UserPermisisons::updateColumns() const
{
    std::vector<std::string> ret;
    return ret;
}

void UserPermisisons::updateArgs(drogon::orm::internal::SqlBinder &binder) const
{
}
Json::Value UserPermisisons::toJson() const
{
    Json::Value ret;
    return ret;
}

Json::Value UserPermisisons::toMasqueradedJson(
    const std::vector<std::string> &pMasqueradingVector) const
{
    Json::Value ret;
    if(pMasqueradingVector.size() == 0)
    {
        return ret;
    }
    LOG_ERROR << "Masquerade failed";
    return ret;
}

bool UserPermisisons::validateJsonForCreation(const Json::Value &pJson, std::string &err)
{
    return true;
}
bool UserPermisisons::validateMasqueradedJsonForCreation(const Json::Value &pJson,
                                                         const std::vector<std::string> &pMasqueradingVector,
                                                         std::string &err)
{
    if(pMasqueradingVector.size() != 0)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
    }
    catch(const Json::LogicError &e)
    {
      err = e.what();
      return false;
    }
    return true;
}
bool UserPermisisons::validateJsonForUpdate(const Json::Value &pJson, std::string &err)
{
    return true;
}
bool UserPermisisons::validateMasqueradedJsonForUpdate(const Json::Value &pJson,
                                                       const std::vector<std::string> &pMasqueradingVector,
                                                       std::string &err)
{
    if(pMasqueradingVector.size() != 0)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
    }
    catch(const Json::LogicError &e)
    {
      err = e.what();
      return false;
    }
    return true;
}
bool UserPermisisons::validJsonOfField(size_t index,
                                       const std::string &fieldName,
                                       const Json::Value &pJson,
                                       std::string &err,
                                       bool isForCreation)
{
    switch(index)
    {
        default:
            err="Internal error in the server";
            return false;
    }
    return true;
}
