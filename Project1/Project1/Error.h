#ifndef	ERROR_H
#define ERROR_H

#include <string>

#define PUSH_ERROR(contextSTR) \
error.push_back({__LINE__, __FILE__, contextSTR});
#define PUSH_DEFAULT \
error.push_back({__LINE__, __FILE__, GetStrFromVkResult(vkResult)});

struct Error
{
	int32_t line;
	std::string file;
	std::string context;

	static void Add(std::vector<Error>& _errorsA, std::vector<Error> _errorsB)
	{
		size_t errorACount = _errorsA.size();
		_errorsA.resize(_errorsA.size() + _errorsB.size());

		for (size_t i = 0; i != _errorsB.size(); ++i)
			_errorsA[errorACount + i] = _errorsB[i];
	}
};



#endif