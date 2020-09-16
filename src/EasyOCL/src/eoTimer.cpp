#include"eoTimer.h"


void EOCL::eoTimer::Start()
{
	QueryPerformanceFrequency(&m_lTimeElapse);
	QueryPerformanceCounter(&m_lStart);
}
void EOCL::eoTimer::End()
{
	QueryPerformanceCounter(&m_lEnd);
	m_dTimeElapse += (double)(m_lEnd.QuadPart - m_lStart.QuadPart) / m_lTimeElapse.QuadPart;

}
double EOCL::eoTimer::Print()
{
	printf("%s: ave %8.6lf ms in %d times\n", m_description.c_str(), 1000 * m_dTimeElapse / m_count, m_count);
	return 1000 * m_dTimeElapse / m_count;
}