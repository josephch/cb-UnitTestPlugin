
#ifndef __UNITTESTPROCESS_H__
#define __UNITTESTPROCESS_H__

#include <wx/process.h>

class UnitTest;

class UnitTestProcess : public wxProcess
{
	public:
		UnitTestProcess(UnitTest *parent);

		virtual void OnTerminate(int pid, int status) override;

	private:
		UnitTest *parent;
};

#endif // __UNITTESTPROCESS_H__
