/*注意事项:
初始运行请修改main函数里的infile_bsdl路径，dir路径，
以及connenct_database.cpp文件内process_database中连接数据库的用户名与密码（为自己电脑中的用户名与密码）*/




#include"cbsdlreader.h"
#include "connect_database.h"
#include "ReadProtelNetList.h"
#include "ReadCadenceProtelNetList.h"
#include "ReadNetListContext.h"
#include<stack>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include<stdio.h>
#include <string>
#include <list>
#include<cctype>
#include <winsock.h>
#include <mysql.h>
#pragma comment(lib,"libmySQL.lib")  //要加不然会编译出错
using namespace std;
string BSDL_Name(const string& str);
int main()
{
	bool execute = 0;//0代表故障注入解析,1代表故障注入直接采用主程序数据库*************************************
	vector<vector<string>>Net;//界面传入二维vector,不限个数,每个一维vector代表一次故障注入,一维vector内的元素仅能1个或两个*********************************************************
	//一个元素必须为可测网络名(一个网络名包含两个引脚),默认执行短路故障注入;两个元素为悬空网络名(每个网络名下仅一个引脚),默认执行断路故障注入;
	string dir = "C:/Users/changeneversay/Desktop/BSDL/BScan_Demo/BScan_Demo.NET";//无论如何都需要修改路径********************
	string Netlist_name = "BScan_Demo.NET";//传递网表名字,带后缀******************
	string password = "change";//需要修改密码*********************************************************************
	string BSDL_dir = "C:/Users/changeneversay/Desktop/BSDL/BS-file/bsdl_file/XC18V256_PC20.bsdl";//若execute = 0,需要修改路径保证程序正确&&&&&
	ifstream inFile_BSDL(BSDL_dir);

	string BSDL_name = BSDL_Name(BSDL_dir);//根据路径获取边界扫描名称，用于创建 连接数据库  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	vector<string>test1{ "NetJ2_16" };
	vector<string>test2{ "NetU2_16"};
	Net.push_back(test1);
	Net.push_back(test2);
	MyDataBase m;
	if (execute == 0)
	{
		if (!inFile_BSDL)
		{
			cout << "打开边界扫描文件失败！" << endl; 
		}
		ostringstream temp;
		temp << inFile_BSDL.rdbuf();
		string BsdlFileContent = temp.str();
		CBsdlReader* reader = new CBsdlReader;
		reader->ProcessBsdlFile(BsdlFileContent);//执行BSDL文件解析


		string type = "DefaultType";
		ReadNetListContext* p = new ReadNetListContext(type, dir);
		ReadNetList* p0 = p->getNetListp();      // 指向 解析相应网表的派生类 的基类指针
		auto netNum = p0->getNetNum();
		cout << netNum << endl;
		auto compoNum = p0->getComponentNum();
		cout << compoNum << endl;

		vector<vector<string>> netListInfo = p0->getNetListInfo();         // 一个网络全部信息存在一个vector<string>中
		vector<vector<string>> componentInfo = p0->getComponentInfo();       // 存放器件名称及类型
		auto c = reader->Get_BRdata();//获取BR数据
		auto a = reader->Get_portdata();
		auto b = reader->Get_constantdata();

		//建立数据库 将数据存入数据库
		string y = m.Process_database(password, a, b, c, BSDL_name, netListInfo, componentInfo);
		m.Process_Chain(y, password);
		string database2 = y + "_BSDL_Fault_Injection";
		ifstream inFile_Netlist(dir);
		ostringstream temp_Netlist;
		temp_Netlist << inFile_Netlist.rdbuf();
		string Netlist_FileContent = temp_Netlist.str();//读取网表的内容

		//m.Process_Fault_injection(password,dir, Netlist_name ,database2, Netlist_FileContent, Net);//故障注入

		delete reader;
		reader = nullptr;
		delete p;
		p = nullptr;
		p0 = nullptr;
		//system("pause");
		return 0;
	}
	else
    {
		string database1 = BSDL_name +  "_BSDL_DATA";
		ifstream inFile_Netlist(dir);
		ostringstream temp_Netlist;
		temp_Netlist << inFile_Netlist.rdbuf();
		string Netlist_FileContent = temp_Netlist.str();//读取网表的内容
        m.Process_Fault_injection(password, dir, Netlist_name,database1, Netlist_FileContent, Net);
		//system("pause");
		return 0;
	}
	
}
string BSDL_Name(const string& str)
{
	stack<char>sta;
	for (auto it = str.end() - 1; it != str.begin(); --it)
	{
		if (*it == '.')
		{
			--it;
			while (*it != '/')
			{
				sta.push(*it);
				--it;
			}
			break;
		}
	}
	string s;
	while (!sta.empty())
	{
		s.push_back(sta.top());
		sta.pop();
	}
	return s;
}