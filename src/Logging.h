#pragma once

namespace Minim
{
	typedef void(*LogFunc)(const char *);

	void enableLogging();
	void disableLogging();

	void setErrorLog(LogFunc func);
	void setDebugLog(LogFunc func);

	void error(const char * errorMsg);
	void debug(const char * debugMsg);
}

