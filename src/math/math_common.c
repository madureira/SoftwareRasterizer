#include "math/math_common.h"

#define MATH_SIN_COS_TABLE_SIZE 361

static f32 sin_table[MATH_SIN_COS_TABLE_SIZE];
static f32 cos_table[MATH_SIN_COS_TABLE_SIZE];

static bool sin_cos_tables_built = false;

static void math_sin_cos_table_index(f32 angle_degree, i32* theta_int, f32* theta_frac)
{
    angle_degree = fmodf(angle_degree, 360.0f);

    // make angle positive
    if (angle_degree < 0.0f)
    {
        angle_degree += 360.0f;
    }

    // compute floor of theta and fractional part to interpolate
    *theta_int = (i32)angle_degree;
    *theta_frac = angle_degree - (f32)*theta_int;
}

void math_build_sin_cos_tables(void)
{
    for (int degree = 0; degree <= 360; ++degree)
    {
        f32 radians = (f32)degree * (MATH_PI / 180.0f);

        sin_table[degree] = sinf(radians);
        cos_table[degree] = cosf(radians);
    }

    sin_cos_tables_built = true;
}

f32 math_fast_sin(f32 angle_degree)
{
    assert(isfinite(angle_degree));
    assert(sin_cos_tables_built
           && "math_build_sin_cos_tables() must be called before math_fast_sin()");

    i32 theta_int;
    f32 theta_frac;
    math_sin_cos_table_index(angle_degree, &theta_int, &theta_frac);

    /*
     * now compute the value of sin(angle) using the lookup table
     * and interpolating the fractional part.
     */
    return sin_table[theta_int] + theta_frac * (sin_table[theta_int + 1] - sin_table[theta_int]);
}

f32 math_fast_cos(f32 angle_degree)
{
    assert(isfinite(angle_degree));
    assert(sin_cos_tables_built
           && "math_build_sin_cos_tables() must be called before math_fast_cos()");

    i32 theta_int;
    f32 theta_frac;
    math_sin_cos_table_index(angle_degree, &theta_int, &theta_frac);

    /*
     * now compute the value of cos(angle) using the lookup table
     * and interpolating the fractional part.
     */
    return cos_table[theta_int] + theta_frac * (cos_table[theta_int + 1] - cos_table[theta_int]);
}

void math_fast_sincos(f32 angle_degree, f32* out_sin, f32* out_cos)
{
    assert(isfinite(angle_degree));
    assert(out_sin != NULL);
    assert(out_cos != NULL);
    assert(sin_cos_tables_built
           && "math_build_sin_cos_tables() must be called before math_fast_sincos()");

    i32 theta_int;
    f32 theta_frac;
    math_sin_cos_table_index(angle_degree, &theta_int, &theta_frac);

    *out_sin =
        sin_table[theta_int] + theta_frac * (sin_table[theta_int + 1] - sin_table[theta_int]);
    *out_cos =
        cos_table[theta_int] + theta_frac * (cos_table[theta_int + 1] - cos_table[theta_int]);
}
