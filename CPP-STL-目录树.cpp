#include "stdafx.h"
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <regex>
#include <io.h>
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
using namespace std;
string getExtend(string name);
void printExtend(map<string, int>* map);
struct Node;
struct Node{
	string name;//文件名字
	string path;//文件路径
	list<Node> child;//孩子节点
	int number = 0;//定义层数 打印的空格的个数
	string extend="*";//文件扩展名
};//创建B树
//获取文件大小
double getFileSize(string FileName) {
	string name = FileName;
	double size;
	struct _stat info;
	_stat(name.data(), &info);
	size = info.st_size;
	return size;
}
double getFileSizeWithSize(string FileName, __time64_t *ct, __time64_t *at,__time64_t * t) {
	string name = FileName;
	double size;
	struct _stat info;
	_stat(name.data(), &info);
	size = info.st_size;
	*ct = info.st_ctime;
	*at = info.st_atime;
	*t = info.st_mtime;
	return size;
}
//获取当前文件名
void string_replace(string &strBig, const string &strsrc, const string &strdst) {
	string::size_type pos = 0;
	string::size_type srclen = strsrc.size();
	string::size_type dstlen = strdst.size();
	while ((pos = strBig.find(strsrc, pos)) != string::npos) {
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}
string GetPathName(string strFullNmae) {
	if (strFullNmae.empty()) {
		return "";
	}
	string_replace(strFullNmae, "/", "\\");
	string::size_type iPos = strFullNmae.find_last_of("\\") + 1;
	return strFullNmae.substr(iPos, strFullNmae.length() - iPos);
}
list<Node> myTree(string path, list<Node> q, int number) {
	list<Node> mytree = q;
	//Node * myNode;
	long hFile = 0;
	struct _finddata_t fileinfo;
	string pstr;
	if ((hFile = _findfirst(pstr.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
		//第一次查找
	{
		//第二层节点
		list<Node> qone;
		Node * pone;
		do {
			if ((fileinfo.attrib&_A_SUBDIR))//如果是文件夹
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)  //进入文件夹查找  
				{
					pone = new Node();
					pstr = pstr.assign(path).append("\\").append(fileinfo.name);
					pone->name = GetPathName(pstr);
					pone->path = pstr;
					pone->number = number;
					qone.push_back(*pone);

					qone = myTree(pstr, qone, number + 1);
				}
			}
			else //如果查找到的不是是文件夹   
			{
				//cout << fileinfo.name << endl;
				pone = new Node();
				pstr = pstr.assign(path).append("\\").append(fileinfo.name);
				pone->name = GetPathName(pstr);//获取当前文件名
				pone->path = pstr;//存入当前路径
				pone->number = number + 1;
				pone->extend = getExtend(pone->name); 
				qone.push_back(*pone);
				//files.push_back(p.assign(fileinfo.name));  //将文件路径保存，也可以只保存文件名:    p.assign(path).append("\\").append(fileinfo.name)  
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		mytree.back().child = qone;
		_findclose(hFile); //结束查找  
	}
	return mytree;
}
list<Node> getTree(string path,list<Node> q,int number) {//传入初始路径
	list<Node> qq = q;
	Node * p;
	struct _stat filestat;
	if ((_stat(path.c_str(), &filestat) == 0) && (filestat.st_mode & _S_IFDIR)) {
		/*	如果当前路径是一个文件夹
			将文件夹存入，遍历文件夹内文件
		*/
		//cout << "是目录" << endl;
		p = new Node();
		p->name = GetPathName(path);//获取当前文件名
		p->path = path;//存入当前路径
		p->number = number;
		qq.push_back(*p);//当前路径入队
		qq =myTree(path, qq, number + 1);
		
	 }
	else {
		/*
			如果不是文件夹
		*/
		p = new Node();
		p->name = GetPathName(path);
		p->path = path;
		p->number = number;
		p->extend = getExtend(p->name);
		qq.push_back(*p);
	}
	return qq;
 }
//输出树形结构
void printTree(list<Node> tree,map<string,int>* maap) {
	list<Node>::iterator itor;
	list<Node> myTree = tree;
	map<string, int> mymap = *maap;
	itor = myTree.begin();
	while (itor != myTree.end()) {
		if ((*itor).number > 0) {
			for (int i = 0; i < (*itor).number - 1; i++)
				cout << "     ";
			cout << "+-----";
		}
		
		if ((*itor).child.size() != 0) {
			cout << "*";
		}
		cout << (*itor).name;
		//cout << " 扩展：" << (*itor).extend;
		mymap.insert(pair<string, int>((*itor).extend, 1));
		cout << endl;
		if ((*itor).child.size()!=0) {
			printTree((*itor).child,&mymap);
		}
		*itor++;
	}
	*maap = mymap;
}
//获取文件扩展名
string getExtend(string name) {
	//string ext = "*.";
	string ext2;
	//r("(([a-zA-Z0-9])+)\\.(([a-zA-Z])+)");输出3
	regex r("(((\\w)|(\\W))*)(\\.)([a-zA-Z]+)");
	if (regex_match(name, r)) {
		ext2 = regex_replace(name, r, string("$6"));
		//ext = ext + ext2;
	}
	//cout << " 扩展名为：" << ext2 ;
	return ext2;
}
void printExtend(map<string, int>* map) {
	std::map<string, int> mp = (*map);
	std::map<string, int>::iterator iter;
	cout << "后缀" << endl;
	iter = mp.find("*");
	mp.erase(iter);
	for (iter = mp.begin(); iter != mp.end(); iter++) {
		cout << (*iter).first << "   ";
	}
	cout << endl;
}
void findExtend(string ext, list<Node> tree) {
	list<Node>::iterator iter;
	iter = tree.begin();
	while (iter != tree.end()) {
		if ((*iter).extend == ext) {
			cout << "文件名：" << (*iter).name << endl;
			cout<< "目录：" << (*iter).path  << endl;
			cout << "size:" << getFileSize((*iter).path)/1024 <<"KB"<< endl;
			cout << endl;
		}
		if ((*iter).child.size() != 0) {
			findExtend(ext, (*iter).child);
		}
		*iter++;
	}
}
void printFind(list<Node> tree) {
	string ex;
	cout << endl;
	cout << "请输入要查找的后缀名：";
	cin >> ex;
	findExtend(ex, tree);
}
void findExtendWithSize(string ext, list<Node> tree, double min, double max) {
	list<Node>::iterator iter;
	__time64_t ctime;
	__time64_t atime;
	__time64_t mtime;
	iter = tree.begin();
	while (iter != tree.end()) {
		if ((*iter).extend == ext) {
			double size = getFileSizeWithSize((*iter).path,&ctime,&atime,&mtime) / 1024;
			if (size <= max && size >= min) {
				cout << "文件名：" << (*iter).name << endl;
				cout << "目录：" << (*iter).path << endl;
				cout << "size:" << size << "KB" << endl;
				//日期格式化
				tm tm_ctime;
				tm tm_atime;
				tm tm_mtime;
				_localtime64_s(&tm_ctime, &ctime);
				_localtime64_s(&tm_atime, &atime);
				_localtime64_s(&tm_mtime, &mtime);
				cout << "创建时间：";
				cout << tm_ctime.tm_year + 1900 << "-";
				cout << tm_ctime.tm_mon + 1 << "-";
				cout << tm_ctime.tm_mday << "  ";
				cout << tm_ctime.tm_hour << ":";
				cout << tm_ctime.tm_min << ":";
				cout << tm_ctime.tm_sec << endl;
				cout << "修改时间：";
				cout << tm_mtime.tm_year + 1900 << "-";
				cout << tm_mtime.tm_mon + 1 << "-";
				cout << tm_mtime.tm_mday << "  ";
				cout << tm_mtime.tm_hour << ":";
				cout << tm_mtime.tm_min << ":";
				cout << tm_mtime.tm_sec << endl;
				cout << "最近打开时间：";
				cout << tm_atime.tm_year + 1900 << "-";
				cout << tm_atime.tm_mon + 1 << "-";
				cout << tm_atime.tm_mday << "  ";
				cout << tm_atime.tm_hour << ":";
				cout << tm_atime.tm_min << ":";
				cout << tm_atime.tm_sec << endl;
				
				cout << endl;
			}
		}
		if ((*iter).child.size() != 0) {
			findExtendWithSize(ext, (*iter).child,min,max);
		}
		*iter++;
	}
}
void printFindWithSize(list<Node> tree) {
	string ex;
	double min;
	double max;
	cout << endl;
	cout << "请输入要查找的后缀名：";
	cin >> ex;
	cout << "请输入文件大小区间（KB）：";
	cin >> min;
	cout << " --- ";
	cin >> max;
	findExtendWithSize(ex, tree,min,max);
}
int main() {
	list<Node> lis;
	map<string, int> mmap;
	string route;
	//lis = getTree(R"(E:\2017-2018-2-课程文件\计网)",lis,0);//不转义原始字符串
	cout<< "将路径复制于此，不必考虑转义字符" << endl;
	cin >> route;
	lis = getTree(route, lis, 0);
	printTree(lis, &mmap);
	cout << endl;
	while (true) {
		int flag = 0;
		cout << endl;
		cout << "请输入操作数：\n1.输入新路径\n2.打印目录树\n3.打印扩展名\n4.查找扩展名(without size)\n5.查找扩展名(with size)\n6.退出" << endl;
		cin >> flag;
		switch (flag) {
		case 1:
			cout << "将路径复制于此，不必考虑转义字符" << endl;
			cin >> route;
			lis = getTree(route,lis,0);
			printTree(lis, &mmap);
			system("pause");
			break;
		case 2:
			printTree(lis, &mmap);
			system("pause");
			break;
		case 3:
			printExtend(&mmap);
			system("pause");
			break;
		case 4:
			printFind(lis);
			system("pause");
			break;
		case 5:
			printFindWithSize(lis);
			system("pause");
			break;
		case 6:
			return 0;
		}
	}
	cout << "退出" << endl;
	system("pause");
	return 0;
}