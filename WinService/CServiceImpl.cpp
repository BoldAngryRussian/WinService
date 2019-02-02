#include "pch.h"
#include "CServiceImpl.h"
#include "ILogger.h"

using namespace SRV;

CServiceImpl::CServiceImpl()
{
}


CServiceImpl::~CServiceImpl()
{
}

void _fastcall CServiceImpl::start()
{
	spdlog::info("The service is being started!");
}

void _fastcall CServiceImpl::stop()
{
	spdlog::info("The service is being stopped!");
}

void _fastcall CServiceImpl::install()
{
	spdlog::info("The service is being installed!");
}