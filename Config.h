#include <iostream>
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include <vector>
#include <queue>


// 사용자 정의 헤더 Defined
#include "Server.h"
#include "ClientSession.h"
#include "../../Protocol.h"
//#include "CircularBuffer.h"

using boost::asio::ip::tcp;
using namespace std;

#define PORT_NUMBER		20000				// Port Num
#define MAX_SESSION_CNT 10					// 클라이언트 수용 인원

#define MAX_RECEIVE_BUFFER_LEN	512			// Receive Buffer Size

#define MAX_NAME_LEN	20
#define MAX_MESSAGE_LEN	129