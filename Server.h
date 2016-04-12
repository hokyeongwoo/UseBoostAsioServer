#pragma once

#include "Config.h"

class Server
{
public:
	Server(boost::asio::io_service& io_service)
		: m_Acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT_NUMBER))
	{
		m_blsAccepting = true;
	}

	~Server()
	{
		// 추후에 람다로 변경하자...
		for (size_t i = 0; i < m_SessionList.size(); ++i)
		{
			if (m_SessionList[i]->Socket().is_open())
			{
				m_SessionList[i]->Socket().close();
			}

			delete m_SessionList[i];
		}
	}

	void Init(const int nMaxSessionCount)
	{
		for (int i = 0; i < nMaxSessionCount; ++i)
		{
			ClientSession* pSession = new ClientSession(i, m_Acceptor.get_io_service(), this);
			m_SessionList.push_back(pSession);
			m_SessionQueue.push_back(i);
		}
	}

	void Start()
	{
		std::cout << "** Server Start!!! **" << std::endl;

		PostAccept();
	}

	void CloseSession(const int nSessionID)
	{
		std::cout << "Client Session Close! SessionID : " << nSessionID << std::endl;
		m_SessionQueue.push_back(nSessionID);

		if (m_blsAccepting == false)
		{
			PostAccept();
		}
	}

	void ProcessPacket(const int nSessionID, const char* pData)
	{
		PACKET_HEADER* pheader = (PACKET_HEADER*)pData;

		// Packet Type 구분하여 처리하는 곳
		switch(pheader->nID)
		{
		case CS_LOGIN:
			{
				std::cout << "Recv : Login Req!" << std::endl;

				LoginRequest* pPacket = (LoginRequest*)pData;
				
				LoginResult sendPacket;
				memset(&sendPacket, 0, sizeof(LoginResult));
				sendPacket.nID = SC_LOGIN;
				sendPacket.nSize = sizeof(LoginResult);

				m_SessionList[nSessionID]->PostSend(false, sendPacket.nSize, (char*)&sendPacket);

				std::cout << "Send : Login Result!" << std::endl;
			}
			break;
		case CS_CHATMESSAGE:
			{
				ChatMessage* pPacket = (ChatMessage*)pData;

				NOTICE_ChatMessage sendPacket;
				memset(&sendPacket, 0, sizeof(NOTICE_ChatMessage));
				sendPacket.nID = SC_CHATMESSAGE;
				sendPacket.nSize = sizeof(NOTICE_ChatMessage);
				strncpy_s(sendPacket.szMessage, MAX_MESSAGE_LEN, pPacket->szMessage, MAX_MESSAGE_LEN - 1);

				for (size_t i = 0; i < m_SessionList.size(); ++i)
				{
					if (m_SessionList[i]->Socket().is_open())
					{
						if(m_SessionList[i]->SessionID() != nSessionID)
							m_SessionList[i]->PostSend(false, sendPacket.nSize, (char*)&sendPacket);
					}
				}
			}
			break;
		}
	}

private:
	bool PostAccept()
	{
		if (m_SessionQueue.empty())
		{
			m_blsAccepting = false;
			return false;
		}

		m_blsAccepting = true;

		int nSessionID = m_SessionQueue.front();

		m_SessionQueue.pop_front();

		m_Acceptor.async_accept(m_SessionList[nSessionID]->Socket(),
			boost::bind(&Server::handle_accept, this, m_SessionList[nSessionID],
				boost::asio::placeholders::error));

		return true;
	}

	void handle_accept(ClientSession* pSession, const boost::system::error_code& err)
	{
		if (!err)
		{
			std::cout << "Client Connection Success!!! SessionID: " << pSession->SessionID() << std::endl;

			pSession->Init();
			pSession->PostReceive();

			PostAccept();
		}
		else
		{
			std::cout << "Error!! ErrorNo: " << err.value() << " Error Message: " << err.message() << std::endl;
		}
	}

private:
	int m_nSeqNumber;
	bool m_blsAccepting;
	boost::asio::ip::tcp::acceptor m_Acceptor;

	std::vector<ClientSession*> m_SessionList;
	std::deque<int> m_SessionQueue;
};