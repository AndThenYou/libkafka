#pragma once

#include "rdkafkacpp.h"
#include <iostream>


class Consumer;
class Producer;

class EventCb : public RdKafka::EventCb
{
public:

	EventCb(Consumer* consumer);

	EventCb(Producer* producer);

	virtual ~EventCb();

	void event_cb(RdKafka::Event& event);
private:
	Consumer * m_pcsConsumer;
	Producer * m_pcsProdecer;
};