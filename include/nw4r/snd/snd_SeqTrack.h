#ifndef NW4R_SND_SEQ_TRACK_H
#define NW4R_SND_SEQ_TRACK_H

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_Channel.h"
#include <nw4r/snd/snd_Types.h>
#include "nw4r/snd/snd_Lfo.h"
#include "nw4r/snd/snd_MoveValue.h"

namespace nw4r { namespace snd { namespace detail { class SeqPlayer; }}}

namespace nw4r { namespace snd
{
    enum SeqMute
    {
        MUTE_OFF,
        MUTE_NO_STOP,
        MUTE_RELEASE,
        MUTE_STOP,
    };
}}

namespace nw4r { namespace snd { namespace detail
{
    class SeqTrack
    {
    public:
        static int const CALL_STACK_DEPTH = 3;
    public:
        enum ParseResult
        {
            PARSE_RESULT_CONTINUE,
            PARSE_RESULT_FINISH,
        };
    public:
        struct CallStack
        {
            bool loopFlag;
            u8 loopCount;

            u8 const *address;
        };

        struct ParserTrackParam
        {
            u8 const *baseAddr;
            u8 const *currentAddr;
            bool cmpFlag;
            bool noteWaitFlag;
            bool tieFlag;
            bool monophonicFlag;
            CallStack callStack[CALL_STACK_DEPTH];
            u8 callStackDepth;

            s32 wait;
            bool muteFlag;
            bool silenceFlag;
            bool noteFinishWait;
            bool portaFlag;
            bool damperFlag;

            int bankNo;
            int prgNo;
            LfoParam lfoParam;
            u8 lfoTarget;

            f32 sweepPitch;
            MoveValue<u8, s16> volume;
            MoveValue<s8, s16> pan;
            MoveValue<s8, s16> surroundPan;
            MoveValue<s8, s16> pitchBend;
            u8 volume2;
            u8 velocityRange;
            u8 bendRange;
            s8 initPan;
            s8 transpose;
            u8 priority;
            u8 portaKey;
            u8 portaTime;
            u8 attack;
            u8 decay;
            u8 sustain;
            u8 release;

            s16 envHold;
            u8 mainSend;
            u8 fxSend[AUX_BUS_NUM];
            u8 biquadType;

            f32 lpfFreq;
            f32 biquadValue;
        };
    public:
        SeqTrack();
        virtual ~SeqTrack();

        virtual ParseResult Parse(bool doNoteOn) = 0;

        void InitParam();
        void SetSeqData(void const *seqBase, s32 seqOffset);
        void Open();
        void Close();

        u8 GetPlayerTrackNo() const { return mPlayerTrackNo; }
        bool IsOpened() const { return mOpenFlag; }
        ParserTrackParam &GetParserTrackParam() { return mParserTrackParam; }
        s16 volatile *GetVariablePtr(int varNo);
        SeqPlayer *GetSeqPlayer() { return mSeqPlayer; }
        Channel *GetLastChannel() const { return mChannelList; }

        void SetPlayerTrackNo(int playerTrackNo);
        void SetVolume(f32);
        void SetTrackVariable(int variable, s16 value);
        void SetPitch(f32);
        void SetPan(f32);
        void SetSurroundPan(f32);
        void SetPanRange(f32);
        void SetMute(SeqMute mute);
        void SetSilence(bool, int);
        void SetModDepth(f32);
        void SetModSpeed(f32);
        void SetBiquadFilter(int, f32);
        void SetLpfFreq(f32);
        void SetSeqPlayer(SeqPlayer *player) { mSeqPlayer = player; }

        void AddChannel(Channel *channel);

        void UpdateChannelLength();
        void UpdateChannelParam();
        void UpdateChannelRelease(Channel *channel);

        void ReleaseAllChannel(int release);
        void PauseAllChannel(bool flag);
        void StopAllChannel();
        void FreeAllChannel();

        Channel *NoteOn(int key, int velocity, s32 length, bool tieFlag);

        int ParseNextTick(bool doNoteOn);

        static void ChannelCallbackFunc(Channel *dropChannel,
                                        Channel::ChannelCallbackStatus status,
                                        u32 userData);
    public:
        static int const MUTE_RELEASE_VALUE = -1;
        static int const PAUSE_RELEASE_VALUE;

        static int const TRACK_VARIABLE_NUM = 16;

        static int const PARSER_PARAM_SIZE;

        static int const MAX_ENVELOPE_VALUE = 127;
        static int const INVALID_ENVELOPE = 255;

        static int const DEFAULT_PORTA_KEY = 60;
        static int const DEFAULT_BENDRANGE = 2;
        static int const DEFAULT_PRIORITY = 64;
    private:
        u8 mPlayerTrackNo;
        bool mOpenFlag;

        f32 mExtVolume;
        f32 mExtPitch;
        f32 mExtPan;
        f32 mExtSurroundPan;
        f32 mPanRange;
        ParserTrackParam mParserTrackParam;
        s16 volatile mTrackVariable[TRACK_VARIABLE_NUM];
        SeqPlayer *mSeqPlayer;
        Channel *mChannelList;
    };
}}}

#endif
