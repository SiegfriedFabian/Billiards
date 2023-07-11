#version 410 core
// Outputs colors in RGBA
out vec4 FragColor;
uniform float width;
uniform float height;

// void main()
// {   
//     vec2 pos = gl_FragCoord.xy;
// 	FragColor = vec4(0.9, 0.0, 0.0 , 1.0);
// }

const float PI = 3.141592653589793238;
const float LINE_THICKNESS = 0.001;
const int ITERATIONS = 100;
// Visual scale factor
const float SCALE = 25.0;

// Polygonal table, if you change N, you also have to change the number of
// vertices in the array.
const int N = 3;
const vec2[N] VERTICES = vec2[](
    vec2(cos(PI *  3./6.), sin(PI *  3./6.)),
    vec2(cos(PI *  7./6.), sin(PI *  7./6.)),
    vec2(cos(PI * 11./6.), sin(PI * 11./6.))
);

// Vector operations
vec2 project(in vec2 v, in vec2 onto) {
    return onto * dot(v, onto) / (onto.x * onto.x + onto.y * onto.y);
}

vec2 perp(in vec2 v, in vec2 onto) {
    return v - project(v, onto);
}

// Geometry
struct Circle {
    vec2 center;
    float radius;
};

struct Ray {
    vec2 source;
    vec2 direction;
};

struct Line {
    float a;
    float b;
    float c;
};

Line lineFromRay(in Ray ray) {
    vec2 m = vec2(ray.direction.y, -ray.direction.x);
    return Line(m.x, m.y, -(m.x * ray.source.x + m.y * ray.source.y));
}

Line lineFromTwoPoints(in vec2 p1, in vec2 p2) {
    return lineFromRay(Ray(p1, p2 - p1));
}

vec2 intersectLines(in Line l1, in Line l2) {
    float d = l1.a * l2.b - l1.b * l2.a;
    return vec2(
        -l2.b * l1.c + l1.b * l2.c,
        l2.a * l1.c - l1.a * l2.c) / d;
}

vec2 intersectRays(in Ray r1, in Ray r2) {
    return intersectLines(lineFromRay(r1), lineFromRay(r2));
}

bool pointOnRay(in vec2 pt, in Ray ray) {
    vec2 dp = pt - ray.source;
    bool closeToLine = length(perp(dp, ray.direction)) < LINE_THICKNESS;
    bool rightSide = dot(dp, ray.direction) > 0.;
    return closeToLine && rightSide;
}

bool pointOnLine(in vec2 pt, in Line line) {
    return abs(line.a * pt.x + line.b * pt.y + line.c)
    / length(vec2(line.a,line.b)) < LINE_THICKNESS;
}

bool pointOnCircle(in vec2 pt, in Circle circle) {
    return abs(length(pt - circle.center) - circle.radius) < LINE_THICKNESS;
}

bool pointInCircle(in vec2 pt, in Circle circle) {
    return length(pt - circle.center) - circle.radius < 0.;
}

Ray ccwTangent(in Circle circle, in vec2 pt) {
    float hyp = length(circle.center - pt);
    float theta = atan(pt.y - circle.center.y, pt.x - circle.center.x);
    float dt = acos(circle.radius / hyp);
    vec2 tp = circle.center + circle.radius * vec2(cos(theta - dt), sin(theta - dt));
    return Ray(tp, pt - tp);
}

Ray cwTangent(in Circle circle, in vec2 pt) {
    float hyp = length(circle.center - pt);
    float theta = atan(pt.y - circle.center.y, pt.x - circle.center.x);
    float dt = acos(circle.radius / hyp);
    vec2 tp = circle.center + circle.radius * vec2(cos(theta + dt), sin(theta + dt));
    return Ray(tp, pt - tp);
}

// -1 is left, 0 is on, 1 is right
float lcr(in vec2 pt, in Ray ray) {
    vec2 dp = pt - ray.source;
    return -sign(ray.direction.x * dp.y - ray.direction.y * dp.x);
}

bool pointInsideTable(in vec2 pt) {
    bool inside = true;
    for (int i = 0; i < N; i++) {
        vec2 v1 = VERTICES[i];
        vec2 v2 = VERTICES[(i + 1) % N];
        if (lcr(pt, Ray(v1, v2 - v1)) > 0.) {
            inside = false;
        }
    }
    return inside;
}

bool inForwardRegion(in int n, in vec2 pt) {
    if (n < 0 || n >= N) {
        return false;
    }
    vec2 vn = VERTICES[n];
    vec2 vl = VERTICES[(N + n - 1) % N];
    vec2 vr = VERTICES[(n + 1) % N];
    return lcr(pt, Ray(vn, vl - vn)) > 0. && lcr(pt, Ray(vn, vr - vn)) > 0.;
}

bool inReverseRegion(in int n, in vec2 pt) {
    if (n < 0 || n >= N) {
        return false;
    }
    vec2 vn = VERTICES[n];
    vec2 vl = VERTICES[(N + n - 1) % N];
    vec2 vr = VERTICES[(n + 1) % N];
    return lcr(pt, Ray(vn, vr - vn)) < 0. && lcr(pt, Ray(vn, vl - vn)) < 0.;
}

int findForwardRegion(in vec2 pos) {
    for (int i = 0; i < N; i++) {
        if (inForwardRegion(i, pos)) {
            return i;
        }
    }
    return -1;
}

int findReverseRegion(in vec2 pos) {
    for (int i = 0; i < N; i++) {
        if (inReverseRegion(i, pos)) {
            return i;
        }
    }
    return -1;
}

vec2 fourthBilliardMap(in vec2 pt) {
    int rreg = findReverseRegion(pt);
    vec2 vf = VERTICES[findForwardRegion(pt)];
    vec2 vr = VERTICES[rreg];
    
    float l = length(vr - pt);
    vec2 tpf = pt + normalize(pt - vf) * l;
    
    vec2 dr = pt - vr;
    Ray fray = Ray(vr, vec2(-dr.y, dr.x));
    vec2 df = tpf - pt;
    Ray rray = Ray(tpf, vec2(-df.y, df.x));
    vec2 center = intersectRays(fray, rray);
    float radius = length(center - vr);
    Circle cir = Circle(center, radius);
    
    Ray ray;
    for (int i = 0; i < N; i++) {
        if (i + 1 == rreg) {
            continue;
        }
        vec2 v1 = VERTICES[i];
        vec2 v2 = VERTICES[(i + 1) % N];
        vec2 v3 = VERTICES[(i + 2) % N];
        ray = ccwTangent(cir, v2);
        if (lcr(v1, ray) < 0. && lcr(v3, ray) < 0.) {
            break;
        }
    }
    Line tl = lineFromRay(ray);
    return intersectLines(tl, lineFromTwoPoints(vr, pt));
}

bool isFutureSingularity(in vec2 pt, in Ray[N] rays) {
    vec2 current = pt;
    for (int i = 0; i < ITERATIONS; i++) {
        for (int j = 0; j < N; j++) {
            if (pointOnRay(current, rays[j])) {
                return true;
            }
        }
        current = fourthBilliardMap(current);
    }
    return false;
}

vec2 pixelToWorld(in vec2 px) {
    return SCALE * 2.0 * (px/vec2(width)
        - vec2(0.5, 0.5 * height / width));
}

void main()
{
    vec2 pos = pixelToWorld(gl_FragCoord.xy);
    // vec2 mousePos = pixelToWorld(iMouse.xy);
    // vec2 image = fourthBilliardMap(mousePos);
    
    if (pointInsideTable(pos)) {
        FragColor = vec4(1,0,1,1);
        return;
    }
    
    Ray[N] rays;
    for (int i = 0; i < N; i++) {
        vec2 v2 = VERTICES[(i+1) % N];
        vec2 v1 = VERTICES[i];
        rays[i] = Ray(v2, v1 - v2);
    }

    bool singularity = isFutureSingularity(pos, rays);
    if (singularity) {
        FragColor = vec4(0.8, 0.3, 0.1, 1);
    }
}