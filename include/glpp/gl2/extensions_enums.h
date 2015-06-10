/**
 * This file was autogenerated by glgen (https://github.com/seshbot/glgen) on 2015-06-11
 * Command line: src/gen.py assets/gl.xml -p assets/gl-patch.xml -o ../glpp/ --includesubdir include/glpp --sourcesubdir src --namespace gl2 --force --cpp --gl2only
 */

#ifndef EXTENSIONS_ENUMS__H
#define EXTENSIONS_ENUMS__H


#include "../types.h"

namespace gl2 {
  enum class color_table_parameter_p_name_sgi_t : unsigned int {
    color_table_scale = 0x80D6,
    color_table_scale_sgi = 0x80D6,
    color_table_bias = 0x80D7,
    color_table_bias_sgi = 0x80D7,
  };

  enum class color_table_target_sgi_t : unsigned int {
    proxy_texture_color_table_sgi = 0x80BD,
    color_table = 0x80D0,
    post_convolution_color_table = 0x80D1,
    post_color_matrix_color_table = 0x80D2,
    proxy_color_table = 0x80D3,
    proxy_color_table_sgi = 0x80D3,
    proxy_post_convolution_color_table = 0x80D4,
    proxy_post_convolution_color_table_sgi = 0x80D4,
    proxy_post_color_matrix_color_table = 0x80D5,
    proxy_post_color_matrix_color_table_sgi = 0x80D5,
  };

  enum class context_flag_flags_t : unsigned int {
    context_flag_debug_bit = 0x00000002,
    context_flag_robust_access_bit_arb = 0x00000004,
  };
  CREATE_ENUM_BITMASK_TYPE(context_flag_mask_t, context_flag_flags_t)

  enum class convolution_border_mode_ext_t : unsigned int {
    reduce = 0x8016,
    reduce_ext = 0x8016,
  };

  enum class convolution_parameter_ext_t : unsigned int {
    convolution_border_mode = 0x8013,
    convolution_border_mode_ext = 0x8013,
    convolution_filter_scale = 0x8014,
    convolution_filter_scale_ext = 0x8014,
    convolution_filter_bias = 0x8015,
    convolution_filter_bias_ext = 0x8015,
  };

  enum class convolution_target_ext_t : unsigned int {
    convolution_1d = 0x8010,
    convolution_2d = 0x8011,
  };

  enum class ffd_mask_sgix_flags_t : unsigned int {
    texture_deformation_bit_sgix = 0x00000001,
    geometry_deformation_bit_sgix = 0x00000002,
  };
  CREATE_ENUM_BITMASK_TYPE(ffd_mask_sgix_mask_t, ffd_mask_sgix_flags_t)

  enum class ffd_target_sgix_t : unsigned int {
  };

  enum class fragment_light_model_parameter_sgix_t : unsigned int {
    fragment_light_model_local_viewer_sgix = 0x8408,
    fragment_light_model_two_side_sgix = 0x8409,
    fragment_light_model_ambient_sgix = 0x840A,
    fragment_light_model_normal_interpolation_sgix = 0x840B,
  };

  enum class fragment_shader_color_mod_mask_ati_flags_t : unsigned int {
    comp_bit_ati = 0x00000002,
    negate_bit_ati = 0x00000004,
    bias_bit_ati = 0x00000008,
  };
  CREATE_ENUM_BITMASK_TYPE(fragment_shader_color_mod_mask_ati_mask_t, fragment_shader_color_mod_mask_ati_flags_t)

  enum class fragment_shader_dest_mask_ati_flags_t : unsigned int {
    red_bit_ati = 0x00000001,
    green_bit_ati = 0x00000002,
    blue_bit_ati = 0x00000004,
  };
  CREATE_ENUM_BITMASK_TYPE(fragment_shader_dest_mask_ati_mask_t, fragment_shader_dest_mask_ati_flags_t)

  enum class fragment_shader_dest_mod_mask_ati_flags_t : unsigned int {
    _2x_bit_ati = 0x00000001,
    _4x_bit_ati = 0x00000002,
    _8x_bit_ati = 0x00000004,
    half_bit_ati = 0x00000008,
    quarter_bit_ati = 0x00000010,
    eighth_bit_ati = 0x00000020,
    saturate_bit_ati = 0x00000040,
  };
  CREATE_ENUM_BITMASK_TYPE(fragment_shader_dest_mod_mask_ati_mask_t, fragment_shader_dest_mod_mask_ati_flags_t)

  enum class get_color_table_parameter_p_name_sgi_t : unsigned int {
    color_table_scale_sgi = 0x80D6,
    color_table_bias_sgi = 0x80D7,
    color_table_format_sgi = 0x80D8,
    color_table_width_sgi = 0x80D9,
    color_table_red_size_sgi = 0x80DA,
    color_table_green_size_sgi = 0x80DB,
    color_table_blue_size_sgi = 0x80DC,
    color_table_alpha_size_sgi = 0x80DD,
    color_table_luminance_size_sgi = 0x80DE,
    color_table_intensity_size_sgi = 0x80DF,
  };

  enum class get_convolution_parameter_t : unsigned int {
    convolution_border_mode_ext = 0x8013,
    convolution_filter_scale_ext = 0x8014,
    convolution_filter_bias_ext = 0x8015,
    convolution_format_ext = 0x8017,
    convolution_width_ext = 0x8018,
    convolution_height_ext = 0x8019,
    max_convolution_width_ext = 0x801A,
    max_convolution_height_ext = 0x801B,
  };

  enum class get_histogram_parameter_p_name_ext_t : unsigned int {
    histogram_width_ext = 0x8026,
    histogram_format_ext = 0x8027,
    histogram_red_size_ext = 0x8028,
    histogram_green_size_ext = 0x8029,
    histogram_blue_size_ext = 0x802A,
    histogram_alpha_size_ext = 0x802B,
    histogram_luminance_size_ext = 0x802C,
    histogram_sink_ext = 0x802D,
  };

  enum class get_minmax_parameter_p_name_ext_t : unsigned int {
    minmax_format = 0x802F,
    minmax_format_ext = 0x802F,
    minmax_sink = 0x8030,
    minmax_sink_ext = 0x8030,
  };

  enum class histogram_target_ext_t : unsigned int {
    histogram = 0x8024,
    proxy_histogram = 0x8025,
    proxy_histogram_ext = 0x8025,
  };

  enum class light_env_parameter_sgix_t : unsigned int {
    light_env_mode_sgix = 0x8407,
  };

  enum class list_parameter_name_t : unsigned int {
    list_priority_sgix = 0x8182,
  };

  enum class map_buffer_usage_flags_t : unsigned int {
    map_read_bit = 0x0001,
    map_write_bit = 0x0002,
    map_invalidate_range_bit = 0x0004,
    map_invalidate_buffer_bit = 0x0008,
    map_flush_explicit_bit = 0x0010,
    map_unsynchronized_bit = 0x0020,
    map_persistent_bit = 0x0040,
    map_coherent_bit = 0x0080,
    dynamic_storage_bit = 0x0100,
    client_storage_bit = 0x0200,
    sparse_storage_bit_arb = 0x0400,
  };
  CREATE_ENUM_BITMASK_TYPE(map_buffer_usage_mask_t, map_buffer_usage_flags_t)

  enum class map_texture_format_intel_t : unsigned int {
    layout_default_intel = 0,
    layout_linear_intel = 1,
    layout_linear_cpu_cached_intel = 2,
  };

  enum class memory_barrier_flags_t : unsigned int {
    vertex_attrib_array_barrier_bit = 0x00000001,
    vertex_attrib_array_barrier_bit_ext = 0x00000001,
    element_array_barrier_bit = 0x00000002,
    element_array_barrier_bit_ext = 0x00000002,
    uniform_barrier_bit = 0x00000004,
    uniform_barrier_bit_ext = 0x00000004,
    texture_fetch_barrier_bit = 0x00000008,
    texture_fetch_barrier_bit_ext = 0x00000008,
    shader_global_access_barrier_bit_nv = 0x00000010,
    shader_image_access_barrier_bit = 0x00000020,
    shader_image_access_barrier_bit_ext = 0x00000020,
    command_barrier_bit = 0x00000040,
    command_barrier_bit_ext = 0x00000040,
    pixel_buffer_barrier_bit = 0x00000080,
    pixel_buffer_barrier_bit_ext = 0x00000080,
    texture_update_barrier_bit = 0x00000100,
    texture_update_barrier_bit_ext = 0x00000100,
    buffer_update_barrier_bit = 0x00000200,
    buffer_update_barrier_bit_ext = 0x00000200,
    framebuffer_barrier_bit = 0x00000400,
    framebuffer_barrier_bit_ext = 0x00000400,
    transform_feedback_barrier_bit = 0x00000800,
    transform_feedback_barrier_bit_ext = 0x00000800,
    atomic_counter_barrier_bit = 0x00001000,
    atomic_counter_barrier_bit_ext = 0x00001000,
    shader_storage_barrier_bit = 0x00002000,
    client_mapped_buffer_barrier_bit = 0x00004000,
    query_buffer_barrier_bit = 0x00008000,
    all_barrier_bits = 0xFFFFFFFF,
    all_barrier_bits_ext = 0xFFFFFFFF,
  };
  CREATE_ENUM_BITMASK_TYPE(memory_barrier_mask_t, memory_barrier_flags_t)

  enum class minmax_target_ext_t : unsigned int {
    minmax = 0x802E,
  };

  enum class occlusion_query_event_mask_amd_t : unsigned int {
    query_depth_pass_event_bit_amd = 0x00000001,
    query_depth_fail_event_bit_amd = 0x00000002,
    query_stencil_fail_event_bit_amd = 0x00000004,
    query_depth_bounds_fail_event_bit_amd = 0x00000008,
    query_all_event_bits_amd = 0xFFFFFFFF,
  };

  enum class path_rendering_mask_nv_flags_t : unsigned int {
    font_x_min_bounds_bit_nv = 0x00010000,
    font_y_min_bounds_bit_nv = 0x00020000,
    font_x_max_bounds_bit_nv = 0x00040000,
    font_y_max_bounds_bit_nv = 0x00080000,
    font_units_per_em_bit_nv = 0x00100000,
    font_ascender_bit_nv = 0x00200000,
    font_descender_bit_nv = 0x00400000,
    font_height_bit_nv = 0x00800000,
    bold_bit_nv = 0x01,
    glyph_width_bit_nv = 0x01,
    font_max_advance_width_bit_nv = 0x01000000,
    glyph_height_bit_nv = 0x02,
    italic_bit_nv = 0x02,
    font_max_advance_height_bit_nv = 0x02000000,
    glyph_horizontal_bearing_x_bit_nv = 0x04,
    font_underline_position_bit_nv = 0x04000000,
    glyph_horizontal_bearing_y_bit_nv = 0x08,
    font_underline_thickness_bit_nv = 0x08000000,
    glyph_horizontal_bearing_advance_bit_nv = 0x10,
    glyph_has_kerning_bit_nv = 0x100,
    font_has_kerning_bit_nv = 0x10000000,
    glyph_vertical_bearing_x_bit_nv = 0x20,
    font_num_glyph_indices_bit_nv = 0x20000000,
    glyph_vertical_bearing_y_bit_nv = 0x40,
    glyph_vertical_bearing_advance_bit_nv = 0x80,
  };
  CREATE_ENUM_BITMASK_TYPE(path_rendering_mask_nv_mask_t, path_rendering_mask_nv_flags_t)

  enum class performance_query_caps_mask_intel_flags_t : unsigned int {
    perfquery_single_context_intel = 0x00000000,
    perfquery_global_context_intel = 0x00000001,
  };
  CREATE_ENUM_BITMASK_TYPE(performance_query_caps_mask_intel_mask_t, performance_query_caps_mask_intel_flags_t)

  enum class pixel_store_resample_mode_t : unsigned int {
    resample_replicate_sgix = 0x842E,
    resample_zero_fill_sgix = 0x842F,
    resample_decimate_sgix = 0x8430,
  };

  enum class pixel_store_subsample_rate_t : unsigned int {
    pixel_subsample_4444_sgix = 0x85A2,
    pixel_subsample_2424_sgix = 0x85A3,
    pixel_subsample_4242_sgix = 0x85A4,
  };

  enum class pixel_tex_gen_parameter_name_sgis_t : unsigned int {
    pixel_fragment_rgb_source_sgis = 0x8354,
    pixel_fragment_alpha_source_sgis = 0x8355,
  };

  enum class sample_pattern_sgis_t : unsigned int {
    _1pass_ext = 0x80A1,
    _1pass_sgis = 0x80A1,
    _2pass_0_ext = 0x80A2,
    _2pass_0_sgis = 0x80A2,
    _2pass_1_ext = 0x80A3,
    _2pass_1_sgis = 0x80A3,
    _4pass_0_ext = 0x80A4,
    _4pass_0_sgis = 0x80A4,
    _4pass_1_ext = 0x80A5,
    _4pass_1_sgis = 0x80A5,
    _4pass_2_ext = 0x80A6,
    _4pass_2_sgis = 0x80A6,
    _4pass_3_ext = 0x80A7,
    _4pass_3_sgis = 0x80A7,
  };

  enum class separable_target_ext_t : unsigned int {
    separable_2d = 0x8012,
  };

  enum class sync_object_flags_t : unsigned int {
    sync_flush_commands_bit = 0x00000001,
  };
  CREATE_ENUM_BITMASK_TYPE(sync_object_mask_t, sync_object_flags_t)

  enum class texture_filter_func_sgis_t : unsigned int {
    filter4_sgis = 0x8146,
  };

  enum class texture_storage_mask_amd_flags_t : unsigned int {
    texture_storage_sparse_bit_amd = 0x00000001,
  };
  CREATE_ENUM_BITMASK_TYPE(texture_storage_mask_amd_mask_t, texture_storage_mask_amd_flags_t)

  enum class use_program_stage_flags_t : unsigned int {
    vertex_shader_bit = 0x00000001,
    vertex_shader_bit_ext = 0x00000001,
    fragment_shader_bit = 0x00000002,
    fragment_shader_bit_ext = 0x00000002,
    geometry_shader_bit = 0x00000004,
    geometry_shader_bit_ext = 0x00000004,
    geometry_shader_bit_oes = 0x00000004,
    tess_control_shader_bit = 0x00000008,
    tess_control_shader_bit_ext = 0x00000008,
    tess_control_shader_bit_oes = 0x00000008,
    tess_evaluation_shader_bit = 0x00000010,
    tess_evaluation_shader_bit_ext = 0x00000010,
    tess_evaluation_shader_bit_oes = 0x00000010,
    compute_shader_bit = 0x00000020,
    all_shader_bits = 0xFFFFFFFF,
    all_shader_bits_ext = 0xFFFFFFFF,
  };
  CREATE_ENUM_BITMASK_TYPE(use_program_stage_mask_t, use_program_stage_flags_t)

  enum class vertex_hints_mask_pgi_flags_t : unsigned int {
    vertex23_bit_pgi = 0x00000004,
    vertex4_bit_pgi = 0x00000008,
    color3_bit_pgi = 0x00010000,
    color4_bit_pgi = 0x00020000,
    edgeflag_bit_pgi = 0x00040000,
    index_bit_pgi = 0x00080000,
    mat_ambient_bit_pgi = 0x00100000,
    mat_ambient_and_diffuse_bit_pgi = 0x00200000,
    mat_diffuse_bit_pgi = 0x00400000,
    mat_emission_bit_pgi = 0x00800000,
    mat_color_indexes_bit_pgi = 0x01000000,
    mat_shininess_bit_pgi = 0x02000000,
    mat_specular_bit_pgi = 0x04000000,
    normal_bit_pgi = 0x08000000,
    texcoord1_bit_pgi = 0x10000000,
    texcoord2_bit_pgi = 0x20000000,
    texcoord3_bit_pgi = 0x40000000,
    texcoord4_bit_pgi = 0x80000000,
  };
  CREATE_ENUM_BITMASK_TYPE(vertex_hints_mask_pgi_mask_t, vertex_hints_mask_pgi_flags_t)

  enum class framebuffer_attachment_t : unsigned int {
    depth_stencil_attachment = 0x821A,
    color_attachment0 = 0x8CE0,
    color_attachment1 = 0x8CE1,
    color_attachment2 = 0x8CE2,
    color_attachment3 = 0x8CE3,
    color_attachment4 = 0x8CE4,
    color_attachment5 = 0x8CE5,
    color_attachment6 = 0x8CE6,
    color_attachment7 = 0x8CE7,
    color_attachment8 = 0x8CE8,
    color_attachment9 = 0x8CE9,
    color_attachment10 = 0x8CEA,
    color_attachment11 = 0x8CEB,
    color_attachment12 = 0x8CEC,
    color_attachment13 = 0x8CED,
    color_attachment14 = 0x8CEE,
    color_attachment15 = 0x8CEF,
    depth_attachment = 0x8D00,
    stencil_attachment = 0x8D20,
  };

  enum class framebuffer_status_t : unsigned int {
    framebuffer_undefined = 0x8219,
    framebuffer_complete = 0x8CD5,
    framebuffer_incomplete_attachment = 0x8CD6,
    framebuffer_incomplete_missing_attachment = 0x8CD7,
    framebuffer_incomplete_draw_buffer = 0x8CDB,
    framebuffer_unsupported = 0x8CDD,
    framebuffer_incomplete_multisample = 0x8D56,
  };

  enum class framebuffer_target_t : unsigned int {
    read_framebuffer = 0x8CA8,
    draw_framebuffer = 0x8CA9,
    framebuffer = 0x8D40,
  };

  enum class precision_type_t : unsigned int {
    low_float = 0x8DF0,
    medium_float = 0x8DF1,
    high_float = 0x8DF2,
    low_int = 0x8DF3,
    medium_int = 0x8DF4,
    high_int = 0x8DF5,
  };

  enum class renderbuffer_target_t : unsigned int {
    renderbuffer = 0x8D41,
  };

} // namespace gl2

#endif // #ifndef EXTENSIONS_ENUMS__H
