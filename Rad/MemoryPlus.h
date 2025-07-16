#pragma once
#include <memory>

// Not needed in C++17
// eg AutoUniquePtr<char>(malloc(100), free)
template <class C, class F>
auto AutoUniquePtr(typename std::unique_ptr<C, F>::pointer c, F f)
{
    return std::unique_ptr<C, F>(c, f);
}

template<class P, class F>
auto MakeUniqueHandle(P p, F f)
{
    return std::unique_ptr<std::remove_pointer_t<P>, F>(p, f);
}

inline auto AutoGetDC(HWND hWnd) { return MakeUniqueHandle(GetDC(hWnd), [hWnd](HDC hDC) { ReleaseDC(hWnd, hDC); }); }
inline auto AutoSelectObject(HDC hDC, HGDIOBJ hObj) { return MakeUniqueHandle(SelectObject(hDC, hObj), [hDC](HGDIOBJ hObj) { SelectObject(hDC, hObj); }); }

template<class P> auto AutoGlobalLock(HGLOBAL hGlobal) { return MakeUniqueHandle(static_cast<P>(GlobalLock(hGlobal)), [hGlobal](P) { GlobalUnlock(hGlobal); }); }

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
