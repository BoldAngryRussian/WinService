#include "pch.h"
#include "ILogger.h"
#include "spdlog/sinks/basic_file_sink.h"

std::mutex ILogger::_Mutex;
std::atomic<ILogger*> ILogger::_Logger;

const ILogger* ILogger::inst()
{
	auto logger = _Logger.load();
	if (nullptr == logger){
		std::lock_guard<std::mutex> lock(_Mutex);
		logger = _Logger.load();
		if (nullptr == logger){
			logger = new ILogger();
			_Logger.store(logger);
		}
	}
	return logger;
}

void ILogger::init()
{
	ILogger::inst();
}

ILogger::ILogger()
{
	_MLoggers.clear();
	_MLoggers.reserve(5);	
	
	auto spdLogger = _AddLogger("file_logger", std::string(LOGSPATH)
													.append("service.log"));
	// change log pattern
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][%P][%t][--%L--] %v");
	spdlog::set_level(spdlog::level::debug); // Set global log level to debug	
	spdlog::set_default_logger(spdLogger->log);	
	spdlog::flush_every(std::chrono::seconds(1));

	_DfltLogger = spdLogger->log;

	_MLoggers.insert(std::make_pair(spdLogger->LoggerName, spdLogger));
}

ILogger::~ILogger()
{
	spdlog::shutdown();
}

std::shared_ptr<spdlog::logger> ILogger::GetDefault() const
{
	return _DfltLogger;
}

std::shared_ptr<ILogger::SpdlogInformation> ILogger::_AddLogger(const std::string& pName, const std::string& pFile)
{
	auto spdLogger = std::make_shared<SpdlogInformation>();
	spdLogger->LoggerName	= pName;
	spdLogger->FileName		= pFile;	
	spdLogger->log = spdlog::basic_logger_mt(spdLogger->LoggerName, spdLogger->FileName);	
	return spdLogger;
}

const bool ILogger::AddLogger(const std::string& pName, const std::string& pFile2Save)
{
	std::lock_guard<std::mutex> locker(_Mutex);
	auto it = _MLoggers.find(pName);
	if (it == _MLoggers.end()){
		auto spdLogger = _AddLogger(pName, pFile2Save);
		_MLoggers.insert(std::make_pair(spdLogger->LoggerName, spdLogger));
	}
	return true;
}

const bool ILogger::DelLogger(const std::string& pName)
{
	std::lock_guard<std::mutex> locker(_Mutex);
	_MLoggers.erase(pName);
	return true;
}

const bool ILogger::SetDefault(const std::string& pName)
{
	std::lock_guard<std::mutex> locker(_Mutex);
	
	const auto it = _MLoggers.find(pName);
	if (it != _MLoggers.end()){
		spdlog::set_default_logger(it->second->log);
	}
	return true;
}