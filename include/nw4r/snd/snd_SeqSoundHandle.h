#ifndef NW4R_SND_SEQ_SOUND_HANDLE_H
#define NW4R_SND_SEQ_SOUND_HANDLE_H

#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_SeqSound.h>
#include <nw4r/snd/snd_SeqTrack.h>
#include <nw4r/ut/ut_NonCopyable.h>

namespace nw4r { namespace snd { namespace detail { class SeqSound; }}}

namespace nw4r { namespace snd
{

    class SeqSoundHandle : private ut::NonCopyable
    {

    public:
        SeqSoundHandle(SoundHandle*);
        ~SeqSoundHandle() { DetachSound(); }

        bool IsAttachedSound() const { return mSound != nullptr; }

        void DetachSound();

        u32 GetTick() const {
            if (IsAttachedSound())
                return mSound->GetTick();
            return 0;
        }

        void WriteVariable(int varNo, s16 value) {
            if (IsAttachedSound())
                mSound->WriteVariable(varNo, value);
        }

        void ReadVariable(int varNo, s16 *value) {
            if (IsAttachedSound())
                mSound->ReadVariable(varNo, value);
        }

        static bool WriteGlobalVariable(int varNo, s16 value) {
            return detail::SeqSound::WriteGlobalVariable(varNo, value);
        }

        void SetTrackMute(u32 trackFlags, SeqMute mute) {
            if (IsAttachedSound())
                mSound->SetTrackMute(trackFlags, mute);
        }

        void SetTempoRatio(f32 tempoRatio) {
            if (IsAttachedSound())
                mSound->SetTempoRatio(tempoRatio);
        }

        void SetVolume(u32 trackFlags, f32 volume) {
            if (IsAttachedSound())
                mSound->SetTrackVolume(trackFlags, volume);
        }

    private:
        detail::SeqSound *mSound;
    };
}}

#endif
