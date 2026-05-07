#pragma once
#include <string>

class CreateClubRequest
{
    friend class CreateClubRespons;
public:
    CreateClubRequest() {}
    CreateClubRequest(const std::string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        SERIALIZE_MEMBER(writer, lUid);
        SERIALIZE_MEMBER(writer, sClubAvatar);
        SERIALIZE_MEMBER(writer, lClubID);
        SERIALIZE_MEMBER(writer, sClubName);
        SERIALIZE_MEMBER(writer, sClubNotice);
        SERIALIZE_MEMBER(writer, sClubPassWord);
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
            SET_DOC_MEMBER(d, lUid);
            SET_DOC_MEMBER(d, sClubAvatar);
            SET_DOC_MEMBER(d, lClubID);
            SET_DOC_MEMBER(d, sClubName);
            SET_DOC_MEMBER(d, sClubNotice);
            SET_DOC_MEMBER(d, sClubPassWord);
        }
        catch (const std::exception &e)
        {
            std::string errInfo = ::toString(__FILE__, ":", __LINE__, ":CreateClubRequest decode error!");
            throw logic_error(errInfo);
        }
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        return 0;
    }

private:
    CInteger _lUid;         // 用户UID
    CString _sClubAvatar;   // logo
    CInteger _lClubID;      // 俱乐部ID
    CString _sClubName;     // 名称
    CString _sClubNotice;   // 宣言
    CString _sClubPassWord; // 密码
};
class CreateClubRespons
{
public:
    CreateClubRespons() {}
    CreateClubRespons(const string &json)
    {
        this->Deserialize(json);
    }
    template <typename Writer>
    void Serialize(Writer &writer) const
    {
        writer.StartObject();
        SERIALIZE_MEMBER(writer, lUid);
        SERIALIZE_MEMBER(writer, sClubAvatar);
        SERIALIZE_MEMBER(writer, lClubID);
        SERIALIZE_MEMBER(writer, sClubName);
        SERIALIZE_MEMBER(writer, sClubNotice);
        SERIALIZE_MEMBER(writer, sClubPassWord);
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
        SET_DOC_MEMBER(d, lUid);
        SET_DOC_MEMBER(d, sClubAvatar);
        SET_DOC_MEMBER(d, lClubID);
        SET_DOC_MEMBER(d, sClubName);
        SET_DOC_MEMBER(d, sClubNotice);
        SET_DOC_MEMBER(d, sClubPassWord);
    }

    static tars::Int32 handler(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf)
    {
        FUNC_ENTRY("");
        __TRY__

        // STEP1 解码
        CreateClubRequest request;
        decode(reqBuf, request);

        // STEP2 具体业务处理
        int64_t resultCode = RESULT_CODE_SUCCESS;
        Club::InnerAdminClubCreateReq req;
        Club::InnerAdminClubCreateResp resp;
        req.uId = request._lUid;
        req.clubAvatar = request._sClubAvatar;
        req.clubID = request._lClubID;
        req.clubName = request._sClubName;
        req.clubNotice = request._sClubNotice;
        req.clubPassWord = request._sClubPassWord;
        int iRet = g_app.getOuterFactoryPtr()->getSocialServerPrx(request._lUid)->InnerAdminCreateClub(req, resp);
        if (iRet != 0)
        {
            ROLLLOG_ERROR << "InnerAdminCreateClub failed, iRet:" << iRet << ", request._lUid:" << request._lUid << ", request._sClubID:" << request._lClubID << endl;
            resultCode = RESULT_CODE_FAIL;
        }

        // STEP3 填充数据
        encode(resultCode, request, rspBuf);
        
        __CATCH__
        FUNC_EXIT("", 0);
        return 0;
    }

private:

    static void encode(int64_t resultCode, CreateClubRequest &request, vector<tars::Char> &rspBuf)
    {
        CreateClubRespons  response;
        response._lUid.assign(request._lUid);
        response._sClubAvatar.assign(request._sClubAvatar);
        response._lClubID.assign(request._lClubID);
        response._sClubName.assign(request._sClubName);
        response._sClubNotice.assign(request._sClubNotice);
        response._sClubPassWord.assign(request._sClubPassWord);

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
    CInteger _lUid;         // 用户UID
    CString _sClubAvatar;   // logo
    CInteger _lClubID;      // 俱乐部ID
    CString _sClubName;     // 名称
    CString _sClubNotice;   // 宣言
    CString _sClubPassWord; // 密码
};