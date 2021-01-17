// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include <noggit/Log.h>

#include <cstring>
#include <ctime>
#include <fstream>
#include <qlogging.h>
#include <QIODevice>

QDebug operator<<(QDebug dbg, const std::string& message)
{
	const QString body = QString::fromStdString(message);
	dbg.nospace() << body;
	return dbg.maybeSpace();
}

QDebug operator<<(QDebug dbg, const std::filesystem::path& message)
{
	const QString body = QString::fromStdString(message.string());
	dbg.nospace() << body;
	return dbg.maybeSpace();
}

QDebug operator<<(QDebug dbg, const std::error_code& message) 
{
	const QString body = QString::fromStdString(message.message());
	dbg.nospace() << body;
	return dbg.maybeSpace();
}

QDebug WarningLog(&QString());
QDebug DebugLog(&QString());
QDebug LogOut(&QString());

QDebug& _LogError(const char* pFile, int pLine)
{
	WarningLog.stream->ts.flush();
	return WarningLog << clock() * 1000 / CLOCKS_PER_SEC << " - ("
					 << ((strrchr(pFile, '/')
							  ? strrchr(pFile, '/')
							  : (strrchr(pFile, '\\') ? strrchr(pFile, '\\')
													  : pFile - 1)) +
						 1)
					 << ":" << pLine << "): [Error] ";
}
QDebug& _LogDebug(const char* pFile, int pLine)
{
	DebugLog.stream->ts.flush();
	return DebugLog << clock() * 1000 / CLOCKS_PER_SEC << " - ("
					 << ((strrchr(pFile, '/')
							  ? strrchr(pFile, '/')
							  : (strrchr(pFile, '\\') ? strrchr(pFile, '\\')
													  : pFile - 1)) +
						 1)
					 << ":" << pLine << "): [Debug] ";
}
QDebug& _Log(const char* pFile, int pLine)
{
	LogOut.stream->ts.flush();
	return LogOut << clock() * 1000 / CLOCKS_PER_SEC << " - ("
					 << ((strrchr(pFile, '/')
							  ? strrchr(pFile, '/')
							  : (strrchr(pFile, '\\') ? strrchr(pFile, '\\')
													  : pFile - 1)) +
						 1)
					 << ":" << pLine << "): ";
}

#if DEBUG__LOGGINGTOCONSOLE
void InitLogging() { LogDebug << "Logging to console window." << "\n"; }
#else
namespace
{
	std::ofstream gLogStream;
}

class LogDevice : public QIODevice
{

protected:
	qint64 readData(char* data, qint64 maxlen) override
	{ return maxlen; }

	qint64 writeData(const char* data, qint64 len) override
	{
		OutputDebugString(data);
		return len;
	}
};

LogDevice gLogDevice;

void InitLogging()
{
	gLogDevice.open(QIODevice::ReadWrite);
	WarningLog = QDebug(&gLogDevice);
	DebugLog = QDebug(&gLogDevice);
	LogOut = QDebug(&gLogDevice);
}
#endif
