#ifndef NW4R_SND_SOUND_STARTABLE_H
#define NW4R_SND_SOUND_STARTABLE_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace snd {

class SoundHandle;

class SoundStartable {
public:
    enum StartResult {
        START_SUCCESS,
        START_ERR_LOW_PRIORITY,
        START_ERR_INVALID_LABEL_STRING,
        START_ERR_INVALID_SOUNDID,
        START_ERR_NOT_DATA_LOADED,
        START_ERR_NOT_ENOUGH_PLAYER_HEAP,
        START_ERR_CANNOT_OPEN_FILE,
        START_ERR_NOT_AVAILABLE,
        START_ERR_CANNOT_ALLOCATE_TRACK,
        START_ERR_NOT_ENOUGH_INSTANCE,
        START_ERR_INVALID_PARAMETER,
        START_ERR_INVALID_SEQ_START_LOCATION_LABEL,

        START_ERR_USER = 128,
        START_ERR_UNKNOWN = 255
    };

    struct StartInfo {
        enum EnableFlag {
            ENABLE_START_OFFSET = (1 << 0),
            ENABLE_PLAYER_ID = (1 << 1),
            ENABLE_PLAYER_PRIORITY = (1 << 2),
            ENABLE_ACTOR_PLAYER_ID = (1 << 3),
            ENABLE_SEQ_SOUND_INFO = (1 << 4)
        };

        enum StartOffsetType {
            START_OFFSET_TYPE_MILLISEC,
            START_OFFSET_TYPE_TICK,
            START_OFFSET_TYPE_SAMPLE
        };

        struct SeqSoundInfo {
            SeqSoundInfo() : seqDataAddress(nullptr), startLocationLabel(nullptr) {}

            void* seqDataAddress;
            const char* startLocationLabel;
        };

        StartInfo() : enableFlag(0) {}

        u32 enableFlag;
        StartOffsetType startOffsetType;
        int startOffset;
        u32 playerId;
        int playerPriority;
        int actorPlayerId;
        SeqSoundInfo seqSoundInfo;
    };

public:
    virtual ~SoundStartable() {}

    virtual StartResult detail_SetupSound(SoundHandle* pHandle, u32 id,
                                          bool hold,
                                          const StartInfo* pStartInfo) = 0;
    virtual u32 detail_ConvertLabelStringToSoundId(const char* pLabel) = 0;

    StartResult detail_StartSound(SoundHandle* pHandle, u32 id,
                                  const StartInfo* pStartInfo);
    StartResult detail_StartSound(SoundHandle* pHandle, const char* pLabel,
                                  const StartInfo* pStartInfo);
    StartResult detail_HoldSound(SoundHandle* pHandle, u32 id,
                                 const StartInfo* pStartInfo);
    StartResult detail_HoldSound(SoundHandle* pHandle, const char* pLabel,
                                 const StartInfo* pStartInfo);
    StartResult detail_PrepareSound(SoundHandle* pHandle, u32 id,
                                    const StartInfo* pStartInfo);
    StartResult detail_PrepareSound(SoundHandle* pHandle, const char* pLabel,
                                    const StartInfo* pStartInfo);

    bool StartSound(SoundHandle* pHandle, u32 id) {
        return detail_StartSound(pHandle, id, nullptr) == START_SUCCESS;
    }
    bool StartSound(SoundHandle* pHandle, const char* pLabel) {
        return detail_StartSound(pHandle, pLabel, nullptr) == START_SUCCESS;
    }
    bool StartSound(SoundHandle* pHandle, unsigned int id) {
        return detail_StartSound(pHandle, id, nullptr) == START_SUCCESS;
    }
    bool StartSound(SoundHandle* pHandle, int id) {
        return detail_StartSound(pHandle, id, nullptr) == START_SUCCESS;
    }

    bool HoldSound(SoundHandle* pHandle, u32 id) {
        return detail_HoldSound(pHandle, id, nullptr) == START_SUCCESS;
    }
    bool HoldSound(SoundHandle* pHandle, const char* pLabel) {
        return detail_HoldSound(pHandle, pLabel, nullptr) == START_SUCCESS;
    }
    bool HoldSound(SoundHandle* pHandle, unsigned int id) {
        return detail_HoldSound(pHandle, id, nullptr) == START_SUCCESS;
    }
    bool HoldSound(SoundHandle* pHandle, int id) {
        return detail_HoldSound(pHandle, id, nullptr) == START_SUCCESS;
    }

    bool PrepareSound(SoundHandle* pHandle, u32 id) {
        return detail_PrepareSound(pHandle, id, nullptr) == START_SUCCESS;
    }
    bool PrepareSound(SoundHandle* pHandle, unsigned int id) {
        return detail_PrepareSound(pHandle, id, nullptr) == START_SUCCESS;
    }
    bool PrepareSound(SoundHandle* pHandle, int id) {
        return detail_PrepareSound(pHandle, id, nullptr) == START_SUCCESS;
    }
};

}
}

#endif
