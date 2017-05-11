#include "Logging.h"
#include <stdio.h>

namespace Minim
{
	static bool g_bDebugOn = false;
	static LogFunc g_errorLog = 0;
	static LogFunc g_debugLog = 0;

	void enableLogging()
	{
		g_bDebugOn = true;
	}

	void disableLogging()
	{
		g_bDebugOn = false;
	}

	void setErrorLog(LogFunc func)
	{
		g_errorLog = func;
	}

	void setDebugLog(LogFunc func)
	{
		g_debugLog = func;
	}

	void error(const char * msg)
	{
		if (g_errorLog)
		{
			g_errorLog(msg);
		}
		else
		{
			printf("\nMinim Error: %s\n", msg);
		}
	}

	void debug(const char * msg)
	{
		if (g_bDebugOn)
		{
			if (g_debugLog)
			{
				g_debugLog(msg);
			}
			else
			{
				printf("\nMinim Debug: %s\n", msg);
			}
		}
	}
}