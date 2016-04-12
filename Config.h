#include <iostream>
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include <vector>
#include <queue>


// ����� ���� ��� Defined
#include "Server.h"
#include "ClientSession.h"
#include "../../Protocol.h"
//#include "CircularBuffer.h"

using boost::asio::ip::tcp;
using namespace std;

#define PORT_NUMBER		20000				// Port Num
#define MAX_SESSION_CNT 10					// Ŭ���̾�Ʈ ���� �ο�

#define MAX_RECEIVE_BUFFER_LEN	512			// Receive Buffer Size

#define MAX_NAME_LEN	20
#define MAX_MESSAGE_LEN	129