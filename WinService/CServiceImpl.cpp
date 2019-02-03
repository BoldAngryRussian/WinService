#include "pch.h"
#include "strsafe.h"
#include "CServiceImpl.h"
#include "ILogger.h"

using namespace SRV;

HANDLE                  CServiceImpl::ghSvcStopEvent = NULL;
SERVICE_STATUS          CServiceImpl::gSvcStatus;
SERVICE_STATUS_HANDLE   CServiceImpl::gSvcStatusHandle;

CServiceImpl::CServiceImpl()	
{
}


CServiceImpl::~CServiceImpl()
{
}

void _fastcall CServiceImpl::start()
{
	spdlog::info("The service is being started!");
	// TO_DO: Add any additional services for the process to this table.
	SERVICE_TABLE_ENTRYW DispatchTable[] =
	{
		{ (LPWSTR)SVCNAME, (LPSERVICE_MAIN_FUNCTION)_SvcMain },
		{ NULL, NULL }
	};
	spdlog::info("Dispatch table created.");
	// This call returns when the service has stopped. 
	// The process should simply terminate when the call returns.
	spdlog::info("Starting the dispatcher...");
	if (!StartServiceCtrlDispatcherW(DispatchTable))
	{
		_ReportEvent((LPTSTR)TEXT("StartServiceCtrlDispatcher"));
		spdlog::info("Can't start dispatcher!");
	}
}

void _fastcall CServiceImpl::stop()
{
	spdlog::info("The service is being stopped!");
}

/// <summary>
/// Install service action realozation
/// </summary>
/// <returns></returns>
void _fastcall CServiceImpl::install()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR szPath[MAX_PATH];

	spdlog::info("The service is being installed!");

	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		spdlog::info("Can't install service! Error is {0}", GetLastError());
		return;
	}

	//	Getting a handle to SCM database
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == schSCManager)
	{
		spdlog::info("Can't open manager! Error is {0}", GetLastError());
		return;
	}

	// Creating the service

	schService = CreateService(
		schSCManager,				// SCM database
		SVCNAME,					// name of service 
		SVCNAME,					// service name to display 
		SERVICE_ALL_ACCESS,			// desired access
		SERVICE_WIN32_OWN_PROCESS,	// service type 
		SERVICE_DEMAND_START,		// start type 
		SERVICE_ERROR_NORMAL,		// error control type 
		szPath,						// path to service's binary 
		NULL,						// no load ordering group 
		NULL,						// no tag identifier 
		NULL,						// no dependencies 
		NULL,						// LocalSystem account 
		NULL
	);

	if (NULL == schService)
	{
		spdlog::info("Can't create service! Error is {0}", GetLastError());
		return;
	}
	else spdlog::info("The service installed successefully!");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

VOID WINAPI CServiceImpl::_SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// Register the handler function for the service
	gSvcStatusHandle = RegisterServiceCtrlHandler(
													SVCNAME,
													SvcCtrlHandler);

	if (!gSvcStatusHandle)
	{	
		spdlog::info("Can't register hadnler main function!");
		return;
	}
	// These SERVICE_STATUS members remain as set here
	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM
	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.
	SvcInit(dwArgc, lpszArgv);
}

VOID CServiceImpl::SvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// TO_DO: Declare and set any required variables.
	//   Be sure to periodically call ReportSvcStatus() with 
	//   SERVICE_START_PENDING. If initialization fails, call
	//   ReportSvcStatus with SERVICE_STOPPED.

	// Create an event. The control handler function, SvcCtrlHandler,
	// signals this event when it receives the stop control code.
	ghSvcStopEvent = CreateEvent(
		NULL,    // default security attributes
		TRUE,    // manual reset event
		FALSE,   // not signaled
		NULL);   // no name

	if (ghSvcStopEvent == NULL)
	{
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}

	// Report running status when initialization is complete.
	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
	// TO_DO: Perform work until service stops.	
	spdlog::info("The service start successefuly!");
	while (1)
	{
		// Check whether to stop the service.
		WaitForSingleObject(ghSvcStopEvent, INFINITE);
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}
}

VOID CServiceImpl::ReportSvcStatus(DWORD dwCurrentState,DWORD dwWin32ExitCode,DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.
	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else 
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else 
		gSvcStatus.dwCheckPoint = dwCheckPoint++;
	
	gSvcStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

VOID CServiceImpl::SvcCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code. 

	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		// Signal the service to stop.

		SetEvent(ghSvcStopEvent);
		ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

		return;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;
	}

}

VOID CServiceImpl::_ReportEvent(LPTSTR szFunction)
{
	HANDLE hEventSource;
	LPCTSTR lpszStrings[2];
	TCHAR Buffer[80];

	hEventSource = RegisterEventSource(NULL, SVCNAME);

	if (NULL != hEventSource)
	{
		StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

		lpszStrings[0] = SVCNAME;
		lpszStrings[1] = Buffer;

		::ReportEvent(hEventSource,        // event log handle
			EVENTLOG_ERROR_TYPE, // event type
			0,                  // event category
			0,           // event identifier
			NULL,                // no security identifier
			2,                   // size of lpszStrings array
			0,                   // no binary data
			lpszStrings,         // array of strings
			NULL);               // no binary data

		DeregisterEventSource(hEventSource);
	}
}

ServicePtr SRV::CreatePDCService()
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