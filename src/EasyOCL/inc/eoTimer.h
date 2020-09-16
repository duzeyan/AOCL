
#include<string>
#if defined(_WIN32)
#include<Windows.h>
#else
#endif
namespace EOCL{
	class eoTimer
	{
	public:
		eoTimer(std::string des, int count) :m_description(des), m_count(count), m_dTimeElapse(0){};
		void Start();
		void End();
		double Print();
	private:
		int m_count;
		std::string m_description;
		LARGE_INTEGER  m_lStart;
		LARGE_INTEGER  m_lEnd;
		LARGE_INTEGER  m_lTimeElapse;
		double m_dTimeElapse;
	};

}