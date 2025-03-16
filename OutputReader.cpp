#include "OutputReader.h"
#include "UnitTest.h"
#include "UnitTestProcess.h"

#include <iostream>

using namespace std;

OutputReader::OutputReader(UnitTest *parent) : m_Parent(parent)
{
}

OutputReader::~OutputReader()
{
}

void OutputReader::Start()
{
	outString = _("");
	errString = _("");

	wxTimer::Start(10, false);
}

void OutputReader::Notify()
{
	ReadOutput();
}

void OutputReader::ReadOutput()
{
	UnitTestProcess *process = m_Parent->GetProcess();

	if (process != nullptr)
	{
		wxInputStream *outputStream = process->GetInputStream();
		wxInputStream *errorStream = process->GetErrorStream();

		// read
		if (outputStream != nullptr)
		{
			while (process->IsInputAvailable() == true)
			{
				if (outputStream->Eof() == true)
				{
					SendLine(outString);
					outString = _("");
					break;
				}

				wxChar c = outputStream->GetC();

				if (c == '\n')
				{
					SendLine(outString);
					outString = _("");
				}
				else
				{
					outString += c;
				}
			}
		}

		if (errorStream != nullptr)
		{
			while (process->IsErrorAvailable() == true)
			{
				if (errorStream->Eof() == true)
				{
					SendLine(errString);
					errString = _("");
					break;
				}

				wxChar c = errorStream->GetC();

				if (c == '\n')
				{
					SendLine(errString);
					errString = _("");
				}
				else
				{
					errString += c;
				}
			}
		}
	}
}

void OutputReader::SendLine(const wxString &line)
{
	m_Parent->OnOutput(line);
}
