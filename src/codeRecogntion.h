/**
	������Ҫ��������ʶ����ʹ�õģ�ͬʱ�ṩһ�±�����صĽӿ�
*/
#ifndef H_CODERECOGNTION_H
#define H_CODERECOGNTION_H

enum eCodeType 
{
	CODE_TYPE_UNKNOWN,//type unknown
	CODE_TYPE_ASCII,//ASCII
	CODE_TYPE_GB,//GB2312,GBK,GB10380
	CODE_TYPE_UTF8,//UTF-8
	CODE_TYPE_BIG5//BIG5
};


class CCodeRecogtion
{
public:
	CCodeRecogtion();
	~CCodeRecogtion();
public:
	/**
		���ܣ�ʶ��һ���ı��ı��룬ֻʶ��GBK��utf8 ��BIG5�����ı��뷵�ز�֪��
		����: sLine �ַ��� nLen ��ʾ���ĳ���
		����ֵ: ��ʾ�ı�������
	*/
	static eCodeType getTextType(const char* sLine, int nLen);
	/**
		����: utf8������ַ�����ȡ1��uft8�ĺ��ֻ��ַ���������ռ�õ��ֽ�
		�����ô˺��������ֽ���ʹ��
		������sLine��ʾ�ı� nUnicode[out] 1�����ֵ�unicode��ֵ
		����ֵ: ��ʾ1�����ֻ����ַ�ռ�õ��ֽ���
	*/
	static int utf8ToUnicode(const char* sLine, int& nUnicode);
private:
	//��Χ�� ˫�ֽ� ch 0xA1-0xF7 cl 0xA1-0xFE
	static bool isGB2312(unsigned char ch, unsigned char cl);
	//big���� ch 0xA1-0xFE cl 0x40-0x7E��64-126��|| 0xA1��0xFE��161-254��
	static bool isBig5(unsigned char ch, unsigned char cl);
	//gbk���� ch 0x81  0xfe cl  0x40-0x7e || 0x80-0xfe
	static bool isGBK(unsigned char ch, unsigned char cl);
	static bool isAscii(int c);
	//utf8����ʶ�𷽷� �����ѵ�
	static bool isUtf8(const char* line, int len);
};
#endif//H_CODERECOGNTION_H
