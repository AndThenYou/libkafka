// demo.cpp : 定义控制台应用程序的入口点。
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
	//初始化环境
	JZ::Init();
	//获取consumer
	JZ::IConsumer * consumer = JZ::CreateConsumer();
	//绑定broker、groupid、回调函数
	consumer->Start("localhost", "222", ConsumeMessage);
	//追加订阅主题
	consumer->AddSubscribe("test");
	
	//enter to exit
	getchar();

	//exit
	consumer->Stop();

	//绑定broker、groupid、回调函数
	consumer->Start("localhost", "222", ConsumeMessage);
	//追加订阅主题
	consumer->AddSubscribe("test");

	//enter to exit
	getchar();

	//exit
	consumer->Stop();
	delete consumer;
	//反初始化
	JZ::Uninit();
}

void TestProducer()
{
	//初始化环境
	JZ::Init();
	//获取producer
	JZ::IProducer * producer = JZ::CreateProducer();
	//绑定broker、topic
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
		//发布消息
		producer->Produce(strLine);
	}
	producer->Stop();
	delete producer;
	//反初始化
	JZ::Uninit();
}

int main()
{
	//TestCosunmer();
	TestProducer();
	system("pause");
    return 0;
}

