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

string source_dpth("/home/gaoqi/server/vela_data/vehRecordData/Img_0/REC00");
string target_dpth( "/home/gaoqi/workspace/Data/forLabel_4/REC00");
string g_image_format ( ".jpg");
string prefix;


const string getImageFileName(int *base, int *pcounter, const string file_path_name, const string image_format)
{
	std::stringstream ss;
	string ret, prefix;

	int counter = *pcounter;
	if(counter >= 0 && counter < 10)
	{
		prefix = "0000";
	}
	else if(counter >= 10 && counter < 45)
	{
		prefix = "000";
	}
	else if(counter == 45)
	{
		prefix = "0000";
		*pcounter = 0;
		(*base)+=1;
	}
	if(*base<100)
		ss << file_path_name << '0' << *base << "_" << prefix << *pcounter << image_format;
	else
		ss << file_path_name << *base << "_" << prefix << *pcounter << image_format;

	ret = ss.str().c_str();
	ss.str("");

	return ret;

}

const string getImageFileName(int counter, const string file_path_name, const string image_format)
{
	std::stringstream ss;
	string ret, prefix;

	if(counter >= 0 && counter < 10)
	{
		prefix = "0000";
	}
	else if(counter >= 10 && counter < 100)
	{
		prefix = "000";
	}
	else if(counter >= 100 && counter < 1000)
	{
		prefix = "00";
	}
	else if(counter >= 1000 && counter < 10000)
	{
		prefix = "0";
	}
	else
	{
		prefix = "";
	}

	ss << file_path_name << prefix << counter << image_format;

	ret = ss.str().c_str();
	ss.str("");

	return ret;

}

string getImageFileName(string targetPath, int counter)
{
	std::stringstream ss;
	string ret, pre;

	if(counter >= 0 && counter < 10)
	{
		pre = "0000";
	}
	else if(counter >= 10 && counter < 100)
	{
		pre = "000";
	}
	else if(counter >= 100 && counter < 1000)
	{
		pre = "00";
	}
	else if(counter >= 1000 && counter < 10000)
	{
		pre = "0";
	}
	else
	{
		pre = "";
	}

	ss << targetPath << prefix << pre << counter << g_image_format;

	ret = ss.str().c_str();
	ss.str("");

	return ret;
}

void copyImageFromName(int nameType)
{
	int base = 10;
	int g_counter = 0;
	std::string source;
	std::string target;

//	if(nameType == 1)
//	{
//		g_image_format = "_left.png";
//	}else{
//		if(nameType != 0)
//		{
//			return;
//		}
//	}

	int breakCounter = 0;

	while(breakCounter<100){
		if(base >= 0)
		{
			source = getImageFileName(&base, &g_counter, source_dpth, g_image_format);
			target = getImageFileName(&base, &g_counter, target_dpth, g_image_format);
		}else{
			source = getImageFileName(g_counter, source_dpth, g_image_format);
			target = getImageFileName(g_counter, target_dpth, g_image_format);
		}
//		source = getImageFileName(source_dpth, g_counter);
//		target = getImageFileName(target_dpth, g_counter);
		cv::Mat temp = cv::imread(source);
		if(temp.data != NULL)
		{
			cout << g_counter << endl;
			cv::imwrite(target, temp);
			breakCounter = 0;
		}
		breakCounter ++;
		g_counter += 45;
	}
	cout << "Copy done!" << endl;
}

int isPNG(char* name)
{
	int length = strlen(name);
	if(length>4 && strcmp(name+length-4,".png")==0)
	{
		return 1;
	}else
		return 0;
}

bool isValidPath(char* path)
{
	string strToTest(path);
	if(strToTest.find("chessboard")!=-1 || strToTest.find("vo")!=-1 || strToTest.find("lane")!=-1)
		return false;
	else
		return true;
}

int getNameType(char* name)
{
	string strName(name);
	int length = strlen(name);
	if(strName.find(".png")==length-4)
	{
		int lc = strName.find("_left");
		int rc = strName.find("_right");
		if((lc>0&&lc-(length-9)==0) || (rc>0&&rc-(length-10)==0))
		{
			prefix.clear();
			if(lc-(length-9)==0)
			{
				prefix.insert(0,name,length-9-5);
			}
			if(rc-(length-10)==0)
			{
				prefix.insert(0,name,length-10-5);
			}
			return 1;
		}
		prefix.clear();
		if(length-4-5>0)
			prefix.insert(0,name,length-4-5);
		return 0;
	}
	return -1;
}

void listDir(char *path)
{
        DIR              *pDir ;
        struct dirent    *ent  ;
        int               i=0  ;
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
					if(isPNG(ent->d_name)==1)
					{
                        if(isValidPath(path))
                        {
                        	source_dpth.clear();
                        	source_dpth = path;
                        	source_dpth += "/";
                        	int nameType = getNameType(ent->d_name);
                        	cout << "Into path：" << source_dpth << ent->d_name << endl;
                        	copyImageFromName(nameType);
                        	return;
                        }
					}
				}
        }

}

int copyImageThroughTransferFile()
{
        listDir("/home/gaoqi/server/vela_data");
        return 0;
}

int main() {
	copyImageFromName(1);
//	copyImageThroughTransferFile();
	cout << "Done!" << endl;
	return 0;
}
