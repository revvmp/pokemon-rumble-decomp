#ifndef GAME_IDLGINFO_H
#define GAME_IDLGINFO_H

#include <game/ui/IDlg.h>
#include <memory>

class IDlgInfo : public IDlg {
public:
    virtual ~IDlgInfo() {}
    virtual bool IsShown() = 0;
    virtual bool Show(int player) = 0;
    virtual void Clear() = 0;
    virtual void SetTitle(int page, const wchar_t* pTitle) = 0;
    virtual void SetLine(int page, unsigned int line, const wchar_t* pFirst, const wchar_t* pSecond, const wchar_t* pThird) = 0;
    virtual void SetText(int page, unsigned int line, const wchar_t* pText) = 0;

    static std::tr1::shared_ptr<IDlgInfo> Create(bool flag);
};

#endif
