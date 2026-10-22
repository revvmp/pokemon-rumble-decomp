#include <game/NPpuVM.h>
#include <game/Message.h>
#include <game/Locale.h>
#include <NIntl/basic_formatter.h>
#include <string>

static std::wstring Format(NPpu::CEngine* pEngine, int count, const wchar_t* pFormat);
static std::string Format(NPpu::CEngine* pEngine, int count, const char* pFormat);

int CatGets(NPpu::CEngine* pEngine) {
    const wchar_t* pDefault = pEngine->PopWString();
    int message = pEngine->PopInt();
    pEngine->PushWString(GetMessageText(message, pDefault));
    return 0;
}

int GetConsoleRegion(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetConsoleRegion());
    return 0;
}

int GetGameRegion(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetGameRegion());
    return 0;
}

int GetConsoleLang(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetConsoleLang());
    return 0;
}

int GetGameLang(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetGameLang());
    return 0;
}

int Format(NPpu::CEngine* pEngine) {
    int count = pEngine->FetchArgCount();
    if (count == 0) {
        pEngine->PushNull();
        return 0;
    }
    NPpu::Slice format;
    pEngine->GetArg(count - 1, format);
    if (format.IsString()) {
        const char* pResult = pEngine->AllocString(Format(pEngine, count - 1, pEngine->ToString(format)));
        pEngine->Drop(count);
        pEngine->PushString(pResult);
    } else if (format.IsWString()) {
        const wchar_t* pResult = pEngine->AllocWString(Format(pEngine, count - 1, pEngine->ToWString(format)));
        pEngine->Drop(count);
        pEngine->PushWString(pResult);
    } else {
        pEngine->Drop(count);
        pEngine->PushNull();
    }
    return 0;
}

static std::wstring Format(NPpu::CEngine* pEngine, int count, const wchar_t* pFormat) {
    NIntl::basic_formatter<wchar_t> formatter(pFormat);
    while (count > 0) {
        NPpu::Slice value;
        pEngine->GetArg(count - 1, value);
        switch (value.mType) {
        case NPpu::Slice::TYPE_INT:
            formatter % value.ToInt();
            break;
        case NPpu::Slice::TYPE_FLOAT:
            formatter % value.ToFloat();
            break;
        case NPpu::Slice::TYPE_STRING:
        case NPpu::Slice::TYPE_STRING_PTR:
            formatter % pEngine->ToString(value);
            break;
        case NPpu::Slice::TYPE_WSTRING:
        case NPpu::Slice::TYPE_WSTRING_PTR:
            formatter % pEngine->ToWString(value);
            break;
        case NPpu::Slice::TYPE_FUNCTION:
            formatter % value.ToInt();
            break;
        case NPpu::Slice::TYPE_REFERENCE:
            formatter % reinterpret_cast<int>(value.GetReference());
            break;
        default:
            formatter % 0;
            break;
        }
        --count;
    }
    return formatter.str();
}

static std::string Format(NPpu::CEngine* pEngine, int count, const char* pFormat) {
    NIntl::basic_formatter<char> formatter(pFormat);
    while (count > 0) {
        NPpu::Slice value;
        pEngine->GetArg(count - 1, value);
        switch (value.mType) {
        case NPpu::Slice::TYPE_INT:
            formatter % value.ToInt();
            break;
        case NPpu::Slice::TYPE_FLOAT:
            formatter % value.ToFloat();
            break;
        case NPpu::Slice::TYPE_STRING:
        case NPpu::Slice::TYPE_STRING_PTR:
            formatter % pEngine->ToString(value);
            break;
        case NPpu::Slice::TYPE_WSTRING:
        case NPpu::Slice::TYPE_WSTRING_PTR:
            formatter % pEngine->ToWString(value);
            break;
        case NPpu::Slice::TYPE_FUNCTION:
            formatter % value.ToInt();
            break;
        case NPpu::Slice::TYPE_REFERENCE:
            formatter % reinterpret_cast<int>(value.GetReference());
            break;
        default:
            formatter % 0;
            break;
        }
        --count;
    }
    return formatter.str();
}
