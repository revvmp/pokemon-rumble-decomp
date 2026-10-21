#ifndef GAME_SCRIPTAUDIO_H
#define GAME_SCRIPTAUDIO_H

namespace NPpu {
class CEngine;
}

int PlayBGMTest(NPpu::CEngine* pEngine);
int StartSETest(NPpu::CEngine* pEngine);
int ContinueBGMTest(NPpu::CEngine* pEngine);
int StopBGM(NPpu::CEngine* pEngine);
int method_startSE(NPpu::CEngine* pEngine);
int method_stopSE(NPpu::CEngine* pEngine);
int method_getPitch(NPpu::CEngine* pEngine);
int method_setPitch(NPpu::CEngine* pEngine);
int GetPokemonVoiceId(NPpu::CEngine* pEngine);

#endif
