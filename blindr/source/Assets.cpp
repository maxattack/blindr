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

#include "Macros.h"
#include "Assets.h"
#include <SDL.h>

//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/mman.h>
#include <cstdio>
#include <string>

using namespace std;

int Assets::load(const char* name, void **outData, size_t *outLength) {
	string path = string(name)+".bin";
	SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
	
	// get the length
	if (SDL_RWseek(file, 0, RW_SEEK_END) != -1) {
		*outLength = (size_t) SDL_RWtell(file);
		SDL_RWseek(file, 0, RW_SEEK_SET);
	} else {
		return -1;
	}
	
	*outData = malloc(*outLength);
	if (SDL_RWread(file, *outData, *outLength, 1) == -1) {
		return -1;
	}
	SDL_RWclose(file);

	//	do pointer fixup
	uint8_t *bytes = (uint8_t*)*outData;
	file = SDL_RWFromFile((std::string(name)+".pf").c_str(), "rb");
	uint32_t offset;
	while(SDL_RWread(file, &offset, 4, 1)) {
		*reinterpret_cast<size_t*>(&bytes[offset]) += size_t(bytes);
	}
	SDL_RWclose(file);

	return 0;
}

int Assets::unload(void *p, size_t length) {
	free(p);
	return 0;
}


//
//int Assets::load(const char *path, void **outData, size_t *outLength) {
//	// memory_map the bin
//	int fd = open((std::string(path)+".bin").c_str(), O_RDONLY);
//	if (fd == -1) {
//		return 0;
//	}
//	struct stat sb;
//	if (fstat(fd, &sb)) {
//		close(fd);
//		return 0;
//	}
//	if (!S_ISREG(sb.st_mode)) {
//		close(fd);
//		return 0;
//	}
//	size_t len = size_t(sb.st_size);
//	void *p = mmap(0, len, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
//	if (p == MAP_FAILED) {
//		close(fd);
//		return 0;
//	}
//	close(fd);
//
//	//	do pointer fixup
//	uint8_t *bytes = (uint8_t*)p;
//	FILE *ptr = fopen((std::string(path)+".pf").c_str(), "rb");
//	ASSERT(ptr);
//	uint32_t offset;
//	while(fread(&offset, 4, 1, ptr)) {
//		*reinterpret_cast<size_t*>(bytes + offset) += size_t(p);
//	}
//	fclose(ptr);
//
//	// write results
//	*outData = p;
//	*outLength = len;
//	return 0;
//}
//
//int Assets::unload(void *p, size_t length) {
//	if (length == 0) {
//		return -1;
//	} else {
//		munmap(p, length);
//	}
//	return 0;
//}
