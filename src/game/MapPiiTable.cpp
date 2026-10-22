#include <game/MapDesc.h>

MapPiiTable::MapPiiTable() : mRowCount(0), mColumnCount(0) {}

MapPiiTable::~MapPiiTable() {}

void MapPiiTable::Clear() {
    std::vector<NPpu::CValue> values;
    mValues.swap(values);
    std::set<std::string> strings;
    mStrings.swap(strings);
    std::set<std::wstring> wstrings;
    mWideStrings.swap(wstrings);
    mRowCount = 0;
    mColumnCount = 0;
}

#pragma dont_inline on

void MapPiiTable::Initialize(int rowCount, int columnCount) {
    mRowCount = rowCount;
    mColumnCount = columnCount;
    mValues.resize((rowCount + 1) * columnCount);
    mStrings.clear();
    mWideStrings.clear();
}

void MapPiiTable::SetValue(int row, int column, const NPpu::CValue& rValue, const NPpu::CEngine* pEngine) {
    int index = GetIndex(row, column);
    if (rValue.IsWString()) {
        GetValue(index).SetWStringPtr(InsertWString(pEngine->ToWString(rValue)));
    } else if (rValue.IsString()) {
        GetValue(index).SetStringPtr(InsertString(pEngine->ToString(rValue)));
    } else {
        GetValue(index) = rValue;
    }
}

#pragma dont_inline reset


NPpu::CValue& MapPiiTable::GetValue(int index) {
    return mValues[index];
}

NPpu::CValue* MapPiiTable::GetCell(int row, int column) {
    return &mValues[GetIndex(row, column)];
}

int MapPiiTable::GetIndex(int row, int column) const {
    return (row + 1) * mColumnCount + column;
}
const char* MapPiiTable::InsertString(const char* pString) {
    std::string key(pString);
    std::set<std::string>::iterator it = mStrings.find(key);
    if (it == mStrings.end()) {
        it = mStrings.insert(key).first;
    }
    return (*it).c_str();
}

const wchar_t* MapPiiTable::InsertWString(const wchar_t* pString) {
    std::wstring key(pString);
    std::set<std::wstring>::iterator it = mWideStrings.find(key);
    if (it == mWideStrings.end()) {
        it = mWideStrings.insert(key).first;
    }
    return (*it).c_str();
}
