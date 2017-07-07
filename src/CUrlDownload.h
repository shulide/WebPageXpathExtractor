/*
 * CUrlDownload.h
 *
 *  Created on: 2015-5-11
 *      Author: tang
 */

#ifndef CURLDOWNLOAD_H_
#define CURLDOWNLOAD_H_
#include <string>
using std::string;

#define TIME_INFINIT  	-1

/*
 * 基于libcurl的URL文件下载
 */
class CUrlDownload
{
public:
	CUrlDownload();
	virtual ~CUrlDownload();

	bool Download(const string &url ,const size_t timeout_s=TIME_INFINIT,bool bLocation=false);

protected:
	static int WriteMemoryCallback(char *data, size_t size, size_t nmemb, void *stream);
	static int CodeConv(string& strSrc);

public:
	string m_strHeader;
	string m_strContent;
};

#endif /* CURLDOWNLOAD_H_ */
