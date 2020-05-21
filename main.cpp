/*ע������:
��ʼ�������޸�main�������infile_bsdl·����dir·����
�Լ�connenct_database.cpp�ļ���process_database���������ݿ���û��������루Ϊ�Լ������е��û��������룩*/




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
#pragma comment(lib,"libmySQL.lib")  //Ҫ�Ӳ�Ȼ��������
using namespace std;
string BSDL_Name(const string& str);
int main()
{
	bool execute = 0;//0�������ע�����,1�������ע��ֱ�Ӳ������������ݿ�*************************************
	vector<vector<string>>Net;//���洫���άvector,���޸���,ÿ��һάvector����һ�ι���ע��,һάvector�ڵ�Ԫ�ؽ���1��������*********************************************************
	//һ��Ԫ�ر���Ϊ�ɲ�������(һ��������������������),Ĭ��ִ�ж�·����ע��;����Ԫ��Ϊ����������(ÿ���������½�һ������),Ĭ��ִ�ж�·����ע��;
	string dir = "C:/Users/changeneversay/Desktop/BSDL/BScan_Demo/BScan_Demo.NET";//������ζ���Ҫ�޸�·��********************
	string Netlist_name = "BScan_Demo.NET";//������������,����׺******************
	string password = "change";//��Ҫ�޸�����*********************************************************************
	string BSDL_dir = "C:/Users/changeneversay/Desktop/BSDL/BS-file/bsdl_file/XC18V256_PC20.bsdl";//��execute = 0,��Ҫ�޸�·����֤������ȷ&&&&&
	ifstream inFile_BSDL(BSDL_dir);

	string BSDL_name = BSDL_Name(BSDL_dir);//����·����ȡ�߽�ɨ�����ƣ����ڴ��� �������ݿ�  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	vector<string>test1{ "NetJ2_16" };
	vector<string>test2{ "NetU2_16"};
	Net.push_back(test1);
	Net.push_back(test2);
	MyDataBase m;
	if (execute == 0)
	{
		if (!inFile_BSDL)
		{
			cout << "�򿪱߽�ɨ���ļ�ʧ�ܣ�" << endl; 
		}
		ostringstream temp;
		temp << inFile_BSDL.rdbuf();
		string BsdlFileContent = temp.str();
		CBsdlReader* reader = new CBsdlReader;
		reader->ProcessBsdlFile(BsdlFileContent);//ִ��BSDL�ļ�����


		string type = "DefaultType";
		ReadNetListContext* p = new ReadNetListContext(type, dir);
		ReadNetList* p0 = p->getNetListp();      // ָ�� ������Ӧ����������� �Ļ���ָ��
		auto netNum = p0->getNetNum();
		cout << netNum << endl;
		auto compoNum = p0->getComponentNum();
		cout << compoNum << endl;

		vector<vector<string>> netListInfo = p0->getNetListInfo();         // һ������ȫ����Ϣ����һ��vector<string>��
		vector<vector<string>> componentInfo = p0->getComponentInfo();       // ����������Ƽ�����
		auto c = reader->Get_BRdata();//��ȡBR����
		auto a = reader->Get_portdata();
		auto b = reader->Get_constantdata();

		//�������ݿ� �����ݴ������ݿ�
		string y = m.Process_database(password, a, b, c, BSDL_name, netListInfo, componentInfo);
		m.Process_Chain(y, password);
		string database2 = y + "_BSDL_Fault_Injection";
		ifstream inFile_Netlist(dir);
		ostringstream temp_Netlist;
		temp_Netlist << inFile_Netlist.rdbuf();
		string Netlist_FileContent = temp_Netlist.str();//��ȡ���������

		//m.Process_Fault_injection(password,dir, Netlist_name ,database2, Netlist_FileContent, Net);//����ע��

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
		string Netlist_FileContent = temp_Netlist.str();//��ȡ���������
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