// Little Polygon SDK
// Copyright (C) 2013 Max Kaufmann
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

#include "Graphics.h"

void Graphics::Shader::compile(const char *vsrc, const char *fsrc) {
	mProg = glCreateProgram();
	mVert = glCreateShader(GL_VERTEX_SHADER);
	mFrag = glCreateShader(GL_FRAGMENT_SHADER);
	const char preamble[] = "#define mediump  \n#define highp  \n#define lowp  \n";
	const char* vsrcList[] = { preamble, vsrc };
	const char* fsrcList[] = { preamble, fsrc };
	int vlengths[] = { (int) strlen(preamble), (int) strlen(vsrc) };
	int flengths[] = { (int) strlen(preamble), (int) strlen(fsrc) };
	glShaderSource(mVert, 2, vsrcList, vlengths);
	glShaderSource(mFrag, 2, fsrcList, flengths);
	glCompileShader(mVert);
	glCompileShader(mFrag);

	GLint result;
    glGetShaderiv(mVert, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        GLchar buf[256];
        int len;
        glGetShaderInfoLog(mVert, 256, &len, buf);
        printf("%s\n", buf);
        exit(-1);
    }

    glGetShaderiv(mFrag, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        GLchar buf[256];
        int len;
        glGetShaderInfoLog(mFrag, 256, &len, buf);
        printf("%s\n", buf);
        exit(-1);
    }


	glAttachShader(mProg, mVert);
	glAttachShader(mProg, mFrag);
	glLinkProgram(mProg);
	
	glGetProgramiv(mProg, GL_LINK_STATUS, &result);
	if (result != GL_TRUE) {
		GLchar buf[256];
		int len;
		glGetProgramInfoLog(mProg, 256, &len, buf);
		printf("%s\n", buf);
		exit(-1);
	}
}

void Graphics::Shader::release() {
	glDeleteProgram(mProg);
	glDeleteShader(mFrag);
	glDeleteShader(mVert);
}

