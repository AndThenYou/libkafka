#include "EventCb.h"
#include "Consumer.h"
#include "Producer.h"
#include "fnlog/include/fn_macro.h"


EventCb::EventCb(Consumer* consunmer) 
	: m_pcsConsumer(consunmer)
	, m_pcsProdecer(nullptr)
{

}

EventCb::EventCb(Producer* producer)
	: m_pcsProdecer(producer)
	, m_pcsConsumer(nullptr)
{

}

EventCb::~EventCb()
{

}

void EventCb::event_cb(RdKafka::Event& event)
{
	switch (event.type())
	{
	case RdKafka::Event::EVENT_ERROR:
		LogError() << "Error: " << RdKafka::err2str(event.err()) << ", subscribe: " << event.str();
		if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
		{
			LogError() << "All brokers connect failed.";
		}
		break;

	case RdKafka::Event::EVENT_STATS:
		LogInfo() << "Status: " << event.str();
		break;

	case RdKafka::Event::EVENT_LOG:
		LogWarn() << "Log-" << event.severity() << ", fac: " << event.fac().c_str() << ", message: " << event.str().c_str();
		break;
	case RdKafka::Event::EVENT_THROTTLE:
		LogInfo() << "Throttled: " << event.throttle_time() << "ms by broker-name:" <<
			event.broker_name() << ", broker-id: " << (int)event.broker_id();
		break;

	default:
		std::cerr << "EVENT " << event.type() <<
			" (" << RdKafka::err2str(event.err()) << "), " <<
			event.str() << std::endl;
		break;
	}
}