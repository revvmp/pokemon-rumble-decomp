#include <nw4r/ut/ut_algorithm.h>

#include <nw4r/snd/snd_MmlParser.h>

#include <nw4r/snd/snd_Lfo.h>
#include <nw4r/snd/snd_MmlSeqTrack.h>
#include <nw4r/snd/snd_MoveValue.h>
#include <nw4r/snd/snd_SeqPlayer.h>
#include <nw4r/snd/snd_SeqTrack.h>
#include <nw4r/snd/snd_Util.h>

#include <revolution/os.h>

namespace nw4r {
namespace snd {
namespace detail {

namespace {

const int PLAYER_VARIABLE_MIN = 0;
const int GLOBAL_VARIABLE_MIN =
    PLAYER_VARIABLE_MIN + SeqPlayer::PLAYER_VARIABLE_NUM;
const int TRACK_VARIABLE_MIN =
    GLOBAL_VARIABLE_MIN + SeqPlayer::GLOBAL_VARIABLE_NUM;
const int TRACK_VARIABLE_MAX = TRACK_VARIABLE_MIN + SeqTrack::TRACK_VARIABLE_NUM;

}

bool MmlParser::mPrintVarEnabledFlag;

MmlSeqTrack::ParseResult MmlParser::Parse(MmlSeqTrack* pTrack,
                                          bool doNoteOn) const {
    SeqPlayer* pPlayer = pTrack->GetSeqPlayer();
    SeqTrack::ParserTrackParam& rTrackParam = pTrack->GetParserTrackParam();

    SeqArgType argType;
    SeqArgType argType2 = SEQ_ARG_NONE;
    bool useArgType = false;
    bool doExecCommand = true;

    u32 cmd = ReadByte(&rTrackParam.currentAddr);

    if (cmd == MML_EXEC_IF) {
        cmd = ReadByte(&rTrackParam.currentAddr);
        doExecCommand = rTrackParam.cmpFlag != false;
    }

    if (cmd == MML_ARG_2_S16) {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType2 = SEQ_ARG_S16;
    } else if (cmd == MML_ARG_2_RANDOM) {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType2 = SEQ_ARG_RANDOM;
    } else if (cmd == MML_ARG_2_VARIABLE) {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType2 = SEQ_ARG_VARIABLE;
    }

    if (cmd == MML_ARG_1_RANDOM) {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType = SEQ_ARG_RANDOM;
        useArgType = true;
    } else if (cmd == MML_ARG_1_VARIABLE) {
        cmd = ReadByte(&rTrackParam.currentAddr);
        argType = SEQ_ARG_VARIABLE;
        useArgType = true;
    }

    if (!(cmd & MML_CMD_MASK)) {
        u8 velocity = ReadByte(&rTrackParam.currentAddr);
        s32 length = ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                             useArgType ? argType : SEQ_ARG_VMIDI);
        int key = cmd + rTrackParam.transpose;

        if (!doExecCommand) {
            return MmlSeqTrack::PARSE_RESULT_CONTINUE;
        }

        key = ut::Clamp(key, 0, 127);

        if (!rTrackParam.muteFlag && doNoteOn) {
            NoteOnCommandProc(pTrack, key, velocity, length > 0 ? length : -1,
                              rTrackParam.tieFlag);
        }

        if (rTrackParam.noteWaitFlag) {
            rTrackParam.wait = length;

            if (length == 0) {
                rTrackParam.noteFinishWait = true;
            }
        }
    } else {
        s32 commandArg1 = 0;
        s32 commandArg2 = 0;

        switch (cmd & MML_CMD_SET_MASK) {
        case 0x80: {
            switch (cmd) {
            case MML_WAIT: {
                s32 wait = ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                                   useArgType ? argType : SEQ_ARG_VMIDI);

                if (doExecCommand) {
                    rTrackParam.wait = wait;
                }
                break;
            }
            case MML_SET_PRGNO: {
                commandArg1 =
                    ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                            useArgType ? argType : SEQ_ARG_VMIDI);

                if (doExecCommand) {
                    CommandProc(pTrack, cmd, commandArg1, commandArg2);
                }
                break;
            }
            case MML_OPEN_TRACK: {
                u8 trackNo = ReadByte(&rTrackParam.currentAddr);
                u32 offset = Read24(&rTrackParam.currentAddr);

                if (doExecCommand) {
                    commandArg1 = trackNo;
                    commandArg2 = offset;
                    CommandProc(pTrack, cmd, commandArg1, commandArg2);
                }
                break;
            }
            case MML_JUMP: {
                u32 offset = Read24(&rTrackParam.currentAddr);

                if (doExecCommand) {
                    commandArg1 = offset;
                    CommandProc(pTrack, cmd, commandArg1, commandArg2);
                }
                break;
            }
            case MML_CALL: {
                u32 offset = Read24(&rTrackParam.currentAddr);

                if (doExecCommand) {
                    commandArg1 = offset;
                    CommandProc(pTrack, cmd, commandArg1, commandArg2);
                }
                break;
            }
            }
            break;
        }
        case 0xB0:
        case 0xC0:
        case 0xD0: {
            u8 arg = ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                             useArgType ? argType : SEQ_ARG_U8);

            if (argType2 != SEQ_ARG_NONE) {
                commandArg2 = ReadArg(&rTrackParam.currentAddr, pPlayer,
                                      pTrack, argType2);
            }

            if (!doExecCommand) {
                break;
            }

            switch (cmd) {
            case MML_SET_TRANSPOSE:
            case MML_SET_PITCH_BEND:
                commandArg1 = *reinterpret_cast<s8*>(&arg);
                break;
            default:
                commandArg1 = *reinterpret_cast<u8*>(&arg);
                break;
            }

            CommandProc(pTrack, cmd, commandArg1, commandArg2);
            break;
        }
        case 0x90: {
            if (doExecCommand) {
                CommandProc(pTrack, cmd, commandArg1, commandArg2);
            }
            break;
        }
        case 0xE0: {
            commandArg1 = static_cast<s16>(
                ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                        useArgType ? argType : SEQ_ARG_S16));

            if (doExecCommand) {
                CommandProc(pTrack, cmd, commandArg1, commandArg2);
            }
            break;
        }
        case 0xF0: {
            switch (cmd) {
            case MML_ALLOC_TRACK: {
                Read16(&rTrackParam.currentAddr);
                break;
            }
            case MML_EOF: {
                if (doExecCommand) {
                    return MmlSeqTrack::PARSE_RESULT_FINISH;
                }
                break;
            }
            case MML_EX_COMMAND: {
                u32 cmdex = ReadByte(&rTrackParam.currentAddr);

                switch (cmdex & MML_CMD_SET_MASK) {
                case MML_EX_USERPROC: {
                    commandArg1 = static_cast<u16>(
                        ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                                useArgType ? argType : SEQ_ARG_S16));

                    if (doExecCommand) {
                        CommandProc(pTrack, (cmd << 8) + cmdex, commandArg1,
                                    commandArg2);
                    }
                    break;
                }
                case MML_EX_ARITHMETIC:
                case MML_EX_LOGIC: {
                    commandArg1 = ReadByte(&rTrackParam.currentAddr);
                    commandArg2 = static_cast<s16>(
                        ReadArg(&rTrackParam.currentAddr, pPlayer, pTrack,
                                useArgType ? argType : SEQ_ARG_S16));

                    if (doExecCommand) {
                        CommandProc(pTrack, (cmd << 8) + cmdex, commandArg1,
                                    commandArg2);
                    }
                    break;
                }
                }
            }
            default: {
                if (doExecCommand) {
                    CommandProc(pTrack, cmd, commandArg1, commandArg2);
                }
                break;
            }
            }
            break;
        }
        case 0xA0: {
            break;
        }
        }
    }

    return MmlSeqTrack::PARSE_RESULT_CONTINUE;
}

void MmlParser::CommandProc(MmlSeqTrack* pTrack, u32 command, s32 commandArg1,
                            s32 commandArg2) const {
    SeqPlayer* pPlayer = pTrack->GetSeqPlayer();
    SeqTrack::ParserTrackParam& rTrackParam = pTrack->GetParserTrackParam();
    SeqPlayer::ParserPlayerParam& rPlayerParam =
        pPlayer->GetParserPlayerParam();

    if (command <= MML_CMD_MAX) {
        switch (command) {
        case MML_SET_TEMPO: {
            rPlayerParam.tempo =
                ut::Clamp<int>(commandArg1, TEMPO_MIN, TEMPO_MAX);
            break;
        }
        case MML_SET_TIMEBASE: {
            rPlayerParam.timebase = commandArg1;
            break;
        }
        case MML_SET_PRGNO: {
            if (commandArg1 < 0x10000) {
                rTrackParam.prgNo = commandArg1 & 0xFFFF;
            }
            break;
        }
        case MML_SET_MUTE: {
            pTrack->SetMute(static_cast<SeqMute>(commandArg1));
            break;
        }
        case MML_SET_TRACK_VOLUME: {
            rTrackParam.volume.SetTarget(commandArg1, commandArg2);
            break;
        }
        case MML_SET_TRACK_VOLUME2: {
            rTrackParam.volume2 = commandArg1;
            break;
        }
        case MML_SET_TRACK_VELOCITY_RANGE: {
            rTrackParam.velocityRange = commandArg1;
            break;
        }
        case MML_SET_PLAYER_VOLUME: {
            rPlayerParam.volume = commandArg1;
            break;
        }
        case MML_SET_TRANSPOSE: {
            rTrackParam.transpose = commandArg1;
            break;
        }
        case MML_SET_PITCH_BEND: {
            rTrackParam.pitchBend.SetTarget(commandArg1, commandArg2);
            break;
        }
        case MML_SET_BEND_RANGE: {
            rTrackParam.bendRange = commandArg1;
            break;
        }
        case MML_SET_PAN: {
            rTrackParam.pan.SetTarget(commandArg1 - PAN_CENTER, commandArg2);
            break;
        }
        case MML_SET_INIT_PAN: {
            rTrackParam.initPan = commandArg1 - PAN_CENTER;
            break;
        }
        case MML_SET_SURROUND_PAN: {
            rTrackParam.surroundPan.SetTarget(commandArg1, commandArg2);
            break;
        }
        case MML_SET_PRIORITY: {
            rTrackParam.priority = commandArg1;
            break;
        }
        case MML_SET_NOTE_WAIT: {
            rTrackParam.noteWaitFlag = commandArg1;
            break;
        }
        case MML_SET_PORTATIME: {
            rTrackParam.portaTime = commandArg1;
            break;
        }
        case MML_SET_LFO_DEPTH: {
            rTrackParam.lfoParam.depth = static_cast<u8>(commandArg1) / 128.0f;
            break;
        }
        case MML_SET_LFO_SPEED: {
            rTrackParam.lfoParam.speed =
                static_cast<u8>(commandArg1) * (100.0f / 256.0f);
            break;
        }
        case MML_SET_LFO_TARGET: {
            rTrackParam.lfoTarget = commandArg1;
            break;
        }
        case MML_SET_LFO_RANGE: {
            rTrackParam.lfoParam.range = commandArg1;
            break;
        }
        case MML_SET_LFO_DELAY: {
            rTrackParam.lfoParam.delay = commandArg1 * 5;
            break;
        }
        case MML_SET_SWEEP_PITCH: {
            rTrackParam.sweepPitch = commandArg1 / 64.0f;
            break;
        }
        case MML_SET_ATTACK: {
            rTrackParam.attack = commandArg1;
            break;
        }
        case MML_SET_DECAY: {
            rTrackParam.decay = commandArg1;
            break;
        }
        case MML_SET_SUSTAIN: {
            rTrackParam.sustain = commandArg1;
            break;
        }
        case MML_SET_RELEASE: {
            rTrackParam.release = commandArg1;
            break;
        }
        case MML_SET_ENV_HOLD: {
            rTrackParam.envHold = commandArg1 & 0xFF;
            break;
        }
        case MML_RESET_ADSR: {
            rTrackParam.attack = 0xFF;
            rTrackParam.decay = 0xFF;
            rTrackParam.sustain = 0xFF;
            rTrackParam.release = 0xFF;
            rTrackParam.envHold = 0xFF;
            break;
        }
        case MML_SET_DAMPER: {
            rTrackParam.damperFlag = static_cast<u8>(commandArg1) >= 64;
            break;
        }
        case MML_SET_TIE: {
            rTrackParam.tieFlag = commandArg1;
            pTrack->ReleaseAllChannel(-1);
            pTrack->FreeAllChannel();
            break;
        }
        case MML_SET_MONOPHONIC: {
            rTrackParam.monophonicFlag = commandArg1;

            if (rTrackParam.monophonicFlag) {
                pTrack->ReleaseAllChannel(-1);
                pTrack->FreeAllChannel();
            }
            break;
        }
        case MML_SET_PORTAMENTO: {
            rTrackParam.portaKey = commandArg1 + rTrackParam.transpose;
            rTrackParam.portaFlag = true;
            break;
        }
        case MML_SET_PORTASPEED: {
            rTrackParam.portaFlag = commandArg1 != 0;
            break;
        }
        case MML_SET_LPF_FREQ: {
            rTrackParam.lpfFreq = (commandArg1 - 64) / 64.0f;
            break;
        }
        case MML_SET_BIQUAD_TYPE: {
            rTrackParam.biquadType = commandArg1;
            break;
        }
        case MML_SET_BIQUAD_VALUE: {
            rTrackParam.biquadValue = commandArg1 / 127.0f;
            break;
        }
        case MML_SET_FX_SEND_A: {
            rTrackParam.fxSend[AUX_A] = commandArg1;
            break;
        }
        case MML_SET_FX_SEND_B: {
            rTrackParam.fxSend[AUX_B] = commandArg1;
            break;
        }
        case MML_SET_FX_SEND_C: {
            rTrackParam.fxSend[AUX_C] = commandArg1;
            break;
        }
        case MML_SET_MAIN_SEND: {
            rTrackParam.mainSend = commandArg1;
            break;
        }
        case MML_PRINT_VAR: {
            if (mPrintVarEnabledFlag) {
                const volatile s16* pVariable =
                    GetVariablePtr(pPlayer, pTrack, commandArg1);

                OSReport("#%08x[%d]: printvar %sVAR_%d(%d) = %d\n", pPlayer,
                         pTrack->GetPlayerTrackNo(),
                         commandArg1 >= TRACK_VARIABLE_MIN    ? "T"
                         : commandArg1 >= GLOBAL_VARIABLE_MIN ? "G"
                                                              : "",
                         commandArg1 >= TRACK_VARIABLE_MIN
                             ? commandArg1 - TRACK_VARIABLE_MIN
                         : commandArg1 >= GLOBAL_VARIABLE_MIN
                             ? commandArg1 - GLOBAL_VARIABLE_MIN
                             : commandArg1,
                         commandArg1, *pVariable);
            }
            break;
        }
        case MML_OPEN_TRACK: {
            SeqTrack* pNewTrack = pPlayer->GetPlayerTrack(commandArg1);

            if (pNewTrack == nullptr) {
                break;
            }

            if (pNewTrack == pTrack) {
                break;
            }

            pNewTrack->Close();
            pNewTrack->SetSeqData(rTrackParam.baseAddr, commandArg2);
            pNewTrack->Open();
            break;
        }
        case MML_JUMP: {
            rTrackParam.currentAddr = rTrackParam.baseAddr + commandArg1;
            break;
        }
        case MML_CALL: {
            if (rTrackParam.callStackDepth >= CALL_STACK_DEPTH) {
                break;
            }

            SeqTrack::CallStack* pCallStack =
                &rTrackParam.callStack[rTrackParam.callStackDepth];

            pCallStack->address = rTrackParam.currentAddr;
            pCallStack->loopFlag = false;

            rTrackParam.callStackDepth++;
            rTrackParam.currentAddr = rTrackParam.baseAddr + commandArg1;
            break;
        }
        case MML_RET: {
            SeqTrack::CallStack* pCallStack = nullptr;

            while (rTrackParam.callStackDepth) {
                rTrackParam.callStackDepth--;

                if (!rTrackParam.callStack[rTrackParam.callStackDepth]
                         .loopFlag) {
                    pCallStack =
                        &rTrackParam.callStack[rTrackParam.callStackDepth];
                    break;
                }
            }

            if (pCallStack == nullptr) {
                break;
            }

            rTrackParam.currentAddr = pCallStack->address;
            break;
        }
        case MML_LOOP_START: {
            if (rTrackParam.callStackDepth >= CALL_STACK_DEPTH) {
                break;
            }

            SeqTrack::CallStack* pCallStack =
                &rTrackParam.callStack[rTrackParam.callStackDepth];

            pCallStack->address = rTrackParam.currentAddr;
            pCallStack->loopCount = commandArg1;
            pCallStack->loopFlag = true;

            rTrackParam.callStackDepth++;
            break;
        }
        case MML_LOOP_END: {
            if (rTrackParam.callStackDepth == 0) {
                break;
            }

            SeqTrack::CallStack* pCallStack =
                &rTrackParam.callStack[rTrackParam.callStackDepth - 1];

            if (!pCallStack->loopFlag) {
                break;
            }

            u8 loopCount = pCallStack->loopCount;

            if (loopCount != 0 && --loopCount == 0) {
                rTrackParam.callStackDepth--;
            } else {
                pCallStack->loopCount = loopCount;
                rTrackParam.currentAddr = pCallStack->address;
            }
            break;
        }
        }
    } else if (command <= MML_EX_CMD_MAX) {
        u32 cmdex = command & 0xFF;
        volatile s16* pVariable = nullptr;

        if ((cmdex & 0xF0) == MML_EX_ARITHMETIC ||
            (cmdex & 0xF0) == MML_EX_LOGIC) {
            pVariable = GetVariablePtr(pPlayer, pTrack, commandArg1);

            if (pVariable == nullptr) {
                return;
            }
        }

        switch (cmdex) {
        case MML_EX_SET: {
            *pVariable = commandArg2;
            break;
        }
        case MML_EX_APL: {
            *pVariable += commandArg2;
            break;
        }
        case MML_EX_AMI: {
            *pVariable -= commandArg2;
            break;
        }
        case MML_EX_AMU: {
            *pVariable *= commandArg2;
            break;
        }
        case MML_EX_ADV: {
            if (commandArg2 != 0) {
                *pVariable /= commandArg2;
            }
            break;
        }
        case MML_EX_ALS: {
            if (commandArg2 >= 0) {
                *pVariable <<= commandArg2;
            } else {
                *pVariable >>= -commandArg2;
            }
            break;
        }
        case MML_EX_RND: {
            bool negative = false;

            if (commandArg2 < 0) {
                negative = true;
                commandArg2 = static_cast<s16>(-commandArg2);
            }

            s32 random = Util::CalcRandom();
            random *= commandArg2 + 1;
            random >>= 16;

            if (negative) {
                random = -random;
            }

            *pVariable = random;
            break;
        }
        case MML_EX_AAD: {
            *pVariable &= commandArg2;
            break;
        }
        case MML_EX_AOR: {
            *pVariable |= commandArg2;
            break;
        }
        case MML_EX_AER: {
            *pVariable ^= commandArg2;
            break;
        }
        case MML_EX_ACO: {
            *pVariable = ~static_cast<u16>(commandArg2);
            break;
        }
        case MML_EX_AMD: {
            if (commandArg2 != 0) {
                *pVariable %= commandArg2;
            }
            break;
        }
        case MML_EX_EQ: {
            rTrackParam.cmpFlag = *pVariable == commandArg2;
            break;
        }
        case MML_EX_GE: {
            rTrackParam.cmpFlag = *pVariable >= commandArg2;
            break;
        }
        case MML_EX_GT: {
            rTrackParam.cmpFlag = *pVariable > commandArg2;
            break;
        }
        case MML_EX_LE: {
            rTrackParam.cmpFlag = *pVariable <= commandArg2;
            break;
        }
        case MML_EX_LT: {
            rTrackParam.cmpFlag = *pVariable < commandArg2;
            break;
        }
        case MML_EX_NE: {
            rTrackParam.cmpFlag = *pVariable != commandArg2;
            break;
        }
        case MML_EX_USERPROC: {
            pPlayer->CallSeqUserprocCallback(commandArg1, pTrack);
            break;
        }
        }
    }
}

Channel* MmlParser::NoteOnCommandProc(MmlSeqTrack* pTrack, int key,
                                      int velocity, s32 length,
                                      bool tieFlag) const {
    return pTrack->NoteOn(key, velocity, length, tieFlag);
}

u16 MmlParser::Read16(const u8** ppData) const {
    u16 value = ReadByte(ppData);
    value <<= 8;
    value |= ReadByte(ppData);
    return value;
}

u32 MmlParser::Read24(const u8** ppData) const {
    u32 value = ReadByte(ppData);
    value <<= 8;
    value |= ReadByte(ppData);
    value <<= 8;
    value |= ReadByte(ppData);
    return value;
}

s32 MmlParser::ReadVar(const u8** ppData) const {
    s32 value = 0;
    u8 byte;

    for (int i = 0;; i++) {
        byte = ReadByte(ppData);
        value <<= 7;
        value |= byte & 0x7F;

        if (!(byte & 0x80)) {
            break;
        }
    }

    return value;
}

s32 MmlParser::ReadArg(const u8** ppData, SeqPlayer* pPlayer,
                       SeqTrack* pTrack, SeqArgType argType) const {
    s32 value;

    switch (argType) {
    case SEQ_ARG_U8: {
        value = ReadByte(ppData);
        break;
    }
    case SEQ_ARG_S16: {
        value = Read16(ppData);
        break;
    }
    case SEQ_ARG_VMIDI: {
        value = ReadVar(ppData);
        break;
    }
    case SEQ_ARG_VARIABLE: {
        u8 varNo = ReadByte(ppData);
        const volatile s16* pVariable = GetVariablePtr(pPlayer, pTrack, varNo);

        if (pVariable != nullptr) {
            value = *pVariable;
        }
        break;
    }
    case SEQ_ARG_RANDOM: {
        s32 random;
        s16 min = Read16(ppData);
        s16 max = Read16(ppData);

        random = Util::CalcRandom();
        random *= max - min + 1;
        random >>= 16;
        random += min;

        value = random;
        break;
    }
    }

    return value;
}

volatile s16* MmlParser::GetVariablePtr(SeqPlayer* pPlayer, SeqTrack* pTrack,
                                        int varNo) const {
    if (varNo < TRACK_VARIABLE_MIN) {
        return pPlayer->GetVariablePtr(varNo);
    }

    if (varNo < TRACK_VARIABLE_MAX) {
        return pTrack->GetVariablePtr(varNo - TRACK_VARIABLE_MIN);
    }

    return nullptr;
}

u32 MmlParser::ParseAllocTrack(const void* pBase, u32 seqOffset,
                               u32* pAllocTrack) {
    const u8* pData = static_cast<const u8*>(ut::AddOffsetToPtr(pBase, seqOffset));

    if (*pData != MML_ALLOC_TRACK) {
        *pAllocTrack = 1;
        return seqOffset;
    } else {
        u32 tracks = *++pData;
        tracks <<= 8;
        tracks |= *++pData;

        *pAllocTrack = tracks;
        return seqOffset + 3;
    }
}

}
}
}
