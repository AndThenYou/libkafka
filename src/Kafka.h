#pragma once

#include <sstream>
#include <string>

#ifdef WIN32
#ifndef JZ_API
#ifdef _WIN32
#ifdef JZ_EXPORT
#define JZ_API extern "C" __declspec(dllexport)
#else
#define JZ_API extern "C" __declspec(dllimport)
#endif
#else
#define JZ_API
#endif
#endif

#else
#define JZ_API	 extern "C"
#endif

namespace JZ
{
	using std::string;

	typedef void(*ConsumerMessageCb)(string & strTopic, string& strMessage, void *pUserData);

	enum KafkaErrorCode
	{
		E_NO_ERROR = 0,
		E_CONNECT_FAIL,
		E_INVALID_BROKER,
		E_BROKERS_DOWN,
		E_INVALID_GROUP_ID,
		E_INVALID_MSG_SIZE,
		E_UNKNOW_ERROR,
	};

	/*-----------------------------------------------------------------
	* 
	@brief 接口类  消费者客户端
	*
	------------------------------------------------------------------*/
	class IConsumer
	{
	public:
		IConsumer() {};
		virtual ~IConsumer() {};

		//strBroker   kafka的broker地址，例如 localhost:9092
		//strGroupId  消费者组id, 同一个组的消费者不能同时消费同一个消息
		//fnCb		  接收到订阅主题的消息回调函数
		//pUserData	  用户数据，透传给fnCb
		virtual KafkaErrorCode Start(const string & strBroker, const string & strGroupId,
			ConsumerMessageCb fnCb, void* pUserData = nullptr) = 0;

		//strTopic   追加主题订阅
		virtual KafkaErrorCode AddSubscribe(const string & strTopic) = 0;

		//停止订阅线程
		virtual void Stop() = 0;

	};

	/*--------------------------------------------------------------------
	*
	@brief 接口类  生产者客户端
	*
	----------------------------------------------------------------------*/
	class IProducer
	{
	public:
		IProducer() {};
		virtual ~IProducer() {};

		//strBroker   kafka的broker地址，例如 localhost:9092
		//strTopic	  生产消息的主题
		virtual KafkaErrorCode Start(const string & strBroker, const string & strTopic) = 0;

		//message    生产发布消息
		virtual KafkaErrorCode Produce(const string& message) = 0;

		//停止
		virtual void Stop() = 0;
	};

	//------------------------------导出接口------------------------------

	//环境初始化，必须先调用，仅调用一次，与Uninit()配套
	JZ_API bool Init();

	//生成消费者
	JZ_API IConsumer* CreateConsumer();

	//生成生产者
	JZ_API IProducer* CreateProducer();

	//反初始化
	JZ_API void Uninit();

}

