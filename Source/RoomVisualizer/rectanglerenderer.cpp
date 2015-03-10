#include "RoomVisualizer.h"
#include "rectanglerenderer.h"
#include <map>
#include <algorithm>
#include <iterator>

using namespace std;

enum {
    EVENT_NONE=0,
    EVENT_RECT_START,
    EVENT_RECT_END
};
class Event {
    public:
        Event(double xx, double bot, double top, int t=EVENT_NONE)
            : type(t), x(xx), y1(bot), y2(top) {;}

        int type;
        double x;
        double y1, y2;
};

bool evtcmp(Event i, Event j) {
    if (i.x == j.x) {
        return i.type == EVENT_RECT_END;
    } else {
        return i.x < j.x;
    }
}

typedef map<double, pair<double,int> > mddi;
inline Rect endRectangle(Rect base, int x, int y, mddi::iterator i, double cx, mddi& s) {
    Rect newRect = base;
    newRect.p[x] = i->second.first;
    newRect.p[y] = i->first;
    newRect.w = cx - newRect.p[x];
    mddi::iterator ni = i;
    ni++;
    if (ni == s.end()) {
        newRect.h = base.h+base.p[y] - i->first;
    } else {
        newRect.h = ni->first - i->first;
    }
    return newRect;
}

void rectangleDiff(
        Rect base,
        vector<Rect>& diff,
        vector<Rect>& generated)
{
    pair<int,int> ax = base.axisIndices();
    int x = ax.first;
    int y = ax.second;

    mddi segments;
    segments.insert(make_pair(base.p[y], make_pair(base.p[x], 0)));

    // Populate events
    vector<Event> events;
    for (int i = 0; i < diff.size(); ++i) {
        events.push_back(Event(diff[i].p[x], diff[i].p[y], diff[i].p[y]+diff[i].h, EVENT_RECT_START));
        events.push_back(Event(diff[i].p[x]+diff[i].w, diff[i].p[y], diff[i].p[y]+diff[i].h, EVENT_RECT_END));
    }
    sort(events.begin(), events.end(), evtcmp);

    for (int i = 0; i < events.size(); ++i) {
        mddi::iterator lower;
        mddi::iterator upper;
        if (events[i].type == EVENT_RECT_START) {
            lower = segments.upper_bound(events[i].y1);
            --lower;
            upper = segments.upper_bound(events[i].y2);
            --upper;
            int tmp = upper->second.second;
            upper++;
            for (mddi::iterator it = lower; it != upper; ++it) {
                if (it->second.second == 0) {
                    Rect newRect = endRectangle(base, x, y, it, events[i].x, segments);

                    if (newRect.w > 0 && newRect.h > 0) {
                        generated.push_back(newRect);
                    }
                }
            }
            lower->second.first = events[i].x;
            lower++;
            if (lower != upper) {
                segments.erase(lower, upper);
            }
            segments.insert(make_pair(events[i].y1, make_pair(events[i].x, 1)));
            segments.insert(make_pair(events[i].y2, make_pair(events[i].x, tmp)));
        } else if (events[i].type == EVENT_RECT_END) {
            lower = segments.find(events[i].y1);
            upper = segments.find(events[i].y2);
            lower--;
            if (lower->second.second == 0) {
                Rect newRect = endRectangle(base, x, y, lower, events[i].x, segments);
                if (newRect.w > 0 && newRect.h > 0)
                    generated.push_back(newRect);
            }
            if (upper != segments.end()) {
                if (upper->second.second == 0) {
                    Rect newRect = endRectangle(base, x, y, upper, events[i].x, segments);
                    if (newRect.w > 0 && newRect.h > 0)
                        generated.push_back(newRect);
                }
                segments.erase(upper);
            }
            lower->second = make_pair(events[i].x, 0);
            segments.erase(++lower);
        }
    }
    for (mddi::iterator it = segments.begin(); it != segments.end(); ++it) {
        if (it->second.second == 0) {
            Rect newRect = endRectangle(base, x, y, it, base.p[x]+base.w, segments);
            if (newRect.w > 0 && newRect.h > 0)
                generated.push_back(newRect);
        }
    }
    for (int i = 0; i < generated.size(); ++i) {
        generated[i].normal = base.normal;
        generated[i].material = base.material;
    }
}

void rectanglesToTriangles(
        vector<Rect>& rectangles,
        vector<double>& triangles)
{
    for (int i = 0; i < rectangles.size(); ++i) {
        Rect& r = rectangles[i];
        double coords[4][3];
        for (int z = 0; z < 4; ++z) {
            for (int j = 0; j < 3; ++j) {
                coords[z][j] = rectangles[i].p[j];
            }
        }
        pair<int,int> ax = rectangles[i].axisIndices();
        int x = ax.first;
        int y = ax.second;
        coords[1][x] += rectangles[i].w;
        coords[2][x] += rectangles[i].w;
        coords[2][y] += rectangles[i].h;
        coords[3][y] += rectangles[i].h;
        const int indices[] = {
            0,1,2,
            0,2,1,
            2,3,0,
            2,0,3,
        };
        for (int j = 0; j < 4*3; ++j) {
            for (int k = 0; k < 3; ++k) {
                triangles.push_back(coords[indices[j]][k]);
            }
        }
    }
}
/*
#include <iostream>
int main() {
    Rect r(0,0,0);
    r.w = 1; r.h = 1; r.axis = 0;
    cout << findAxes(r).first << "," << findAxes(r).second << "***" << endl;
    vector<Rect> rectangles;
    // Test triangles creation
    vector<double> vertices;
    rectangles.push_back(r);
    rectanglesToTriangles(rectangles, vertices);
    for (int i = 0; i < vertices.size(); ++i) {
        cout << vertices[i] << " ";
        if (i%3==2) cout << endl;
    }
    cout << "---------------------" << endl;
    // Test wall ending
    Rect base(0,0,0);
    base.w = 10; base.h = 10; base.axis = 0;
    rectangles.clear();
    vector<Rect> gen;
    Rect r1(0,2,1);
    r1.w = 3; r1.h = 2; r1.axis = 0;
    Rect r2(0,2,5);
    r2.w = 3; r2.h = 2; r2.axis = 0;
    rectangles.push_back(r1);
    rectangles.push_back(r2);
    rectangleDiff(base, rectangles, gen);
    for (int i = 0; i < gen.size(); ++i) {
        cout << "Rectangle from " << gen[i].p[1] << "," << gen[i].p[2] << " to ";
        cout << gen[i].p[1] + gen[i].w << "," << gen[i].p[2]+gen[i].h << endl;
    }
    return 0;
}*/
