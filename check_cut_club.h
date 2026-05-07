#pragma once
#include <string>
#include "servant/Application.h"
#include "Java2RoomProto.h"
#include "RoomServant.h"

using namespace JFGame;

class CheckCutClubRequest
{
    friend class CheckCutClubRespons;
public:
    CheckCutClubRequest() {}
    CheckCutClubRequest(const std::string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();

        SERIALIZE_MEMBER(writer, sUid);
        SERIALIZE_MEMBER(writer, sClubID);
        SERIALIZE_MEMBER(writer, sId);
        SERIALIZE_MEMBER(writer, sPassword);
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

            SET_DOC_MEMBER(d, sUid);
            SET_DOC_MEMBER(d, sClubID);
            SET_DOC_MEMBER(d, sId);
            SET_DOC_MEMBER(d, sPassword);
        }
        catch (const std::exception &e)
        {
            std::string errInfo = ::toString(__FILE__, ":", __LINE__, ":CheckCutClubRequest decode error!");
            throw logic_error(errInfo);
        }
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        return 0;
    }

private:
    CString _sUid;          // 会长UID     
    CString _sClubID;       // 俱乐部ID
    CString _sId;           // 唯一ID
    CString _sPassword;     // 密码
};
class CheckCutClubRespons
{
public:
    CheckCutClubRespons() {}
    CheckCutClubRespons(const string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        SERIALIZE_MEMBER(writer, iLevel);
        SERIALIZE_MEMBER(writer, iPetPool);
        SERIALIZE_MEMBER(writer, iInsurePool);
        SERIALIZE_MEMBER(writer, iPetCut);
        SERIALIZE_MEMBER(writer, iInsureCut);
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
        SET_DOC_MEMBER(d, iLevel);
        SET_DOC_MEMBER(d, iPetPool);
        SET_DOC_MEMBER(d, iInsurePool);
        SET_DOC_MEMBER(d, iPetCut);
        SET_DOC_MEMBER(d, iInsureCut);
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        FUNC_ENTRY("");
        __TRY__

        // STEP1 解码
        CheckCutClubRequest request;
        decode(reqBuf, request);

        // STEP2 具体业务处理
        int64_t resultCode = RESULT_CODE_SUCCESS;
       
        Club::InnerClubCheckCutReq req;
        Club::InnerClubCheckCutResp resp;
        req.clubId = S2L(request._sClubID);
        req.uId = S2L(request._sUid);
        req.id = S2L(request._sId);
        req.password = request._sPassword;
        int iRet = g_app.getOuterFactoryPtr()->getSocialServerPrx(req.clubId)->InnerClubCheckCut(req, resp);
        if (iRet != 0)
        {
            ROLLLOG_ERROR << "InnerClubCheckCut failed, iRet:" << iRet << ", request._sClubID:" << req.clubId << endl;
            resultCode = RESULT_CODE_FAIL;
        }

        // STEP3 填充数据
        encode(resultCode, request, rspBuf, resp.level, resp.petPool, resp.insurePool, resp.petCut, resp.insureCut);
        
        __CATCH__
        FUNC_EXIT("", 0);
        return 0;
    }

private:

    static void encode(int64_t resultCode, CheckCutClubRequest &request, vector<tars::Char> &rspBuf, int64_t level, int64_t petPool, int64_t insurePool, int64_t petCut, int64_t insureCut)
    {
        CheckCutClubRespons  response;
        response._iLevel.assign(level);
        response._iPetPool.assign(petPool);
        response._iInsurePool.assign(insurePool);
        response._iPetCut.assign(petCut);
        response._iInsureCut.assign(insureCut);

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
    CInteger _iLevel;       // 分成时的等级
    CInteger _iPetPool;     // 抽水流水
    CInteger _iInsurePool;  // 保险流水
    CInteger _iPetCut;      // 抽水分成
    CInteger _iInsureCut;   // 保险分成
};
