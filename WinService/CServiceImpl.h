#pragma once

#include <memory>
#include <mutex>

#include <Windows.h>
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
		
	protected:
		/// <summary>
		/// Entry point to Application event log
		/// </summary>
		/// <param name="szFunction"></param>
		/// <returns></returns>
		VOID _ReportEvent(LPTSTR szFunction);

		VOID WINAPI _SvcMain(DWORD dwArgc, LPTSTR *lpszArgv);		
		VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
		VOID WINAPI SvcCtrlHandler(DWORD dwCtrl);
	private:
		HANDLE                  ghSvcStopEvent;
		SERVICE_STATUS          gSvcStatus;
		SERVICE_STATUS_HANDLE   gSvcStatusHandle;
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


