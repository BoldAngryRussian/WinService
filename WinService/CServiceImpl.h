#pragma once

#include <memory>
#include <mutex>

#include "IService.h"
#include "Defines.h"

namespace SRV
{

	typedef std::shared_ptr<IService> ServicePtr;

	class CServiceImpl :
		public IService
	{
	public:
		CServiceImpl();
		~CServiceImpl();
	public:
		virtual void _fastcall start() override;
		virtual void _fastcall stop() override;
		virtual void _fastcall install() override;
	};

	ServicePtr CreateService()
	{
		static std::mutex Mutex;
		static ServicePtr service = nullptr;

		if (nullptr == service)
		{
			std::lock_guard<std::mutex> lock(Mutex);
			if (nullptr == service)
			{
				service = std::make_shared<CServiceImpl>();
			}
		}
		return service;
	}
}


