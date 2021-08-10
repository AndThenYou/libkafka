#include "Kafka.h"
#include "Producer.h"
#include "Consumer.h"
#include "fnlog/include/fn_macro.h"

int g_nLogInit = 0;

bool JZ::Init()
{
	if (g_nLogInit == 0)
	{
		FNLog::FastStartDefaultLogger();
	}
	g_nLogInit++;
	return true;
}

void JZ::Uninit()
{
	g_nLogInit--;
	if (g_nLogInit == 0)
	{
		FNLog::StopLogger(FNLog::GetDefaultLogger());
	}
}


JZ::IConsumer* JZ::CreateConsumer()
{
	LogDebug() << "Create consumer.";
	return new Consumer;
}

JZ::IProducer* JZ::CreateProducer()
{
	LogDebug() << "Create producer.";
	return new Producer;
}