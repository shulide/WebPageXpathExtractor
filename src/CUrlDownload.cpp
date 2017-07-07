/*
 * CUrlDownload.cpp
 *
 *  Created on: 2015-5-11
 *      Author: tang
 */

#include "CUrlDownload.h"
//#include "codeRecogntion.h"
//#include "TransCoding.h"
#include "codeconv.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

using namespace std;
///////////////////////////////////////////////////////////////////////////////////////////
int CUrlDownload::WriteMemoryCallback(char *data, size_t size, size_t nmemb, void *stream)
{
	string *pResult = (string*)stream;
	if(NULL==pResult)
		return 0;

	pResult->append(data, size*nmemb);
	return int(size*nmemb);
}

int CUrlDownload::CodeConv(string& strSrc)
{
#if 0
	CCodeRecogtion m_Code;
	CTransCoding m_Trans;
	int ret = m_Code.getTextType(strSrc.c_str(),strSrc.length());
	int iResult = 0;
	string strResult;
	char *pResult = NULL;
	switch(ret)
	{
	case CODE_TYPE_GB:
		pResult = new char [2 * strSrc.length() + 1];
		memset(pResult,0,2 * strSrc.length() + 1);
		m_Trans.GbkToUtf8((char *)strSrc.c_str(),strSrc.length(),pResult,iResult);
		strResult = string(pResult);
		delete[] pResult;
		break;
	case CODE_TYPE_BIG5:
		pResult = new char [2 * strSrc.length() + 1];
		memset(pResult,0,2 * strSrc.length() + 1);
		m_Trans.Big5ToUtf8((char *)strSrc.c_str(),strSrc.length(),pResult,iResult);
		strResult = string(pResult);
		delete[] pResult;
		break;
	default:
		strResult = strSrc;
	}
	strSrc = strResult;
	return 0;
#endif
	return codeconv_text_convert(strSrc);
}
///////////////////////////////////////////////////////////////////////////

CUrlDownload::CUrlDownload()
{
}

CUrlDownload::~CUrlDownload()
{
	m_strHeader.clear();
	m_strContent.clear();
}

bool CUrlDownload::Download(const string &strURL,const size_t timeout_s/*=TIME_INFINIT*/ ,bool bLocation/*=false*/)
{
	CURL *curl_handle=NULL;
	CURLcode ret=CURLE_OK;

	m_strContent.clear();
	m_strHeader.clear();

	ret=curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	if(NULL==curl_handle)
		return false;

	curl_easy_setopt(curl_handle, CURLOPT_URL, strURL.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &CUrlDownload::WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,&m_strContent);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, &CUrlDownload::WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &m_strHeader);

	//curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:38.0) Gecko/20100101 Firefox/38.0");
	curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1) ;
	//curl_easy_setopt(curl_handle, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "gzip");

	if(bLocation)
	{
		curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 2); //设置重定向的最大次数
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);//设置301、302跳转跟随location
	}

	if(timeout_s>0)
	{
		curl_easy_setopt(curl_handle,CURLOPT_TIMEOUT ,timeout_s);
	}

	//cout<<"=================================="<<endl;
	/* get it! ,maybe timeout*/
	ret=curl_easy_perform(curl_handle);
	//cout<<"=================================="<<endl;

	if(ret!=CURLE_OK)
		return false;

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	
	if (m_strContent.size()>0)
	{
		CodeConv(m_strContent);
		return true;
	}

	return false;
}
