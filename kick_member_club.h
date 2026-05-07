#pragma once
#include <string>
#include "servant/Application.h"
#include "Java2RoomProto.h"
#include "RoomServant.h"

using namespace JFGame;

class KickMemberClubRequest
{
    friend class KickMemberClubRespons;
public:
    KickMemberClubRequest() {}
    KickMemberClubRequest(const std::string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        SERIALIZE_MEMBER(writer, sUids);
        SERIALIZE_MEMBER(writer, sUid);
        SERIALIZE_MEMBER(writer, sClubID);
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
            SET_DOC_MEMBER(d, sUids);
            SET_DOC_MEMBER(d, sUid);
            SET_DOC_MEMBER(d, sClubID);
        }
        catch (const std::exception &e)
        {
            std::string errInfo = ::toString(__FILE__, ":", __LINE__, ":AuditClubRequest decode error!");
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
    CString  _sUids;        // 成员人UID
    CString _sUid;          // 会长UID     
    CString _sClubID;       // 俱乐部ID  
};
class KickMemberClubRespons
{
public:
    KickMemberClubRespons() {}
    KickMemberClubRespons(const string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
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
        SET_DOC_MEMBER(d, sSuccUids);
        SET_DOC_MEMBER(d, sFailUids);
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        FUNC_ENTRY("");
        __TRY__

        // STEP1 解码
        KickMemberClubRequest request;
        decode(reqBuf, request);

        // STEP2 具体业务处理
        int64_t resultCode = RESULT_CODE_SUCCESS;
        string sSuccUids;
        string sFailUids;
        std::vector<long> vResultUids;
        std::vector<long> vReqUids = request.getUids();
       
        Club::InnerClubKickMemberListReq req;
        Club::InnerClubKickMemberListResp resp;
        req.clubId = S2L(request._sClubID);
        req.uId = S2L(request._sUid);
        req.uIds = vReqUids;
        int iRet = g_app.getOuterFactoryPtr()->getSocialServerPrx(req.clubId)->InnerClubKickMemberList(req, resp);
        if (iRet != 0)
        {
            ROLLLOG_ERROR << "InnerClubKickMember failed, iRet:" << iRet << ", request._sClubID:" << req.clubId << endl;
            resultCode = RESULT_CODE_FAIL;
        }
        vResultUids.insert(vResultUids.begin(), resp.uIds.begin(), resp.uIds.end());
        
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

    static void encode(int64_t resultCode, KickMemberClubRequest &request, vector<tars::Char> &rspBuf, const string &sSuccUids, const string &sFailUids)
    {
        KickMemberClubRespons  response;
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
    CString  _sSuccUids;    // 成功的uid
    CString  _sFailUids;    // 失败的uid
};