#ifndef NW4R_UT_ARCHIVE_FONT_H
#define NW4R_UT_ARCHIVE_FONT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_ArchiveFontBase.h>

namespace nw4r {
namespace ut {

class ArchiveFont : public detail::ArchiveFontBase {
public:
    static const u32 HEADER_SIZE = 16 * 1024;

public:
    ArchiveFont();
    virtual ~ArchiveFont();

    virtual void GetGlyph(Glyph* pGlyph, u16 ch) const;

    static u32 GetRequireBufferSize(const void* pBrfna,
                                    const char* pGlyphGroups = LOAD_GLYPH_ALL);

    bool Construct(void* pBuffer, u32 bufferSize, const void* pBrfna,
                   const char* pGlyphGroups = LOAD_GLYPH_ALL);
    void* Destroy();

private:
    void InitStreamingConstruct(ConstructContext* pContext, void* pBuffer,
                                u32 bufferSize, const char* pGlyphGroups);
    ConstructResult StreamingConstruct(ConstructContext* pContext,
                                       const void* pStream, u32 streamSize);

    void GetGlyphFromIndex(Glyph* pGlyph, u16 index) const;
};

}
}

#endif
