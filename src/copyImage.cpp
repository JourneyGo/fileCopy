//============================================================================
// Name        : TestDebugMode.cpp
// Author      : Gaoqi
// Version     :
// Copyright   : Enjoy
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;

string source_dpth("/");
string target_dpth( "~/");
string g_image_format ( ".png");
string prefix("");
string appendix("_left");
int numOfbase = 0;
int g_counter = 0;
int numlength=0;//num of index
int init_num = 0;
int interval = 10;
int addition = 100;
int baseStart = 0;
vector<string > ban_word;

const string getImageFileName(int *pbase, int *pcounter, const string targetPath)
{
	std::stringstream ss;
	string ret, base_pre, pre;

	int counter = *pcounter;
	int base = *pbase;

	for(int i=1; i<numlength; i++)
	{
		int upper_bound = pow(10,i);
		if(counter < upper_bound)
		{
			string temp(numlength-i,'0');
			pre = temp;
			break;
		}
	}
	if(counter > pow(10,numlength))
	{
		*pcounter = init_num;
		*pbase = base+1;
		base = *pbase;
	}
	for(int i=1; i<numOfbase; i++)
	{
		int upper_bound = pow(10,i);
		if(base < upper_bound)
		{
			string temp(numOfbase-i,'0');
			base_pre = temp;
			break;
		}
	}

	ss << targetPath << "/" << prefix << base_pre << base << "_" << pre << counter << appendix << g_image_format;

	ret = ss.str().c_str();
	ss.str("");

	return ret;

}

string getImageFileName(string targetPath, int counter)
{
	std::stringstream ss;
	string ret, pre;

	for(int i=1; i<numlength; i++)
	{
		int upper_bound = pow(10,i);
		if(counter < upper_bound)
		{
			string temp(numlength-i,'0');
			pre = temp;
			break;
		}
	}

	ss << targetPath << "/" << prefix << pre << counter << appendix << g_image_format;

	ret = ss.str().c_str();
	ss.str("");

	return ret;
}

int judge(char * fileName)
{
	if(numOfbase >0  && prefix.length()>numOfbase+1){
		char nn[10] = {'\0'};
		strncpy(nn,fileName+prefix.length()-numOfbase-1,numOfbase);
		for(int i = 0; i< numOfbase; i++)
		{
			if(nn[i]<48 || nn[i]>57)//not a number
				return -1;
			prefix = prefix.substr(0,prefix.length()-numOfbase-1);
			return 0;
		}
	}
	return -1;
}

void copyImageFromName(char* fileName)
{
	g_counter = init_num;
	std::string source;
	std::string target;

	int breakCounter = 0;

	int base = judge(fileName);

	if(base>=0) base = baseStart;

	while(breakCounter<interval){
		if(base >= 0)
		{
			source = getImageFileName(&base, &g_counter, source_dpth);
			target = getImageFileName(&base, &g_counter, target_dpth);
		}else{
			source = getImageFileName(source_dpth, g_counter);
			target = getImageFileName(target_dpth, g_counter);
		}
		cv::Mat temp = cv::imread(source);
		if(temp.data != NULL)
		{
			cout << g_counter << endl;
			cv::imwrite(target, temp);
			breakCounter = 0;
		}
		breakCounter ++;
		g_counter += addition;
	}
	cout << "Copy folder done!" << endl;
}

int isType(char* name)
{
	int length = strlen(name);
	if(length>g_image_format.length() && strcmp(name+length-g_image_format.length(), g_image_format.data())==0)
	{
		return 1;
	}else
		return 0;
}

bool isValidPath(char* path)
{
	string strToTest(path);
	vector<string >::iterator it = ban_word.begin();
	for(it = ban_word.begin(); it != ban_word.end(); it++)
	{
		int idx = strToTest.find(*it);
		if(idx != -1)
		{
			return false;
		}
	}
	return true;
}

int getNameType(char* name)
{
	string strName(name);
	int length = strlen(name);
	int appendixlength = appendix.length();
	int lc = strName.find(appendix);
	if((lc>0&&lc-(length-appendixlength-g_image_format.length())==0))
	{
		prefix.clear();
		prefix.insert(0,name,length-numlength-appendixlength-g_image_format.length());
		return 1;
	}
	if(appendixlength>0)
		return -1;
	prefix.clear();
	if(length-4-5>0)
		prefix.insert(0,name,length-g_image_format.length()-numlength);
	return 0;
}

void listDir(char *path)
{
	DIR              *pDir ;
	struct dirent    *ent  ;
	char              childpath[512];

	pDir=opendir(path);
	memset(childpath,0,sizeof(childpath));

	while((ent=readdir(pDir))!=NULL)
	{
		if(ent->d_type & DT_DIR)
		{
			if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
					continue;

			sprintf(childpath,"%s/%s",path,ent->d_name);
			if(isValidPath(childpath));
				listDir(childpath);
		}
		else
		{
			if(isType(ent->d_name)==1)
			{
				if(isValidPath(path))
				{
					source_dpth.clear();
					source_dpth = path;
					int nameType = getNameType(ent->d_name);
					if(nameType < 0) continue;
					cout << "Into path：" << source_dpth << endl;
					copyImageFromName(ent->d_name);
					return;
				}
			}
		}
	}
}

int copyImageThroughTransferFile()
{
	char temp[512] = {'\0'};
	strcpy(temp,source_dpth.data());
	listDir(temp);
	return 0;
}

void printHelp()
{
	cout << "图片选择拷贝工具：用于提取文件夹中名称为特定数字的文件 "<< endl;
	cout << "		至少提供要扫描的起始位置，存储的目标位置" << endl;
	cout << "	-source 		起始扫描位置" << endl;
	cout << "	-target 		目标位置" << endl;
	cout << "	-format 		默认（.png）" << endl;
	cout << "	-init_Idx 		文件起始索引数" << endl;
	cout << "	-num_length 		文件夹中数字的位数" << endl;
	cout << "	-interval		文件数字间可能有的最大间隔" << endl;
	cout << "	-addition		文件数字递增量" << endl;
	cout << "	-ban_word 		要跳过含有某个关键词的文件夹，可以用这个，每个词请重新打一边这个参数" << endl;
	cout << "	-baselength		如果数字前还有基本数字 Eg：something_876_12345_something.png 这个值应该是 3" << endl;
	cout << "	-basebeginning		基本数字的起始值 Eg：876" << endl;
}

void usingDefault()
{
	source_dpth = "/home/gaoqi/server/vela_data/40cm_baseline/20150312/back/tmp/RAW_back1/all00_20150212_145626";
	target_dpth = "/home/gaoqi/workspace/Data/test";
	g_image_format = ".png";
	prefix = "";
	appendix = "_left";
	numOfbase = 0;
	numlength=5;
	init_num = 0;
	interval = 10;
	addition = 100;
	ban_word.push_back("vo");
	ban_word.push_back("chessboard");
	ban_word.push_back("AE");
}

int main(int argc, char** argv) {
	if(argc < 3 || argc%2 == 0)
	{
		cout << "参数输入错误！"<< endl;
		printHelp();
		return -1;
	}
	usingDefault();
	for(int i = 1; i < argc; i += 2)
	{
		if(strcmp(argv[i],"-format")==0)
		{
			g_image_format = argv[i+1];
			continue;
		}
		if(strcmp(argv[i],"-prefix")==0)
		{
			prefix = argv[i+1];
			continue;
		}
		if(strcmp(argv[i],"-source")==0)
		{
			source_dpth = argv[i+1];
			continue;
		}
		if(strcmp(argv[i],"-target")==0)
		{
			target_dpth = argv[i+1];
			continue;
		}
		if(strcmp(argv[i],"-ban_word")==0)
		{
			ban_word.push_back(string(argv[i+1]));
			continue;
		}
		if(strcmp(argv[i],"-num_length")==0)
		{
			numlength = atoi(argv[i+1]);
			continue;
		}
		if(strcmp(argv[i],"-init_Idx")==0)
		{
			init_num = atoi(argv[i+1]);
			continue;
		}
		if(strcmp(argv[i],"-interval")==0)
		{
			interval = atoi(argv[i+1]);
			continue;
		}
		if(strcmp(argv[i],"-addition")==0)
		{
			addition = atoi(argv[i+1]);
			continue;
		}
		if(strcmp(argv[i],"-baselength")==0)
		{
			numOfbase = atoi(argv[i+1]);
			continue;
		}
		if(strcmp(argv[i],"-basebeginning")==0)
		{
			baseStart = atoi(argv[i+1]);
			continue;
		}
		if(strcmp(argv[i],"-param")==0)
		{
			if(strcmp(argv[i+1],"default") == 0)
				usingDefault();
			break;
		}
	}
	copyImageThroughTransferFile();
	cout << "Done!" << endl;
	return 0;
}
