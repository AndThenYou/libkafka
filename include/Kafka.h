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
	@brief �ӿ���  �����߿ͻ���
	*
	------------------------------------------------------------------*/
	class IConsumer
	{
	public:
		IConsumer() {};
		virtual ~IConsumer() {};

		//strBroker   kafka��broker��ַ������ localhost:9092
		//strGroupId  ��������id, ͬһ����������߲���ͬʱ����ͬһ����Ϣ
		//fnCb		  ���յ������������Ϣ�ص�����
		//pUserData	  �û����ݣ�͸����fnCb
		virtual KafkaErrorCode Start(const string & strBroker, const string & strGroupId,
			ConsumerMessageCb fnCb, void* pUserData = nullptr) = 0;

		//strTopic   ׷�����ⶩ��
		virtual KafkaErrorCode AddSubscribe(const string & strTopic) = 0;

		//ֹͣ�����߳�
		virtual void Stop() = 0;

	};

	/*--------------------------------------------------------------------
	*
	@brief �ӿ���  �����߿ͻ���
	*
	----------------------------------------------------------------------*/
	class IProducer
	{
	public:
		IProducer() {};
		virtual ~IProducer() {};

		//strBroker   kafka��broker��ַ������ localhost:9092
		//strTopic	  ������Ϣ������
		virtual KafkaErrorCode Start(const string & strBroker, const string & strTopic) = 0;

		//message    ����������Ϣ
		virtual KafkaErrorCode Produce(const string& message) = 0;

		//ֹͣ
		virtual void Stop() = 0;
	};

	//------------------------------�����ӿ�------------------------------

	//������ʼ���������ȵ��ã�������һ�Σ���Uninit()����
	JZ_API bool Init();

	//����������
	JZ_API IConsumer* CreateConsumer();

	//����������
	JZ_API IProducer* CreateProducer();

	//����ʼ��
	JZ_API void Uninit();

}

