#ifndef __OUTPUTREADER_H__
#define __OUTPUTREADER_H__

#include <wx/timer.h>

class UnitTest;
class UnitTestProcess;

class OutputReader : public wxTimer
{
	public:
		OutputReader(UnitTest *parent);
		~OutputReader();

		void Start();
		void ReadOutput();

	private:
		UnitTest *m_Parent;

		wxString outString, errString;

		virtual void Notify() override;

		void SendLine(const wxString &line);

};

#endif // __OUTPUTREADER_H__

