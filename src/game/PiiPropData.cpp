#include <game/PiiPropData.h>

static int GetPropertyRow(int monsno, int form) {
    switch (monsno) {
    case 386:
        if (form != 0 && form <= 3) {
            return form + 495;
        }
        break;
    case 413:
        if (form != 0 && form <= 2) {
            return form + 498;
        }
        break;
    case 487:
        if (form != 0 && form <= 1) {
            return form + 500;
        }
        break;
    case 492:
        if (form != 0 && form <= 1) {
            return form + 501;
        }
        break;
    case 479:
        if (form != 0 && form <= 5) {
            return form + 502;
        }
        break;
    }
    return monsno;
}

PiiPropData::PiiPropData() : mSpecies(508) {}

#pragma dont_inline on

void PiiPropData::Initialize(int columns) {
    mProperties.Initialize(508, columns);
}

void PiiPropData::SetProperty(u16 monsno, u8 form, int formScope, int column, const NPpu::CValue& rValue, const NPpu::CEngine* pEngine) {
    if (formScope == 1) {
        mProperties.SetValue(monsno, column, rValue, pEngine);
        for (int i = 1;; ++i) {
            int row = GetPropertyRow(monsno, i);
            if (row == monsno) {
                break;
            }
            mProperties.SetValue(row, column, rValue, pEngine);
        }
    } else {
        mProperties.SetValue(GetPropertyRow(monsno, form), column, rValue, pEngine);
    }
}

const NPpu::CValue& PiiPropData::GetProperty(u16 monsno, u8 form, int column) const {
    return *mProperties.GetCell(GetPropertyRow(monsno, form), column);
}
