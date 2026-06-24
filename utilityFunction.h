#ifndef __UTILITY_FUNCTION_H__
#define __UTILITY_FUNCTION_H__

#include "globe.h"
#include<time.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include "Define.h"

//TODO 这里或许应该加个namespace
static std::string datetimeToString(time_t time)
{
	tm *tm_ = localtime(&time);                // 将time_t格式转换为tm结构体
	int year, month, day;// 定义时间的各个int临时变量。
	year = tm_->tm_year + 1900;                // 临时变量，年，由于tm结构体存储的是从1900年开始的时间，所以临时变量int为tm_year加上1900。
	month = tm_->tm_mon + 1;                   // 临时变量，月，由于tm结构体的月份存储范围为0-11，所以临时变量int为tm_mon加上1。
	day = tm_->tm_mday;                        // 临时变量，日。

	int hour = tm_->tm_hour;
	int minute = tm_->tm_min;
	int second = tm_->tm_sec;

	std::ostringstream os;
	os << year << "-"
		<< std::setw(2) << std::setfill('0') << month	<< "-"
		<< std::setw(2) << std::setfill('0') << day		<< " "
		<< std::setw(2) << std::setfill('0') << hour	<< ":"
		<< std::setw(2) << std::setfill('0') << minute	<< ":"
		<< std::setw(2) << std::setfill('0') << second;
	std::string date = os.str();
	return date;                                // 返回转换日期时间后的string变量。
}

static std::string getNow()
{
	return datetimeToString(time(nullptr));
}

//拆分字符串
static vector<std::string> split(const string& str, const string& pattern)
{
	return TC_Common::sepstr<string>(str, pattern);
}

template<typename T>
static void decode(const vector<tars::Char>& reqBuf, T& request)
{
	string postData(reqBuf.begin(), reqBuf.end());
	GMRequest req;
	req.Deserialize(postData);

	std::string param = req.getParams();
	request.Deserialize(param);
}

// 将响应编码发送给客户端 TODO 改名为toClientQuery
template<typename T>
void toClient(int64_t resultCode, const std::string& errDesc, std::vector<T>& responseVec, int64_t totalItems, int64_t totalPages, vector<tars::Char>& rspBuf)
{
	std::string resultData = "[";
	for (size_t  i = 0; i < responseVec.size(); ++i)
	{
		T& item = responseVec.at(i);
		std::string json; 
		item.toString(json);
		resultData += json;
		if (i != responseVec.size() - 1){
			resultData += ",";
		}
	}
	resultData += "]";							// 总页数
	GMResponse rsp(resultCode, errDesc, resultData, totalItems, totalPages);
	std::string resultJson;
	rsp.toString(resultJson);
	rspBuf.assign(resultJson.begin(), resultJson.end());
}

// 删除的时候resultData、totalItems和totalPages为null
void toClientDelete(int64_t resultCode, const std::string& errDesc, vector<tars::Char>& rspBuf)
{
	//GMResponse rsp(resultCode, errDesc);
	GMResponse rsp(resultCode, errDesc, "", 0, 0);
	std::string resultJson;
	rsp.toString(resultJson);
	rspBuf.assign(resultJson.begin(), resultJson.end());
}

// 插入和更新用到的响应函数
template<typename T>
void toClientInsertUpdate(int64_t resultCode, const std::string& errDesc, std::vector<T>& responseVec, vector<tars::Char>& rspBuf)
{
	std::string resultData = "[";
	for (size_t i = 0; i < responseVec.size(); ++i)
	{
		T& item = responseVec.at(i);
		std::string json;
		item.toString(json);
		resultData += json;
		if (i != responseVec.size() - 1) {
			resultData += ",";
		}
	}
	resultData += "]";						
	//GMResponse rsp(resultCode, errDesc, resultData);
	GMResponse rsp(resultCode, errDesc, resultData, 0, 0);
	std::string resultJson;
	rsp.toString(resultJson);
	rspBuf.assign(resultJson.begin(), resultJson.end());
}

template<typename T>
std::string toString(T t)
{
	ostringstream os;
	os << t;
	return os.str();
}

template<typename T, typename... Args>
std::string toString(T head, Args... args)
{
	ostringstream os;
	os << head;
	return os.str() + toString(args...);
}

#endif
