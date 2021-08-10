#include "Producer.h"
#include "fnlog/include/fn_macro.h"

Producer::Producer()
	: m_csEventCb(this)
	, m_pcsGlobalConf(nullptr)
	, m_pcsTopicConf(nullptr)
	, m_pcsProducer(nullptr)
	, m_pcsTopic(nullptr)
{
}

Producer::~Producer()
{
	if (m_pcsGlobalConf)
		delete m_pcsGlobalConf;
	if (m_pcsTopicConf)
		delete m_pcsTopicConf;
	if (m_pcsProducer)
		delete m_pcsProducer;
	if (m_pcsTopic)
		delete m_pcsTopic;
}

JZ::KafkaErrorCode Producer::Start(const string & strBroker, const string & strTopic)
{
	string strErr;
	m_strBroker = strBroker;
	m_nPartition = RdKafka::Topic::PARTITION_UA;
	m_strTopic = strTopic;

	m_pcsGlobalConf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	m_pcsTopicConf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

	if (m_pcsGlobalConf == nullptr || m_pcsTopicConf == nullptr)
	{
		LogError() << "Create RdKafka::Conf fail.";
		return JZ::E_UNKNOW_ERROR;
	}
	RdKafka::Conf::ConfResult res;
	res = m_pcsTopicConf->set("partitioner_cb", &m_csHashPartitionerCb, strErr);
	if (res != RdKafka::Conf::CONF_OK)
	{
		LogError() << "Set partitioner_cb fail.";
		return JZ::E_UNKNOW_ERROR;
	}

	res = m_pcsGlobalConf->set("metadata.broker.list", m_strBroker, strErr);
	if (RdKafka::Conf::ConfResult::CONF_OK != res)
	{
		LogError() << "Set metadata.broker.list fail.";
		return JZ::E_UNKNOW_ERROR;
	}
	res = m_pcsGlobalConf->set("event_cb", &m_csEventCb, strErr);
	if (RdKafka::Conf::ConfResult::CONF_OK != res)
	{
		LogError() << "Set event_cb fail.";
		return JZ::E_UNKNOW_ERROR;
	}
	res = m_pcsGlobalConf->set("dr_cb", &m_csDeliveryReportCb, strErr);
	if (RdKafka::Conf::ConfResult::CONF_OK != res)
	{
		LogError() << "set dr_cb fail.";
		return JZ::E_UNKNOW_ERROR;
	}

	m_pcsProducer = RdKafka::Producer::create(m_pcsGlobalConf, strErr);
	if (m_pcsProducer == nullptr)
	{
		LogError() << "Create RdKafka::Producer fail.";
		return JZ::E_UNKNOW_ERROR;
	}
	m_pcsTopic = RdKafka::Topic::create(m_pcsProducer, strTopic, m_pcsTopicConf, strErr);
	if (m_pcsTopic == nullptr)
	{
		LogError() << "Create RdKafka::Topic fail.";
		return JZ::E_UNKNOW_ERROR;
	}
	LogInfo() << "Init success.";
	return JZ::E_NO_ERROR;
}

JZ::KafkaErrorCode Producer::Produce(const string & strMessage)
{
	if (m_pcsProducer == nullptr || m_pcsTopic == nullptr)
	{
		LogError() << "Can't create producer or topic, produce message fail.";
		return JZ::E_UNKNOW_ERROR;
	}
	string key = strMessage.substr(0, 5);//根据line前5个字符串作为key值
	RdKafka::ErrorCode resp =
		m_pcsProducer->produce(m_pcsTopic, m_nPartition,
			RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
			const_cast<char *>(strMessage.c_str()), strMessage.size(),
			key.c_str(), key.size(), NULL);
	if (resp != RdKafka::ERR_NO_ERROR)
		LogError() << "Produce failed: " << RdKafka::err2str(resp);
	else
		LogInfo() << "Produced message (" << strMessage.size() << " bytes)";
	m_pcsProducer->poll(0);
	return JZ::E_NO_ERROR;
}

void Producer::Stop()
{

}