// TYRANOFORCE
// Copyright (C) 2013 Frederic Tarabout and Max Kaufmann
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Audio.h"
#include "SDL.h"
#include <cstring>
#include <SDL_mixer.h>
#include <zlib.h>

// save loaded chunks in a table so we can unload them all
// en-masse
struct SampleChunkPair {
	SampleAsset *asset;
	Mix_Chunk *chunk;
};
#define CHUNK_CAPACITY 64
static int chunkCount;
SampleChunkPair loadedChunks[CHUNK_CAPACITY];

void Audio::init() {
	Mix_Init(MIX_INIT_MOD | MIX_INIT_MODPLUG | MIX_INIT_FLAC | MIX_INIT_FLUIDSYNTH | MIX_INIT_MP3 | MIX_INIT_OGG);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
	chunkCount = 0;
}

void Audio::release() {
	Mix_CloseAudio();
	Mix_Quit();
}

struct WaveHeader {
	uint8_t ChunkId[4];
	uint32_t ChunkSize;
	uint8_t Format[4];
	
	uint8_t Subchunk1ID[4];
	uint32_t Subchunk1Size;
	uint16_t AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	
	uint8_t Subchunk2ID[4];
	uint32_t Subchunk2Size;
	
	void init(int numChannels, int sampleRate, int sampleWidth, int numSamples) {
		NumChannels = numChannels;
		SampleRate = sampleRate;
		
		ByteRate = sampleRate * numChannels * sampleWidth;
		BlockAlign = numChannels * sampleWidth;
		BitsPerSample = (sampleWidth<<3);
		Subchunk2Size = numSamples * numChannels * sampleWidth;
		ChunkSize = 36 + Subchunk2Size;
	}
};

static void doLoadSample(SampleAsset *sample) {
	ASSERT(chunkCount < CHUNK_CAPACITY);
	// Allocate a buffer for the RW_ops structure to read from 
	Bytef *scratch = (Bytef*) malloc(sample->size + sizeof(WaveHeader));
	{
	// Mixer expects a WAVE header on PCM data, so let's provide it :P
	WaveHeader hdr = {{'R','I','F','F'},0,{'W','A','V','E'},{'f','m','t',' '},16,1,1,0,0,0,0,{'d','a','t','a'},0};
	int sampleCount = sample->size / (sample->sampleWidth * sample->channelCount);
	hdr.init(sample->channelCount, sample->frequency, sample->sampleWidth, sampleCount);
	memcpy(scratch, &hdr, sizeof(WaveHeader));
	}
	// Now decompress the actual PCM data
	uLongf size = sample->size;
	uncompress(scratch + sizeof(WaveHeader), &size, (const Bytef*)sample->compressedData, sample->compressedSize);
	// load the chunk
	Mix_Chunk *chunk = Mix_LoadWAV_RW(SDL_RWFromMem(scratch, sample->size + sizeof(WaveHeader)),1);
	ASSERT(chunk);
	// register pair
	loadedChunks[chunkCount].asset = sample;
	loadedChunks[chunkCount].chunk = chunk;
	chunkCount++;
	// save handle in asset (index+1)
	sample->bufferHandle = chunkCount;
	free(scratch);
}

void Audio::playSample(SampleAsset *sample) {
	if (sample->bufferHandle == 0) { doLoadSample(sample); }
	Mix_PlayChannel(-1, loadedChunks[sample->bufferHandle-1].chunk, 0);
}


