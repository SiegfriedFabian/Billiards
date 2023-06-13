#ifndef MYVECTOR_CLASS_H
#define MYVECTOR_CLASS_H
#include <math.h>


template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}


//--------------------------
struct vec2 {
	//--------------------------
	float x, y;

	float& operator[](int j) { return *(&x + j); }
	float operator[](int j) const { return *(&x + j); }
	vec2(float x0 = 0, float y0 = 0) { x = x0; y = y0; }
	vec2 operator*(float a) const { return vec2(x * a, y * a); }
	vec2 operator/(float a) const { return vec2(x / a, y / a); }
	vec2 operator+(const vec2& v) const { return vec2(x + v.x, y + v.y); }
	vec2 operator-(const vec2& v) const { return vec2(x - v.x, y - v.y); }
	vec2 operator*(const vec2& v) const { return vec2(x * v.x, y * v.y); }
	vec2 operator-() const { return vec2(-x, -y); }
};

inline vec2 round(const vec2& v) {
	return vec2(floor(v.x + 0.5), floor(v.y + 0.5));
}

inline float dot(const vec2& v1, const vec2& v2) {
	return (v1.x * v2.x + v1.y * v2.y);
}

inline float det(const vec2& v1, const vec2& v2) {
	return (v1.x * v2.y - v1.y * v2.x);
}

inline float length(const vec2& v) { return sqrtf(dot(v, v)); }

inline vec2 normalize(const vec2& v) { return v * (1 / length(v)); }

inline vec2 operator*(float a, const vec2& v) { return vec2(v.x * a, v.y * a); }

inline vec2 project(const vec2& pos, const vec2& p, const vec2& dir) {
	vec2 d = normalize(dir);
	return dot(pos - p, d) * d + p;
}


//---------------------------
struct mat2 { // row-major matrix 2x2
	//---------------------------
	vec2 rows[2];
public:
	mat2() {}
	mat2(float m00, float m01,
		float m10, float m11) {
		rows[0][0] = m00; rows[0][1] = m01;
		rows[1][0] = m10; rows[1][1] = m11;
	}
	mat2(vec2 row0, vec2 row1) {
		rows[0] = row0; rows[1] = row1;
	}
	vec2& operator[](int i) { return rows[i]; }
	vec2 operator[](int i) const { return rows[i]; }
	operator float* () const { return (float*)this; }
};

inline vec2 operator*(const vec2& v, const mat2& mat) {
	return v[0] * mat[0] + v[1] * mat[1];
}


inline vec2 operator*(const mat2& mat, const vec2& v) {
	return vec2(dot(mat[0], v), dot(mat[1], v));
}

inline mat2 operator*(const mat2& left, const mat2& right) {
	mat2 result;
	for (int i = 0; i < 2; i++) result.rows[i] = left.rows[i] * right;
	return result;
}

// vec2 with doubles
struct vec2_d {
	//--------------------------
	double x, y;

	double& operator[](int j) { return *(&x + j); }
	double operator[](int j) const { return *(&x + j); }
	vec2_d(double x0 = 0, double y0 = 0) { x = x0; y = y0; }
	vec2_d(vec2 v) { x = v.x; y = v.y; }
	vec2_d operator*(double a) const { return vec2_d(x * a, y * a); }
	vec2_d operator/(double a) const { return vec2_d(x / a, y / a); }
	vec2_d operator+(const vec2_d& v) const { return vec2_d(x + v.x, y + v.y); }
	vec2_d operator-(const vec2_d& v) const { return vec2_d(x - v.x, y - v.y); }
	vec2_d operator*(const vec2_d& v) const { return vec2_d(x * v.x, y * v.y); }
	vec2_d operator-() const { return vec2_d(-x, -y); }
	vec2 toFloat() const { return vec2(x, y); }
};

inline vec2_d round(const vec2_d& v) {
	return vec2_d(floor(v.x + 0.5), floor(v.y + 0.5));
}

inline double dot(const vec2_d& v1, const vec2_d& v2) {
	return (v1.x * v2.x + v1.y * v2.y);
}

inline double det(const vec2_d& v1, const vec2_d& v2) {
	return (v1.x * v2.y - v1.y * v2.x);
}

inline double length(const vec2_d& v) { return sqrt(dot(v, v)); }

inline vec2_d normalize(const vec2_d& v) { return v * (1 / length(v)); }

inline vec2_d operator*(double a, const vec2_d& v) { return vec2_d(v.x * a, v.y * a); }

inline vec2_d project(const vec2_d& pos, const vec2_d& p, const vec2_d& dir) {
	vec2_d d = normalize(dir);
	return dot(pos - p, d) * d + p;
}


//---------------------------
struct mat2_d { // row-major matrix 2x2 for doubles
	//---------------------------
	vec2_d rows[2];

public:
	mat2_d() {}
	mat2_d(double m00, double m01,
		double m10, double m11) {
		rows[0][0] = m00; rows[0][1] = m01;
		rows[1][0] = m10; rows[1][1] = m11;
	}
	mat2_d(vec2_d row0, vec2_d row1) {
		rows[0] = row0; rows[1] = row1;
	}
	vec2_d& operator[](int i) { return rows[i]; }
	vec2_d operator[](int i) const { return rows[i]; }
	operator float* () const { return (float*)this; }
};

inline vec2_d operator*(const vec2_d& v, const mat2_d& mat) {
	return v[0] * mat[0] + v[1] * mat[1];
}

inline vec2_d operator*(const mat2_d& mat, const vec2_d& v) {
	return vec2_d(dot(mat[0], v), dot(mat[1], v));
}

inline mat2_d operator*(const mat2_d& left, const mat2_d& right) {
	mat2_d result;
	for (int i = 0; i < 2; i++) result.rows[i] = left.rows[i] * right;
	return result;
}
//--------------------------
struct vec3 {
	//--------------------------
	float x, y, z;

	vec3(float x0 = 0, float y0 = 0, float z0 = 0) { x = x0; y = y0; z = z0; }
	vec3(vec2 v) { x = v.x; y = v.y; z = 0; }

	vec3 operator*(float a) const { return vec3(x * a, y * a, z * a); }
	vec3 operator/(float a) const { return vec3(x / a, y / a, z / a); }
	vec3 operator+(const vec3& v) const { return vec3(x + v.x, y + v.y, z + v.z); }
	vec3 operator-(const vec3& v) const { return vec3(x - v.x, y - v.y, z - v.z); }
	vec3 operator*(const vec3& v) const { return vec3(x * v.x, y * v.y, z * v.z); }
	vec3 operator-()  const { return vec3(-x, -y, -z); }
};

inline float dot(const vec3& v1, const vec3& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }

inline float length(const vec3& v) { return sqrtf(dot(v, v)); }

inline vec3 normalize(const vec3& v) { return v * (1 / length(v)); }

inline vec3 cross(const vec3& v1, const vec3& v2) {
	return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

inline vec3 operator*(float a, const vec3& v) { return vec3(v.x * a, v.y * a, v.z * a); }

//--------------------------
struct vec4 {
	//--------------------------
	float x, y, z, w;

	vec4(float x0 = 0, float y0 = 0, float z0 = 0, float w0 = 0) { x = x0; y = y0; z = z0; w = w0; }
	float& operator[](int j) { return *(&x + j); }
	float operator[](int j) const { return *(&x + j); }

	vec4 operator*(float a) const { return vec4(x * a, y * a, z * a, w * a); }
	vec4 operator/(float d) const { return vec4(x / d, y / d, z / d, w / d); }
	vec4 operator+(const vec4& v) const { return vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
	vec4 operator-(const vec4& v)  const { return vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
	vec4 operator*(const vec4& v) const { return vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
	void operator+=(const vec4 right) { x += right.x; y += right.y; z += right.z; w += right.w; }
};

inline float dot(const vec4& v1, const vec4& v2) {
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
}

inline vec4 operator*(float a, const vec4& v) {
	return vec4(v.x * a, v.y * a, v.z * a, v.w * a);
}

//---------------------------
struct mat4 { // row-major matrix 4x4
	//---------------------------
	vec4 rows[4];
public:
	mat4() {}
	mat4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) {
		rows[0][0] = m00; rows[0][1] = m01; rows[0][2] = m02; rows[0][3] = m03;
		rows[1][0] = m10; rows[1][1] = m11; rows[1][2] = m12; rows[1][3] = m13;
		rows[2][0] = m20; rows[2][1] = m21; rows[2][2] = m22; rows[2][3] = m23;
		rows[3][0] = m30; rows[3][1] = m31; rows[3][2] = m32; rows[3][3] = m33;
	}
	mat4(vec4 it, vec4 jt, vec4 kt, vec4 ot) {
		rows[0] = it; rows[1] = jt; rows[2] = kt; rows[3] = ot;
	}

	vec4& operator[](int i) { return rows[i]; }
	vec4 operator[](int i) const { return rows[i]; }
	operator float* () const { return (float*)this; }
};

inline vec4 operator*(const vec4& v, const mat4& mat) {
	return v[0] * mat[0] + v[1] * mat[1] + v[2] * mat[2] + v[3] * mat[3];
}

inline mat4 operator*(const mat4& left, const mat4& right) {
	mat4 result;
	for (int i = 0; i < 4; i++) result.rows[i] = left.rows[i] * right;
	return result;
}

inline mat4 TranslateMatrix(vec3 t) {
	return mat4(vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(t.x, t.y, t.z, 1));
}

inline mat4 ScaleMatrix(vec3 s) {
	return mat4(vec4(s.x, 0, 0, 0),
		vec4(0, s.y, 0, 0),
		vec4(0, 0, s.z, 0),
		vec4(0, 0, 0, 1));
}

inline mat4 RotationMatrix(float angle, vec3 w) {
	float c = cosf(angle), s = sinf(angle);
	w = normalize(w);
	return mat4(vec4(c * (1 - w.x * w.x) + w.x * w.x, w.x * w.y * (1 - c) + w.z * s, w.x * w.z * (1 - c) - w.y * s, 0),
		vec4(w.x * w.y * (1 - c) - w.z * s, c * (1 - w.y * w.y) + w.y * w.y, w.y * w.z * (1 - c) + w.x * s, 0),
		vec4(w.x * w.z * (1 - c) + w.y * s, w.y * w.z * (1 - c) - w.x * s, c * (1 - w.z * w.z) + w.z * w.z, 0),
		vec4(0, 0, 0, 1));
}

#endif