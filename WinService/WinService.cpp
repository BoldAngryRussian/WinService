/// <summary>
/// This file is describimg the new windows service
/// C++ realization by Kosov Alex 2019
/// </summary>

#include <Windows.h>
#include <iostream>

#include "pch.h"
#include "Defines.h"
#include "converter.h"
#include "ILogger.h"
#include "CServiceImpl.h"

void GetCMDLineTraceInfo();

int __cdecl main(int argc, char** argv)
{
	ILogger::init();

	spdlog::info("Service started. Version '{0}'", SRV_VERSION);	

	auto service = SRV::CreatePDCService();
	service->start();
	return 0;

	if (argc != 2)
	{
		spdlog::info("Incorrect number of arguments!");
		GetCMDLineTraceInfo();
	}
	else
	{
		std::string sParam = *++argv;
		spdlog::debug("The number of parameters '{0}'. Checking the command '{1}'", argc, sParam);

		auto action  = SRV::convert::FromString(sParam);
		auto service = SRV::CreatePDCService();

		switch (action)
		{
			case SRV::convert::install:	service->install();	break;
			case SRV::convert::start:	service->start();	break;
			case SRV::convert::stop:	service->stop();	break;
			default:
				spdlog::info("Unknown action! Don't know what to do!");
				break;
		}
	}

	spdlog::info("Service stopped!");
	return 0;
}

void GetCMDLineTraceInfo()
{
	spdlog::debug("Description: Command-line tool that controls a service.");
	spdlog::debug("Usage: svccontrol [command] [service_name]");
	spdlog::debug("Avaliable commands : start,stop");
}