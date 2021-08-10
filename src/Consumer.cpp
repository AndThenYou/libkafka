#include "Consumer.h"
#include "fnlog/include/fn_macro.h"

static bool exit_eof = true;
static int eof_cnt = 0;
static int partition_cnt = 0;
static int verbosity = 1;

void ConsumeCB::consume_cb(RdKafka::Message & message, void * opaque)
{
	Consumer::MessageConsume(&message, opaque);
}

Consumer::Consumer() 
	:m_bRunning(false)
	,m_csEventCb(this)
	,m_pcsConsumer(nullptr)
	,m_fnMessageCb(nullptr)
	,m_fnUserData(nullptr)
{
	m_pcsGlobalConf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	m_pcsTopicConf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
}

Consumer::~Consumer()
{
	delete m_pcsTopicConf;
	delete m_pcsGlobalConf;
}

void Consumer::MessageConsume(RdKafka::Message* message, void* opaque) 
{
	Consumer* pThis = (Consumer*)opaque;
	switch (message->err()) 
	{
	case RdKafka::ERR__TIMED_OUT:
		break;
	case RdKafka::ERR_NO_ERROR:
		{
			/* Real message */
			int len = static_cast<int>(message->len());
			const char* szMsg = static_cast<const char *>(message->payload());
			string strMessage(szMsg, len);
			if (pThis->m_fnMessageCb)
				pThis->m_fnMessageCb(message->topic_name(), strMessage, pThis->m_fnMessageCb);
			LogDebug() << "Consume message: " << strMessage;
		}
		break;
	case RdKafka::ERR_UNKNOWN_TOPIC_OR_PART:
		{
			LogWarn() << "The topic has not been created yet.topic:" << message->topic_name();
		}
		break;
	case RdKafka::ERR__PARTITION_EOF:
		{
			/* Last message */
			if (exit_eof && ++eof_cnt == partition_cnt) 
			{
				LogAlarm() << "Stop!!! EOF reached for all " << partition_cnt << " partition(s)";
				pThis->Stop();
			}
		}
		break;
	case RdKafka::ERR__UNKNOWN_TOPIC:
	case RdKafka::ERR__UNKNOWN_PARTITION:
		{
			LogError() << "Stop!!! Consume failed: " << message->errstr();
			pThis->Stop();
		}
		break;
	default:
		/* Errors */
		LogError() << "Stop!!! Consume failed: " << message->errstr();
		pThis->Stop();
		break;
	}
}

JZ::KafkaErrorCode Consumer::Start(const string & strBoker, const string & strGroupId, ConsumerMessageCb fnCb, void* pUserData)
{
	if (m_bRunning)
	{
		return  JZ::E_NO_ERROR;
	}
	m_strBroker = strBoker;
	m_strGroup = strGroupId;

	m_fnMessageCb = fnCb;
	m_fnUserData = pUserData;

	string strErr;
	RdKafka::Conf::ConfResult res;
	//设置group id
	res = m_pcsGlobalConf->set("group.id", m_strGroup, strErr);
	if (RdKafka::Conf::CONF_OK != res)
	{
		LogError() << "Set group.id fail, err: " << strErr;
		return JZ::E_INVALID_GROUP_ID;
	}
	//设置broker
	res = m_pcsGlobalConf->set("bootstrap.servers", m_strBroker, strErr);
	if (RdKafka::Conf::CONF_OK != res)
	{
		LogError() << "Set bootstrap.servers fail, err: " << strErr;
		return JZ::E_INVALID_BROKER;
	}
	//设置消费回调
	res = m_pcsGlobalConf->set("consume_cb", &m_csConsumeCb, strErr);
	if (RdKafka::Conf::CONF_OK != res)
	{
		LogError() << "Set consume_cb fail, err: " << strErr;
		return JZ::E_UNKNOW_ERROR;
	}
	//设置事件回调
	res = m_pcsGlobalConf->set("event_cb", &m_csEventCb, strErr);
	if (RdKafka::Conf::CONF_OK != res)
	{
		LogError() << "Set event_cb fail, err: " << strErr;
		return JZ::E_UNKNOW_ERROR;
	}
	//自动创建主题
	res = m_pcsGlobalConf->set("allow.auto.create.topics", "true", strErr);
	if (RdKafka::Conf::CONF_OK != res)
	{
		LogError() << "Set allow.auto.create.topics fail, err: " << strErr;
		return JZ::E_UNKNOW_ERROR;
	}
	//设置topic conf
	res = m_pcsGlobalConf->set("default_topic_conf", m_pcsTopicConf, strErr);
	if (RdKafka::Conf::CONF_OK != res)
	{
		LogError() << "Set default_topic_conf fail, err: " << strErr;
		return JZ::E_UNKNOW_ERROR;
	}

	m_pcsConsumer = RdKafka::KafkaConsumer::create(m_pcsGlobalConf, strErr);
	if (m_pcsConsumer == nullptr)
	{
		LogError() << "Create RdKafka::KafkaConsumer fail.";
		return JZ::E_UNKNOW_ERROR;
	}
	
	m_bRunning = true;
	m_thread = std::move(std::thread(ThreadEntry, this));

	return JZ::E_NO_ERROR;
}

void Consumer::Stop()
{
	m_bRunning = false;
	m_vecTopics.clear();
	if (m_thread.joinable())
	{
		m_thread.join();
	}
	if (m_pcsConsumer)
	{
		m_pcsConsumer->close();
		delete m_pcsConsumer;
		m_pcsConsumer = nullptr;
	}
	return;
}

JZ::KafkaErrorCode Consumer::AddSubscribe(const string & strTopic)
{
	for (auto it : m_vecTopics)
	{
		if (it == strTopic)
			return JZ::E_NO_ERROR;
	}

	m_vecTopics.push_back(strTopic);
	RdKafka::ErrorCode res = m_pcsConsumer->subscribe(m_vecTopics);
	if (RdKafka::ErrorCode::ERR_NO_ERROR != res)
	{
		LogError() << "subscribe fail.error code:" << res;
		return JZ::E_UNKNOW_ERROR;
	}
	LogInfo() << "Add subscribe topic:" << strTopic;
	return JZ::E_NO_ERROR;
}

void Consumer::ThreadEntry(void * pUserData)
{
	Consumer *pThis = (Consumer*)pUserData;
	pThis->Run();
}

void Consumer::Run()
{
	while (m_bRunning)
	{
		RdKafka::Message *msg = m_pcsConsumer->consume(5000);
		Consumer::MessageConsume(msg, this);
		delete msg;
	}
}