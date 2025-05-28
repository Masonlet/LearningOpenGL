#include "parser.hpp"
#include <string>

const unsigned int strEquals(const unsigned char* a, const unsigned char* b) {
	while (*a && *b) {
		if (*a != *b)
			return 0;
		++a, ++b;
	}

	return *a == *b;
}

const unsigned char* skipToNextLine(const unsigned char* p) {
	while (*p != '\n' && *p != '\0') ++p;
	return (*p == '\n') ? p + 1 : p;
}
const unsigned char* skipToNextWord(const unsigned char* p) {
	while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') ++p;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
	return p;
}
const unsigned char* skipWhitespace(const unsigned char* p) {
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
	return p;
}

const unsigned char* parseToken(const unsigned char* p, unsigned char* out, const size_t maxLength) {
	p = skipWhitespace(p);

	size_t i = 0;
	while (*p && *p != ' ' && *p != '\t' && *p != '\r' && i < maxLength - 1)
		out[i++] = *p++;

	out[i] = '\0';

	return p;
}
const unsigned char* parseFloat(const unsigned char* p, float& out) {
	p = skipWhitespace(p);
	out = 0.0f;
	bool neg = false;

	if (*p == '-') {
		neg = true;
		p++;
	}

	//Parse Integer
	while (*p >= '0' && *p <= '9') 
		out = out * 10.0f + (*p++ - '0');

	//Parse Fractional
	if (*p == '.') {
		p++;
		float frac = 0.0f;
		float factor = 0.1f;
		while (*p >= '0' && *p <= '9') {
			frac += (*p++ - '0') * factor;
			factor *= 0.1f;
		}

		out += frac;
	}

	if (neg) out = -out;
	return p;
}

const unsigned char* parseStringUInt(const unsigned char* p, unsigned int& out) {
	p = skipWhitespace(p);
	out = 0;

	if (*p < '0' || *p > '9')
		return nullptr; 

	while (*p >= '0' && *p <= '9') {
		out = out * 10 + (*p++ - '0');
	}

	return p;
}
const unsigned int parseBinaryUINT(unsigned char* buffer) {
	/* bit shifting <<
	* x 00000001 
	* << becomes,
	* x 00000010
	*/

	/* bitwise or |
	* x	    00000011
	* y	    00000110
	* x | y	00000111
	*/
	return (buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3]);
}

const unsigned char* parseHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut) {
	while(*p){
		p = skipWhitespace(p);

		if (p[0] == 'e') {
			if (strncmp((const char*)p, "element", 7) == 0 && (p[7] == ' ' || p[7] == '\t')) { //element
				p += 7;
				p = skipWhitespace(p);

				if (strncmp((const char*)p, "vertex", 6) == 0 && (p[6] == ' ' || p[6] == '\t')) {
					p += 6;
					p = parseStringUInt(p, numVerticesOut);
				} else if (strncmp((const char*)p, "face", 4) == 0 && (p[4] == ' ' || p[4] == '\t')) {
					p += 4;
					p = parseStringUInt(p, numTrianglesOut);
				}
			} else if (strncmp((const char*)p, "property", 8) == 0) {
				p += 8;
				p = skipWhitespace(p);

				p = skipToNextWord(p);

			} else if (strncmp((const char*)p, "end_header", 10) == 0) {
				p = skipToNextLine(p);
				break;
			} 
		}
		
		p = skipToNextLine(p);
	}

	return p;
}
