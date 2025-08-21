#pragma once

// + vp - PARAMETER SPACE VERTEX PREFIX
//* Geometry
constexpr char VERTEX_PREFIX = 'v';
constexpr char NORMAL_PREFIX = 'n';
constexpr char TEXTURE_PREFIX = 't';

//* Elements
constexpr char FACE_PREFIX = 'f';
constexpr char POINT_PREFIX = 'p';
constexpr char LINE_PREFIX = 'l';

//* Groups & Objects
constexpr char GROUP_PREFIX = 'g';
constexpr char OBJECT_PREFIX = 'o';
constexpr char SMOOTHING_PREFIX = 's';
constexpr char MERGING_GROUP_PREFIX[3] = "mg";
constexpr char COLOR_INTERPOLATION_PREFIX[9] = "c_interp";
constexpr char DISSOLVE_INTERPOLATION_PREFIX[9] = "d_interp";

//* Freeform Curves & Surfaces
constexpr char CURVE_PREFIX[5] = "curv";
constexpr char CURVE_PREFIX_2D[6] = "curv2";
constexpr char SURFACE_PREFIX[5] = "surf";
constexpr char PARAMETER_PREFIX[5] = "parm";
constexpr char DEGREE_PREFIX[4] = "deg";
constexpr char BASIS_MATRIX_PREFIX[5] = "bmat";
constexpr char STEP_SIZE_PREFIX[5] = "step";
constexpr char CUR_SUR_TYPE_PREFIX[7] = "cstype";
constexpr char CUR_SUR_END_PREFIX[4] = "end";


//* Materials(.mtl)
constexpr char MATERIAL_LIB_PREFIX[7] = "mtllib";
constexpr char MATERIAL_USE_PREFIX[7] = "usemtl";
constexpr char NEW_MATERIAL[7] = "newmtl";
constexpr char SHADOW_CASTING_G_PREFIX[11] = "shadow_obj";
constexpr char RAY_TRACING_G_PREFIX[10] = "trace_obj";

//* Bezier & Special
constexpr char BEVEL_PREFIX[6] = "bevel";
constexpr char LEVEL_OF_DETAIL_PREFIX[4] = "lod";
constexpr char CURVE_APPROXIMATION_PREFIX[6] = "ctech";
constexpr char SURFACE_APPROXIMATION_PREFIX[6] = "stech";