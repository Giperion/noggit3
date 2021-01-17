#include <noggit/Log.h>
#include <noggit/errorHandling.h>

#include <csignal>
#include <string>
#include <vector>

#ifndef WIN32
#include <execinfo.h>
#else
#include <errhandlingapi.h>
#include <win/StackWalker.h>
#endif

namespace noggit
{
	void printStacktrace()
	{
#ifndef WIN32
		std::vector<void*> frames(32);

		std::size_t actual(0);
		while ((actual = backtrace(frames.data(), frames.size())) ==
			   frames.size())
		{
			frames.resize(frames.size() * 2);
		}
		frames.resize(actual);
		char** strings(backtrace_symbols(frames.data(), frames.size()));

		LogError << "Obtained " << frames.size() << " stack frames."
				 << std::endl;

		for (size_t i(0); i < frames.size(); ++i)
			LogError << "- " << strings[i] << std::endl;

		free(strings);
#else
		StackWalker sw;
		sw.ShowCallstack();
#endif
	}

	namespace
	{
		void leave(int sig)
		{
			// Reset to defaults.
			signal(SIGABRT, SIG_DFL);
			signal(SIGFPE, SIG_DFL);
			signal(SIGILL, SIG_DFL);
			signal(SIGSEGV, SIG_DFL);
			signal(SIGTERM, SIG_DFL);

			std::string description;
			std::string sign;

			switch (sig)
			{
				case SIGABRT:
					sign = "SIGABRT";
					description =
						"Abnormal termination, such as instigated by the abort "
						"function. (Abort.)";
					break;
				case SIGFPE:
					sign = "SIGFPE";
					description =
						"Erroneous arithmetic operation, such as divide by 0 "
						"or overflow. (Floating point exception.)";
					break;
				case SIGILL:
					sign = "SIGILL";
					description =
						"An ‘invalid object program’ has been detected. This "
						"usually means that there is an illegal instruction in "
						"the program. (Illegal instruction.)";
					break;
				case SIGINT:
					sign = "SIGINT";
					description =
						"Interactive attention signal; on interactive systems "
						"this is usually generated by typing some ‘break-in’ "
						"key at the terminal. (Interrupt.)";
					break;
				case SIGSEGV:
					sign = "SIGSEGV";
					description =
						"Invalid storage access; most frequently caused by "
						"attempting to store some value in an object pointed "
						"to by a bad pointer. (Segment violation.)";
					break;
				case SIGTERM:
					sign = "SIGTERM";
					description =
						"Termination request made to the program. (Terminate.)";
					break;
				default:
					sign = "SIGUNK";
					description = "Unknown Exception!";
					break;
			}

			LogError
				<< "There was an exception of type \"" << sign << "\"\n\""
				<< description
				<< "\".\nPlease excuse the inconvenience. You may want to "
				   "report this error including the log to the developers.\n"
				<< std::endl;

			printStacktrace();

			exit(sig);
		}

#ifdef _WIN32
		LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS* ExceptionInfo)
		{
			auto code = ExceptionInfo->ExceptionRecord->ExceptionCode;

			switch (code)
			{
				case EXCEPTION_ACCESS_VIOLATION:
					LogError << "EXCEPTION_ACCESS_VIOLATION" << std::endl;
					break;
				case EXCEPTION_DATATYPE_MISALIGNMENT:
					LogError << "EXCEPTION_DATATYPE_MISALIGNMENT" << std::endl;
					break;
				case EXCEPTION_BREAKPOINT:
					LogError << "EXCEPTION_BREAKPOINT" << std::endl;
					break;
				case EXCEPTION_SINGLE_STEP:
					LogError << "EXCEPTION_SINGLE_STEP" << std::endl;
					break;
				case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
					LogError << "EXCEPTION_ARRAY_BOUNDS_EXCEEDED" << std::endl;
					break;
				case EXCEPTION_FLT_DENORMAL_OPERAND:
					LogError << "EXCEPTION_FLT_DENORMAL_OPERAND" << std::endl;
					break;
				case EXCEPTION_FLT_DIVIDE_BY_ZERO:
					LogError << "EXCEPTION_FLT_DIVIDE_BY_ZERO" << std::endl;
					break;
				case EXCEPTION_FLT_INEXACT_RESULT:
					LogError << "EXCEPTION_FLT_INEXACT_RESULT" << std::endl;
					break;
				case EXCEPTION_FLT_INVALID_OPERATION:
					LogError << "EXCEPTION_FLT_INVALID_OPERATION" << std::endl;
					break;
				case EXCEPTION_FLT_OVERFLOW:
					LogError << "EXCEPTION_FLT_OVERFLOW" << std::endl;
					break;
				case EXCEPTION_FLT_STACK_CHECK:
					LogError << "EXCEPTION_FLT_STACK_CHECK" << std::endl;
					break;
				case EXCEPTION_FLT_UNDERFLOW:
					LogError << "EXCEPTION_FLT_UNDERFLOW" << std::endl;
					break;
				case EXCEPTION_INT_DIVIDE_BY_ZERO:
					LogError << "EXCEPTION_INT_DIVIDE_BY_ZERO" << std::endl;
					break;
				case EXCEPTION_INT_OVERFLOW:
					LogError << "EXCEPTION_INT_OVERFLOW" << std::endl;
					break;
				case EXCEPTION_PRIV_INSTRUCTION:
					LogError << "EXCEPTION_PRIV_INSTRUCTION" << std::endl;
					break;
				case EXCEPTION_IN_PAGE_ERROR:
					LogError << "EXCEPTION_IN_PAGE_ERROR" << std::endl;
					break;
				case EXCEPTION_ILLEGAL_INSTRUCTION:
					LogError << "EXCEPTION_ILLEGAL_INSTRUCTION" << std::endl;
					break;
				case EXCEPTION_NONCONTINUABLE_EXCEPTION:
					LogError << "EXCEPTION_NONCONTINUABLE_EXCEPTION"
							 << std::endl;
					break;
				case EXCEPTION_STACK_OVERFLOW:
					LogError << "EXCEPTION_STACK_OVERFLOW" << std::endl;
					break;
				case EXCEPTION_INVALID_DISPOSITION:
					LogError << "EXCEPTION_INVALID_DISPOSITION" << std::endl;
					break;
				case EXCEPTION_GUARD_PAGE:
					LogError << "EXCEPTION_GUARD_PAGE" << std::endl;
					break;
				case EXCEPTION_INVALID_HANDLE:
					LogError << "EXCEPTION_INVALID_HANDLE" << std::endl;
					break;
#ifdef STATUS_POSSIBLE_DEADLOCK
				case EXCEPTION_POSSIBLE_DEADLOCK:
					LogError << "EXCEPTION_POSSIBLE_DEADLOCK" << std::endl;
					break;
#endif
				default:
					LogError << "Exception code=" << code << std::endl;
			}

			printStacktrace();

			return EXCEPTION_CONTINUE_SEARCH;
		}
#endif

	}  // namespace

	void RegisterErrorHandlers()
	{
#ifdef _WIN32
		SetUnhandledExceptionFilter(windows_exception_handler);
#endif

		signal(SIGABRT, leave);
		signal(SIGFPE, leave);
		signal(SIGILL, leave);
		signal(SIGSEGV, leave);
		signal(SIGTERM, leave);
	}
}  // namespace noggit
