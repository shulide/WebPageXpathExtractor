/*
 * Use XPath to extract content
 */
#include <string>
#include <iostream>
#include "tidy/tidy.h"
#include "tidy/tidybuffio.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"
#include "CUrlDownload.h"
#include "regexhelper.h"
#include "HttpURL.h"
#include "UtilFun.h"

using namespace std;;

void  string_replace( std::string &strContent, const std::string &strsrc, const std::string &strdst )
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while( (pos=strContent.find(strsrc, pos)) != std::string::npos )
	{
		strContent.replace( pos, srclen, strdst );
		pos += dstlen;
	}
}

bool HtmlTidy(std::string &src)
{
	TidyBuffer output = {0};
	TidyBuffer errbuf = {0};
	int rc = -1;
	Bool ok;

	TidyDoc tdoc = tidyCreate();                     // Initialize "document"
	ok = tidyOptSetBool( tdoc, TidyXhtmlOut, yes );  // Convert to XHTML
	if ( ok )
		rc = tidySetErrorBuffer( tdoc, &errbuf );      // Capture diagnostics
	if ( rc >= 0 )
		rc = tidyParseString( tdoc, src.c_str() );           // Parse the input
	if ( rc >= 0 )
		rc = tidyCleanAndRepair( tdoc );               // Tidy it up!
	if ( rc >= 0 )
		rc = tidyRunDiagnostics( tdoc );               // Kvetch
	if ( rc > 1 )                                    // If error, force output.
		rc = ( tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1 );
	if ( rc >= 0 )
		rc = tidySaveBuffer( tdoc, &output );          // Pretty Print

	if ( rc >= 0 )
	{
		src.assign((char *)output.bp,output.size);
	}

	tidyBufFree( &output );
	tidyBufFree( &errbuf );
	tidyRelease( tdoc );

	if(rc>=0)
		return true;

	return false;
}

static bool HtmlToXml(string &src,bool tidy)
{
	int ret=-1;
	string_replace(src,":=","=");
	string_replace(src,"<![endif]-->","<! [endif] -->");
	string_replace(src,";</script>","; \n</script>");

	if(tidy)
	{
		if(!HtmlTidy(src))
			return false;
	}

	//Utils::Save2File("tidyhtml.txt",src);

	TidyDoc tidy_doc=tidyCreate();
        TidyBuffer output_buffer= {0};

        bool ok=tidyOptSetBool(tidy_doc,TidyXmlOut,yes)
                && tidyOptSetBool(tidy_doc,TidyNumEntities,yes)
                && tidyOptSetBool(tidy_doc,TidyShowWarnings,no);
        if(ok)
        {
                tidySetInCharEncoding(tidy_doc, "utf8");
                ret=tidyParseString(tidy_doc, src.c_str());
        }

        if(ret>=0)
        {
                tidyOptSetBool(tidy_doc, TidyXhtmlOut , no);
                tidyOptSetBool(tidy_doc,TidyXmlOut,yes);
                tidySetOutCharEncoding(tidy_doc, "utf8");
                tidyOptSetBool(tidy_doc, TidyForceOutput, yes);
                tidyOptSetBool(tidy_doc,TidyEncloseBodyText,yes);
                tidyCleanAndRepair( tidy_doc );
                ret=tidySaveBuffer(tidy_doc,&output_buffer);
        }

        if(ret<0)
	{
                return false;
	}

        src.assign((char *)output_buffer.bp,output_buffer.size);

        tidyBufFree(&output_buffer);
        tidyRelease(tidy_doc);

        return true;
}

static xmlDocPtr getDocPtr(char* docname) 
{  
    	xmlDocPtr doc = NULL;  
    	xmlKeepBlanksDefault(0);  
  
    	doc = xmlParseFile(docname);  
    	if(NULL == doc) 
	{  
        	fprintf(stderr, "document cannot be parsed!\n");    
    	}  
    	
	return doc;  
}  

static xmlXPathObjectPtr getXPathObjectPtr(const xmlDocPtr doc,const xmlChar* xpath_exp) 
{
	xmlXPathObjectPtr result;
	xmlXPathContextPtr context;

	context = xmlXPathNewContext(doc);
	if(NULL==context)
	{
		fprintf(stderr,"Context is Null in xmlXPathNewContext()\n");
		return NULL;
	}

	result = xmlXPathEvalExpression(xpath_exp, context);
	xmlXPathFreeContext(context);

	if(NULL == result) 
	{
		fprintf(stderr, "Result Error in xmlXPathEvalExpression()\n");
		return NULL;
	}

	fprintf(stderr,"XPath:%s\n",xpath_exp);
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)) 
	{
		xmlXPathFreeObject(result);
		fprintf(stderr, "Empty node set!\n");
		return NULL;
	}
	
	return result;
}


string getNodeStr(xmlDocPtr& doc, string& exp)
{   
	string res;
	xmlChar* sub_xpath_exp = (xmlChar*)(exp.c_str());
	xmlXPathObjectPtr sub_xpath_obj =  getXPathObjectPtr(doc, sub_xpath_exp);
	if(sub_xpath_obj == NULL)
		return ""; 

	xmlNodePtr sub_cur = sub_xpath_obj->nodesetval->nodeTab[0];
	sub_cur = sub_cur->children;

	while(sub_cur != NULL)
	{   
		res += string((const char *)xmlNodeGetContent(sub_cur));
		sub_cur = sub_cur->next;
	}   
	xmlXPathFreeObject(sub_xpath_obj);
	return res;
}


void DeleteSubString(string &str,const string substr)
{
    	string::size_type pos;
	size_t len=substr.length();
    	
	while(1)
	{
		pos=str.find(substr.c_str());
        	if(string::npos==pos)
            		break;
		else
        		str.erase(pos,len);
    	}
}

int main(int argc, char *argv[])
{
	if(argc<2)
	{
		fprintf(stderr,"Usage %s [url] [xpath]\n",argv[0]);
		return -1;
	}

	//string strUrl="http://www.163.com";
	string strUrl=argv[1];
	CHttpURL convert(strUrl);

	CUrlDownload loader;
	if(!loader.Download(strUrl.c_str(),50))
    	{
	    	fprintf(stderr,"Download URL Error:%s\n",strUrl.c_str());
        	return -1;
    	}

//	cout<<"+++++++++++++++++++++++++++++++++++++++"<<endl;
//	cout<<loader.m_strContent<<endl;
//	cout<<"+++++++++++++++++++++++++++++++++++++++"<<endl;

	const char *refresh_regex="<meta .*?=\"refresh\".*content.*?\">";
	string stmp=RegexAPI::RegexHelper::Match(loader.m_strContent.c_str(),refresh_regex);
	if(!stmp.empty() && stmp.length()<1024)
	{
		string stmp1 = stmp;
		size_t nPos1 = stmp1.find("url=") + strlen("url=");
		size_t nPos2 = stmp1.find("\"", nPos1);
		string stmpUrl = stmp.substr(nPos1, nPos2 - nPos1);
		string newurl=convert.toAbsolute(stmpUrl);

		loader.m_strContent.clear();
		if(!loader.Download(newurl.c_str(),50))
        	{
                	fprintf(stderr,"Download URL Error:%s\n",strUrl.c_str());
                	return -1;
        	}
	}
	

	string page=loader.m_strContent;
	if(page.empty()) 
	{
		fprintf(stderr,"Page Content Empty!\n");
		return -1;
	}
	
	DeleteSubString(page,"&nbsp;");
	if(!HtmlToXml(page,true))
	{
		fprintf(stderr,"Convert html to xml failed!\n");
		return -1;
	}

	//xmlChar* xpath_exp = BAD_CAST("//div[@id='syncad_0']/ul/li/a");
	xmlChar* xpath_exp = (xmlChar*)(argv[2]);
	xmlDocPtr doc = xmlParseMemory(page.c_str(),page.length());
	xmlXPathObjectPtr result = getXPathObjectPtr(doc, xpath_exp);
	if(NULL==result)
	{
		fprintf(stderr,"Extract by no tidy html!\n");

		if(NULL!=doc) xmlFreeDoc(doc);

		page.clear();		
		page=loader.m_strContent;
		if(!HtmlToXml(page,false))
		{
			fprintf(stderr,"Convert html to xml failed!\n");
			xmlCleanupParser();
			return -1;
		}

		doc = xmlParseMemory(page.c_str(),page.length());
		result = getXPathObjectPtr(doc, xpath_exp);
		if(NULL==result)
		{
			fprintf(stderr,"Xpath extract nothing!\n");
			xmlCleanupParser();
			return -1;
		}
	}

	xmlNodeSetPtr nodeset = result->nodesetval;
	for(int i = 0; i < nodeset->nodeNr; i++)
	{
		xmlNodePtr cur = nodeset->nodeTab[i];
		char* url=(char*)xmlGetProp(cur, (const xmlChar*)"href");
		char* txt=(char *)xmlNodeGetContent(cur);
		if(NULL==url || strlen(url)<4)
			continue;

		string newurl=convert.toAbsolute(url);
		fprintf(stdout,"%s[%s]\n",newurl.c_str(),txt);
	}
		
	xmlXPathFreeObject(result);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	
	return 0;
}
