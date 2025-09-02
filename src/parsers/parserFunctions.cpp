	#include "parsers/parserFunctions.hpp"
	#include "utils/log.hpp"
	#include <cstdlib>  
	#include <cstring>
	#include <cstdio>

	static inline bool isDelim(unsigned char c, bool comma = true) {
		return c == 0 || c == ' ' || c == '\t' || c == '\n' || c == '\r' || (comma && c == ',');
	}

	const unsigned char* skipToNextLine(const unsigned char* p) {
		if (!p) return nullptr;
		while (*p && *p != '\n' && *p != '\r') ++p;
		if (*p == '\r') ++p;
		if (*p == '\n') ++p;
		return p;
	}
	const unsigned char* skipWhitespace(const unsigned char* p, bool skipComma) {
		while (p && *p && isDelim(*p, skipComma)) ++p;
		return p ? p : nullptr;
	}
	const unsigned char* trimEOL(const unsigned char* p, const unsigned char* end) {
		while (end && *end && end > p && (end[-1] == '\n' || end[-1] == '\r' || end[-1] == ',')) --end;
		return end ? end : nullptr;
	}

	bool parseUInt(const unsigned char*& p, unsigned int& out) {
		p = skipWhitespace(p);
		if (!p || *p == '\0' || *p < '0' || *p > '9') return false;

		out = 0;
		while (*p >= '0' && *p <= '9') out = out * 10 + (*p++ - '0');
		return true;
	}
	bool parseBool(const unsigned char*& p, bool& out) {
		p = skipWhitespace(p);
		if (!p || *p == '\0') return false;

		if (*p == '1') { ++p; out = true;  return true; }
		if (*p == '0') { ++p; out = false; return true; }

		unsigned char tok[6]{};
		if (!parseToken(p, tok, sizeof(tok))) return false;

		const char* str = reinterpret_cast<const char*>(tok);
		if (strcmp(str, "true") == 0 || strcmp(str, "on") == 0) { out = true; return true; }
		if (strcmp(str, "false") == 0 || strcmp(str, "off") == 0) { out = false; return true; }

		return false;
	}

	static inline int parseFloatSign(const unsigned char*& p) {
		int s = 1;
		if (*p == '+') ++p;
		else if (*p == '-') { ++p; s = -1; }
		return s;
	}
	static inline int parseFloatDigit(const unsigned char*& p, unsigned long long& u) {
		int n = 0;
		while (*p >= '0' && *p <= '9') {
			u = u * 10ULL + (unsigned long long)(*p - '0');
			++p;
			++n;
		}
		return n;
	}
	static const double POW10_NEG9[10] = {
		1.0, 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9
	};
	bool parseFloat(const unsigned char*& p, float& out) {
		p = skipWhitespace(p);
		if (!p || *p == '\0') return false;

		const unsigned char* start = p;
		int sign = parseFloatSign(p);
		unsigned long long i = 0ULL;
		int iCount = parseFloatDigit(p, i);

		double val = static_cast<double>(i);
		bool hasFrac{ 0 };
		if (*p == '.') {
			++p;
			unsigned long long f = 0ULL;
			int kept = 0;

			while (*p >= '0' && *p <= '9') {
				hasFrac = 1;
				if (kept < 9) {
					f = f * 10ULL + static_cast<unsigned long long>(*p - '0');
					++kept;
				}
				++p;
			}
			if (kept > 0) val += static_cast<double>(f) * POW10_NEG9[kept];
		}

		if (iCount == 0 && !hasFrac) { 
			p = start;
			return false;
		}

		if (*p == 'e' || *p == 'E') {
			++p;
			int esign = 1;
			if (*p == '+') ++p;
			else if (*p == '-') { esign = -1; ++p; }

			int any = 0, e = 0;
			while (*p >= '0' && *p <= '9') {
				any = 1;
				if (e < 100000000) e = e * 10 + (*p - '0');
				++p;
			}
			if (!any) { 
				p = start; 
				return false; 
			}

			static const double P10P[39] = {
				1.0,1e1,1e2,1e3,1e4,1e5,1e6,1e7,1e8,1e9,
				1e10,1e11,1e12,1e13,1e14,1e15,1e16,1e17,1e18,1e19,
				1e20,1e21,1e22,1e23,1e24,1e25,1e26,1e27,1e28,1e29,
				1e30,1e31,1e32,1e33,1e34,1e35,1e36,1e37,1e38
			};
			static const double P10N[39] = {
				1.0,1e-1,1e-2,1e-3,1e-4,1e-5,1e-6,1e-7,1e-8,1e-9,
				1e-10,1e-11,1e-12,1e-13,1e-14,1e-15,1e-16,1e-17,1e-18,1e-19,
				1e-20,1e-21,1e-22,1e-23,1e-24,1e-25,1e-26,1e-27,1e-28,1e-29,
				1e-30,1e-31,1e-32,1e-33,1e-34,1e-35,1e-36,1e-37,1e-38
			};

			if (esign >= 0) val *= (e > 38 ? 1e308 : P10P[e]);
			else            val *= (e > 38 ? 0.0   : P10N[e]);
		}

		if (sign < 0) val = -val;
		out = static_cast<float>(val);
		return true;
	}

	bool parseVec2(const unsigned char*& p, Vec2& out) {
		return parseFloat(p, out.x) && parseFloat(p, out.y);
	}
	bool parseVec3(const unsigned char*& p, Vec3& out) {
		return parseFloat(p, out.x) && parseFloat(p, out.y) && parseFloat(p, out.z);
	}
	bool parseVec4(const unsigned char*& p, Vec4& out) {
		return parseFloat(p, out.x) && parseFloat(p, out.y) && parseFloat(p, out.z) && parseFloat(p, out.w);
	}

	bool parseToken(const unsigned char*& p, unsigned char* out, const size_t maxLength) {
		if (!p || !out || maxLength == 0) return false;
		p = skipWhitespace(p);
		if (!p || *p == '\0') return false;

		size_t i = 0;
		while (*p && !isDelim(*p) && i + 1 < maxLength) out[i++] = *p++;
		out[i] = '\0';
		return true;
	}

	static bool parseNumericColour(const unsigned char*& p, Vec4& out) {
		const unsigned char* original = p;
		if (parseVec4(p, out)) {
			if (out.x > 1.0f) out.x /= 255.0f;
			if (out.y > 1.0f) out.y /= 255.0f;
			if (out.z > 1.0f) out.z /= 255.0f;
			if (out.w > 1.0f) out.w /= 255.0f;
			return true;
		}

		p = original;
		Vec3 rgb;
		if (parseVec3(p, rgb)) {
			out = { rgb.x, rgb.y, rgb.z, 1.0f };
			if (out.x > 1) out.x /= 255.f;
			if (out.y > 1) out.y /= 255.f;
			if (out.z > 1) out.z /= 255.f;
			return true;
		}

		p = original;
		return false;
	}
	static bool parseSpecialColour(const char* name, ColourMode& modeOut) {
		if      (strcmp(name, "Random")  == 0) modeOut = ColourMode::Random;
		else if (strcmp(name, "Rainbow") == 0) modeOut = ColourMode::VerticalGradient;
		else if (strcmp(name, "PLY")     == 0) modeOut = ColourMode::PLYColour;
		else return false;
		return true;
	}
	static bool parseNamedColour(const unsigned char*& p, Vec4& colour, ColourMode& mode) {
		unsigned char input[64]{};
		if (!parseToken(p, input, sizeof(input)) || !p) return false;

		const char* name = reinterpret_cast<const char*>(input);
		if      (strcmp(name, "Red") == 0)    colour = { 1.0f, 0.0f, 0.0f, 1.0f };
		else if (strcmp(name, "Green") == 0)  colour = { 0.0f, 1.0f, 0.0f, 1.0f };
		else if (strcmp(name, "Blue") == 0)   colour = { 0.0f, 0.0f, 1.0f, 1.0f };
		else if (strcmp(name, "Yellow") == 0) colour = { 1.0f, 1.0f, 0.0f, 1.0f };
		else if (strcmp(name, "White") == 0)  colour = { 1.0f, 1.0f, 1.0f, 1.0f };
		else if (strcmp(name, "Gray") == 0
					|| strcmp(name, "Grey") == 0)   colour = { 0.5f, 0.5f, 0.5f, 1.0f };
		else return (parseSpecialColour(name, mode));
		return true;
	}
	bool parseColour(const unsigned char*& p, Vec4& colourOut, ColourMode& modeOut) {
		const unsigned char* original = p;
		if (parseNumericColour(p, colourOut)) return true;
		p = original;
		if (parseNamedColour(p, colourOut, modeOut)) return true;
		p = original;
		return error("Parser", "parseColour", "Unknown colour format");
	}
	bool parseLightType(const unsigned char*& p, unsigned int& typeOut) {
		p = skipWhitespace(p);
		if (!p || *p == '\0') return false;
		if (*p == '0') { ++p; typeOut = 0; return true; }
		if (*p == '1') { ++p; typeOut = 1; return true; }
		if (*p == '2') { ++p; typeOut = 2; return true; }

		const unsigned char* original = p;
		if (parseUInt(p, typeOut)) return true;
		
		p = original;
		unsigned char typeName[64]{};
		if (!parseToken(p, typeName, sizeof(typeName)) || !p) { 
			p = original; 
			return false; 
		}

		if      (strcmp((char*)typeName, "Point") == 0)       typeOut = 0;
		else if (strcmp((char*)typeName, "Spot") == 0)        typeOut = 1;
		else if (strcmp((char*)typeName, "Directional") == 0) typeOut = 2;
		else return error("Parser", "parseLightType", "Unknown light type");
		return true;
	}
	bool parseCameraType(const unsigned char*& p, unsigned int& typeOut) {
		p = skipWhitespace(p);
		if (!p || *p == '\0') return false;

		const unsigned char* original = p;
		if (parseUInt(p, typeOut)) return true;

		p = original;
		unsigned char typeName[64]{};
		if (!parseToken(p, typeName, sizeof(typeName)) || !p) {
			p = original;
			return false;
		}

		if      (strcmp((char*)typeName, "FreeCam") == 0)    typeOut = 0;
		else if (strcmp((char*)typeName, "DungeonCam") == 0) typeOut = 1;
		else if (strcmp((char*)typeName, "ModernCam") == 0)  typeOut = 2;
		else return error("Parser", "parseCameraType", "Unknown camera type");
		return true;
	}