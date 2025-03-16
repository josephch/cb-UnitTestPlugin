
#include "UnitTestProcess.h"
#include "UnitTest.h"

#include <iostream>

UnitTestProcess::UnitTestProcess(UnitTest *parent) : parent(parent)
{
}

void UnitTestProcess::OnTerminate(int pid, int status)
{
	std::cout << "Terminated" << std::endl;

	parent->OnTestRunFinished();
}
