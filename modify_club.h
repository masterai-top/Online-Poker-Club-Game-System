#pragma once
#include <string>
#include "servant/Application.h"
#include "Java2RoomProto.h"
#include "RoomServant.h"

using namespace JFGame;

class ModifyClubRequest
{
    friend class ModifyClubRespons;
public:
    ModifyClubRequest() {}
    ModifyClubRequest(const std::string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        SERIALIZE_MEMBER(writer, iType);
        SERIALIZE_MEMBER(writer, sUids);
        SERIALIZE_MEMBER(writer, sClubID);
        SERIALIZE_MEMBER(writer, sRoomID);
        SERIALIZE_MEMBER(writer, lTakenIn);
        writer.EndObject();
    }

    void toString(std::string &json)
    {
        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        Serialize(writer);
        json = sb.GetString();
    }

    void Deserialize(const string &json)
    {
        try
        {
            Document d;
            if (d.Parse(json.c_str()).HasParseError())
            {
                throw logic_error("parse json error. raw data : " + json);
            }
            SET_DOC_MEMBER(d, iType);
            SET_DOC_MEMBER(d, sUids);
            SET_DOC_MEMBER(d, sClubID);
            SET_DOC_MEMBER(d, sRoomID);
            SET_DOC_MEMBER(d, lTakenIn);
        }
        catch (const std::exception &e)
        {
            std::string errInfo = ::toString(__FILE__, ":", __LINE__, ":ModifyClubRequest decode error!");
            throw logic_error(errInfo);
        }
    }

    std::vector<long> getUids()
    {
        std::vector<long> vResult;
        if (_sUids.isNull())
        {
            return vResult;
        }
        std::string strInfo = _sUids;
        std::vector<std::string> vecStr = split(strInfo, ",");
        for (auto &item : vecStr)
        {
            long uid = S2L(item);
            vResult.push_back(uid);

        }
        return vResult;
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        return 0;
    }

private:
    CInteger _iType;        // 操作类型 0：加入俱乐部， 1: 退出俱乐部 2：加入牌局， 3：退出牌局
    CString  _sUids;
    CString _sClubID;
    CString _sRoomID;
    CInteger _lTakenIn;     
};
class ModifyClubRespons
{
public:
    ModifyClubRespons() {}
    ModifyClubRespons(const string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        SERIALIZE_MEMBER(writer, iType);
        SERIALIZE_MEMBER(writer, sSuccUids);
        SERIALIZE_MEMBER(writer, sFailUids);
        writer.EndObject();
    }

    void toString(std::string &json)
    {
        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        Serialize(writer);
        json = sb.GetString();
    }

    void Deserialize(const string &json)
    {
        Document d;
        if (d.Parse(json.c_str()).HasParseError())
        {
            throw logic_error("parse json error. raw data : " + json);
        }
        SET_DOC_MEMBER(d, iType);
        SET_DOC_MEMBER(d, sSuccUids);
        SET_DOC_MEMBER(d, sFailUids);
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        FUNC_ENTRY("");
        __TRY__

        // STEP1 解码
        ModifyClubRequest request;
        decode(reqBuf, request);

        // STEP2 具体业务处理
        int64_t resultCode = RESULT_CODE_SUCCESS;
        string sSuccUids;
        string sFailUids;
        std::vector<long> vResultUids;
        std::vector<long> vReqUids = request.getUids();
        if(request._iType == 0)//加入俱乐部
        {
            Club::InnerClubApplyListReq req;
            Club::InnerClubApplyListResp resp;
            req.clubId = S2L(request._sClubID);
            req.uIds = vReqUids;
            int iRet = g_app.getOuterFactoryPtr()->getSocialServerPrx(req.clubId)->InnerClubApplyList(req, resp);
            if (iRet != 0)
            {
                ROLLLOG_ERROR << "InnerClubApplyList failed, iRet:" << iRet << ", request._sClubID:" << req.clubId << endl;
                resultCode = RESULT_CODE_FAIL;
            }
            vResultUids.insert(vResultUids.begin(), resp.uIds.begin(), resp.uIds.end());
        }
        else if (request._iType == 1)   // 退出俱乐部
        {
            Club::InnerClubExitListReq req;
            Club::InnerClubExitListResp resp;
            req.clubId = S2L(request._sClubID);
            req.uIds = vReqUids;
            int iRet = g_app.getOuterFactoryPtr()->getSocialServerPrx(req.clubId)->InnerClubExitList(req, resp);
            if (iRet != 0)
            {
                ROLLLOG_ERROR << "InnerClubExitList failed, iRet:" << iRet << ", request._sClubID:" << req.clubId << endl;
                resultCode = RESULT_CODE_FAIL;
            }
            vResultUids.insert(vResultUids.begin(), resp.uIds.begin(), resp.uIds.end());
        }
        else
        {
            string sServantPrx;
            int ret = g_app.getOuterFactoryPtr()->getRoomServerPrx("1:1001:22013900", sServantPrx);
            if (ret != 0)
            {
                ROLLLOG_ERROR << "get room prx err , ret: " << ret << endl;
                resultCode = RESULT_CODE_FAIL;
            }

            auto pServantObj = Application::getCommunicator()->stringToProxy<JFGame::RoomServantPrx>(sServantPrx);
            if (!pServantObj)
            {
                resultCode = RESULT_CODE_FAIL;
                ret = -2;
            }

            if(ret == 0)
            {
                java2room::EnterOrExitRoomReq req;
                java2room::EnterOrExitRoomResp resp;
                req.flag = request._iType -2;
                req.roomKey = request._sRoomID;
                req.buyCount = request._lTakenIn;
                req.uIds = vReqUids;

                ROLLLOG_DEBUG << "EnterOrExitRoomReq: " << printTars(req) << endl;

                int ret = pServantObj->onEnterOrExitRoom(req, resp);
                if(ret != 0)
                {
                    resultCode = RESULT_CODE_FAIL;
                    ret = -3;
                }
                vResultUids.insert(vResultUids.begin(), resp.uIds.begin(), resp.uIds.end());
                ROLLLOG_DEBUG << "EnterOrExitRoomResp: " << printTars(resp) << endl;
            }    
        }

        for(auto uid : vReqUids)
        {
            auto it = std::find_if(vResultUids.begin(), vResultUids.end(), [uid](long r_uid)->bool{
                return uid == r_uid;
            });
            if(it != vResultUids.end())
            {
                sSuccUids += L2S(uid) + ",";
            }
            else
            {
                sFailUids += L2S(uid) + ",";
            }
        }

        sSuccUids = sSuccUids.length() > 0 ? sSuccUids.substr(0, sSuccUids.length() - 1) : sSuccUids;
        sFailUids = sFailUids.length() > 0 ? sFailUids.substr(0, sFailUids.length() - 1) : sFailUids;

        // STEP3 填充数据
        encode(resultCode, request, rspBuf, sSuccUids, sFailUids);
        
        __CATCH__
        FUNC_EXIT("", 0);
        return 0;
    }

private:

    static void encode(int64_t resultCode, ModifyClubRequest &request, vector<tars::Char> &rspBuf, const string &sSuccUids, const string &sFailUids)
    {
        ModifyClubRespons  response;
        response._iType.assign(request._iType);
        response._sSuccUids.assign(sSuccUids);
        response._sFailUids.assign(sFailUids);

        // resultData是数组
        std::string json;
        response.toString(json);
        std::string resultData = "[" + json + "]";

        int64_t totalItems = 1;  //总条数
        int64_t totalPages = 1;  //总页数
        GMResponse rsp(resultCode, "", resultData, totalItems, totalPages);
        std::string resultJson;
        rsp.toString(resultJson);
        rspBuf.assign(resultJson.begin(), resultJson.end());
    }

private:
    CInteger _iType;        // 操作类型 0：加入俱乐部， 1：加入牌局， 2：退出牌局
    CString  _sSuccUids;//成功的uid
    CString  _sFailUids;//失败的uid
};