// testsubject.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>




#include "curl/curl.h"
#include "curl/easy.h"

#define HTTP_TIMEOUT 1500

inline size_t OnWriteData(const void* lpBuffer, size_t nItemSize, size_t nItemCount, void* lpVoid)
{
	if (lpBuffer == NULL || lpVoid == NULL)
	{
		return 0;
	}

	(*(std::string*)lpVoid).append((const char*)lpBuffer, nItemSize * nItemCount);

	return nItemCount;
}

#define HTTP_FORMAT(url, cookie, post, ssl, timeout, head, response, responsecode) \
    CURL* pcURL = curl_easy_init(); if (pcURL == NULL) return CURLE_FAILED_INIT;\
    curl_easy_setopt(pcURL, CURLOPT_URL, (url));\
    curl_easy_setopt(pcURL, CURLOPT_READFUNCTION, NULL);\
    curl_easy_setopt(pcURL, CURLOPT_WRITEFUNCTION, &OnWriteData);\
    curl_easy_setopt(pcURL, CURLOPT_HEADERDATA, (head));\
    curl_easy_setopt(pcURL, CURLOPT_WRITEDATA, (response));\
    curl_easy_setopt(pcURL, CURLOPT_NOSIGNAL, 1);\
    curl_easy_setopt(pcURL, CURLOPT_CONNECTTIMEOUT_MS, (timeout));\
    curl_easy_setopt(pcURL, CURLOPT_TIMEOUT_MS, (timeout) * 2);\
    if ((cookie) != NULL && strlen(cookie) != 0) curl_easy_setopt(pcURL, CURLOPT_HTTPHEADER, curl_slist_append(NULL, (cookie)));\
    if ((post) != NULL) { curl_easy_setopt(pcURL, CURLOPT_POST, 1); curl_easy_setopt(pcURL, CURLOPT_POSTFIELDS, (post)); }\
    if ((ssl) == NULL) { curl_easy_setopt(pcURL, CURLOPT_SSL_VERIFYPEER, 0); curl_easy_setopt(pcURL, CURLOPT_SSL_VERIFYHOST, 0); }\
    else { curl_easy_setopt(pcURL, CURLOPT_SSL_VERIFYPEER, 1); curl_easy_setopt(pcURL, CURLOPT_CAINFO, (ssl)); }\
    int iResult = curl_easy_perform(pcURL); curl_easy_getinfo(pcURL, CURLINFO_RESPONSE_CODE, (responsecode));\
    curl_easy_cleanup(pcURL); return iResult;

int HTTP_POST(const std::string& szURL, const std::string& szCookie, const std::string& szPostData, std::string& szHeader, std::string& szResponse, int& iResponseCode)
{
	HTTP_FORMAT(szURL.c_str(), szCookie.c_str(), szPostData.c_str(), NULL, HTTP_TIMEOUT, (void*)&szHeader, (void*)&szResponse, &iResponseCode);
}

int main()
{

	{
		std::string szUrl = "http://192.168.0.207:10000/streaming/start";
		std::string szCookie = "Content-Type application/json; charset=UTF-8";
		std::string szPostData = "{\n   \"deviceCode\" : \"30010000001310000001\",\n   \"deviceIp\" : \"\",\n   \"parentID\" : \"10012001001110004444\",\n   \"receiveIp\" : \"172.16.10.47\",\n   \"receivePort\" : 54259,\n   \"streamProfile\" : 0\n}\n";
		std::string szHeader, szResponse;
		int iResponseCode = 0;

		int iHttpRet = HTTP_POST(szUrl.c_str(), szCookie, szPostData.c_str(), szHeader, szResponse, iResponseCode);
	}

    return 0;
}

