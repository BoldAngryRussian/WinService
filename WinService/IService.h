#pragma once

/// <summary>
/// Service interface
/// </summary>
struct IService
{
	/// <summary>
	/// Start the service
	/// </summary>
	/// <returns></returns>
	virtual void _fastcall start() = 0;
	/// <summary>
	/// Stop the service
	/// </summary>
	/// <returns></returns>
	virtual void _fastcall stop() = 0;
	/// <summary>
	/// Install the service
	/// </summary>
	/// <returns></returns>
	virtual void _fastcall install() = 0;
};

