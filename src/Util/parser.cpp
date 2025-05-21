#include "parser.hpp"

const int strEquals(const char* a, const char* b) {
	while (*a && *b) {
		if (*a != *b)
			return 0;
		++a, ++b;
	}

	return *a == *b;
}

const char* skipToNextLine(const char* p) {
	while (*p != '\n' && *p != '\0') ++p;
	return (*p == '\n') ? p + 1 : p;
}
const char* skipWhitespace(const char* p) {
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
	return p;
}

const char* parseToken(const char* p, char* out, const size_t maxLength) {
	skipWhitespace(p);

	size_t i = 0;
	while (*p && *p != ' ' && *p != '\t' && *p != '\r' && i < maxLength - 1)
		out[i++] = *p++;

	out[i] = '\0';

	return p;
}
const char* parseFloat(const char* p, float& out) {
	p = skipWhitespace(p);
	out = 0.0f;
	bool neg = false;

	if (*p == '-') {
		neg = true;
		p++;
	}

	while (*p >= '0' && *p <= '9') {
		out = out * 10.0f + (*p++ - '0');
	}

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
const char* parseUInt(const char* p, unsigned int& out) {
	p = skipWhitespace(p);
	out = 0;

	if (*p < '0' || *p > '9')
		return nullptr; 

	while (*p >= '0' && *p <= '9') {
		out = out * 10 + (*p++ - '0');
	}

	return p;
}

const char* parseHeader(const char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut) {
	while(*p){
		p = skipWhitespace(p);

		if (p[0] == 'e') {
			if (p[1] == 'l' && p[2] == 'e' && p[3] == 'm' && p[4] == 'e' && p[5] == 'n' && p[6] == 't'  && (p[7]== ' ' || p[7] == '\t')) { //element
				p += 7;
				p = skipWhitespace(p);

				if (p[0] == 'v' && p[1] == 'e' && p[2] == 'r' && p[3] == 't' && p[4] == 'e' && p[5] == 'x'  && (p[6]== ' ' || p[6] == '\t')) {
					p += 6;
					p = parseUInt(p, numVerticesOut);
				} 
				else if (p[0] == 'f' && p[1] == 'a' && p[2] == 'c' && p[3] == 'e' && (p[4]== ' ' || p[4] == '\t')) {
					p += 4;
					p = parseUInt(p, numTrianglesOut);
				} 
			}
			else if (p[1] == 'n' && p[2] == 'd' && p[3] == '_' && p[4] == 'h' && p[5] == 'e'  && p[6] == 'a' && p[7] == 'd' && p[8] == 'e' && p[9] == 'r') {
				p = skipToNextLine(p);
				break;
			} 
		}
		
		p = skipToNextLine(p);
	}

	return p;
}
