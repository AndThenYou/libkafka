// demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "../include/Kafka.h"

#include <iostream>
#include <string>
#include <Windows.h>

using std::string;

int cnt = 0;

void ConsumeMessage(string & strTopic, string& strMessage, void *pUserData)
{
	std::cout << "topic:" << strTopic << " message:" << strMessage << std::endl;
}

void TestCosunmer()
{
	//��ʼ������
	JZ::Init();
	//��ȡconsumer
	JZ::IConsumer * consumer = JZ::CreateConsumer();
	//��broker��groupid���ص�����
	consumer->Start("localhost", "222", ConsumeMessage);
	//׷�Ӷ�������
	consumer->AddSubscribe("test");
	
	//enter to exit
	getchar();

	//exit
	consumer->Stop();

	//��broker��groupid���ص�����
	consumer->Start("localhost", "222", ConsumeMessage);
	//׷�Ӷ�������
	consumer->AddSubscribe("test");

	//enter to exit
	getchar();

	//exit
	consumer->Stop();
	delete consumer;
	//����ʼ��
	JZ::Uninit();
}

void TestProducer()
{
	//��ʼ������
	JZ::Init();
	//��ȡproducer
	JZ::IProducer * producer = JZ::CreateProducer();
	//��broker��topic
	producer->Start("localhost", "test");
	while (true)
	{
		string strLine;
		std::getline(std::cin, strLine);
		if (strLine == "q")
		{
			//enter "q" to exit
			break;
		}
		//������Ϣ
		producer->Produce(strLine);
	}
	producer->Stop();
	delete producer;
	//����ʼ��
	JZ::Uninit();
}

int main()
{
	//TestCosunmer();
	TestProducer();
	system("pause");
    return 0;
}

