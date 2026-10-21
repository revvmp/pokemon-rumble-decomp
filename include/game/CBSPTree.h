#ifndef GAME_CBSPTREE_H
#define GAME_CBSPTREE_H

class CBSPTree {
public:
    CBSPTree();
    ~CBSPTree();

    void SetData(void* pData);

private:
    void* mpData;
};

#endif
