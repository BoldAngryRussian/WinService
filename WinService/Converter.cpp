#pragma once
#include "pch.h"
#include "converter.h"

namespace SRV
{
	namespace convert
	{		
		const StartServersAttributes FromString(const std::string& pValue)
		{
			if (0 == std::string("install").compare(pValue))
			{
				return install;
			}
			else if (0 == std::string("start").compare(pValue))
			{
				return start;
			}
			else if (0 == std::string("stop").compare(pValue))
			{
				return stop;
			}
			else return unknown;
		}
	}
}