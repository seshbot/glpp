/**
 * This file was autogenerated by glgen (https://github.com/seshbot/glgen) on 2015-06-16
 * Command line: src/gen.py assets/gl.xml -p assets/gl-patch.xml -o ../glpp/ --includesubdir include/glpp --sourcesubdir src --namespace gl2 --force --cpp --gl2only
 */

#ifndef ENUMS__H
#define ENUMS__H


#include "../types.h"

namespace gl2 {
  enum class accum_op_t : unsigned int {
    accum = 0x0100,
    load = 0x0101,
    return_ = 0x0102,
    mult = 0x0103,
    add = 0x0104,
  };

  enum class alpha_function_t : unsigned int {
    never = 0x0200,
    less = 0x0201,
    equal = 0x0202,
    lequal = 0x0203,
    greater = 0x0204,
    notequal = 0x0205,
    gequal = 0x0206,
    always = 0x0207,
  };

  enum class attrib_flags_t : unsigned int {
    current_bit = 0x00000001,
    point_bit = 0x00000002,
    line_bit = 0x00000004,
    polygon_bit = 0x00000008,
    polygon_stipple_bit = 0x00000010,
    pixel_mode_bit = 0x00000020,
    lighting_bit = 0x00000040,
    fog_bit = 0x00000080,
    depth_buffer_bit = 0x00000100,
    accum_buffer_bit = 0x00000200,
    stencil_buffer_bit = 0x00000400,
    viewport_bit = 0x00000800,
    transform_bit = 0x00001000,
    enable_bit = 0x00002000,
    color_buffer_bit = 0x00004000,
    hint_bit = 0x00008000,
    eval_bit = 0x00010000,
    list_bit = 0x00020000,
    texture_bit = 0x00040000,
    scissor_bit = 0x00080000,
    multisample_bit = 0x20000000,
    multisample_bit_3dfx = 0x20000000,
    multisample_bit_arb = 0x20000000,
    multisample_bit_ext = 0x20000000,
    all_attrib_bits = 0xFFFFFFFF,
  };
  CREATE_ENUM_BITMASK_TYPE(attrib_mask_t, attrib_flags_t)

  enum class blend_equation_mode_ext_t : unsigned int {
    func_add = 0x8006,
    min = 0x8007,
    max = 0x8008,
    func_subtract = 0x800A,
    func_reverse_subtract = 0x800B,
  };

  enum class blending_factor_dest_t : unsigned int {
    zero = 0,
    src_color = 0x0300,
    one_minus_src_color = 0x0301,
    src_alpha = 0x0302,
    one_minus_src_alpha = 0x0303,
    dst_alpha = 0x0304,
    one_minus_dst_alpha = 0x0305,
    constant_color_ext = 0x8001,
    one_minus_constant_color_ext = 0x8002,
    constant_alpha_ext = 0x8003,
    one_minus_constant_alpha_ext = 0x8004,
    one = 1,
  };

  enum class blending_factor_src_t : unsigned int {
    zero = 0,
    src_alpha = 0x0302,
    one_minus_src_alpha = 0x0303,
    dst_alpha = 0x0304,
    one_minus_dst_alpha = 0x0305,
    dst_color = 0x0306,
    one_minus_dst_color = 0x0307,
    src_alpha_saturate = 0x0308,
    constant_color_ext = 0x8001,
    one_minus_constant_color_ext = 0x8002,
    constant_alpha_ext = 0x8003,
    one_minus_constant_alpha_ext = 0x8004,
    one = 1,
  };

  enum class clear_buffer_flags_t : unsigned int {
    depth_buffer_bit = 0x00000100,
    accum_buffer_bit = 0x00000200,
    stencil_buffer_bit = 0x00000400,
    color_buffer_bit = 0x00004000,
    coverage_buffer_bit_nv = 0x00008000,
  };
  CREATE_ENUM_BITMASK_TYPE(clear_buffer_mask_t, clear_buffer_flags_t)

  enum class client_attrib_flags_t : unsigned int {
    client_pixel_store_bit = 0x00000001,
    client_vertex_array_bit = 0x00000002,
    client_all_attrib_bits = 0xFFFFFFFF,
  };
  CREATE_ENUM_BITMASK_TYPE(client_attrib_mask_t, client_attrib_flags_t)

  enum class clip_plane_name_t : unsigned int {
    clip_distance0 = 0x3000,
    clip_plane0 = 0x3000,
    clip_distance1 = 0x3001,
    clip_plane1 = 0x3001,
    clip_distance2 = 0x3002,
    clip_plane2 = 0x3002,
    clip_distance3 = 0x3003,
    clip_plane3 = 0x3003,
    clip_distance4 = 0x3004,
    clip_plane4 = 0x3004,
    clip_distance5 = 0x3005,
    clip_plane5 = 0x3005,
    clip_distance6 = 0x3006,
    clip_distance7 = 0x3007,
  };

  enum class color_material_face_t : unsigned int {
    front = 0x0404,
    back = 0x0405,
    front_and_back = 0x0408,
  };

  enum class color_material_parameter_t : unsigned int {
    ambient = 0x1200,
    diffuse = 0x1201,
    specular = 0x1202,
    emission = 0x1600,
    ambient_and_diffuse = 0x1602,
  };

  enum class color_pointer_type_t : unsigned int {
    byte_ = 0x1400,
    unsigned_byte_ = 0x1401,
    short_ = 0x1402,
    unsigned_short_ = 0x1403,
    int_ = 0x1404,
    unsigned_int_ = 0x1405,
    float_ = 0x1406,
    double_ = 0x140A,
  };

  enum class cull_face_mode_t : unsigned int {
    front = 0x0404,
    back = 0x0405,
    front_and_back = 0x0408,
  };

  enum class depth_function_t : unsigned int {
    never = 0x0200,
    less = 0x0201,
    equal = 0x0202,
    lequal = 0x0203,
    greater = 0x0204,
    notequal = 0x0205,
    gequal = 0x0206,
    always = 0x0207,
  };

  enum class draw_buffer_mode_t : unsigned int {
    none = 0,
    none_oes = 0,
    front_left = 0x0400,
    front_right = 0x0401,
    back_left = 0x0402,
    back_right = 0x0403,
    front = 0x0404,
    back = 0x0405,
    left = 0x0406,
    right = 0x0407,
    front_and_back = 0x0408,
    aux0 = 0x0409,
    aux1 = 0x040A,
    aux2 = 0x040B,
    aux3 = 0x040C,
  };

  enum class enable_cap_t : unsigned int {
    point_smooth = 0x0B10,
    line_smooth = 0x0B20,
    line_stipple = 0x0B24,
    polygon_smooth = 0x0B41,
    polygon_stipple = 0x0B42,
    cull_face = 0x0B44,
    lighting = 0x0B50,
    color_material = 0x0B57,
    fog = 0x0B60,
    depth_test = 0x0B71,
    stencil_test = 0x0B90,
    normalize = 0x0BA1,
    alpha_test = 0x0BC0,
    dither = 0x0BD0,
    blend = 0x0BE2,
    index_logic_op = 0x0BF1,
    color_logic_op = 0x0BF2,
    scissor_test = 0x0C11,
    texture_gen_s = 0x0C60,
    texture_gen_t = 0x0C61,
    texture_gen_r = 0x0C62,
    texture_gen_q = 0x0C63,
    auto_normal = 0x0D80,
    map1_color_4 = 0x0D90,
    map1_index = 0x0D91,
    map1_normal = 0x0D92,
    map1_texture_coord_1 = 0x0D93,
    map1_texture_coord_2 = 0x0D94,
    map1_texture_coord_3 = 0x0D95,
    map1_texture_coord_4 = 0x0D96,
    map1_vertex_3 = 0x0D97,
    map1_vertex_4 = 0x0D98,
    map2_color_4 = 0x0DB0,
    map2_index = 0x0DB1,
    map2_normal = 0x0DB2,
    map2_texture_coord_1 = 0x0DB3,
    map2_texture_coord_2 = 0x0DB4,
    map2_texture_coord_3 = 0x0DB5,
    map2_texture_coord_4 = 0x0DB6,
    map2_vertex_3 = 0x0DB7,
    map2_vertex_4 = 0x0DB8,
    texture_1d = 0x0DE0,
    texture_2d = 0x0DE1,
    polygon_offset_point = 0x2A01,
    polygon_offset_line = 0x2A02,
    clip_plane0 = 0x3000,
    clip_plane1 = 0x3001,
    clip_plane2 = 0x3002,
    clip_plane3 = 0x3003,
    clip_plane4 = 0x3004,
    clip_plane5 = 0x3005,
    light0 = 0x4000,
    light1 = 0x4001,
    light2 = 0x4002,
    light3 = 0x4003,
    light4 = 0x4004,
    light5 = 0x4005,
    light6 = 0x4006,
    light7 = 0x4007,
    convolution_1d_ext = 0x8010,
    convolution_2d_ext = 0x8011,
    separable_2d_ext = 0x8012,
    histogram_ext = 0x8024,
    minmax_ext = 0x802E,
    polygon_offset_fill = 0x8037,
    rescale_normal_ext = 0x803A,
    texture_3d_ext = 0x806F,
    vertex_array = 0x8074,
    normal_array = 0x8075,
    color_array = 0x8076,
    index_array = 0x8077,
    texture_coord_array = 0x8078,
    edge_flag_array = 0x8079,
    interlace_sgix = 0x8094,
    multisample_sgis = 0x809D,
    sample_alpha_to_mask_sgis = 0x809E,
    sample_alpha_to_one_sgis = 0x809F,
    sample_mask_sgis = 0x80A0,
    texture_color_table_sgi = 0x80BC,
    color_table_sgi = 0x80D0,
    post_convolution_color_table_sgi = 0x80D1,
    post_color_matrix_color_table_sgi = 0x80D2,
    texture_4d_sgis = 0x8134,
    pixel_tex_gen_sgix = 0x8139,
    sprite_sgix = 0x8148,
    reference_plane_sgix = 0x817D,
    ir_instrument1_sgix = 0x817F,
    calligraphic_fragment_sgix = 0x8183,
    framezoom_sgix = 0x818B,
    fog_offset_sgix = 0x8198,
    shared_texture_palette_ext = 0x81FB,
    async_histogram_sgix = 0x832C,
    pixel_texture_sgis = 0x8353,
    async_tex_image_sgix = 0x835C,
    async_draw_pixels_sgix = 0x835D,
    async_read_pixels_sgix = 0x835E,
    fragment_lighting_sgix = 0x8400,
    fragment_color_material_sgix = 0x8401,
    fragment_light0_sgix = 0x840C,
    fragment_light1_sgix = 0x840D,
    fragment_light2_sgix = 0x840E,
    fragment_light3_sgix = 0x840F,
    fragment_light4_sgix = 0x8410,
    fragment_light5_sgix = 0x8411,
    fragment_light6_sgix = 0x8412,
    fragment_light7_sgix = 0x8413,
  };

  enum class error_code_t : unsigned int {
    no_error = 0,
    invalid_enum = 0x0500,
    invalid_value = 0x0501,
    invalid_operation = 0x0502,
    stack_overflow = 0x0503,
    stack_underflow = 0x0504,
    out_of_memory = 0x0505,
  };

  enum class feed_back_token_t : unsigned int {
    pass_through_token = 0x0700,
    point_token = 0x0701,
    line_token = 0x0702,
    polygon_token = 0x0703,
    bitmap_token = 0x0704,
    draw_pixel_token = 0x0705,
    copy_pixel_token = 0x0706,
    line_reset_token = 0x0707,
  };

  enum class feedback_type_t : unsigned int {
    _2d = 0x0600,
    _3d = 0x0601,
    _3d_color = 0x0602,
    _3d_color_texture = 0x0603,
    _4d_color_texture = 0x0604,
  };

  enum class fog_coordinate_pointer_type_t : unsigned int {
    float_ = 0x1406,
    double_ = 0x140A,
  };

  enum class fog_mode_t : unsigned int {
    exp = 0x0800,
    exp2 = 0x0801,
    linear = 0x2601,
  };

  enum class fog_parameter_t : unsigned int {
    fog_index = 0x0B61,
    fog_density = 0x0B62,
    fog_start = 0x0B63,
    fog_end = 0x0B64,
    fog_mode = 0x0B65,
    fog_color = 0x0B66,
    fog_offset_value_sgix = 0x8199,
  };

  enum class fog_pointer_type_ext_t : unsigned int {
    float_ = 0x1406,
    double_ = 0x140A,
  };

  enum class fog_pointer_type_ibm_t : unsigned int {
    float_ = 0x1406,
    double_ = 0x140A,
  };

  enum class front_face_direction_t : unsigned int {
    cw = 0x0900,
    ccw = 0x0901,
  };

  enum class get_map_query_t : unsigned int {
    coeff = 0x0A00,
    order = 0x0A01,
    domain = 0x0A02,
  };

  enum class get_p_name_t : unsigned int {
    line_width = 0x0B21,
    cull_face = 0x0B44,
    cull_face_mode = 0x0B45,
    front_face = 0x0B46,
    depth_range = 0x0B70,
    depth_test = 0x0B71,
    depth_writemask = 0x0B72,
    depth_clear_value = 0x0B73,
    depth_func = 0x0B74,
    stencil_test = 0x0B90,
    stencil_clear_value = 0x0B91,
    stencil_func = 0x0B92,
    stencil_value_mask = 0x0B93,
    stencil_fail = 0x0B94,
    stencil_pass_depth_fail = 0x0B95,
    stencil_pass_depth_pass = 0x0B96,
    stencil_ref = 0x0B97,
    stencil_writemask = 0x0B98,
    viewport = 0x0BA2,
    dither = 0x0BD0,
    blend = 0x0BE2,
    scissor_box = 0x0C10,
    scissor_test = 0x0C11,
    color_clear_value = 0x0C22,
    color_writemask = 0x0C23,
    unpack_alignment = 0x0CF5,
    pack_alignment = 0x0D05,
    max_texture_size = 0x0D33,
    max_viewport_dims = 0x0D3A,
    subpixel_bits = 0x0D50,
    red_bits = 0x0D52,
    green_bits = 0x0D53,
    blue_bits = 0x0D54,
    alpha_bits = 0x0D55,
    depth_bits = 0x0D56,
    stencil_bits = 0x0D57,
    polygon_offset_units = 0x2A00,
    blend_color = 0x8005,
    blend_equation_rgb = 0x8009,
    polygon_offset_fill = 0x8037,
    polygon_offset_factor = 0x8038,
    texture_binding_2d = 0x8069,
    sample_alpha_to_coverage = 0x809E,
    sample_coverage = 0x80A0,
    sample_buffers = 0x80A8,
    samples = 0x80A9,
    sample_coverage_value = 0x80AA,
    sample_coverage_invert = 0x80AB,
    blend_dst_rgb = 0x80C8,
    blend_src_rgb = 0x80C9,
    blend_dst_alpha = 0x80CA,
    blend_src_alpha = 0x80CB,
    generate_mipmap_hint = 0x8192,
    aliased_point_size_range = 0x846D,
    aliased_line_width_range = 0x846E,
    active_texture = 0x84E0,
    max_renderbuffer_size = 0x84E8,
    texture_binding_cube_map = 0x8514,
    max_cube_map_texture_size = 0x851C,
    num_compressed_texture_formats = 0x86A2,
    compressed_texture_formats = 0x86A3,
    stencil_back_func = 0x8800,
    stencil_back_fail = 0x8801,
    stencil_back_pass_depth_fail = 0x8802,
    stencil_back_pass_depth_pass = 0x8803,
    blend_equation_alpha = 0x883D,
    max_vertex_attribs = 0x8869,
    max_texture_image_units = 0x8872,
    array_buffer_binding = 0x8894,
    element_array_buffer_binding = 0x8895,
    max_vertex_texture_image_units = 0x8B4C,
    max_combined_texture_image_units = 0x8B4D,
    current_program = 0x8B8D,
    implementation_color_read_type = 0x8B9A,
    implementation_color_read_format = 0x8B9B,
    stencil_back_ref = 0x8CA3,
    stencil_back_value_mask = 0x8CA4,
    stencil_back_writemask = 0x8CA5,
    framebuffer_binding = 0x8CA6,
    renderbuffer_binding = 0x8CA7,
    shader_binary_formats = 0x8DF8,
    num_shader_binary_formats = 0x8DF9,
    shader_compiler = 0x8DFA,
    max_vertex_uniform_vectors = 0x8DFB,
    max_varying_vectors = 0x8DFC,
    max_fragment_uniform_vectors = 0x8DFD,
  };

  enum class get_pixel_map_t : unsigned int {
    pixel_map_i_to_i = 0x0C70,
    pixel_map_s_to_s = 0x0C71,
    pixel_map_i_to_r = 0x0C72,
    pixel_map_i_to_g = 0x0C73,
    pixel_map_i_to_b = 0x0C74,
    pixel_map_i_to_a = 0x0C75,
    pixel_map_r_to_r = 0x0C76,
    pixel_map_g_to_g = 0x0C77,
    pixel_map_b_to_b = 0x0C78,
    pixel_map_a_to_a = 0x0C79,
  };

  enum class get_pointerv_p_name_t : unsigned int {
    feedback_buffer_pointer = 0x0DF0,
    selection_buffer_pointer = 0x0DF3,
    vertex_array_pointer = 0x808E,
    vertex_array_pointer_ext = 0x808E,
    normal_array_pointer = 0x808F,
    normal_array_pointer_ext = 0x808F,
    color_array_pointer = 0x8090,
    color_array_pointer_ext = 0x8090,
    index_array_pointer = 0x8091,
    index_array_pointer_ext = 0x8091,
    texture_coord_array_pointer = 0x8092,
    texture_coord_array_pointer_ext = 0x8092,
    edge_flag_array_pointer = 0x8093,
    edge_flag_array_pointer_ext = 0x8093,
    instrument_buffer_pointer_sgix = 0x8180,
  };

  enum class get_texture_parameter_t : unsigned int {
    texture_width = 0x1000,
    texture_height = 0x1001,
    texture_components = 0x1003,
    texture_internal_format = 0x1003,
    texture_border_color = 0x1004,
    texture_border_color_nv = 0x1004,
    texture_border = 0x1005,
    texture_mag_filter = 0x2800,
    texture_min_filter = 0x2801,
    texture_wrap_s = 0x2802,
    texture_wrap_t = 0x2803,
    texture_red_size = 0x805C,
    texture_green_size = 0x805D,
    texture_blue_size = 0x805E,
    texture_alpha_size = 0x805F,
    texture_luminance_size = 0x8060,
    texture_intensity_size = 0x8061,
    texture_priority = 0x8066,
    texture_resident = 0x8067,
    texture_depth_ext = 0x8071,
    texture_wrap_r_ext = 0x8072,
    detail_texture_level_sgis = 0x809A,
    detail_texture_mode_sgis = 0x809B,
    detail_texture_func_points_sgis = 0x809C,
    sharpen_texture_func_points_sgis = 0x80B0,
    shadow_ambient_sgix = 0x80BF,
    dual_texture_select_sgis = 0x8124,
    quad_texture_select_sgis = 0x8125,
    texture_4dsize_sgis = 0x8136,
    texture_wrap_q_sgis = 0x8137,
    texture_min_lod_sgis = 0x813A,
    texture_max_lod_sgis = 0x813B,
    texture_base_level_sgis = 0x813C,
    texture_max_level_sgis = 0x813D,
    texture_filter4_size_sgis = 0x8147,
    texture_clipmap_center_sgix = 0x8171,
    texture_clipmap_frame_sgix = 0x8172,
    texture_clipmap_offset_sgix = 0x8173,
    texture_clipmap_virtual_depth_sgix = 0x8174,
    texture_clipmap_lod_offset_sgix = 0x8175,
    texture_clipmap_depth_sgix = 0x8176,
    post_texture_filter_bias_sgix = 0x8179,
    post_texture_filter_scale_sgix = 0x817A,
    texture_lod_bias_s_sgix = 0x818E,
    texture_lod_bias_t_sgix = 0x818F,
    texture_lod_bias_r_sgix = 0x8190,
    generate_mipmap_sgis = 0x8191,
    texture_compare_sgix = 0x819A,
    texture_compare_operator_sgix = 0x819B,
    texture_lequal_r_sgix = 0x819C,
    texture_gequal_r_sgix = 0x819D,
    texture_max_clamp_s_sgix = 0x8369,
    texture_max_clamp_t_sgix = 0x836A,
    texture_max_clamp_r_sgix = 0x836B,
  };

  enum class hint_mode_t : unsigned int {
    dont_care = 0x1100,
    fastest = 0x1101,
    nicest = 0x1102,
  };

  enum class hint_target_t : unsigned int {
    perspective_correction_hint = 0x0C50,
    point_smooth_hint = 0x0C51,
    line_smooth_hint = 0x0C52,
    polygon_smooth_hint = 0x0C53,
    fog_hint = 0x0C54,
    prefer_doublebuffer_hint_pgi = 0x1A1F8,
    conserve_memory_hint_pgi = 0x1A1FD,
    reclaim_memory_hint_pgi = 0x1A1FE,
    native_graphics_begin_hint_pgi = 0x1A203,
    native_graphics_end_hint_pgi = 0x1A204,
    always_fast_hint_pgi = 0x1A20C,
    always_soft_hint_pgi = 0x1A20D,
    allow_draw_obj_hint_pgi = 0x1A20E,
    allow_draw_win_hint_pgi = 0x1A20F,
    allow_draw_frg_hint_pgi = 0x1A210,
    allow_draw_mem_hint_pgi = 0x1A211,
    strict_depthfunc_hint_pgi = 0x1A216,
    strict_lighting_hint_pgi = 0x1A217,
    strict_scissor_hint_pgi = 0x1A218,
    full_stipple_hint_pgi = 0x1A219,
    clip_near_hint_pgi = 0x1A220,
    clip_far_hint_pgi = 0x1A221,
    wide_line_hint_pgi = 0x1A222,
    back_normals_hint_pgi = 0x1A223,
    vertex_data_hint_pgi = 0x1A22A,
    vertex_consistent_hint_pgi = 0x1A22B,
    material_side_hint_pgi = 0x1A22C,
    max_vertex_hint_pgi = 0x1A22D,
    pack_cmyk_hint_ext = 0x800E,
    unpack_cmyk_hint_ext = 0x800F,
    phong_hint_win = 0x80EB,
    clip_volume_clipping_hint_ext = 0x80F0,
    texture_multi_buffer_hint_sgix = 0x812E,
    generate_mipmap_hint = 0x8192,
    generate_mipmap_hint_sgis = 0x8192,
    program_binary_retrievable_hint = 0x8257,
    convolution_hint_sgix = 0x8316,
    scalebias_hint_sgix = 0x8322,
    line_quality_hint_sgix = 0x835B,
    vertex_preclip_sgix = 0x83EE,
    vertex_preclip_hint_sgix = 0x83EF,
    texture_compression_hint = 0x84EF,
    texture_compression_hint_arb = 0x84EF,
    vertex_array_storage_hint_apple = 0x851F,
    multisample_filter_hint_nv = 0x8534,
    transform_hint_apple = 0x85B1,
    texture_storage_hint_apple = 0x85BC,
    fragment_shader_derivative_hint = 0x8B8B,
    fragment_shader_derivative_hint_arb = 0x8B8B,
    fragment_shader_derivative_hint_oes = 0x8B8B,
    binning_control_hint_qcom = 0x8FB0,
  };

  enum class index_pointer_type_t : unsigned int {
    short_ = 0x1402,
    int_ = 0x1404,
    float_ = 0x1406,
    double_ = 0x140A,
  };

  enum class interleaved_array_format_t : unsigned int {
    v2f = 0x2A20,
    v3f = 0x2A21,
    c4ub_v2f = 0x2A22,
    c4ub_v3f = 0x2A23,
    c3f_v3f = 0x2A24,
    n3f_v3f = 0x2A25,
    c4f_n3f_v3f = 0x2A26,
    t2f_v3f = 0x2A27,
    t4f_v4f = 0x2A28,
    t2f_c4ub_v3f = 0x2A29,
    t2f_c3f_v3f = 0x2A2A,
    t2f_n3f_v3f = 0x2A2B,
    t2f_c4f_n3f_v3f = 0x2A2C,
    t4f_c4f_n3f_v4f = 0x2A2D,
  };

  enum class internal_format_t : unsigned int {
    rgb8 = 0x8051,
    rgba4 = 0x8056,
    rgb5_a1 = 0x8057,
    rgba8 = 0x8058,
    rgb10_a2 = 0x8059,
    depth_component16 = 0x81A5,
    depth_component24 = 0x81A6,
  };

  enum class light_env_mode_sgix_t : unsigned int {
    add = 0x0104,
    replace = 0x1E01,
    modulate = 0x2100,
  };

  enum class light_model_color_control_t : unsigned int {
    single_color = 0x81F9,
    separate_specular_color = 0x81FA,
  };

  enum class light_model_parameter_t : unsigned int {
    light_model_local_viewer = 0x0B51,
    light_model_two_side = 0x0B52,
    light_model_ambient = 0x0B53,
    light_model_color_control = 0x81F8,
    light_model_color_control_ext = 0x81F8,
  };

  enum class light_name_t : unsigned int {
    light0 = 0x4000,
    light1 = 0x4001,
    light2 = 0x4002,
    light3 = 0x4003,
    light4 = 0x4004,
    light5 = 0x4005,
    light6 = 0x4006,
    light7 = 0x4007,
    fragment_light0_sgix = 0x840C,
    fragment_light1_sgix = 0x840D,
    fragment_light2_sgix = 0x840E,
    fragment_light3_sgix = 0x840F,
    fragment_light4_sgix = 0x8410,
    fragment_light5_sgix = 0x8411,
    fragment_light6_sgix = 0x8412,
    fragment_light7_sgix = 0x8413,
  };

  enum class light_parameter_t : unsigned int {
    ambient = 0x1200,
    diffuse = 0x1201,
    specular = 0x1202,
    position = 0x1203,
    spot_direction = 0x1204,
    spot_exponent = 0x1205,
    spot_cutoff = 0x1206,
    constant_attenuation = 0x1207,
    linear_attenuation = 0x1208,
    quadratic_attenuation = 0x1209,
  };

  enum class list_mode_t : unsigned int {
    compile = 0x1300,
    compile_and_execute = 0x1301,
  };

  enum class list_name_type_t : unsigned int {
    byte_ = 0x1400,
    unsigned_byte_ = 0x1401,
    short_ = 0x1402,
    unsigned_short_ = 0x1403,
    int_ = 0x1404,
    unsigned_int_ = 0x1405,
    float_ = 0x1406,
    _2_bytes = 0x1407,
    _3_bytes = 0x1408,
    _4_bytes = 0x1409,
  };

  enum class logic_op_t : unsigned int {
    clear = 0x1500,
    and_ = 0x1501,
    and_reverse = 0x1502,
    copy = 0x1503,
    and_inverted = 0x1504,
    noop = 0x1505,
    xor_ = 0x1506,
    or_ = 0x1507,
    nor = 0x1508,
    equiv = 0x1509,
    invert = 0x150A,
    or_reverse = 0x150B,
    copy_inverted = 0x150C,
    or_inverted = 0x150D,
    nand = 0x150E,
    set = 0x150F,
  };

  enum class map_target_t : unsigned int {
    map1_color_4 = 0x0D90,
    map1_index = 0x0D91,
    map1_normal = 0x0D92,
    map1_texture_coord_1 = 0x0D93,
    map1_texture_coord_2 = 0x0D94,
    map1_texture_coord_3 = 0x0D95,
    map1_texture_coord_4 = 0x0D96,
    map1_vertex_3 = 0x0D97,
    map1_vertex_4 = 0x0D98,
    map2_color_4 = 0x0DB0,
    map2_index = 0x0DB1,
    map2_normal = 0x0DB2,
    map2_texture_coord_1 = 0x0DB3,
    map2_texture_coord_2 = 0x0DB4,
    map2_texture_coord_3 = 0x0DB5,
    map2_texture_coord_4 = 0x0DB6,
    map2_vertex_3 = 0x0DB7,
    map2_vertex_4 = 0x0DB8,
    geometry_deformation_sgix = 0x8194,
    texture_deformation_sgix = 0x8195,
  };

  enum class material_face_t : unsigned int {
    front = 0x0404,
    back = 0x0405,
    front_and_back = 0x0408,
  };

  enum class material_parameter_t : unsigned int {
    ambient = 0x1200,
    diffuse = 0x1201,
    specular = 0x1202,
    emission = 0x1600,
    shininess = 0x1601,
    ambient_and_diffuse = 0x1602,
    color_indexes = 0x1603,
  };

  enum class matrix_mode_t : unsigned int {
    modelview = 0x1700,
    modelview0_ext = 0x1700,
    projection = 0x1701,
    texture = 0x1702,
  };

  enum class mesh_mode_1_t : unsigned int {
    point = 0x1B00,
    line = 0x1B01,
  };

  enum class mesh_mode_2_t : unsigned int {
    point = 0x1B00,
    line = 0x1B01,
    fill = 0x1B02,
  };

  enum class normal_pointer_type_t : unsigned int {
    byte_ = 0x1400,
    short_ = 0x1402,
    int_ = 0x1404,
    float_ = 0x1406,
    double_ = 0x140A,
  };

  enum class pixel_copy_type_t : unsigned int {
    color = 0x1800,
    color_ext = 0x1800,
    depth = 0x1801,
    depth_ext = 0x1801,
    stencil = 0x1802,
    stencil_ext = 0x1802,
  };

  enum class pixel_format_t : unsigned int {
    unsigned_short_ = 0x1403,
    unsigned_int_ = 0x1405,
    color_index = 0x1900,
    stencil_index = 0x1901,
    depth_component = 0x1902,
    red = 0x1903,
    red_ext = 0x1903,
    green = 0x1904,
    blue = 0x1905,
    alpha = 0x1906,
    rgb = 0x1907,
    rgba = 0x1908,
    luminance = 0x1909,
    luminance_alpha = 0x190A,
    abgr_ext = 0x8000,
    cmyk_ext = 0x800C,
    cmyka_ext = 0x800D,
    bgra_ext = 0x80E1,
    ycrcb_422_sgix = 0x81BB,
    ycrcb_444_sgix = 0x81BC,
    depth_stencil_oes = 0x84F9,
  };

  enum class pixel_map_t : unsigned int {
    pixel_map_i_to_i = 0x0C70,
    pixel_map_s_to_s = 0x0C71,
    pixel_map_i_to_r = 0x0C72,
    pixel_map_i_to_g = 0x0C73,
    pixel_map_i_to_b = 0x0C74,
    pixel_map_i_to_a = 0x0C75,
    pixel_map_r_to_r = 0x0C76,
    pixel_map_g_to_g = 0x0C77,
    pixel_map_b_to_b = 0x0C78,
    pixel_map_a_to_a = 0x0C79,
  };

  enum class pixel_store_parameter_t : unsigned int {
    unpack_swap_bytes = 0x0CF0,
    unpack_lsb_first = 0x0CF1,
    unpack_row_length = 0x0CF2,
    unpack_row_length_ext = 0x0CF2,
    unpack_skip_rows = 0x0CF3,
    unpack_skip_rows_ext = 0x0CF3,
    unpack_skip_pixels = 0x0CF4,
    unpack_skip_pixels_ext = 0x0CF4,
    unpack_alignment = 0x0CF5,
    pack_swap_bytes = 0x0D00,
    pack_lsb_first = 0x0D01,
    pack_row_length = 0x0D02,
    pack_skip_rows = 0x0D03,
    pack_skip_pixels = 0x0D04,
    pack_alignment = 0x0D05,
    pack_skip_images = 0x806B,
    pack_skip_images_ext = 0x806B,
    pack_image_height = 0x806C,
    pack_image_height_ext = 0x806C,
    unpack_skip_images = 0x806D,
    unpack_skip_images_ext = 0x806D,
    unpack_image_height = 0x806E,
    unpack_image_height_ext = 0x806E,
    pack_skip_volumes_sgis = 0x8130,
    pack_image_depth_sgis = 0x8131,
    unpack_skip_volumes_sgis = 0x8132,
    unpack_image_depth_sgis = 0x8133,
    pixel_tile_width_sgix = 0x8140,
    pixel_tile_height_sgix = 0x8141,
    pixel_tile_grid_width_sgix = 0x8142,
    pixel_tile_grid_height_sgix = 0x8143,
    pixel_tile_grid_depth_sgix = 0x8144,
    pixel_tile_cache_size_sgix = 0x8145,
    pack_resample_sgix = 0x842C,
    unpack_resample_sgix = 0x842D,
    pack_subsample_rate_sgix = 0x85A0,
    unpack_subsample_rate_sgix = 0x85A1,
    pack_resample_oml = 0x8984,
    unpack_resample_oml = 0x8985,
  };

  enum class pixel_tex_gen_mode_t : unsigned int {
    none = 0,
    rgb = 0x1907,
    rgba = 0x1908,
    luminance = 0x1909,
    luminance_alpha = 0x190A,
  };

  enum class pixel_transfer_parameter_t : unsigned int {
    map_color = 0x0D10,
    map_stencil = 0x0D11,
    index_shift = 0x0D12,
    index_offset = 0x0D13,
    red_scale = 0x0D14,
    red_bias = 0x0D15,
    green_scale = 0x0D18,
    green_bias = 0x0D19,
    blue_scale = 0x0D1A,
    blue_bias = 0x0D1B,
    alpha_scale = 0x0D1C,
    alpha_bias = 0x0D1D,
    depth_scale = 0x0D1E,
    depth_bias = 0x0D1F,
    post_convolution_red_scale = 0x801C,
    post_convolution_red_scale_ext = 0x801C,
    post_convolution_green_scale = 0x801D,
    post_convolution_green_scale_ext = 0x801D,
    post_convolution_blue_scale = 0x801E,
    post_convolution_blue_scale_ext = 0x801E,
    post_convolution_alpha_scale = 0x801F,
    post_convolution_alpha_scale_ext = 0x801F,
    post_convolution_red_bias = 0x8020,
    post_convolution_red_bias_ext = 0x8020,
    post_convolution_green_bias = 0x8021,
    post_convolution_green_bias_ext = 0x8021,
    post_convolution_blue_bias = 0x8022,
    post_convolution_blue_bias_ext = 0x8022,
    post_convolution_alpha_bias = 0x8023,
    post_convolution_alpha_bias_ext = 0x8023,
    post_color_matrix_red_scale = 0x80B4,
    post_color_matrix_red_scale_sgi = 0x80B4,
    post_color_matrix_green_scale = 0x80B5,
    post_color_matrix_green_scale_sgi = 0x80B5,
    post_color_matrix_blue_scale = 0x80B6,
    post_color_matrix_blue_scale_sgi = 0x80B6,
    post_color_matrix_alpha_scale = 0x80B7,
    post_color_matrix_alpha_scale_sgi = 0x80B7,
    post_color_matrix_red_bias = 0x80B8,
    post_color_matrix_red_bias_sgi = 0x80B8,
    post_color_matrix_green_bias = 0x80B9,
    post_color_matrix_green_bias_sgi = 0x80B9,
    post_color_matrix_blue_bias = 0x80BA,
    post_color_matrix_blue_bias_sgi = 0x80BA,
    post_color_matrix_alpha_bias = 0x80BB,
    post_color_matrix_alpha_bias_sgi = 0x80BB,
  };

  enum class pixel_type_t : unsigned int {
    byte_ = 0x1400,
    unsigned_byte_ = 0x1401,
    short_ = 0x1402,
    unsigned_short_ = 0x1403,
    int_ = 0x1404,
    unsigned_int_ = 0x1405,
    float_ = 0x1406,
    bitmap = 0x1A00,
    unsigned_byte_3_3_2 = 0x8032,
    unsigned_byte_3_3_2_ext = 0x8032,
    unsigned_short_4_4_4_4 = 0x8033,
    unsigned_short_4_4_4_4_ext = 0x8033,
    unsigned_short_5_5_5_1 = 0x8034,
    unsigned_short_5_5_5_1_ext = 0x8034,
    unsigned_int_8_8_8_8 = 0x8035,
    unsigned_int_8_8_8_8_ext = 0x8035,
    unsigned_int_10_10_10_2 = 0x8036,
    unsigned_int_10_10_10_2_ext = 0x8036,
  };

  enum class point_parameter_name_sgis_t : unsigned int {
    point_size_min = 0x8126,
    point_size_max = 0x8127,
    point_fade_threshold_size = 0x8128,
    point_distance_attenuation = 0x8129,
  };

  enum class polygon_mode_t : unsigned int {
    point = 0x1B00,
    line = 0x1B01,
    fill = 0x1B02,
  };

  enum class primitive_type_t : unsigned int {
    points = 0x0000,
    lines = 0x0001,
    line_loop = 0x0002,
    line_strip = 0x0003,
    triangles = 0x0004,
    triangle_strip = 0x0005,
    triangle_fan = 0x0006,
    quads = 0x0007,
    quads_ext = 0x0007,
    quad_strip = 0x0008,
    polygon = 0x0009,
    lines_adjacency = 0x000A,
    lines_adjacency_arb = 0x000A,
    lines_adjacency_ext = 0x000A,
    line_strip_adjacency = 0x000B,
    line_strip_adjacency_arb = 0x000B,
    line_strip_adjacency_ext = 0x000B,
    triangles_adjacency = 0x000C,
    triangles_adjacency_arb = 0x000C,
    triangles_adjacency_ext = 0x000C,
    triangle_strip_adjacency = 0x000D,
    triangle_strip_adjacency_arb = 0x000D,
    triangle_strip_adjacency_ext = 0x000D,
    patches = 0x000E,
    patches_ext = 0x000E,
  };

  enum class read_buffer_mode_t : unsigned int {
    front_left = 0x0400,
    front_right = 0x0401,
    back_left = 0x0402,
    back_right = 0x0403,
    front = 0x0404,
    back = 0x0405,
    left = 0x0406,
    right = 0x0407,
    aux0 = 0x0409,
    aux1 = 0x040A,
    aux2 = 0x040B,
    aux3 = 0x040C,
  };

  enum class rendering_mode_t : unsigned int {
    render = 0x1C00,
    feedback = 0x1C01,
    select = 0x1C02,
  };

  enum class shading_model_t : unsigned int {
    flat = 0x1D00,
    smooth = 0x1D01,
  };

  enum class stencil_function_t : unsigned int {
    never = 0x0200,
    less = 0x0201,
    equal = 0x0202,
    lequal = 0x0203,
    greater = 0x0204,
    notequal = 0x0205,
    gequal = 0x0206,
    always = 0x0207,
  };

  enum class stencil_op_t : unsigned int {
    zero = 0,
    invert = 0x150A,
    keep = 0x1E00,
    replace = 0x1E01,
    incr = 0x1E02,
    decr = 0x1E03,
  };

  enum class string_name_t : unsigned int {
    vendor = 0x1F00,
    renderer = 0x1F01,
    version = 0x1F02,
    extensions = 0x1F03,
  };

  enum class tex_coord_pointer_type_t : unsigned int {
    short_ = 0x1402,
    int_ = 0x1404,
    float_ = 0x1406,
    double_ = 0x140A,
  };

  enum class texture_coord_name_t : unsigned int {
    s = 0x2000,
    t = 0x2001,
    r = 0x2002,
    q = 0x2003,
  };

  enum class texture_env_mode_t : unsigned int {
    add = 0x0104,
    blend = 0x0BE2,
    modulate = 0x2100,
    decal = 0x2101,
  };

  enum class texture_env_parameter_t : unsigned int {
    texture_env_mode = 0x2200,
    texture_env_color = 0x2201,
  };

  enum class texture_env_target_t : unsigned int {
    texture_env = 0x2300,
  };

  enum class texture_gen_mode_t : unsigned int {
    eye_linear = 0x2400,
    object_linear = 0x2401,
    sphere_map = 0x2402,
  };

  enum class texture_gen_parameter_t : unsigned int {
    texture_gen_mode = 0x2500,
    object_plane = 0x2501,
    eye_plane = 0x2502,
    eye_point_sgis = 0x81F4,
    object_point_sgis = 0x81F5,
    eye_line_sgis = 0x81F6,
    object_line_sgis = 0x81F7,
  };

  enum class texture_mag_filter_t : unsigned int {
    nearest = 0x2600,
    linear = 0x2601,
  };

  enum class texture_min_filter_t : unsigned int {
    nearest = 0x2600,
    linear = 0x2601,
    nearest_mipmap_nearest = 0x2700,
    linear_mipmap_nearest = 0x2701,
    nearest_mipmap_linear = 0x2702,
    linear_mipmap_linear = 0x2703,
  };

  enum class texture_parameter_name_t : unsigned int {
    texture_border_color = 0x1004,
    texture_mag_filter = 0x2800,
    texture_min_filter = 0x2801,
    texture_wrap_s = 0x2802,
    texture_wrap_t = 0x2803,
    texture_priority = 0x8066,
    texture_priority_ext = 0x8066,
    texture_wrap_r = 0x8072,
    texture_wrap_r_ext = 0x8072,
    texture_wrap_r_oes = 0x8072,
    detail_texture_level_sgis = 0x809A,
    detail_texture_mode_sgis = 0x809B,
    shadow_ambient_sgix = 0x80BF,
    dual_texture_select_sgis = 0x8124,
    quad_texture_select_sgis = 0x8125,
    texture_wrap_q_sgis = 0x8137,
    texture_clipmap_center_sgix = 0x8171,
    texture_clipmap_frame_sgix = 0x8172,
    texture_clipmap_offset_sgix = 0x8173,
    texture_clipmap_virtual_depth_sgix = 0x8174,
    texture_clipmap_lod_offset_sgix = 0x8175,
    texture_clipmap_depth_sgix = 0x8176,
    post_texture_filter_bias_sgix = 0x8179,
    post_texture_filter_scale_sgix = 0x817A,
    texture_lod_bias_s_sgix = 0x818E,
    texture_lod_bias_t_sgix = 0x818F,
    texture_lod_bias_r_sgix = 0x8190,
    generate_mipmap = 0x8191,
    generate_mipmap_sgis = 0x8191,
    texture_compare_sgix = 0x819A,
    texture_max_clamp_s_sgix = 0x8369,
    texture_max_clamp_t_sgix = 0x836A,
    texture_max_clamp_r_sgix = 0x836B,
  };

  enum class texture_target_t : unsigned int {
    texture_1d = 0x0DE0,
    texture_2d = 0x0DE1,
    proxy_texture_1d = 0x8063,
    proxy_texture_1d_ext = 0x8063,
    proxy_texture_2d = 0x8064,
    proxy_texture_2d_ext = 0x8064,
    texture_3d = 0x806F,
    texture_3d_ext = 0x806F,
    texture_3d_oes = 0x806F,
    proxy_texture_3d = 0x8070,
    proxy_texture_3d_ext = 0x8070,
    detail_texture_2d_sgis = 0x8095,
    texture_4d_sgis = 0x8134,
    proxy_texture_4d_sgis = 0x8135,
    texture_min_lod = 0x813A,
    texture_min_lod_sgis = 0x813A,
    texture_max_lod = 0x813B,
    texture_max_lod_sgis = 0x813B,
    texture_base_level = 0x813C,
    texture_base_level_sgis = 0x813C,
    texture_max_level = 0x813D,
    texture_max_level_sgis = 0x813D,
    texture_cube_map = 0x8513,
  };

  enum class texture_wrap_mode_t : unsigned int {
    clamp = 0x2900,
    repeat = 0x2901,
    clamp_to_border = 0x812D,
    clamp_to_edge = 0x812F,
  };

  enum class vertex_pointer_type_t : unsigned int {
    short_ = 0x1402,
    int_ = 0x1404,
    float_ = 0x1406,
    double_ = 0x140A,
  };

  enum class attrib_type_t : unsigned int {
    float_ = 0x1406,
    float_vec2 = 0x8B50,
    float_vec3 = 0x8B51,
    float_vec4 = 0x8B52,
    float_mat2 = 0x8B5A,
    float_mat3 = 0x8B5B,
    float_mat4 = 0x8B5C,
  };

  enum class blend_equation_mode_t : unsigned int {
    func_add = 0x8006,
    min = 0x8007,
    max = 0x8008,
    func_subtract = 0x800A,
    func_reverse_subtract = 0x800B,
  };

  enum class blend_func_separate_parameter_ext_t : unsigned int {
    zero = 0,
    src_color = 0x0300,
    one_minus_src_color = 0x0301,
    src_alpha = 0x0302,
    one_minus_src_alpha = 0x0303,
    dst_alpha = 0x0304,
    one_minus_dst_alpha = 0x0305,
    dst_color = 0x0306,
    one_minus_dst_color = 0x0307,
    src_alpha_saturate = 0x0308,
    constant_color = 0x8001,
    one_minus_constant_color = 0x8002,
    constant_alpha = 0x8003,
    one_minus_constant_alpha = 0x8004,
    one = 1,
  };

  enum class buffer_p_name_arb_t : unsigned int {
    buffer_size = 0x8764,
    buffer_usage = 0x8765,
  };

  enum class buffer_target_arb_t : unsigned int {
    array_buffer = 0x8892,
    element_array_buffer = 0x8893,
    pixel_pack_buffer = 0x88EB,
    pixel_unpack_buffer = 0x88EC,
    uniform_buffer = 0x8A11,
    texture_buffer = 0x8C2A,
    transform_feedback_buffer = 0x8C8E,
    copy_read_buffer = 0x8F36,
    copy_write_buffer = 0x8F37,
    draw_indirect_buffer = 0x8F3F,
    shader_storage_buffer = 0x90D2,
    dispatch_indirect_buffer = 0x90EE,
    query_buffer = 0x9192,
    atomic_counter_buffer = 0x92C0,
  };

  enum class buffer_usage_arb_t : unsigned int {
    stream_draw = 0x88E0,
    stream_read = 0x88E1,
    stream_copy = 0x88E2,
    static_draw = 0x88E4,
    static_read = 0x88E5,
    static_copy = 0x88E6,
    dynamic_draw = 0x88E8,
    dynamic_read = 0x88E9,
    dynamic_copy = 0x88EA,
  };

  enum class draw_elements_type_t : unsigned int {
    unsigned_byte_ = 0x1401,
    unsigned_short_ = 0x1403,
    unsigned_int_ = 0x1405,
  };

  enum class framebuffer_texture_target_t : unsigned int {
    texture_2d = 0x0DE1,
    texture_cube_map_positive_x = 0x8515,
    texture_cube_map_negative_x = 0x8516,
    texture_cube_map_positive_y = 0x8517,
    texture_cube_map_negative_y = 0x8518,
    texture_cube_map_positive_z = 0x8519,
    texture_cube_map_negative_z = 0x851A,
  };

  enum class get_programiv_p_name_t : unsigned int {
    delete_status = 0x8B80,
    link_status = 0x8B82,
    validate_status = 0x8B83,
    info_log_length = 0x8B84,
    attached_shaders = 0x8B85,
    active_uniforms = 0x8B86,
    active_uniform_max_length = 0x8B87,
    active_attributes = 0x8B89,
    active_attribute_max_length = 0x8B8A,
  };

  enum class get_shaderiv_p_name_t : unsigned int {
    shader_type = 0x8B4F,
    delete_status = 0x8B80,
    compile_status = 0x8B81,
    info_log_length = 0x8B84,
    shader_source_length = 0x8B88,
  };

  enum class mipmap_target_t : unsigned int {
    texture_1d = 0x0DE0,
    texture_2d = 0x0DE1,
    texture_3d = 0x806F,
    texture_cube_map = 0x8513,
    texture_1d_array = 0x8C18,
  };

  enum class shader_type_t : unsigned int {
    fragment_shader = 0x8B30,
    vertex_shader = 0x8B31,
    geometry_shader = 0x8DD9,
    tess_evaluation_shader = 0x8E87,
    tess_control_shader = 0x8E88,
    compute_shader = 0x91B9,
  };

  enum class stencil_face_direction_t : unsigned int {
    front = 0x0404,
    back = 0x0405,
    front_and_back = 0x0408,
  };

  enum class texture_component_count_t : unsigned int {
    depth_component = 0x1902,
    alpha = 0x1906,
    rgb = 0x1907,
    rgba = 0x1908,
    luminance = 0x1909,
    luminance_alpha = 0x190A,
    bgra_ext = 0x80E1,
    depth_stencil_oes = 0x84F9,
  };

  enum class texture_image_target_t : unsigned int {
    texture_2d = 0x0DE1,
    proxy_texture_2d = 0x8064,
    texture_rectangle = 0x84F5,
    proxy_texture_rectangle = 0x84F7,
    texture_cube_map_positive_x = 0x8515,
    texture_cube_map_negative_x = 0x8516,
    texture_cube_map_positive_y = 0x8517,
    texture_cube_map_negative_y = 0x8518,
    texture_cube_map_positive_z = 0x8519,
    texture_cube_map_negative_z = 0x851A,
    proxy_texture_cube_map = 0x851B,
    texture_1d_array = 0x8C18,
    proxy_texture_1d_array = 0x8C19,
  };

  enum class texture_unit_t : unsigned int {
    texture0 = 0x84C0,
    texture1 = 0x84C1,
    texture2 = 0x84C2,
    texture3 = 0x84C3,
    texture4 = 0x84C4,
    texture5 = 0x84C5,
    texture6 = 0x84C6,
    texture7 = 0x84C7,
    texture8 = 0x84C8,
    texture9 = 0x84C9,
    texture10 = 0x84CA,
    texture11 = 0x84CB,
    texture12 = 0x84CC,
    texture13 = 0x84CD,
    texture14 = 0x84CE,
    texture15 = 0x84CF,
    texture16 = 0x84D0,
    texture17 = 0x84D1,
    texture18 = 0x84D2,
    texture19 = 0x84D3,
    texture20 = 0x84D4,
    texture21 = 0x84D5,
    texture22 = 0x84D6,
    texture23 = 0x84D7,
    texture24 = 0x84D8,
    texture25 = 0x84D9,
    texture26 = 0x84DA,
    texture27 = 0x84DB,
    texture28 = 0x84DC,
    texture29 = 0x84DD,
    texture30 = 0x84DE,
    texture31 = 0x84DF,
  };

  enum class uniform_type_t : unsigned int {
    int_ = 0x1404,
    float_ = 0x1406,
    float_vec2 = 0x8B50,
    float_vec3 = 0x8B51,
    float_vec4 = 0x8B52,
    int_vec2 = 0x8B53,
    int_vec3 = 0x8B54,
    int_vec4 = 0x8B55,
    bool_ = 0x8B56,
    bool_vec2 = 0x8B57,
    bool_vec3 = 0x8B58,
    bool_vec4 = 0x8B59,
    float_mat2 = 0x8B5A,
    float_mat3 = 0x8B5B,
    float_mat4 = 0x8B5C,
    sampler_2d = 0x8B5E,
    sampler_cube = 0x8B60,
  };

  enum class vertex_attrib_pointer_property_arb_t : unsigned int {
    vertex_attrib_array_pointer = 0x8645,
  };

  enum class vertex_attrib_pointer_type_t : unsigned int {
    byte_ = 0x1400,
    unsigned_byte_ = 0x1401,
    short_ = 0x1402,
    unsigned_short_ = 0x1403,
    float_ = 0x1406,
    fixed = 0x140C,
  };

  enum class vertex_attrib_property_arb_t : unsigned int {
    vertex_attrib_array_enabled = 0x8622,
    vertex_attrib_array_size = 0x8623,
    vertex_attrib_array_stride = 0x8624,
    vertex_attrib_array_type = 0x8625,
    current_vertex_attrib = 0x8626,
    vertex_attrib_array_normalized = 0x886A,
    vertex_attrib_array_buffer_binding = 0x889F,
    vertex_attrib_array_integer = 0x88FD,
    vertex_attrib_array_divisor = 0x88FE,
  };

} // namespace gl2

#endif // #ifndef ENUMS__H
