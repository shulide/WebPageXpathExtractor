///////////////////////////////////////////////////////////////////////////////////
//
//		Author		:	stong.yang@gmail.com
//		CreationDate:	2010-9-10
//		Function	:	�������ʽ,����ת��,�ٶȿ�׼ȷ����
//		UpdateLog	:	1.Created on 2010-9-10
//		
///////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_TRANSCODING_H_
#define _INCLUDE_TRANSCODING_H_

class CTransCoding
{
public:
	//Gbkת����Utf-8
	int GbkToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen);
	//Utf-8ת����Gbk
	int Utf8ToGbk(char *pSents,int iLen,char *pRetSents,int &iRetLen);
public:
	//Big5ת����Utf-8
	int Big5ToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen);
	//Utf-8ת����Big5
	int Utf8ToBig5(char *pSents,int iLen,char *pRetSents,int &iRetLen);
public:
	//����ת����Utf-8
	int JapanToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen);
	//Utf-8ת��������
	int Utf8ToJapan(char *pSents,int iLen,char *pRetSents,int &iRetLen);
public:
	//����ת����Utf-8
	int KoreaToUtf8(char *pSents,int iLen,char *pRetSents,int &iRetLen);
	int Utf8ToKorea(char *pSents,int iLen,char *pRetSents,int &iRetLen);
private:
	//Unicodeֵת����UTF8����
	int UnicodeToUtf8(int iWordValue,char *pResult,int &iRetLen);
	//UTF8����ת����Unicodeֵ
	int Utf8ToUnicode(char *pWord,int iLen,int &iWordValue);

};


#endif //_INCLUDE_TRANSCODING_H_