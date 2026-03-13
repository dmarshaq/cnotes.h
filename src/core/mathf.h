#ifndef MATHF_H
#define MATHF_H

/**
 * Math float.
 */

#include "core/type.h"
#include <math.h>
#include <stdbool.h>
#include <float.h>

#define PI 3.14159265358979323846f
#define TAU (PI * 2)

// Change deg2rad and rad2deg to just MACROS
#define deg2rad(deg)                        ((deg) * PI / 180.0f)
#define rad2deg(rad)                        ((rad) * 180.0f / PI)
#define randf()                             ((float)rand() / RAND_MAX)

static inline float right_triangle_hypotenuse(float a, float b) {
    return sqrtf(a * a + b * b);
}

static inline float sig(float a) {
    return (a == 0.0f) ? 0.0f : fabsf(a) / a;
}

static inline int fequal(float a, float b) {
    return fabsf(a - b) < FLT_EPSILON;
}

static inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

static inline float clamp(float a, float min, float max) {
    return fmaxf(min, fminf(a, max));
}

static inline float ease_in_back(float x) {
    return (2.70158f * x * x * x) - (1.70158f * x * x);
}


// Vec2f

typedef struct vec2f {
    float x;
    float y;
} Vec2f;

static inline Vec2f vec2f_make(float x, float y) {
    return (Vec2f){ x, y };
}

static inline Vec2f vec2f_make_angle(float mag, float angle) {
    return (Vec2f){ mag * cosf(angle), mag * sinf(angle) };
}

#define VEC2F_ORIGIN ((Vec2f){ 0.0f,  0.0f })
#define VEC2F_RIGHT  ((Vec2f){ 1.0f,  0.0f })
#define VEC2F_LEFT   ((Vec2f){-1.0f,  0.0f })
#define VEC2F_UP     ((Vec2f){ 0.0f,  1.0f })
#define VEC2F_DOWN   ((Vec2f){ 0.0f, -1.0f })
#define VEC2F_UNIT   ((Vec2f){ 1.0f,  1.0f })

#define vec2f_print(v1)                     printf(#v1 " = ( % 2.2f , % 2.2f )\n", v1.x, v1.y)

static inline Vec2f vec2f_sum(Vec2f v1, Vec2f v2) {
    return vec2f_make(v1.x + v2.x, v1.y + v2.y);
}

static inline Vec2f vec2f_difference(Vec2f v1, Vec2f v2) {
    return vec2f_make(v1.x - v2.x, v1.y - v2.y);
}

static inline Vec2f vec2f_sum_constant(Vec2f v, float c) {
    return vec2f_make(v.x + c, v.y + c);
}

static inline Vec2f vec2f_difference_constant(Vec2f v, float c) {
    return vec2f_make(v.x - c, v.y - c);
}

static inline float vec2f_dot(Vec2f v1, Vec2f v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

static inline float vec2f_cross(Vec2f v1, Vec2f v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

static inline Vec2f vec2f_multi_constant(Vec2f v, float c) {
    return vec2f_make(v.x * c, v.y * c);
}

static inline Vec2f vec2f_divide_constant(Vec2f v, float c) {
    return vec2f_make(v.x / c, v.y / c);
}

static inline float vec2f_magnitude(Vec2f v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static inline float vec2f_distance(Vec2f v1, Vec2f v2) {
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    return sqrtf(dx * dx + dy * dy);
}

static inline Vec2f vec2f_negate(Vec2f v) {
    return vec2f_make(-v.x, -v.y);
}

static inline Vec2f vec2f_normalize(Vec2f v) {
    float mag = vec2f_magnitude(v);
    return (mag == 0.0f) ? VEC2F_ORIGIN : vec2f_divide_constant(v, mag);
}

static inline Vec2f vec2f_lerp(Vec2f v1, Vec2f v2, float t) {
    return vec2f_make(lerp(v1.x, v2.x, t), lerp(v1.y, v2.y, t));
}

static inline Vec2f vec2f_rotate(Vec2f v, float angle) {
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    return vec2f_make(
        cos_a * v.x - sin_a * v.y,
        sin_a * v.x + cos_a * v.y
    );
}

float point_segment_min_distance(Vec2f p, Vec2f a, Vec2f b);

// Vec3f

typedef struct vec3f {
    float x;
    float y;
    float z;
} Vec3f;


static inline Vec3f vec3f_make(float x, float y, float z) {
    return (Vec3f){ x, y, z };
}

#define vec3f_print(v1)                     printf(#v1 " = ( % 2.2f , % 2.2f , %2.2f )\n", v1.x, v1.y, v1.z)

// Vec4f

typedef struct vec4f {
    float x;
    float y;
    float z;
    float w;
} Vec4f;

static inline Vec4f vec4f_make(float x, float y, float z, float w) {
    return (Vec4f){ x, y, z, w };
}

#define VEC4F_WHITE                         ((Vec4f) { 1.0f,  1.0f,  1.0f,  1.0f } )
#define VEC4F_BLACK                         ((Vec4f) { 0.0f,  0.0f,  0.0f,  1.0f } )
#define VEC4F_GREY                          ((Vec4f) { 0.4f,  0.4f,  0.4f,  1.0f } )
#define VEC4F_RED                           ((Vec4f) { 1.0f,  0.0f,  0.0f,  1.0f } )
#define VEC4F_GREEN                         ((Vec4f) { 0.0f,  1.0f,  0.0f,  1.0f } )
#define VEC4F_BLUE                          ((Vec4f) { 0.0f,  0.0f,  1.0f,  1.0f } )
#define VEC4F_YELLOW                        ((Vec4f) { 1.0f,  1.0f,  0.0f,  1.0f } )
#define VEC4F_PINK                          ((Vec4f) { 1.0f,  0.0f,  1.0f,  1.0f } )
#define VEC4F_CYAN                          ((Vec4f) { 0.0f,  1.0f,  1.0f,  1.0f } )

#define vec4f_print(v1)                     printf(#v1 " = ( % 2.2f , % 2.2f , % 2.2f , % 2.2f )\n", v1.x, v1.y, v1.z, v1.w)


static inline Vec4f vec4f_lerp(Vec4f v1, Vec4f v2, float t) {
    return vec4f_make(lerp(v1.x, v2.x, t), lerp(v1.y, v2.y, t), lerp(v1.z, v2.z, t), lerp(v1.w, v2.w, t));
}

// Matrix4f

typedef struct matrix4f {
    float array[16];
} Matrix4f;

#define MATRIX4F_ZERO                                                   ((Matrix4f) {{ 0.0f, 0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f, 0.0f }} )
#define MATRIX4F_IDENTITY                                               ((Matrix4f) {{ 1.0f, 0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f, 1.0f }} )

#define matrix4f_print(matrix)                                          printf(#matrix " =\n[[ % 2.2f , % 2.2f , % 2.2f , % 2.2f ]\n [ % 2.2f , % 2.2f , % 2.2f , % 2.2f ]\n [ % 2.2f , % 2.2f , % 2.2f , % 2.2f ]\n [ % 2.2f , % 2.2f , % 2.2f , % 2.2f ]]\n", matrix.array[0], matrix.array[1], matrix.array[2], matrix.array[3], matrix.array[4], matrix.array[5], matrix.array[6], matrix.array[7], matrix.array[8], matrix.array[9], matrix.array[10], matrix.array[11], matrix.array[12], matrix.array[13], matrix.array[14], matrix.array[15])

static inline Matrix4f matrix4f_orthographic(float left, float right, float bottom, float top, float near, float far) {
    return ((Matrix4f) {{ 2.0f / ((right) - (left)), 0.0f, 0.0f, ((left) + (right)) / ((left) - (right)),    0.0f, 2.0f / ((top) - (bottom)), 0.0f, ((bottom) + (top)) / ((bottom) - (top)),    0.0f, 0.0f, 2.0f / ((near) - (far)), ((near) + (far)) / ((near) - (far)),    0.0f, 0.0f, 0.0f, 1.0f }} );
}

static inline Vec2f matrix4f_mul_vec2f(Matrix4f m, Vec2f v) {
    return (Vec2f){
        .x = m.array[0] * v.x + m.array[1] * v.y + m.array[3],
        .y = m.array[4] * v.x + m.array[5] * v.y + m.array[7]
    };
}

static inline Vec3f matrix4f_mul_vec3f(Matrix4f m, Vec3f v) {
    return (Vec3f){
        .x = m.array[0] * v.x + m.array[1] * v.y + m.array[2] * v.z,
        .y = m.array[4] * v.x + m.array[5] * v.y + m.array[6] * v.z,
        .z = m.array[8] * v.x + m.array[9] * v.y + m.array[10] * v.z
    };
}

static inline Vec4f matrix4f_mul_vec4f(Matrix4f m, Vec4f v) {
    return (Vec4f){
        .x = m.array[0] * v.x + m.array[1] * v.y + m.array[2] * v.z + m.array[3] * v.w,
        .y = m.array[4] * v.x + m.array[5] * v.y + m.array[6] * v.z + m.array[7] * v.w,
        .z = m.array[8] * v.x + m.array[9] * v.y + m.array[10] * v.z + m.array[11] * v.w,
        .w = m.array[12] * v.x + m.array[13] * v.y + m.array[14] * v.z + m.array[15] * v.w
    };
}

Matrix4f matrix4f_multiplication(Matrix4f *multiplier, Matrix4f *target);

// Transform

typedef Matrix4f Transform;

static inline Transform transform_make_translation_2d(Vec2f position) {
    return ((Transform) {{ 1.0f, 0.0f, 0.0f, position.x,   0.0f, 1.0f, 0.0f, position.y,    0.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f, 1.0f }} );
}

static inline Transform transform_make_rotation_2d(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    return ((Transform) {{ c, -s, 0.0f, 0.0f,    s,  c, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f, 1.0f }} );
}

static inline Transform transform_make_scale_2d(Vec2f scale) {
    return ((Transform) {{ scale.x, 0.0f, 0.0f, 0.0f,   0.0f, scale.y, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f, 1.0f }} );
}

Transform transform_make_trs_2d(Vec2f position, float angle, Vec2f scale);

void transform_set_rotation_2d(Transform *transform, float angle);
void transform_set_translation_2d(Transform *transform, Vec2f position);
void transform_flip_y(Transform *transform);
void transform_flip_x(Transform *transform);
void transform_set_flip_x(Transform *transform, float sign);
void transform_set_flip_y(Transform *transform, float sign);

// Function

typedef float (*Function)(float);

#define func_expr(expr) ({ float __fn(float x) { return (expr); } (Function)__fn; })

// Shapes
/**
 * @Todo: Replace macros with inline functions.
 */

/**
 * This macro uses [ domain_start, domain_end ] to specify domain boundaries, and checks if value inside that domain.
 */
#define value_inside_domain(domain_start, domain_end, value) ((domain_start) <= (value) && (value) <= (domain_end))


/**
 * Normals and verticies for all boxes, without rotation.
 *
 *                         up
 *                         ^
 *                         |
 *                         |
 *                         |
 *                p3----------------p1
 *                |                 |
 *                |                 | 
 *  left <--------|        o        |--------> right
 *                |                 |
 *                |                 |
 *                p0----------------p2
 *                         |
 *                         |
 *                         |
 *                         !
 *                         down
 *
 */

// Rework AABB to work with static inline functions, not macros.
typedef struct axis_aligned_bounding_box {
    Vec2f p0;
    Vec2f p1;
} AABB;

#define aabb_make(p0, p1)                                               ((AABB) { p0, p1 } )   
#define aabb_make_dimensions(center, width, height)                     ((AABB) { .p0 = vec2f_make((center).x - (width) / 2, (center).y - (height) / 2), .p1 = vec2f_make((center).x + (width) / 2, (center).y + (height) / 2) } )   
#define aabb_center(aabb)                                               vec2f_make(aabb.p0.x + (aabb.p1.x - aabb.p0.x) / 2, aabb.p0.y + (aabb.p1.y - aabb.p0.y) / 2)

void aabb_move(AABB *box, Vec2f move);

static inline bool aabb_touches_point(AABB *box, Vec2f point) {
    return value_inside_domain(box->p0.x, box->p1.x, point.x) && value_inside_domain(box->p0.y, box->p1.y, point.y);
}


// Rework obb with static inline functions, NOT macros.
typedef struct oriented_bounding_box {
    Vec2f center;
    Vec2f dimensions;
    float rot;
} OBB;

#define obb_make(center, width, height, rotation)                       ((OBB) { center, vec2f_make(width, height), rotation })

#define obb_p0(obb_ptr)                                                 vec2f_sum((obb_ptr)->center, vec2f_rotate(vec2f_make((obb_ptr)->dimensions.x / 2, (obb_ptr)->dimensions.y / 2), (obb_ptr)->rot))
#define obb_p1(obb_ptr)                                                 vec2f_sum((obb_ptr)->center, vec2f_rotate(vec2f_make(-(obb_ptr)->dimensions.x / 2, -(obb_ptr)->dimensions.y / 2), (obb_ptr)->rot))
#define obb_p2(obb_ptr)                                                 vec2f_sum((obb_ptr)->center, vec2f_rotate(vec2f_make((obb_ptr)->dimensions.x / 2, -(obb_ptr)->dimensions.y / 2), (obb_ptr)->rot))
#define obb_p3(obb_ptr)                                                 vec2f_sum((obb_ptr)->center, vec2f_rotate(vec2f_make(-(obb_ptr)->dimensions.x / 2, (obb_ptr)->dimensions.y / 2), (obb_ptr)->rot))

#define obb_right(obb_ptr)                                              vec2f_rotate(VEC2F_RIGHT, (obb_ptr)->rot)
#define obb_up(obb_ptr)                                                 vec2f_rotate(VEC2F_UP, (obb_ptr)->rot) 
#define obb_left (obb_ptr)                                              vec2f_rotate(VEC2F_LEFT, (obb_ptr)->rot)
#define obb_down (obb_ptr)                                              vec2f_rotate(VEC2F_DOWN, (obb_ptr)->rot)

/**
 * Returns AABB that covers or/and encloses specified OBB box.
 */
AABB obb_enclose_in_aabb(OBB *box);





typedef struct quad {
    Vec2f verts[4];
} Quad;

static inline Quad quad_make(Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p3) {
    return (Quad){ p0, p2, p3, p1 };
}

static inline Vec2f quad_center(Quad *quad) {
    return vec2f_make((quad->verts[0].x + quad->verts[1].x + quad->verts[2].x + quad->verts[3].x) / 4, (quad->verts[0].y + quad->verts[1].y + quad->verts[2].y + quad->verts[3].y) / 4);
}

AABB quad_enclose_in_aabb(Quad *quad);









typedef struct circle {
    Vec2f center;
    float radius;
} Circle;




// s64


static inline s64 maxi(s64 x, s64 y) {
    return (x > y) ? x : y;
}

static inline s64 mini(s64 x, s64 y) {
    return (x < y) ? x : y;
}
static inline s64 clampi(s64 a, s64 min, s64 max) {
    return maxi(min, mini(a, max));
}


#endif
