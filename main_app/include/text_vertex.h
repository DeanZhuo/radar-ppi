#ifndef text_vertex_h
#define text_vertex_h

#include <string>
#include <vector>
#include <cstring>
#include "stb_easy_font.h"
#include "RadarTypes.h"

struct Vertex16
{
    float x;
    float y;
    float z;
    unsigned char c[4]; // packed color from stb (ignored here)
};

class TextVertex
{
public:
    // build text into a vertex list suitable for RadarRenderer expecting NDC coords
    // text: string
    // startX, startY: pixel coordinates (stb origin: top-left, y downwards)
    // screenW/screenH: viewport size in pixels (used to convert to NDC)
    // color: final color (Vec4) in floats [0..1]
    std::vector<RadarVertex> build(const std::string &text,
                                   float startX, float startY,
                                   int screenW, int screenH,
                                   const Vec4 &color = Vec4(1.0f, 1.0f, 1.0f, 1.0f),
                                   float scale = 1.0f)
    {
        if (screenW <= 0 || screenH <= 0 || scale <= 0.0f)
            return {};

        // stb expects bytes; each vertex is 16 bytes; quads * 64 bytes.
        const int BUF_SIZE = 64 * 1024; // 64 KB
        std::unique_ptr<char[]> buffer(new char[BUF_SIZE]);

        int num_quads = stb_easy_font_print(startX, startY,
                                            const_cast<char *>(text.c_str()),
                                            nullptr, buffer.get(), BUF_SIZE);
        if (num_quads <= 0)
            return {};

        int num_vertices = num_quads * 4; // 4 vertices per quad
        Vertex16 *vbuf = reinterpret_cast<Vertex16 *>(buffer.get());

        std::vector<RadarVertex> out;
        out.reserve(num_quads * 8); // 8 RadarVertex per quad (4 line segments)

        // Helper: scale a pixel point (px,py) about the origin (startX,startY)
        auto scalePoint = [&](float px, float py) -> Vec2
        {
            float sx = startX + (px - startX) * scale;
            float sy = startY + (py - startY) * scale;
            // convert to NDC
            float ndcX = (sx / (float)screenW) * 2.0f - 1.0f;
            float ndcY = 1.0f - (sy / (float)screenH) * 2.0f;
            return Vec2(ndcX, ndcY);
        };

        // Helper: push a radar vertex
        auto push = [&](const Vec2 &p)
        {
            RadarVertex rv;
            rv.position = p;
            rv.color = color;
            out.push_back(rv);
        };

        // convert each quad -> 4 line segments
        for (int q = 0; q < num_quads; ++q)
        {
            Vertex16 *quad = vbuf + q * 4;

            Vec2 n0 = scalePoint(quad[0].x, quad[0].y);
            Vec2 n1 = scalePoint(quad[1].x, quad[1].y);
            Vec2 n2 = scalePoint(quad[2].x, quad[2].y);
            Vec2 n3 = scalePoint(quad[3].x, quad[3].y);

            // lines: 0-1, 1-2, 2-3, 3-0
            push(n0);
            push(n1);
            push(n1);
            push(n2);
            push(n2);
            push(n3);
            push(n3);
            push(n0);
        }

        return out;
    }
};

#endif