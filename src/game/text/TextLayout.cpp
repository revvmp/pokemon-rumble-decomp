#include <revolution/types.h>
#include <string>
#include <nw4r/ut/ut_TextWriterBase.h>
#include <nw4r/ut/ut_Font.h>
#include <nw4r/lyt/lyt_textBox.h>

std::wstring WrapText(const std::wstring& rText, nw4r::ut::TextWriterBase<wchar_t>& rWriter, f32 width) {
    std::wstring result;
    result.reserve(rText.size());
    enum State { Empty, Word, SpacedWord, Space };
    unsigned long lineStart = 0;
    unsigned long spaceStart;
    unsigned long wordStart;
    State state = Empty;
    f32 completedWidth = 0.0f;
    f32 spaceWidth;
    f32 wordWidth;
    for (unsigned long i = 0; i < rText.size(); ++i) {
        wchar_t character = rText[i];
        if (character == L'\n') {
            switch (state) {
            case Word:
            case SpacedWord:
                result += rText.substr(lineStart, i - lineStart);
                break;
            case Space:
                result += rText.substr(lineStart, spaceStart - lineStart);
                break;
            default:
                break;
            }
            result.push_back(L'\n');
            completedWidth = 0.0f;
            lineStart = i + 1;
            state = Empty;
        } else {
            f32 characterWidth = rWriter.IsWidthFixed() ? rWriter.GetFixedWidth() : rWriter.GetFont()->GetCharWidth(character) * rWriter.GetScaleH();
            if (character == L' ' || character == 0x3000) {
                switch (state) {
                case SpacedWord:
                    completedWidth += spaceWidth;
                case Word:
                    completedWidth += wordWidth;
                case Empty:
                    spaceWidth = 0.0f;
                    spaceStart = i;
                    state = Space;
                case Space:
                    break;
                }
                spaceWidth += characterWidth;
            } else {
                switch (state) {
                case Empty:
                startWord:
                    wordWidth = characterWidth;
                    wordStart = i;
                    state = Word;
                    break;
                case Word:
                    if (width < wordWidth + characterWidth) {
                        result += rText.substr(lineStart, i - lineStart);
                        result.push_back(L'\n');
                        completedWidth = 0.0f;
                        lineStart = i;
                        goto startWord;
                    } else {
                        wordWidth += characterWidth;
                    }
                    break;
                case SpacedWord:
                    wordWidth += characterWidth;
                    if (width < wordWidth + (completedWidth + spaceWidth)) {
                        result += rText.substr(lineStart, spaceStart - lineStart);
                        result.push_back(L'\n');
                        completedWidth = 0.0f;
                        lineStart = wordStart;
                        state = Word;
                    }
                    break;
                case Space:
                    if (width < characterWidth + (completedWidth + spaceWidth)) {
                        result += rText.substr(lineStart, spaceStart - lineStart);
                        result.push_back(L'\n');
                        completedWidth = 0.0f;
                        lineStart = i;
                        goto startWord;
                    } else {
                        wordWidth = characterWidth;
                        wordStart = i;
                        state = SpacedWord;
                    }
                    break;
                }
            }
        }
    }
    result += rText.substr(lineStart);
    return result;
}

std::wstring WrapText(const std::wstring& rText, const nw4r::lyt::TextBox& rTextBox) {
    nw4r::ut::TextWriterBase<wchar_t> writer;
    writer.SetFont(*rTextBox.GetFont());
    const nw4r::lyt::Size& rFontSize = rTextBox.GetFontSize();
    writer.SetFontSize(rFontSize.width, rFontSize.height);
    writer.SetCharSpace(rTextBox.GetCharSpace());
    return WrapText(rText, writer, rTextBox.GetSize().width);
}
