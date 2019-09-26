#include "devilution.h"
#include "miniwin/dsound.h"
#include "stubs.h"
#include <SDL.h>

namespace dvl {

///// DirectSoundBuffer /////

void DirectSoundBuffer::Release()
{
	Mix_FreeChunk(chunk);
};

void DirectSoundBuffer::GetStatus(LPDWORD pdwStatus)
{
	for (int i = 1; i < Mix_AllocateChannels(-1); i++) {
		if (Mix_GetChunk(i) == chunk && Mix_Playing(i)) {
			*pdwStatus = DVL_DSBSTATUS_PLAYING;
			break;
		}
	}
};

void DirectSoundBuffer::Play(int lVolume, int lPan)
{
	int channel = Mix_PlayChannel(-1, chunk, 0);
	if (channel == -1) {
		SDL_Log("Too few channels, skipping sound\n");
		return;
	}

	Mix_Volume(channel, pow(10, lVolume / 2000.0) * MIX_MAX_VOLUME);
	int pan = copysign(pow(10, -abs(lPan) / 2000.0) * 255, lPan);
	Mix_SetPanning(channel, pan > 0 ? pan : 255, pan < 0 ? abs(pan) : 255);
};

void DirectSoundBuffer::Stop()
{
	for (int i = 1; i < Mix_AllocateChannels(-1); i++) {
		if (Mix_GetChunk(i) != chunk) {
			continue;
		}

		Mix_HaltChannel(i);
	}
};

const char *DirectSoundBuffer::SetChunk(BYTE *fileData, DWORD dwBytes)
{
	SDL_RWops *buf1 = SDL_RWFromConstMem(fileData, dwBytes);
	if (buf1 == NULL) {
		return SDL_GetError();
	}

	chunk = Mix_LoadWAV_RW(buf1, 1);
	if (chunk == NULL) {
		return SDL_GetError();
	}

	return NULL;
};

} // namespace dvl
