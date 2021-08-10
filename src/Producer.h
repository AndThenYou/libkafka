#pragma once
#include "rdkafkacpp.h"
#include "EventCb.h"
#include "Kafka.h"

#include <string>

using std::string;

class DeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
	void dr_cb(RdKafka::Message &message) {
		std::cout << "Message delivery for (" << message.len() << " bytes): " <<
			message.errstr() << std::endl;
		if (message.key())
			std::cout << "Key: " << *(message.key()) << ";" << std::endl;
	}
};

class MyHashPartitionerCb : public RdKafka::PartitionerCb {
public:
	int32_t partitioner_cb(const RdKafka::Topic *topic, const std::string *key, int32_t partition_cnt, void *msg_opaque)
	{
		//std::cout << "partition_cnt=" << partition_cnt << std::endl;
		return djb_hash(key->c_str(), key->size()) % partition_cnt;
	}
private:
	static inline unsigned int djb_hash(const char *str, size_t len)
	{
		unsigned int hash = 5381;
		for (size_t i = 0; i < len; i++)
			hash = ((hash << 5) + hash) + str[i];
		//std::cout << "hash1=" << hash << std::endl;

		return hash;
	}
};

class Producer : public JZ::IProducer
{
public:
	Producer();
	~Producer();

	virtual JZ::KafkaErrorCode Start(const string & strBroker, const string & strTopic);

	virtual JZ::KafkaErrorCode Produce(const string& message);

	virtual void Stop();

private:
	string m_strBroker;
	int m_nPartition;
	string m_strTopic;

	RdKafka::Conf * m_pcsGlobalConf;
	RdKafka::Conf * m_pcsTopicConf;
	EventCb  m_csEventCb;
	DeliveryReportCb  m_csDeliveryReportCb;
	MyHashPartitionerCb m_csHashPartitionerCb;

	RdKafka::Topic * m_pcsTopic;
	RdKafka::Producer * m_pcsProducer;
};
