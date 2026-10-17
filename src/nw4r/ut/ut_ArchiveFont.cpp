#include <nw4r/ut/ut_ArchiveFont.h>
#include <nw4r/math/math_arithmetic.h>

#include <revolution/os/OSCache.h>

namespace nw4r {
namespace ut {
namespace {

inline bool IsNullString(const char* pStr) {
    return *pStr == '\0';
}

}

ArchiveFont::ArchiveFont() {}

ArchiveFont::~ArchiveFont() {}

u32 ArchiveFont::GetRequireBufferSize(const void* pBrfna,
                                      const char* pGlyphGroups) {
    if (!IsValidResource(pBrfna, HEADER_SIZE)) {
        return 0;
    }

    detail::FontGlyphGroupsAcs groups(pBrfna);
    int numLoadSheet = 0;
    u32 sizeLoadCWDH = 0;
    u32 sizeLoadCMAP = 0;

    for (int flagSetNo = 0; flagSetNo * 32 < groups.GetNumSheet();
         flagSetNo++) {
        u32 useSheets = 0;

        for (int setNo = 0; setNo < groups.GetNumSet(); setNo++) {
            const char* pSetName = groups.GetSetName(setNo);

            if (IsNullString(pGlyphGroups) ||
                IncludeName(pGlyphGroups, pSetName)) {
                useSheets |= groups.GetUseSheetFlags(setNo, flagSetNo);
            }
        }

        numLoadSheet += math::CntBit1(useSheets);
    }

    for (int flagSetNo = 0; flagSetNo * 32 < groups.GetNumCWDH();
         flagSetNo++) {
        u32 useCWDH = 0;

        for (int setNo = 0; setNo < groups.GetNumSet(); setNo++) {
            const char* pSetName = groups.GetSetName(setNo);

            if (IsNullString(pGlyphGroups) ||
                IncludeName(pGlyphGroups, pSetName)) {
                useCWDH |= groups.GetUseCWDHFlags(setNo, flagSetNo);
            }
        }

        for (int bit = 0; bit < 32; bit++) {
            if ((useCWDH << bit) & 0x80000000) {
                sizeLoadCWDH += groups.GetSizeCWDH(flagSetNo * 32 + bit) -
                                sizeof(BinaryBlockHeader);
            }
        }
    }

    for (int flagSetNo = 0; flagSetNo * 32 < groups.GetNumCMAP();
         flagSetNo++) {
        u32 useCMAP = 0;

        for (int setNo = 0; setNo < groups.GetNumSet(); setNo++) {
            const char* pSetName = groups.GetSetName(setNo);

            if (IsNullString(pGlyphGroups) ||
                IncludeName(pGlyphGroups, pSetName)) {
                useCMAP |= groups.GetUseCMAPFlags(setNo, flagSetNo);
            }
        }

        for (int bit = 0; bit < 32; bit++) {
            if ((useCMAP << bit) & 0x80000000) {
                sizeLoadCMAP += groups.GetSizeCMAP(flagSetNo * 32 + bit) -
                                sizeof(BinaryBlockHeader);
            }
        }
    }

    u32 sizeAdjustTable = RoundUp(groups.GetNumSheet() * sizeof(u16), 4);
    u32 sizeSheets = RoundUp(numLoadSheet * groups.GetSheetSize(), 4);
    u32 sizeBlocks = Max<u32>(sizeLoadCWDH + sizeLoadCMAP,
                              sizeof(CXUncompContextHuffman));

    return RoundUp(sizeAdjustTable + sizeof(FontInformation) +
                       sizeof(FontTextureGlyph),
                   32) +
           sizeSheets + sizeBlocks;
}

inline void ArchiveFont::InitStreamingConstruct(ConstructContext* pContext,
                                         void* pBuffer, u32 bufferSize,
                                         const char* pGlyphGroups) {
    pContext->stream.Init();
    pContext->pGlyphGroups = pGlyphGroups;
    pContext->pGlyphIndexAdjustArray = nullptr;
    pContext->pBufferBegin = static_cast<u8*>(pBuffer);
    pContext->pBufferEnd = static_cast<u8*>(pBuffer) + bufferSize;
    pContext->pCurrent = static_cast<u8*>(pBuffer);
    pContext->pUncomp = static_cast<CXUncompContextHuffman*>(
        RoundDown(pContext->pBufferEnd - sizeof(CXUncompContextHuffman), 4));
    pContext->nextOp = CONSTRUCT_OP_MAX + 1;
    pContext->opSize = 0;
    pContext->numBlocks = 1;
    pContext->blockIndex = 0;
    pContext->sheetIndex = 0;
    pContext->numSheets = 0;
    pContext->glyphsPerSheet = 0;
    pContext->streamOffset = 0;
    pContext->pFINF = nullptr;
    pContext->pPrevCWDH = nullptr;
    pContext->pPrevCMAP = nullptr;
    pContext->op = CONSTRUCT_OP_ANALYZE_FILE_HEADER;
}

ArchiveFont::ConstructResult
ArchiveFont::StreamingConstruct(ConstructContext* pContext,
                                const void* pStream, u32 streamSize) {
    if (mFontInfo != nullptr) {
        return pContext->blockIndex < pContext->numBlocks ? CONSTRUCT_ERROR
                                                          : CONSTRUCT_FINISH;
    }

    ConstructResult result = CONSTRUCT_CONTINUE;
    CachedStreamReader& rReader = pContext->stream;
    CachedStreamReader* pReader = &rReader;

    rReader.Attach(pStream, streamSize);

    while (result == CONSTRUCT_CONTINUE) {
        switch (pContext->op) {
        case CONSTRUCT_OP_DISPATCH: {
            result = ConstructOpDispatch(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_ANALYZE_FILE_HEADER: {
            result = ConstructOpAnalyzeFileHeader(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_ANALYZE_GLGR: {
            result = ConstructOpAnalyzeGLGR(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_ANALYZE_FINF: {
            result = ConstructOpAnalyzeFINF(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_ANALYZE_CMAP: {
            result = ConstructOpAnalyzeCMAP(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_ANALYZE_CWDH: {
            result = ConstructOpAnalyzeCWDH(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_ANALYZE_TGLP: {
            result = ConstructOpAnalyzeTGLP(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_PREPARE_COPY_SHEET: {
            result = ConstructOpPrepareCopySheet(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_PREPARE_EXPAND_SHEET: {
            result = ConstructOpPrepareExpandSheet(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_COPY: {
            result = ConstructOpCopy(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_SKIP: {
            result = ConstructOpSkip(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_EXPAND: {
            result = ConstructOpExpand(pContext, pReader);
            break;
        }

        case CONSTRUCT_OP_FATAL_ERROR: {
            result = ConstructOpFatalError(pContext, pReader);
            break;
        }

        default: {
            pContext->op = CONSTRUCT_OP_FATAL_ERROR;
            result = CONSTRUCT_ERROR;
            break;
        }
        }
    }

    if (result == CONSTRUCT_FINISH && mFontInfo == nullptr) {
        DCFlushRange(pContext->pBufferBegin,
                     pContext->pBufferEnd - pContext->pBufferBegin);
        SetResourceBuffer(pContext->pBufferBegin, pContext->pFINF,
                          pContext->pGlyphIndexAdjustArray);

        if (AdjustIndex(mFontInfo->alterCharIndex) == GLYPH_INDEX_NOT_FOUND) {
            mFontInfo->alterCharIndex = 0;
        }

        InitReaderFunc(GetEncoding());
    }

    return result;
}

bool ArchiveFont::Construct(void* pBuffer, u32 bufferSize, const void* pBrfna,
                            const char* pGlyphGroups) {
    u32 fileSize = static_cast<const BinaryFileHeader*>(pBrfna)->fileSize;
    ConstructContext context;

    InitStreamingConstruct(&context, pBuffer, bufferSize, pGlyphGroups);
    return StreamingConstruct(&context, pBrfna, fileSize) == CONSTRUCT_FINISH;
}

void* ArchiveFont::Destroy() {
    return RemoveResourceBuffer();
}

void ArchiveFont::GetGlyph(Glyph* pGlyph, u16 ch) const {
    GetGlyphFromIndex(pGlyph, GetGlyphIndex(ch));
}

void ArchiveFont::GetGlyphFromIndex(Glyph* pGlyph, u16 index) const {
    const FontTextureGlyph* pTexGlyph = mFontInfo->pGlyph;
    u16 adjustedIndex = AdjustIndex(index);

    if (adjustedIndex == GLYPH_INDEX_NOT_FOUND) {
        index = mFontInfo->alterCharIndex;
        adjustedIndex = AdjustIndex(index);
    }

    u32 cellsInASheet = pTexGlyph->sheetRow * pTexGlyph->sheetLine;

    u32 glyphCell = adjustedIndex % cellsInASheet;
    u32 glyphSheet = adjustedIndex / cellsInASheet;

    u32 unitX = glyphCell % pTexGlyph->sheetRow;
    u32 unitY = glyphCell / pTexGlyph->sheetRow;

    u32 pixelX = unitX * (pTexGlyph->cellWidth + 1);
    u32 pixelY = unitY * (pTexGlyph->cellHeight + 1);

    pGlyph->pTexture =
        pTexGlyph->sheetImage + (glyphSheet * pTexGlyph->sheetSize);

    pGlyph->widths = GetCharWidthsFromIndex(index);
    pGlyph->height = pTexGlyph->cellHeight;

    pGlyph->texFormat = static_cast<GXTexFmt>(pTexGlyph->sheetFormat);

    pGlyph->texWidth = pTexGlyph->sheetWidth;
    pGlyph->texHeight = pTexGlyph->sheetHeight;

    pGlyph->cellX = pixelX + 1;
    pGlyph->cellY = pixelY + 1;
}

}
}
