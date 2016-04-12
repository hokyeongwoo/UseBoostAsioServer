#pragma once

#include "Config.h"

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;
		Server server(io_service);
		server.Init(MAX_SESSION_CNT);
		server.Start();

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	std::cout << "���� ��Ʈ��ũ ����!!" << std::endl;

	return 0;
}
