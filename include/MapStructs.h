#pragma once

#include "util.h"

struct Line;
struct Sector;
struct HitRecord;

// A line segment in the world
struct Line {
    bool isFullWall;

    vec2 p1;
    vec2 p2;

    Sector* side1Sector;
    Sector* side2Sector;

    int getSector(Ray ray)
    {
        vec2 p2p1 = p2-p1;
        float fakeCross = (ray.d.x * p2p1.y) - (ray.d.y * p2p1.x);
        if (fakeCross > 0) return 1;
        else return 2;
    }

    bool intersect(Ray ray, vec2* point)
    {
        vec2 p3(ray.o.x, ray.o.y);
        vec2 p4(ray.o.x - ray.d.x, ray.o.y - ray.d.y);
        float d = (p1.x-p2.x)*(p3.y-p4.y)-(p1.y-p2.y)*(p3.x-p4.x);
        if (d == 0.0f)
        {
            return false;
        }
        float t = ((p1.x-p3.x)*(p3.y-p4.y)-(p1.y-p3.y)*(p3.x-p4.x))/d;
        float u = ((p1.x-p2.x)*(p1.y-p3.y)-(p1.y-p2.y)*(p1.x-p3.x))/d;

        if (t > 0.0f && t < 1.0f && u > 0)
        {
            if (point) *point = vec2(p1.x + t * (p2.x-p1.x), p1.y + t * (p2.y-p1.y));
            return true;
        }
        return false;
    }
};

struct Sector {

    float floorHeight, ceilHeight;
    RGBA ceilColor, floorColor;

    bool isCurrentSector(vec2 playerPos, float playerRot, std::vector<Line> lines) // probably broke lol
    {
        Ray testRay = {playerPos, {cos(playerRot), sin(playerRot)}};
        int numIntersections = 0;
        for (Line line : lines) if (line.side1Sector == this || line.side2Sector == this)
        {
            if (line.intersect(testRay, NULL))
            {
                numIntersections++;
            }
        }
        if (numIntersections%2 > 0) // If odd inside this sector
        {
            return true;
        }
        return false;
    }
};

struct HitRecord {
    float dist;
    Line line;
    int whichSide;
};

bool sortByDist(const HitRecord& A, const HitRecord& B) {
    return A.dist < B.dist;
}
