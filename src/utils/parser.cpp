#include "utils/parser.hpp"

#include <string>
#include <cstring>

const unsigned char* skipToNextLine(const unsigned char* p) {
    if (!p) return nullptr;
    while (*p != '\n' && *p != '\0') ++p;
    return (*p == '\n') ? p + 1 : p;
}
const unsigned char* skipToNextWord(const unsigned char* p) {	
    if (!p) return nullptr;
    while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') ++p;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',') ++p;
    return p;
}
const unsigned char* skipWhitespace(const unsigned char* p) {
    if (!p) return nullptr;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
    return p;
}

const unsigned char* parseToken(const unsigned char* p, unsigned char* out, const size_t maxLength) {
    p = skipWhitespace(p);
    if (!p || *p == '\0') return nullptr;

    size_t i = 0;
    while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && *p != ',' && i < maxLength - 1)
        out[i++] = *p++;

    out[i] = '\0';

    // Clean up newline characters
    while (i > 0 && (out[i - 1] == '\n' || out[i - 1] == '\r')) 
        out[--i] = '\0';
    
    return skipWhitespace(p);
}
const unsigned char* parseFloat(const unsigned char* p, float& out) {
    p = skipWhitespace(p);
    if (!p || *p == '\0') return nullptr;

    char* end = nullptr;
    out = strtof(reinterpret_cast<const char*>(p), &end);

    if (reinterpret_cast<const char*>(p) == end) 
        return nullptr;

    return reinterpret_cast<const unsigned char*>(end);
}

const unsigned char* parseStringUInt(const unsigned char* p, unsigned int& out) {
    p = skipWhitespace(p);
    if (!p || *p == '\0' || *p < '0' || *p > '9') return nullptr;

    out = 0;
    while (*p >= '0' && *p <= '9')
        out = out * 10 + (*p++ - '0');

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

const unsigned char* parsePlyHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut) {
    if (!p) return nullptr;
    
    numVerticesOut = 0;
    numTrianglesOut = 0;
    hasNormalsOut = false;
    hasColoursOut = false;

    bool hasNx = false, hasNy = false, hasNz = false;
    bool hasRed = false, hasGreen = false, hasBlue = false;

    while(*p){
        const char* lineStart = reinterpret_cast<const char*>(p);
        const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
        int lineLen = static_cast<int>(lineEnd - lineStart);

        const unsigned char* trimmed = skipWhitespace(reinterpret_cast<const unsigned char*>(lineStart));

        if (strncmp((const char*)trimmed, "element", 7) == 0 && (trimmed[7] == ' ' || trimmed[7] == '\t')) {
            trimmed = skipWhitespace(trimmed + 7);

            if (strncmp((const char*)trimmed, "vertex", 6) == 0 && (trimmed[6] == ' ' || trimmed[6] == '\t')) {
                trimmed = skipWhitespace(trimmed + 6);
                trimmed = parseStringUInt(trimmed, numVerticesOut);
            } else if (strncmp((const char*)trimmed, "face", 4) == 0 && (trimmed[4] == ' ' || trimmed[4] == '\t')) {
                trimmed = skipWhitespace(trimmed + 4);
                trimmed = parseStringUInt(trimmed, numTrianglesOut);
            }
        }
        else if (strncmp((const char*)trimmed, "property", 8) == 0) {
            trimmed = skipWhitespace(trimmed + 8);

            char property1[32]{}, property2[32]{}, property3[32]{}, property4[32]{};
            trimmed = parseToken(trimmed, (unsigned char*)property1, sizeof(property1));

            if (strcmp(property1, "list") == 0) {
                if (!(trimmed = parseToken(trimmed, (unsigned char*)property2, sizeof(property2))/* Count type */)) {
                    fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property list count type, line: %s\n", lineStart);
                    return nullptr;
                }
                if (!(trimmed = parseToken(trimmed, (unsigned char*)property3, sizeof(property3))/* Value type */)) {
                    fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property list count type, line: %s\n", lineStart);
                    return nullptr;
                }
                trimmed = parseToken(trimmed, (unsigned char*)property4, sizeof(property4)); // Property name
            }
            else {
                trimmed = parseToken(trimmed, (unsigned char*)property2, sizeof(property2)); //Property Name

                if (strcmp(property2, "nx") == 0 || strcmp(property2, "normal_x") == 0) hasNx = true;
                if (strcmp(property2, "ny") == 0 || strcmp(property2, "normal_y") == 0) hasNy = true;
                if (strcmp(property2, "nz") == 0 || strcmp(property2, "normal_z") == 0) hasNz = true;
                if (strcmp(property2, "red") == 0) hasRed = true;
                if (strcmp(property2, "green") == 0) hasGreen = true;
                if (strcmp(property2, "blue") == 0) hasBlue = true;
                if (strcmp(property2, "alpha") == 0 || strcmp(property2, "a") == 0) hasColoursOut = true; // alpha implies color present
            }
        }
        else if (strncmp((const char*)trimmed, "end_header", 10) == 0) {
            hasNormalsOut = hasNx && hasNy && hasNz;
            hasColoursOut = hasRed && hasGreen && hasBlue;

            return skipToNextLine(trimmed); // jump to first vertex line
        }
        else {
            if (!(strncmp((const char*)trimmed, "ply", 3) == 0) && 
                !(strncmp((const char*)trimmed, "format", 6) == 0) && 
                !(strncmp((const char*)trimmed, "comment", 7) == 0)) {
                // Log only unexpected lines
                fprintf(stderr, "[Warning] %.*s\n", lineLen, lineStart);
            }
        }

        p = reinterpret_cast<const unsigned char*>(lineEnd);
    }

    return nullptr; // failed to find end_header
}

const unsigned char* parseVec3(const unsigned char* p, Vec3& out) {
    if (!(p = parseFloat(p, out.x)) || 
        !(p = parseFloat(p, out.y)) || 
        !(p = parseFloat(p, out.z))) 
        return nullptr;
    
    if (*p == ',') ++p;
    return p;
}
const unsigned char* parseVec4(const unsigned char* p, Vec4& out) {
  if (!(p = parseFloat(p, out.x)) ||
    !(p = parseFloat(p, out.y)) ||
    !(p = parseFloat(p, out.z)) ||
    !(p = parseFloat(p, out.w)))
    return nullptr;

  if (*p == ',') ++p;
  return p;
}

const unsigned char* parseColour(const unsigned char* p, Vec4& colourOut, ColourMode& modeOut) {
  colourOut = { 1.0f, 1.0f, 1.0f, 0.0f };
  modeOut = ColourMode::PLYColour;

  p = skipWhitespace(p);
  if (*p != '\0' && *p != '-') {
    Vec3 colour = { 1.0f, 1.0f, 1.0f };
    const unsigned char* original = p;

    if ((p = parseVec3(p, colour))) {
      if (colour.x > 1.0f || colour.y > 1.0f || colour.z > 1.0f) {
        colour.x /= 255.0f;
        colour.y /= 255.0f;
        colour.z /= 255.0f;
      }

      colourOut = { colour, 0.0f };
      modeOut = ColourMode::Solid;
      return p;
    }

    p = original;
    char colourName[64]{};
    p = parseToken(p, (unsigned char*)colourName, sizeof(colourName));


    if (strcmp(colourName, "Red") == 0) colour = { 1.0f, 0.0f, 0.0f };
    else if (strcmp(colourName, "Green") == 0) colour = { 0.0f, 1.0f, 0.0f };
    else if (strcmp(colourName, "Blue") == 0) colour = { 0.0f, 0.0f, 1.0f };
    else if (strcmp(colourName, "Yellow") == 0) colour = { 1.0f, 1.0f, 0.0f };
    else if (strcmp(colourName, "White") == 0) colour = { 1.0f, 1.0f, 1.0f };
    else if (strcmp(colourName, "Gray") == 0 || strcmp(colourName, "Grey") == 0) colour = { 0.5f, 0.5f, 0.5f };
    else if (strcmp(colourName, "Random") == 0) {
      modeOut = ColourMode::Random;
      return p;
    }
    else if (strcmp(colourName, "Rainbow") == 0) {
      modeOut = ColourMode::VerticalGradient;
      return p;
    }

    colourOut = { colour, 0.0f };
    modeOut = ColourMode::Solid;
  }

  return p;
}