#pragma once

class Process
{
	std::unique_ptr<HANDLE> process;
public:
	Process(DWORD pid)
	{
		process = std::unique_ptr<HANDLE>(new HANDLE(OpenProcess(PROCESS_VM_READ, false, pid)));
	}
	Process(DWORD pid, bool write)
	{
		if(write == true)
			process = std::unique_ptr<HANDLE>(new HANDLE(OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, pid)));
		else
			process = std::unique_ptr<HANDLE>(new HANDLE(OpenProcess(PROCESS_VM_READ, false, pid)));
	}

	~Process()
	{
		CloseHandle(*process);
	}

	HANDLE get()
	{
		return *process;
	}
};
