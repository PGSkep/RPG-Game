#ifndef	COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <vector>

struct Command
{
	void(*cmd)(void*);
	void* commandData;

	static inline Command GetCommand(void(*_cmd)(void*), void* _commandData)
	{
		Command command;
		command.cmd = _cmd;
		command.commandData = _commandData;
		return command;
	}
};

class CommandBuffer
{
	std::vector<uint8_t> commandData;
	size_t writeLocation = 0;

public:
	void Reserve(size_t newSize)
	{
		commandData.resize(newSize);
	}

	void* Allocate(size_t size)
	{
		if (writeLocation + size > commandData.size())
			Reserve(commandData.size() + size + 512);

		void* result = &commandData[writeLocation];
		writeLocation += size;
		return result;
	}

	void* Allocate(size_t size, void* dataSrc)
	{
		if (writeLocation + size > commandData.size())
			Reserve(commandData.size() + size + 512);

		void* result = &commandData[writeLocation];

		memcpy(result, dataSrc, size);

		writeLocation += size;
		return result;
	}

	void Reset()
	{
		writeLocation = 0;
	}
};

#endif