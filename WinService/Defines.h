#pragma once

#define LOGSPATH "C:\\Users\\Papa\\source\\repos\\WinService\\Debug\\logs\\"

#define SRV_VERSION "1.0.0.0"

namespace SRV
{
	namespace convert
	{
		enum StartServersAttributes
		{
			install = 0,
			start,
			stop,
			unknown
		};
	}
}
