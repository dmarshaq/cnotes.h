#include "core/mathf.h"
#include "core/type.h"

/**
 * Math float.
 */

float point_segment_min_distance(Vec2f p, Vec2f a, Vec2f b) {
    Vec2f ba = vec2f_difference(b, a);
    Vec2f pa = vec2f_difference(p, a);
    float dot = vec2f_dot(pa, vec2f_normalize(ba));

    if (dot <= 0) {
        return vec2f_magnitude(pa);
    }
    else if (dot >= vec2f_magnitude(ba)) {
        return vec2f_magnitude(vec2f_difference(p, b));
    }
    return sqrtf(vec2f_magnitude(pa) * vec2f_magnitude(pa) - dot * dot);
}

Matrix4f matrix4f_multiplication(Matrix4f *multiplier, Matrix4f *target) {
    Matrix4f result = {
        .array = {0}
    };
    float row_product;
    for (u8 col2 = 0; col2 < 4; col2++) {
        for (u8 row = 0; row < 4; row++) {
            row_product = 0;
            for (u8 col = 0; col < 4; col++) {
                row_product += multiplier->array[row * 4 + col] * target->array[col * 4 + col2];
            }
            result.array[row * 4 + col2] = row_product;
        }
    }
    return result;
}

Transform transform_make_trs_2d(Vec2f position, float angle, Vec2f scale) {
    return (Transform) {
        .array = { scale.x * cosf(angle),                           sig(scale.x) * fabsf(scale.y) * -sinf(angle),   0.0f, position.x, 
                   sig(scale.y) * fabsf(scale.x) * sinf(angle),     scale.y * cosf(angle),                          0.0f, position.y,
                   0.0f,                                            0.0f,                                           1.0f, 0.0f,
                   0.0f,                                            0.0f,                                           0.0f, 1.0f}
    };
}

// Transform transform_make_srt_2d(Vec2f position, float angle, Vec2f scale) {
//     Transform s = transform_make_scale_2d(scale);
//     Transform r = transform_make_rotation_2d(angle);
//     Transform t = transform_make_translation_2d(position);
// 
//     r = matrix4f_multiplication(&s, &r);
//     return matrix4f_multiplication(&r, &t);
// }

void transform_set_rotation_2d(Transform *transform, float angle) {
    float scale_x = sig(transform->array[0]) * sqrtf(transform->array[0] * transform->array[0] + transform->array[4] * transform->array[4]);
    float scale_y = sig(transform->array[5]) * sqrtf(transform->array[1] * transform->array[1] + transform->array[5] * transform->array[5]);
    
    transform->array[0] = scale_x * cosf(angle);
    transform->array[1] = fabsf(scale_y) * sig(scale_x) * -sinf(angle);
    transform->array[4] = fabsf(scale_x) * sig(scale_y) * sinf(angle);
    transform->array[5] = scale_y * cosf(angle);
}

void transform_set_translation_2d(Transform *transform, Vec2f position) {
    transform->array[3] = position.x;
    transform->array[7] = position.y;
}

void transform_flip_y(Transform *transform) {
    transform->array[4] *= -1.0f;
    transform->array[5] *= -1.0f;
}

void transform_flip_x(Transform *transform) {
    transform->array[0] *= -1.0f;
    transform->array[1] *= -1.0f;
}

void transform_set_flip_x(Transform *transform, float sign) {
    transform->array[0] = sig(sign) * fabsf(transform->array[0]);
    transform->array[1] = sig(sign) * -fabsf(transform->array[1]);
}


void aabb_move(AABB *box, Vec2f move) {
    box->p0 = vec2f_sum(box->p0, move);
    box->p1 = vec2f_sum(box->p1, move);
}


AABB obb_enclose_in_aabb(OBB *box) {
    Vec2f verts[4] = {
        obb_p0(box),
        obb_p1(box),
        obb_p2(box),
        obb_p3(box)
    };

    AABB result = aabb_make(verts[0], verts[1]);

    for (u32 i = 0; i < 4; i++) {
        if (result.p0.x > verts[i].x)
            result.p0.x = verts[i].x;
        
        if (result.p0.y > verts[i].y)
            result.p0.y = verts[i].y;

        if (result.p1.x < verts[i].x)
            result.p1.x = verts[i].x;
        
        if (result.p1.y < verts[i].y)
            result.p1.y = verts[i].y;
    }
    
    return result;
}


AABB quad_enclose_in_aabb(Quad *quad) {
    AABB result = aabb_make(quad->verts[0], quad->verts[3]);

    for (u32 i = 0; i < 4; i++) {
        if (result.p0.x > quad->verts[i].x)
            result.p0.x = quad->verts[i].x;
        
        if (result.p0.y > quad->verts[i].y)
            result.p0.y = quad->verts[i].y;

        if (result.p1.x < quad->verts[i].x)
            result.p1.x = quad->verts[i].x;
        
        if (result.p1.y < quad->verts[i].y)
            result.p1.y = quad->verts[i].y;
    }
    
    return result;
}







