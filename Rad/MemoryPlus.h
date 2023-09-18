#pragma once
#include <memory>

// Not needed in C++17
// eg AutoUniquePtr<char>(malloc(100), free)
template <class C, class F>
auto AutoUniquePtr(C c, F f)
{
    return std::unique_ptr<C, F>(c, f);
}

template<class P, class F>
struct HandleDeleter
{
    typedef P pointer;

    HandleDeleter() = delete;

    HandleDeleter(F f)
        : m_f(f)
    {
    }

    void operator()(pointer p) const noexcept
    {
        m_f(p);
    }

    F m_f;
};

template<class P, class F>
auto MakeUniqueHandle(P p, F f)
{
    return std::unique_ptr<P, HandleDeleter<P, F>>(p, f);
}

template<class P>
struct GlobalUnlocker
{
    typedef P pointer;

    GlobalUnlocker() = delete;

    GlobalUnlocker(HGLOBAL hGlobal)
        : m_hGlobal(hGlobal)
    {
    }

    void operator()(pointer p) const noexcept
    {
        GlobalUnlock(m_hGlobal);
    }

    HGLOBAL m_hGlobal;
};

template<class P>
auto AutoGlobalLock(HGLOBAL hGlobal)
{
    return std::unique_ptr<std::remove_pointer<P>, GlobalUnlocker<P>>(static_cast<P>(GlobalLock(hGlobal)), GlobalUnlocker<P>(hGlobal));
}

template <class T>
class out_ptr
{
public:
    typedef typename T::pointer pointer;

    out_ptr(T& t)
        : tt(t)
    {
    }

    operator pointer* ()
    {
        return &pp;
    }

    pointer* get()
    {
        return &pp;
    }

    ~out_ptr()
    {
        tt.reset(pp);
    }

private:
    T& tt;
    pointer pp = {};
};

// Not needed in C++17
template <class T>
auto OutPtr(T& p)
{
    return out_ptr<T>(p);
}
