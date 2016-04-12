#pragma once

#include "Config.h"

class Server;

class ClientSession
{
public:
	ClientSession(int nSessionID, boost::asio::io_service& io_service, Server* pServer);
	~ClientSession();

	int SessionID() { return m_nSessionID; }

	boost::asio::ip::tcp::socket& Socket() { return m_Socket; }

	void Init();

	void PostReceive();
	void PostSend(const bool bImmediately, const int nSize, char* pData);

	void SetName(const char* pszName) { m_Name = pszName; }
	const char* GetName() { return m_Name.c_str(); }

private:
	void handle_write(const boost::system::error_code& err, size_t bytes_transferred);

	void handle_receive(const boost::system::error_code& err, size_t bytes_transferred);

private:
	int m_nSessionID;
	boost::asio::ip::tcp::socket m_Socket;

	std::array<char, MAX_RECEIVE_BUFFER_LEN> m_ReceiveBuffer;

	int m_nPacketBufferMark;
	char m_PacketBuffer[MAX_RECEIVE_BUFFER_LEN * 2];

	bool m_bCompletedWrite;

	std::deque<char*> m_SendDataQueue;

	std::string m_Name;
	
	Server* m_pServer;
};