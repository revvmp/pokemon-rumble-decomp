#include <game/IDlgPiiList.h>

#include <memory>

namespace {

class CDlgPiiListImpl;

class CDlgPiiList : public IDlgPiiList, public std::tr1::enable_shared_from_this<CDlgPiiList> {
public:
    CDlgPiiList() {}
    virtual ~CDlgPiiList() {}
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual bool IsClosable() { return false; }
    virtual bool Close();
    virtual int GetType();
    virtual bool Show(int player, int type);

private:
    std::tr1::shared_ptr<CDlgPiiListImpl> mImpl;
};

}

std::tr1::shared_ptr<IDlgPiiList> IDlgPiiList::Create(bool) {
    return std::tr1::shared_ptr<CDlgPiiList>(new CDlgPiiList);
}
