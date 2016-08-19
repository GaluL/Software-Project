#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SP_LOGGER_OPEN_MODE "wb"

#define SP_LOGGER_ERROR_MSG_TITLE "---ERROR---"
#define SP_LOGGER_WARNING_MSG_TITLE "---WARNING---"
#define SP_LOGGER_INFO_MSG_TITLE "---INFO---"
#define SP_LOGGER_DEBUG_MSG_TITLE "---DEBUG---"

#define SP_LOGGER_MSG_FILE_SECTION "- file:"
#define SP_LOGGER_MSG_FUNC_SECTION "- function:"
#define SP_LOGGER_MSG_LINE_SECTION "- line:"
#define SP_LOGGER_MSG_MSG_SECTION "- message:"

#define NEW_LINE '\n'

#define MAX_INT_CHARACTERS 11

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

typedef enum sp_logger_msg_type_t {
	SP_LOGGER_ERROR_MSG, //Error level
	SP_LOGGER_WARNING_MSG, //Warning level
	SP_LOGGER_INFO_MSG, //Info level
	SP_LOGGER_DEBUG_MSG,
	SP_LOGGER_NO_TITLE_MSG
} SP_LOGGER_MSG_TYPE;

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

const char* getMsgTitleByType(SP_LOGGER_MSG_TYPE msg_type)
{
	switch (msg_type)
	{
		case SP_LOGGER_ERROR_MSG:
		{
			return SP_LOGGER_ERROR_MSG_TITLE;
		}
		case SP_LOGGER_WARNING_MSG:
		{
			return SP_LOGGER_WARNING_MSG_TITLE;
		}
		case SP_LOGGER_INFO_MSG:
		{
			return SP_LOGGER_INFO_MSG_TITLE;
		}
		case SP_LOGGER_DEBUG_MSG:
		{
			return SP_LOGGER_DEBUG_MSG_TITLE;
		}
		default:
		{
			return NULL;
		}
	}
}

SP_LOGGER_MSG spLoggerPrintGenericMsg(SP_LOGGER_MSG_TYPE msg_type, const char* msg,
		int include_func_data, const char* file, const char* function, const int line)
{
	const char* msg_title = NULL;

	// Checks if logger is defined
	if (!logger)
	{
		return SP_LOGGER_UNDIFINED;
	}

	// Checks if msg received and if function data is needed if the metadata has been
	// received
	if (!msg || ((!file || !function || line < 0) && include_func_data))
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	// Getting the massage type
	msg_title = getMsgTitleByType(msg_type);
	if (msg_title)
	{
		// Writing the massage title
		if (fprintf(logger->outputChannel, "%s\n", msg_title) == EOF)
		{
			return SP_LOGGER_WRITE_FAIL;
		}
	}

	// Checks if misfunctioning method data shell be included in msg
	if (include_func_data)
	{
		// Writing the file name of the the misfunctioning method
		if (fprintf(logger->outputChannel, "%s %s\n", SP_LOGGER_MSG_FILE_SECTION,
				file) == EOF)
		{
			return SP_LOGGER_WRITE_FAIL;
		}

		// Writing the misfunctioning function name
		if (fprintf(logger->outputChannel, "%s %s\n", SP_LOGGER_MSG_FUNC_SECTION,
				function) == EOF)
		{
			return SP_LOGGER_WRITE_FAIL;
		}

		// Writing the misfunctioning method line
		if (fprintf(logger->outputChannel, "%s %d\n", SP_LOGGER_MSG_LINE_SECTION, line)
				== EOF)
		{
			return SP_LOGGER_WRITE_FAIL;
		}
	}

	// Writing the error massage
	if (fprintf(logger->outputChannel, "%s %s\n", SP_LOGGER_MSG_MSG_SECTION, msg)
			== EOF)
	{
		return SP_LOGGER_WRITE_FAIL;
	}

	return SP_LOGGER_SUCCESS;
}

/**
 * 	Prints error message. The error message format is given below:
 * 	---ERROR---
 * 	- file: <file>
 *  - function: <function>
 *  - line: <line>
 *  - message: <msg>
 *
 * 	<file> 	   - is the string given by file, it represents the file in which
 * 		   		 the error print call occurred.
 * 	<function> - is the string given by function, it represents the function in which
 * 			   	 the error print call occurred.
 * 	<line> 	   - is the string given by line, it represents the line in which
 * 		   		 the error print call occurred
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 * 	Error messages will be printed at levels:
 *
 * 	SP_LOGGER_ERROR_LEVEL,
 *	SP_LOGGER_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_INFO_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *
 * 	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintError call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintError call ocurred
 * @param line		- A string representing the line in which the function call occurred
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line)
{
	if (!logger)
	{
		return SP_LOGGER_UNDIFINED;
	}

	if (!msg || !file || !function || line < 0)
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	// calling the print method and return it's return value
	return spLoggerPrintGenericMsg(SP_LOGGER_ERROR_MSG, msg, true, file, function, line);
}

/**
 * 	Prints warning message. The warning message format is given below:
 * 	---WARNING---
 * 	- file: <file>
 *  - function: <function>
 *  - line: <line>
 *  - message: <msg>
 *
 * 	<file> 	   - is the string given by file, it represents the file in which
 * 		   		 the warning print call occurred.
 * 	<function> - is the string given by function, it represents the function in which
 * 			   	 the warning print call occurred.
 * 	<line> 	   - is the string given by line, it represents the line in which
 * 		   		 the warning print call occurred
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 * 	Warning messages will be printed at levels:
 *
 *	SP_LOGGER_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_INFO_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *
 *	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintWarning call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintWarning call ocurred
 * @param line		- A string representing the line in which the spLoggerPrintWarning call occurred
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line)
{
	if (!logger)
	{
		return SP_LOGGER_UNDIFINED;
	}

	if (!msg || !file || !function || line < 0)
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	// Check and if meeting error level demand calls the printing method
	if (logger->level >= SP_LOGGER_WARNING_ERROR_LEVEL)
	{
		return spLoggerPrintGenericMsg(SP_LOGGER_WARNING_MSG, msg, true, file, function, line);
	}

	// If no error printing needed - return success
	return SP_LOGGER_SUCCESS;
}

/**
 * 	Prints Info message. The info message format is given below:
 * 	---INFO---
 *  - message: <msg>
 *
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 * 	Info messages will be printed at levels:
 *
 *	SP_LOGGER_INFO_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *
 * 	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If msg is null
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintInfo(const char* msg)
{
	if (!logger)
	{
		return SP_LOGGER_UNDIFINED;
	}

	if (!msg)
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	// Check and if meeting error level demand calls the printing method
	if (logger->level >= SP_LOGGER_INFO_WARNING_ERROR_LEVEL)
	{
		return spLoggerPrintGenericMsg(SP_LOGGER_INFO_MSG, msg, false, NULL, NULL, 0);
	}

	// If no error printing needed - return success
	return SP_LOGGER_SUCCESS;
}

/**
 * 	Prints the debug message. The debug message format is given below:
 * 	---DEBUG---
 * 	- file: <file>
 *  - function: <function>
 *  - line: <line>
 *  - message: <msg>
 *
 * 	<file> 	   - is the string given by file, it represents the file in which
 * 		   		 the debug print call occurred.
 * 	<function> - is the string given by function, it represents the function in which
 * 			   	 the debug print call occurred.
 * 	<line> 	   - is the string given by line, it represents the line in which
 * 		   		 the debug print call occurred
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 * 	Debug messages will be printed at level:
 *
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *
 * 	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintWarning call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintWarning call ocurred
 * @param line		- A string representing the line in which the function call occurred
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line)
{
	if (!logger)
	{
		return SP_LOGGER_UNDIFINED;
	}

	if (!msg || !file || !function || line < 0)
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	// Check and if meeting error level demand calls the printing method
	if (logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL)
	{
		return spLoggerPrintGenericMsg(SP_LOGGER_DEBUG_MSG, msg, true, file, function, line);
	}

	// If no error printing needed - return success
	return SP_LOGGER_SUCCESS;
}

/**
 * The given message is printed. A new line is printed at the end of msg
 * The message will be printed in all levels.
 *
 * @param msg - The message to be printed
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If msg is null
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintMsg(const char* msg)
{
	if (!logger)
	{
		return SP_LOGGER_UNDIFINED;
	}

	if (!msg)
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	// calling the print method and return it's return value
	return spLoggerPrintGenericMsg(SP_LOGGER_NO_TITLE_MSG, msg, false, NULL, NULL, 0);
}
