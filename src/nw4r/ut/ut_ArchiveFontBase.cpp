#include <nw4r/ut/ut_ArchiveFontBase.h>

namespace nw4r {
namespace ut {
namespace detail {
namespace {

inline bool IsNullString(const char* pStr) {
    return *pStr == '\0';
}

inline bool IsBitOn(const u32* pBits, u32 index) {
    return (pBits[index / 32] << (index % 32)) & 0x80000000;
}

}



ArchiveFontBase::ArchiveFontBase() : mGlyphIndexAdjustArray(nullptr) {}

ArchiveFontBase::~ArchiveFontBase() {}

CharWidths ArchiveFontBase::GetCharWidths(u16 ch) const {
    u16 index = GetGlyphIndex(ch);

    if (AdjustIndex(index) == GLYPH_INDEX_NOT_FOUND) {
        index = mFontInfo->alterCharIndex;
    }

    return GetCharWidthsFromIndex(index);
}

bool ArchiveFontBase::HasGlyph(u16 ch) const {
    u16 index = FindGlyphIndex(ch);

    if (index != GLYPH_INDEX_NOT_FOUND) {
        return AdjustIndex(index) != GLYPH_INDEX_NOT_FOUND;
    }

    return false;
}

void ArchiveFontBase::SetResourceBuffer(void* pBuffer, FontInformation* pInfo,
                                        u16* pAdjustTable) {
    ResFontBase::SetResourceBuffer(pBuffer, pInfo);
    mGlyphIndexAdjustArray = pAdjustTable;
}

void* ArchiveFontBase::RemoveResourceBuffer() {
    mGlyphIndexAdjustArray = nullptr;
    return ResFontBase::RemoveResourceBuffer();
}

u16 ArchiveFontBase::AdjustIndex(u16 index) const {
    const FontTextureGlyph* pGlyph = mFontInfo->pGlyph;
    int glyphsPerSheet = pGlyph->sheetRow * pGlyph->sheetLine;

    u16 offset = mGlyphIndexAdjustArray[index / glyphsPerSheet];
    if (offset == ADJUST_OFFSET_SHEET_NOT_LOADED) {
        return GLYPH_INDEX_NOT_FOUND;
    }

    return index - offset;
}

bool ArchiveFontBase::IncludeName(const char* pNameList, const char* pName) {
    u32 nameLen = strlen(pName);
    const char* pFound = pNameList - 1;

    while (true) {
        pFound = strstr(pFound + 1, pName);
        if (pFound == nullptr) {
            return false;
        }

        if (pFound != pNameList) {
            const char* pPrev = pFound - 1;
            while (pPrev > pNameList && *pPrev == ' ') {
                pPrev--;
            }

            if (*pPrev != ',') {
                continue;
            }
        }

        const char* pComma = strchr(pFound, ',');
        u32 len = pComma != nullptr ? pComma - pFound : strlen(pFound);

        const char* pNext = pFound + nameLen;
        const char* pEnd = pFound + len;

        while (pNext < pEnd && *pNext == ' ') {
            pNext++;
        }

        if (pNext == pEnd) {
            return true;
        }
    }
}

bool ArchiveFontBase::IsValidResource(const void* pBrfna, u32 size) {
    const BinaryFileHeader* pHeader =
        static_cast<const BinaryFileHeader*>(pBrfna);

    if (!IsValidBinaryFile(pHeader, SIGNATURE, NW4R_VERSION(1, 4), 2)) {
        return false;
    }

    const FontGlyphGroupsBlock* pGLGR =
        reinterpret_cast<const FontGlyphGroupsBlock*>(pHeader + 1);

    if (pGLGR->header.kind != SIGNATURE_GLYPHGROUP) {
        return false;
    }

    return pGLGR->header.size + sizeof(BinaryFileHeader) <= size;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpDispatch(ConstructContext* pContext,
                                     CachedStreamReader* pStream) {
    if (pContext->blockIndex >= pContext->numBlocks) {
        return CONSTRUCT_FINISH;
    }

    if (pStream->GetRemain() < sizeof(BinaryBlockHeader)) {
        return RequestData(pContext, pStream, sizeof(BinaryBlockHeader));
    }

    pStream->CopyTo(&pContext->header, sizeof(BinaryBlockHeader));

    switch (pContext->header.kind) {
    case SIGNATURE_GLYPHGROUP: {
        pContext->op = CONSTRUCT_OP_ANALYZE_GLGR;
        break;
    }

    case SIGNATURE_FONTINFO: {
        pContext->op = CONSTRUCT_OP_ANALYZE_FINF;
        break;
    }

    case SIGNATURE_CHARMAP: {
        pContext->op = CONSTRUCT_OP_ANALYZE_CMAP;
        break;
    }

    case SIGNATURE_CHARWIDTH: {
        pContext->op = CONSTRUCT_OP_ANALYZE_CWDH;
        break;
    }

    case SIGNATURE_TEXGLYPH: {
        pContext->op = CONSTRUCT_OP_ANALYZE_TGLP;
        break;
    }

    default: {
        pContext->op = CONSTRUCT_OP_FATAL_ERROR;
        return CONSTRUCT_ERROR;
    }
    }

    pContext->blockIndex++;
    pContext->Align(4);

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpAnalyzeFileHeader(ConstructContext* pContext,
                                              CachedStreamReader* pStream) {
    if (pStream->GetRemain() < sizeof(BinaryFileHeader)) {
        return RequestData(pContext, pStream, sizeof(BinaryFileHeader));
    }

    if (pContext->GetRemain() < sizeof(BinaryFileHeader)) {
        return CONSTRUCT_ERROR;
    }

    pStream->CopyTo(pContext->pCurrent, sizeof(BinaryFileHeader));
    pContext->op = CONSTRUCT_OP_DISPATCH;

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpAnalyzeGLGR(ConstructContext* pContext,
                                        CachedStreamReader* pStream) {
    u8* pFileTop = pContext->pCurrent;
    const u32 bodySize = pContext->header.size - sizeof(BinaryBlockHeader);

    if (reinterpret_cast<BinaryFileHeader*>(pFileTop)->signature != SIGNATURE) {
        return CONSTRUCT_ERROR;
    }

    if (pStream->GetRemain() < bodySize) {
        return RequestData(pContext, pStream, bodySize);
    }

    if (pContext->GetRemain() < bodySize) {
        return CONSTRUCT_ERROR;
    }

    BinaryBlockHeader* pBlockHeader =
        reinterpret_cast<BinaryBlockHeader*>(pFileTop + sizeof(BinaryFileHeader));
    const u32 dataSize = pContext->header.size + sizeof(BinaryFileHeader);
    u8* pBlockEnd = reinterpret_cast<u8*>(pBlockHeader) + pContext->header.size;

    memcpy(pBlockHeader, &pContext->header, sizeof(BinaryBlockHeader));
    pStream->CopyTo(pBlockHeader + 1, bodySize);

    if (!IsValidResource(pFileTop, dataSize)) {
        return CONSTRUCT_ERROR;
    }

    FontGlyphGroupsAcs gg(pFileTop);
    const u16 numSheet = gg.GetNumSheet();
    const int glyphsPerSheet = gg.GetGlyphsPerSheet();
    const u32 numBlocks = reinterpret_cast<BinaryFileHeader*>(pFileTop)->dataBlocks;
    const int sizeAdjustTable = (RoundUp)(numSheet * sizeof(u16), 4);

    const u32 remain = pContext->GetRemain();
    u16* pAdjustTable = static_cast<u16*>((RoundDown)(pFileTop + remain - sizeAdjustTable, 2));

    if (remain < (pBlockEnd - pFileTop) + sizeAdjustTable) {
        return CONSTRUCT_ERROR;
    }

    for (int i = 0; i < gg.GetNumSheet(); i++) {
        pAdjustTable[i] = 0;
    }

    for (int setNo = 0; setNo < gg.GetNumSet(); setNo++) {
        const char* pSetName = gg.GetSetName(setNo);

        if (pContext->pGlyphGroups[0] != '\0' && !IncludeName(pContext->pGlyphGroups, pSetName)) {
            continue;
        }

        for (int sheetNo = 0; sheetNo < gg.GetNumSheet(); sheetNo++) {
            if (gg.IsUseSheet(setNo, sheetNo)) {
                pAdjustTable[sheetNo] = 1;
            }
        }
    }

    {
        u32 adjust = 0;

        for (int i = 0; i < gg.GetNumSheet(); i++) {
            if (pAdjustTable[i] == 1) {
                pAdjustTable[i] = adjust;
            } else {
                pAdjustTable[i] = ADJUST_OFFSET_SHEET_NOT_LOADED;
                adjust += gg.GetGlyphsPerSheet();
            }
        }
    }

    u16* pDst = reinterpret_cast<u16*>(pContext->pCurrent);
    pContext->Advance(sizeAdjustTable);
    memmove(pDst, pAdjustTable, ((pAdjustTable + numSheet) - pAdjustTable) * sizeof(u16));

    pContext->pGlyphIndexAdjustArray = pDst;
    pContext->numSheets = numSheet;
    pContext->glyphsPerSheet = glyphsPerSheet;
    pContext->numBlocks = numBlocks;
    pContext->op = CONSTRUCT_OP_DISPATCH;

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpAnalyzeFINF(ConstructContext* pContext,
                                        CachedStreamReader* pStream) {
    u32 size = pContext->header.size - sizeof(BinaryBlockHeader);

    if (pContext->GetRemain() < size) {
        return CONSTRUCT_ERROR;
    }

    pContext->pFINF = reinterpret_cast<FontInformation*>(pContext->pCurrent);
    pContext->SetupTask(CONSTRUCT_OP_COPY, size, CONSTRUCT_OP_DISPATCH);

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpAnalyzeCMAP(ConstructContext* pContext,
                                        CachedStreamReader* pStream) {
    FontCodeMap* pMap = reinterpret_cast<FontCodeMap*>(pContext->pCurrent);

    if (pContext->pPrevCMAP != nullptr) {
        pContext->pPrevCMAP->pNext = pMap;
    } else {
        pContext->pFINF->pMap = pMap;
    }

    pContext->pPrevCMAP = pMap;

    u32 size = pContext->header.size - sizeof(BinaryBlockHeader);

    if (pContext->GetRemain() < size) {
        return CONSTRUCT_ERROR;
    }

    pContext->SetupTask(CONSTRUCT_OP_COPY, size, CONSTRUCT_OP_DISPATCH);

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpAnalyzeCWDH(ConstructContext* pContext,
                                        CachedStreamReader* pStream) {
    FontWidth* pWidth = reinterpret_cast<FontWidth*>(pContext->pCurrent);

    if (pContext->pPrevCWDH != nullptr) {
        pContext->pPrevCWDH->pNext = pWidth;
    } else {
        pContext->pFINF->pWidth = pWidth;
    }

    pContext->pPrevCWDH = pWidth;

    u32 size = pContext->header.size - sizeof(BinaryBlockHeader);

    if (pContext->GetRemain() < size) {
        return CONSTRUCT_ERROR;
    }

    pContext->SetupTask(CONSTRUCT_OP_COPY, size, CONSTRUCT_OP_DISPATCH);

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpAnalyzeTGLP(ConstructContext* pContext,
                                        CachedStreamReader* pStream) {
    if (pStream->GetRemain() < sizeof(FontTextureGlyph)) {
        return RequestData(pContext, pStream, sizeof(FontTextureGlyph));
    }

    if (pContext->GetRemain() < sizeof(FontTextureGlyph)) {
        return CONSTRUCT_ERROR;
    }

    pContext->pFINF->pGlyph =
        reinterpret_cast<FontTextureGlyph*>(pContext->pCurrent);

    pContext->CopyFrom(pStream, sizeof(FontTextureGlyph));

    bool compressed =
        (pContext->pFINF->pGlyph->sheetFormat & SHEET_FORMAT_COMPRESSED) != 0;
    pContext->pFINF->pGlyph->sheetFormat &= SHEET_FORMAT_MASK;

    u16 numSheet = 0;
    for (int i = 0; i < pContext->numSheets; i++) {
        if (pContext->pGlyphIndexAdjustArray[i] !=
            ADJUST_OFFSET_SHEET_NOT_LOADED) {
            numSheet++;
        }
    }

    pContext->pFINF->pGlyph->sheetNum = numSheet;

    u32 sheetOffset =
        reinterpret_cast<u32>(pContext->pFINF->pGlyph->sheetImage);

    pContext->Align(32);
    pContext->pFINF->pGlyph->sheetImage = pContext->pCurrent;

    u32 nextOp = compressed ? CONSTRUCT_OP_PREPARE_EXPAND_SHEET
                            : CONSTRUCT_OP_PREPARE_COPY_SHEET;

    pContext->SetupTask(CONSTRUCT_OP_SKIP,
                        sheetOffset -
                            (pContext->streamOffset + pStream->GetOffset()),
                        nextOp);

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpPrepareCopySheet(ConstructContext* pContext,
                                             CachedStreamReader* pStream) {
    if (pContext->sheetIndex >= pContext->numSheets) {
        pContext->op = CONSTRUCT_OP_DISPATCH;
        return CONSTRUCT_CONTINUE;
    }

    u32 sheetSize = pContext->pFINF->pGlyph->sheetSize;

    if (pContext->pGlyphIndexAdjustArray[pContext->sheetIndex] !=
        ADJUST_OFFSET_SHEET_NOT_LOADED) {
        if (pContext->GetRemain() < sheetSize) {
            return CONSTRUCT_ERROR;
        }

        pContext->SetupTask(CONSTRUCT_OP_COPY, sheetSize,
                            CONSTRUCT_OP_PREPARE_COPY_SHEET);
    } else {
        pContext->SetupTask(CONSTRUCT_OP_SKIP, sheetSize,
                            CONSTRUCT_OP_PREPARE_COPY_SHEET);
    }

    pContext->sheetIndex++;

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpPrepareExpandSheet(ConstructContext* pContext,
                                               CachedStreamReader* pStream) {
    if (pContext->sheetIndex >= pContext->numSheets) {
        pContext->op = CONSTRUCT_OP_DISPATCH;
        return CONSTRUCT_CONTINUE;
    }

    if (pStream->GetRemain() < sizeof(u32) * 2) {
        return RequestData(pContext, pStream, sizeof(u32) * 2);
    }

    u32 compSizeBuf;
    u32 compHeader;

    pStream->CopyTo(&compSizeBuf, sizeof(u32));
    memcpy(&compHeader, pStream->mStreamCur, sizeof(u32));

    u32 compSize = compSizeBuf;
    u32 uncompSize = CXGetUncompressedSize(&compHeader);

    if (pContext->pGlyphIndexAdjustArray[pContext->sheetIndex] !=
        ADJUST_OFFSET_SHEET_NOT_LOADED) {
        if (pContext->GetRemain() < compSize + sizeof(CXUncompContextHuffman)) {
            return CONSTRUCT_ERROR;
        }

        CXInitUncompContextHuffman(pContext->pUncomp, pContext->pCurrent);
        pContext->SetupTask(CONSTRUCT_OP_EXPAND, compSize,
                            CONSTRUCT_OP_PREPARE_EXPAND_SHEET);
        pContext->Advance(uncompSize);
    } else {
        pContext->SetupTask(CONSTRUCT_OP_SKIP, compSize,
                            CONSTRUCT_OP_PREPARE_EXPAND_SHEET);
    }

    pContext->sheetIndex++;

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpCopy(ConstructContext* pContext,
                                 CachedStreamReader* pStream) {
    u32 copySize = pStream->mStreamEnd - pStream->mStreamCur;
    if (copySize > pContext->opSize) {
        copySize = pContext->opSize;
    }

    memcpy(pContext->pCurrent, pStream->mStreamCur, copySize);
    pStream->mStreamCur += copySize;
    pContext->Advance(copySize);

    if (pContext->ReduceTaskSize(copySize) == 0) {
        pContext->op = pContext->nextOp;
    } else {
        return RequestData(pContext, pStream, pContext->opSize);
    }

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpSkip(ConstructContext* pContext,
                                 CachedStreamReader* pStream) {
    u32 skipSize = Min(pContext->opSize, pStream->GetRemain());

    pStream->Skip(skipSize);

    if (pContext->ReduceTaskSize(skipSize) == 0) {
        pContext->op = pContext->nextOp;
    } else {
        return RequestData(pContext, pStream, pContext->opSize);
    }

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpExpand(ConstructContext* pContext,
                                   CachedStreamReader* pStream) {
    u32 expandSize = Min(pContext->opSize, pStream->GetRemain());

    const u8* pSrc = pStream->mStreamCur;
    pStream->mStreamCur += expandSize;
    CXReadUncompHuffman(pContext->pUncomp, pSrc, expandSize);

    if (pContext->ReduceTaskSize(expandSize) == 0) {
        pContext->op = pContext->nextOp;
    } else {
        return RequestData(pContext, pStream, pContext->opSize);
    }

    return CONSTRUCT_CONTINUE;
}

ArchiveFontBase::ConstructResult
ArchiveFontBase::ConstructOpFatalError(ConstructContext* pContext,
                                       CachedStreamReader* pStream) {
    pContext->op = CONSTRUCT_OP_FATAL_ERROR;
    return CONSTRUCT_ERROR;
}

void ArchiveFontBase::CachedStreamReader::Init() {
    mStreamHead = nullptr;
    mStreamCur = nullptr;
    mStreamEnd = nullptr;
    mTempHead = nullptr;
    mTempCur = nullptr;
    mTempEnd = nullptr;
    mRequireSize = 0;
}

void ArchiveFontBase::CachedStreamReader::Attach(const void* pStream,
                                                 u32 size) {
    mStreamHead = static_cast<const u8*>(pStream);
    mStreamCur = static_cast<const u8*>(pStream);
    mStreamEnd = static_cast<const u8*>(pStream) + size;
}

bool ArchiveFontBase::CachedStreamReader::RequestData(
    ConstructContext* pContext, u32 size) {

    u32 remain = GetRemain();

    if (remain == 0) {
        mTempHead = nullptr;
        mTempCur = nullptr;
        mTempEnd = nullptr;
        mRequireSize = 0;
    } else {
        if (pContext->GetRemain() < size * 2) {
            return false;
        }

        u8* pTemp = pContext->pCurrent + size;
        MoveTo(pTemp, remain);

        mTempHead = pTemp;
        mTempCur = pTemp;
        mTempEnd = pTemp + remain;
        mRequireSize = size;
    }

    return true;
}

}
}
}
