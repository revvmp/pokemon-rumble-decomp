#ifndef NW4R_UT_ARCHIVE_FONT_BASE_H
#define NW4R_UT_ARCHIVE_FONT_BASE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_ResFontBase.h>
#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/ut/ut_binaryFileFormat.h>

#include <revolution/cx.h>

#include <string.h>

namespace nw4r {
namespace ut {

struct FontGlyphGroups {
    u32 sheetSize;
    u16 glyphsPerSheet;
    u16 numSet;
    u16 numSheet;
    u16 numCWDH;
    u16 numCMAP;
    u16 nameOffsets[];
};

struct FontGlyphGroupsBlock {
    BinaryBlockHeader header;
    FontGlyphGroups body;
};

namespace detail {

class FontGlyphGroupsAcs {
public:
    explicit FontGlyphGroupsAcs(const void* pBrfna) {
        mpFileTop = static_cast<const u8*>(pBrfna);
        mpData = reinterpret_cast<const FontGlyphGroupsBlock*>(mpFileTop + sizeof(BinaryFileHeader));
        mSizeSheetFlags = (GetNumSheet() + 31) / 32 * sizeof(u32);
        mSizeCWDHFlags = (GetNumCWDH() + 31) / 32 * sizeof(u32);
        mSizeCMAPFlags = (GetNumCMAP() + 31) / 32 * sizeof(u32);
        const u32 offsetSizeSheets =
            RoundUp(sizeof(BinaryFileHeader) + sizeof(BinaryBlockHeader) + sizeof(FontGlyphGroups) -
                          sizeof(u16) + GetNumSet() * sizeof(u16),
                      4);
        const u32 offsetSizeCWDH = RoundUp(offsetSizeSheets + GetNumSheet() * sizeof(u32), 4);
        const u32 offsetSizeCMAP = RoundUp(offsetSizeCWDH + GetNumCWDH() * sizeof(u32), 4);
        const u32 offsetUseSheets = RoundUp(offsetSizeCMAP + GetNumCMAP() * sizeof(u32), 4);
        const u32 offsetUseCWDH = RoundUp(offsetUseSheets + mSizeSheetFlags * GetNumSet(), 4);
        const u32 offsetUseCMAP = RoundUp(offsetUseCWDH + mSizeCWDHFlags * GetNumSet(), 4);
        mpSizeSheetsArray = reinterpret_cast<const u32*>(offsetSizeSheets + reinterpret_cast<u32>(mpFileTop));
        mpSizeCWDHArray = reinterpret_cast<const u32*>(offsetSizeCWDH + reinterpret_cast<u32>(mpFileTop));
        mpSizeCMAPArray = reinterpret_cast<const u32*>(offsetSizeCMAP + reinterpret_cast<u32>(mpFileTop));
        mpUseSheetArray = reinterpret_cast<const u32*>(offsetUseSheets + reinterpret_cast<u32>(mpFileTop));
        mpUseCWDHArray = reinterpret_cast<const u32*>(offsetUseCWDH + reinterpret_cast<u32>(mpFileTop));
        mpUseCMAPArray = reinterpret_cast<const u32*>(offsetUseCMAP + reinterpret_cast<u32>(mpFileTop));
    }

    u32 GetSheetSize() const { return mpData->body.sheetSize; }
    u16 GetGlyphsPerSheet() const { return mpData->body.glyphsPerSheet; }
    u16 GetNumSet() const { return mpData->body.numSet; }
    u16 GetNumSheet() const { return mpData->body.numSheet; }
    u16 GetNumCWDH() const { return mpData->body.numCWDH; }
    u16 GetNumCMAP() const { return mpData->body.numCMAP; }

    const char* GetSetName(int setNo) const {
        return static_cast<const char*>(AddOffsetToPtr(mpFileTop, mpData->body.nameOffsets[setNo]));
    }

    u32 GetSizeCWDH(int index) const { return mpSizeCWDHArray[index]; }
    u32 GetSizeCMAP(int index) const { return mpSizeCMAPArray[index]; }

    u32 GetUseSheetFlags(int setNo, int flagSetNo) const {
        return mpUseSheetArray[setNo * mSizeSheetFlags / sizeof(u32) + flagSetNo];
    }
    u32 GetUseCWDHFlags(int setNo, int flagSetNo) const {
        return mpUseCWDHArray[setNo * mSizeCWDHFlags / sizeof(u32) + flagSetNo];
    }
    u32 GetUseCMAPFlags(int setNo, int flagSetNo) const {
        return mpUseCMAPArray[setNo * mSizeCMAPFlags / sizeof(u32) + flagSetNo];
    }

    bool IsUseSheet(int setNo, int sheetNo) const {
        return IsBitOn(mpUseSheetArray, setNo * mSizeSheetFlags * 8 + sheetNo);
    }

    static bool IsBitOn(const u32* pBits, u32 index) {
        return (pBits[index / 32] << (index % 32)) & 0x80000000;
    }

private:
    const u8* mpFileTop;
    const FontGlyphGroupsBlock* mpData;
    const u32* mpSizeSheetsArray;
    const u32* mpSizeCWDHArray;
    const u32* mpSizeCMAPArray;
    const u32* mpUseSheetArray;
    const u32* mpUseCWDHArray;
    const u32* mpUseCMAPArray;
    u32 mSizeSheetFlags;
    u32 mSizeCWDHFlags;
    u32 mSizeCMAPFlags;
};

class ArchiveFontBase : public ResFontBase {
public:
    static const char LOAD_GLYPH_ALL[1];

    ArchiveFontBase();
    virtual ~ArchiveFontBase();

    virtual CharWidths GetCharWidths(u16 ch) const;
    virtual bool HasGlyph(u16 ch) const;

protected:
    enum ConstructResult {
        CONSTRUCT_MORE_DATA,
        CONSTRUCT_FINISH,
        CONSTRUCT_ERROR,
        CONSTRUCT_CONTINUE
    };

    enum ConstructOp {
        CONSTRUCT_OP_DISPATCH,
        CONSTRUCT_OP_ANALYZE_FILE_HEADER,
        CONSTRUCT_OP_ANALYZE_GLGR,
        CONSTRUCT_OP_ANALYZE_FINF,
        CONSTRUCT_OP_ANALYZE_CMAP,
        CONSTRUCT_OP_ANALYZE_CWDH,
        CONSTRUCT_OP_ANALYZE_TGLP,
        CONSTRUCT_OP_PREPARE_COPY_SHEET,
        CONSTRUCT_OP_PREPARE_EXPAND_SHEET,
        CONSTRUCT_OP_COPY,
        CONSTRUCT_OP_SKIP,
        CONSTRUCT_OP_EXPAND,
        CONSTRUCT_OP_FATAL_ERROR,
        CONSTRUCT_OP_MAX
    };

    struct ConstructContext;

    class CachedStreamReader {
    public:
        void Init();
        void Attach(const void* pStream, u32 size);
        bool RequestData(ConstructContext* pContext, u32 size);

        u32 GetRemain() const {
            return (mStreamEnd - mStreamCur) + (mTempEnd - mTempCur);
        }

        u32 GetOffset() const {
            return (mStreamCur - mStreamHead) + (mTempCur - mTempHead);
        }

        void CopyTo(void* pDst, u32 size) {
            u32 tempRemain = mTempEnd - mTempCur;
            u8* pDstU8 = static_cast<u8*>(pDst);

            if (tempRemain >= size) {
                memcpy(pDstU8, mTempCur, size);
                mTempCur += size;
            } else {
                u32 streamSize = size - tempRemain;
                memcpy(pDstU8, mTempCur, tempRemain);
                memcpy(pDstU8 + tempRemain, mStreamCur, streamSize);
                mTempCur = mTempEnd;
                mStreamCur += streamSize;
            }
        }

        void MoveTo(void* pDst, u32 size) {
            u32 tempRemain = mTempEnd - mTempCur;
            u8* pDstU8 = static_cast<u8*>(pDst);

            if (tempRemain >= size) {
                memmove(pDstU8, mTempCur, size);
                mTempCur += size;
            } else {
                u32 streamSize = size - tempRemain;
                memmove(pDstU8, mTempCur, tempRemain);
                memmove(pDstU8 + tempRemain, mStreamCur, streamSize);
                mTempCur = mTempEnd;
                mStreamCur += streamSize;
            }
        }

        void Skip(u32 size) {
            u32 tempRemain = mTempEnd - mTempCur;

            if (tempRemain > size) {
                mTempCur += size;
            } else {
                u32 streamSize = size - tempRemain;
                mTempCur = mTempEnd;
                mStreamCur += streamSize;
            }
        }

        const u8* mStreamHead;
        const u8* mStreamCur;
        const u8* mStreamEnd;
        const u8* mTempHead;
        const u8* mTempCur;
        const u8* mTempEnd;
        u32 mRequireSize;
    };

    struct ConstructContext {
        u32 GetRemain() const {
            return pBufferEnd - pCurrent;
        }

        void Advance(u32 size) {
            pCurrent += size;
        }

        void CopyFrom(CachedStreamReader* pStream, u32 size) {
            pStream->CopyTo(pCurrent, size);
            Advance(size);
        }

        void Align(u32 alignment) {
            pCurrent = static_cast<u8*>(RoundUp(pCurrent, alignment));
        }

        void SetupTask(u32 task, u32 size, u32 next) {
            opSize = size;
            op = task;
            nextOp = next;
        }

        u32 ReduceTaskSize(u32 size) {
            if (size > opSize) {
                size = opSize;
            }

            opSize -= size;
            return opSize;
        }

        FontInformation* pFINF;
        FontWidth* pPrevCWDH;
        FontCodeMap* pPrevCMAP;
        u32 op;
        BinaryBlockHeader header;
        u32 streamOffset;
        CachedStreamReader stream;
        CXUncompContextHuffman* pUncomp;
        const char* pGlyphGroups;
        u16* pGlyphIndexAdjustArray;
        u8* pBufferBegin;
        u8* pBufferEnd;
        u8* pCurrent;
        u32 nextOp;
        u32 opSize;
        u32 numBlocks;
        u32 blockIndex;
        u16 sheetIndex;
        u16 numSheets;
        u16 glyphsPerSheet;
    };

    static const u32 SIGNATURE = FOURCC('R', 'F', 'N', 'A');
    static const u32 SIGNATURE_GLYPHGROUP = FOURCC('G', 'L', 'G', 'R');
    static const u32 SIGNATURE_FONTINFO = FOURCC('F', 'I', 'N', 'F');
    static const u32 SIGNATURE_CHARMAP = FOURCC('C', 'M', 'A', 'P');
    static const u32 SIGNATURE_CHARWIDTH = FOURCC('C', 'W', 'D', 'H');
    static const u32 SIGNATURE_TEXGLYPH = FOURCC('T', 'G', 'L', 'P');

    static const u16 ADJUST_OFFSET_SHEET_NOT_LOADED = 0xFFFF;
    static const u16 SHEET_FORMAT_COMPRESSED = 0x8000;
    static const u16 SHEET_FORMAT_MASK = 0x7FFF;

protected:
    void SetResourceBuffer(void* pBuffer, FontInformation* pInfo,
                           u16* pAdjustTable);
    void* RemoveResourceBuffer();

    u16 AdjustIndex(u16 index) const;

    static bool IncludeName(const char* pNameList, const char* pName);
    static bool IsValidResource(const void* pBrfna, u32 size);

    static ConstructResult ConstructOpDispatch(ConstructContext* pContext,
                                               CachedStreamReader* pStream);
    static ConstructResult
    ConstructOpAnalyzeFileHeader(ConstructContext* pContext,
                                 CachedStreamReader* pStream);
    static ConstructResult ConstructOpAnalyzeGLGR(ConstructContext* pContext,
                                                  CachedStreamReader* pStream);
    static ConstructResult ConstructOpAnalyzeFINF(ConstructContext* pContext,
                                                  CachedStreamReader* pStream);
    static ConstructResult ConstructOpAnalyzeCMAP(ConstructContext* pContext,
                                                  CachedStreamReader* pStream);
    static ConstructResult ConstructOpAnalyzeCWDH(ConstructContext* pContext,
                                                  CachedStreamReader* pStream);
    static ConstructResult ConstructOpAnalyzeTGLP(ConstructContext* pContext,
                                                  CachedStreamReader* pStream);
    static ConstructResult
    ConstructOpPrepareCopySheet(ConstructContext* pContext,
                                CachedStreamReader* pStream);
    static ConstructResult
    ConstructOpPrepareExpandSheet(ConstructContext* pContext,
                                  CachedStreamReader* pStream);
    static ConstructResult ConstructOpCopy(ConstructContext* pContext,
                                           CachedStreamReader* pStream);
    static ConstructResult ConstructOpSkip(ConstructContext* pContext,
                                           CachedStreamReader* pStream);
    static ConstructResult ConstructOpExpand(ConstructContext* pContext,
                                             CachedStreamReader* pStream);
    static ConstructResult ConstructOpFatalError(ConstructContext* pContext,
                                                 CachedStreamReader* pStream);

    static ConstructResult RequestData(ConstructContext* pContext,
                                       CachedStreamReader* pStream, u32 size) {
        pContext->streamOffset += pStream->GetOffset();
        return pStream->RequestData(pContext, size) ? CONSTRUCT_MORE_DATA
                                                    : CONSTRUCT_ERROR;
    }

private:
    u16* mGlyphIndexAdjustArray;
};

}
}
}

#endif
