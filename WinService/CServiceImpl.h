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
	public:
		static VOID WINAPI _SvcMain(DWORD dwArgc, LPTSTR *lpszArgv);		
		static VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv);
		static VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
		static VOID WINAPI SvcCtrlHandler(DWORD dwCtrl);	
		static HANDLE                  ghSvcStopEvent;
		static SERVICE_STATUS          gSvcStatus;
		static SERVICE_STATUS_HANDLE   gSvcStatusHandle;
	};

	ServicePtr CreatePDCService();
}


