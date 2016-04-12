#pragma once
#include "Server.h"
#include "ClientSession.h"
#include "../../Protocol.h"

#include <boost/circular_buffer.hpp>

ClientSession::ClientSession(int nSessionID, boost::asio::io_service& io_service, Server* pServer)
	: m_Socket(io_service)
	, m_nSessionID(nSessionID)
	, m_pServer(pServer)
{
	m_bCompletedWrite = true;
}

ClientSession::~ClientSession()
{
	while (m_SendDataQueue.empty() == false)
	{
		delete[] m_SendDataQueue.front();
		m_SendDataQueue.pop_front();
	}
}

void ClientSession::Init()
{
	m_nPacketBufferMark = 0;
}

void ClientSession::PostReceive()
{
	m_Socket.async_read_some(boost::asio::buffer(m_ReceiveBuffer)
		, boost::bind(&ClientSession::handle_receive, this
			, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void ClientSession::PostSend(const bool blmmediately, const int nSize, char* pData)
{
	char* pSendData = nullptr;

	if (blmmediately == false)
	{
		pSendData = new char[nSize];
		memcpy(pSendData, pData, nSize);

		m_SendDataQueue.push_back(pSendData);
	}
	else
	{
		pSendData = pData;
	}

	if (m_bCompletedWrite == false)
	{
		return;
	}

	boost::asio::async_write(m_Socket, boost::asio::buffer(pSendData, nSize)
		, boost::bind(&ClientSession::handle_write, this
			, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void ClientSession::handle_write(const boost::system::error_code& err, size_t bytes_transferred)
{
	delete[] m_SendDataQueue.front();
	m_SendDataQueue.pop_front();

	if (m_SendDataQueue.empty() == false)
	{
		m_bCompletedWrite = false;
		char* pData = m_SendDataQueue.front();

		PACKET_HEADER* pHeader = (PACKET_HEADER*)pData;

		PostSend(true, pHeader->nSize, pData);
	}
	else
	{
		m_bCompletedWrite = true;
	}
}

void ClientSession::handle_receive(const boost::system::error_code& err, size_t bytes_transferred)
{
	if(err)
	{
		if (err == boost::asio::error::eof)
		{
			std::cout << "Client Connection Fail!!!" << std::endl;
		}
		else
		{
			std::cout << "Error No: " << err.value() << " Error Message: " << err.message() << std::endl;
		}

		m_pServer->CloseSession(m_nSessionID);
	}
	else
	{
		memcpy(&m_PacketBuffer[m_nPacketBufferMark], m_ReceiveBuffer.data(), bytes_transferred);

		int nPacketData = m_nPacketBufferMark + bytes_transferred;
		int nReadData = 0;

		while (nPacketData > 0)
		{
			if (nPacketData < sizeof(PACKET_HEADER))
			{
				break;
			}

			PACKET_HEADER* pHeader = (PACKET_HEADER*)&m_PacketBuffer[nReadData];

			if (pHeader->nSize <= nPacketData)
			{
				m_pServer->ProcessPacket(m_nSessionID, &m_PacketBuffer[nReadData]);

				nPacketData -= pHeader->nSize;
				nReadData += pHeader->nSize;
			}
			else
			{
				break;
			}
		}

		if (nPacketData > 0)
		{
			char TempBuffer[MAX_RECEIVE_BUFFER_LEN] = { 0, };
			memcpy(&TempBuffer[0], &m_PacketBuffer[nReadData], nPacketData);
			memcpy(&m_PacketBuffer[0], &TempBuffer[0], nPacketData);
		}

		m_nPacketBufferMark = nPacketData;

		PostReceive();
	}
}