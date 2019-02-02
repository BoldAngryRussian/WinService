#include "pch.h"
#include "strsafe.h"
#include "CServiceImpl.h"
#include "ILogger.h"

using namespace SRV;

CServiceImpl::CServiceImpl()
	:ghSvcStopEvent(NULL)
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
													&SvcCtrlHandler);

	if (!gSvcStatusHandle)
	{	
		spdlog::info("Can't register hadnler main function!");
		return;
	}

	// These SERVICE_STATUS members remain as set here
	SERVICE_STATUS          gSvcStatus;
	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM
	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.

//	SvcInit(dwArgc, lpszArgv);
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
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;

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