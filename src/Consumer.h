#pragma once

#include "rdkafkacpp.h"
#include "EventCb.h"
#include "Kafka.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <thread>
#include <vector>

using std::string;
using std::vector;

typedef void(*ConsumerMessageCb)(string & strTopic, string& strMessage, void *pUserData);


class ConsumeCB : public RdKafka::ConsumeCb
{
public:
	void consume_cb(RdKafka::Message &msg, void *opaque);
};

class Consumer : public JZ::IConsumer
{
public:
	Consumer();
	~Consumer();

	virtual JZ::KafkaErrorCode Start(const string & strBroker, const string & strGroupId, 
		ConsumerMessageCb fnCb, void* pUserData = nullptr);
	
	virtual JZ::KafkaErrorCode AddSubscribe(const string & strTopic);

	virtual void Stop();

	static void MessageConsume(RdKafka::Message* message, void* opaque);

private:
	static void ThreadEntry(void * pUserData);
	void Run();

private:
	string m_strBroker;
	string m_strTopic;
	vector<string> m_vecTopics;
	string m_strGroup;

	RdKafka::Conf * m_pcsGlobalConf;
	RdKafka::Conf * m_pcsTopicConf;
	ConsumeCB m_csConsumeCb;
	EventCb m_csEventCb;

	ConsumerMessageCb m_fnMessageCb;
	void* m_fnUserData;

	RdKafka::KafkaConsumer * m_pcsConsumer;

	std::thread m_thread;
	bool m_bRunning;
};