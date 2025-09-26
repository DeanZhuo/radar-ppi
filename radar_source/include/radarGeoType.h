#ifndef RadarGeoType_H
#define RadarGeoType_H

struct Vec2
{
    float x;
    float y;
    Vec2() : x(0), y(0) {}
    Vec2(float _x, float _y) : x(_x), y(_y) {}
};

struct Vec4
{
    float r;
    float g;
    float b;
    float a;
    Vec4() : r(0), g(0), b(0), a(0) {}
    Vec4(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
};

struct RadarVertex
{
    Vec2 position;
    Vec4 color;
};

#endif