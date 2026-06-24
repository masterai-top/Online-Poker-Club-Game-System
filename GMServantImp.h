#ifndef _GMServantImp_H_
#define _GMServantImp_H_

#include "servant/Application.h"
#include "GMServant.h"
#include <map>
#include <functional>
#include "Define.h"

//namespace gm
//{
//  enum E_BUSINESS_TYPE : int;
//}

/**
 *
 *
 */
class GMServantImp : public gm::GMServant
{
public:
    /**
     *
     */
    virtual ~GMServantImp() {}

    /**
     *
     */
    virtual void initialize();

    /**
     *
     */
    virtual void destroy();

public:
    //http请求处理接口
    virtual tars::Int32 doRequest(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf, tars::TarsCurrentPtr current);
    //配牌器
    virtual tars::Int32 getCards(gm::GetCardsResp &resp, tars::TarsCurrentPtr current);

public:
    //
    int onJsonRequest();

private:
    //
    void initHandlers();

private:
    using HandlerType = std::function<tars::Int32(const vector<tars::Char>& reqBuf, const map<std::string, std::string>& extraInfo, vector<tars::Char>& rspBuf)>;
    std::map <int, HandlerType> handlers;
};

/////////////////////////////////////////////////////
#endif
