#pragma once
#include <mutex>
#include <atomic>
#include <unordered_map>
#include "Defines.h"
#include "spdlog/spdlog.h"

/// <summary>
/// This class is a singleton for logger
/// </summary>
class ILogger
{
public:
	/// <summary>
	/// Logger creation
	/// </summary>
	/// <returns></returns>
	static const ILogger* inst();
	/// <summary>
	/// Initializing the default logger
	/// </summary>
	static void init();
public:
	/// <summary>
	/// Additing the new logger
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>
	/// <returns></returns>
	const bool AddLogger(const std::string&, const std::string&);
	/// <summary>
	/// Deleting the logger
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const bool DelLogger(const std::string&);
	/// <summary>
	/// Setting the default logger
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const bool SetDefault(const std::string&);
	/// <summary>
	/// Getting the default logger
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<spdlog::logger> GetDefault() const;
private:
	ILogger();
	~ILogger();

	ILogger(const ILogger&) = delete;
	ILogger(ILogger&&) = delete;
	const ILogger& operator=(const ILogger&) = delete;
protected:
	typedef struct _SpdlogInformation
	{
		std::shared_ptr<spdlog::logger> log;
		std::string LoggerName;
		std::string FileName;
	} SpdlogInformation;
	std::unordered_map<std::string, std::shared_ptr<SpdlogInformation>> _MLoggers;	
	std::shared_ptr<SpdlogInformation> _AddLogger(const std::string&, const std::string&);
private:
	std::shared_ptr<spdlog::logger> _DfltLogger;
	//	atomic pointer to logger
	static std::atomic<ILogger*> _Logger;
	//	Synchronization object
	static std::mutex _Mutex;
};



