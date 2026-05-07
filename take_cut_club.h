#pragma once
#include <string>
#include "servant/Application.h"
#include "Java2RoomProto.h"
#include "RoomServant.h"

using namespace JFGame;

class TakeCutClubRequest
{
    friend class TakeCutClubRespons;
public:
    TakeCutClubRequest() {}
    TakeCutClubRequest(const std::string &json)
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
        }
        catch (const std::exception &e)
        {
            std::string errInfo = ::toString(__FILE__, ":", __LINE__, ":TakeCutClubRequest decode error!");
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
};
class TakeCutClubRespons
{
public:
    TakeCutClubRespons() {}
    TakeCutClubRespons(const string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        SERIALIZE_MEMBER(writer, iId);
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
        SET_DOC_MEMBER(d, iId);
        SET_DOC_MEMBER(d, iPetCut);
        SET_DOC_MEMBER(d, iInsureCut);
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        FUNC_ENTRY("");
        __TRY__

        // STEP1 解码
        TakeCutClubRequest request;
        decode(reqBuf, request);

        // STEP2 具体业务处理
        int64_t resultCode = RESULT_CODE_SUCCESS;
       
        Club::InnerClubTakeCutReq req;
        Club::InnerClubTakeCutResp resp;
        req.clubId = S2L(request._sClubID);
        req.uId = S2L(request._sUid);
        req.id = S2L(request._sId);
        int iRet = g_app.getOuterFactoryPtr()->getSocialServerPrx(req.clubId)->InnerClubTakeCut(req, resp);
        if (iRet != 0)
        {
            ROLLLOG_ERROR << "InnerClubTakeCut failed, iRet:" << iRet << ", request._sClubID:" << req.clubId << endl;
            resultCode = RESULT_CODE_FAIL;
        }

        // STEP3 填充数据
        encode(resultCode, request, rspBuf, resp.id, resp.petCut, resp.insureCut);
        
        __CATCH__
        FUNC_EXIT("", 0);
        return 0;
    }

private:

    static void encode(int64_t resultCode, TakeCutClubRequest &request, vector<tars::Char> &rspBuf, int64_t id, int64_t petCut, int64_t insureCut)
    {
        TakeCutClubRespons  response;
        response._iId.assign(id);
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
    CInteger _iId;          // 唯一ID
    CInteger _iPetCut;      // 抽水分成
    CInteger _iInsureCut;   // 保险分成
};
