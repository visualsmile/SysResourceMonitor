#ifndef SRMSCOPEONEXIT_H_2535F503_D9A6_4812_87B2_A5EEDF2A4C7E
#define SRMSCOPEONEXIT_H_2535F503_D9A6_4812_87B2_A5EEDF2A4C7E
/************************************************************************/
/* 退出宏
 主要用于资源释放，退出作用域时执行对应代码                                                                     */
/************************************************************************/
template <class customFun>
class Scope
{
public:
	Scope(customFun pFun)
		: m_pFun(std::move(pFun))
		, m_bActive(true)
	{
	}
	~Scope()
	{
		if (m_bActive)
			m_pFun();
	}
	void dismiss()
	{
		m_bActive = false;
	}
	Scope(Scope&& oOther)
		: m_pFun(std::move(oOther.m_pFun))
		, m_bActive(oOther.m_bActive)
	{
		oOther.dismiss();
	}

private:
	Scope& operator=(const Scope&)
	{

	};
	customFun m_pFun;
	bool m_bActive;
};

class ScopeOnExit
{
};

template <typename customFun>
Scope<customFun> operator+(ScopeOnExit, customFun&& fn)
{
	return Scope<customFun>(std::forward<customFun>(fn));
}

#define CONNECTION(s1,s2) s1##s2
#define CONNECT(s1,s2) CONNECTION(s1,s2)
#define CONNECT_BY_LINE(str) CONNECT(str,__LINE__)

#define SCOPE_EXIT \
	auto CONNECT_BY_LINE(SCOPE_) = ScopeOnExit() + [&]()

#endif //SRMSCOPEONEXIT_H_2535F503_D9A6_4812_87B2_A5EEDF2A4C7E
