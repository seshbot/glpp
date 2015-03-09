/**
 * This file was autogenerated by glgen (https://github.com/seshbot/glgen) on 2015-03-10
 * Command line: src/gen.py assets/gl.xml -p assets/gl-patch.xml -o ../glpp/ --includesubdir include/glpp --sourcesubdir src --namespace gles2 --force --cpp --es2only --synth
 */

#ifndef EXTENSIONS_ENUMS__H
#define EXTENSIONS_ENUMS__H


#include "../types.h"

namespace gles2 {
  enum class buffer_bit_qcom_flags_t : unsigned int {
    color_buffer_bit0_qcom = 0x00000001,
    color_buffer_bit1_qcom = 0x00000002,
    color_buffer_bit2_qcom = 0x00000004,
    color_buffer_bit3_qcom = 0x00000008,
    color_buffer_bit4_qcom = 0x00000010,
    color_buffer_bit5_qcom = 0x00000020,
    color_buffer_bit6_qcom = 0x00000040,
    color_buffer_bit7_qcom = 0x00000080,
    depth_buffer_bit0_qcom = 0x00000100,
    depth_buffer_bit1_qcom = 0x00000200,
    depth_buffer_bit2_qcom = 0x00000400,
    depth_buffer_bit3_qcom = 0x00000800,
    depth_buffer_bit4_qcom = 0x00001000,
    depth_buffer_bit5_qcom = 0x00002000,
    depth_buffer_bit6_qcom = 0x00004000,
    depth_buffer_bit7_qcom = 0x00008000,
    stencil_buffer_bit0_qcom = 0x00010000,
    stencil_buffer_bit1_qcom = 0x00020000,
    stencil_buffer_bit2_qcom = 0x00040000,
    stencil_buffer_bit3_qcom = 0x00080000,
    stencil_buffer_bit4_qcom = 0x00100000,
    stencil_buffer_bit5_qcom = 0x00200000,
    stencil_buffer_bit6_qcom = 0x00400000,
    stencil_buffer_bit7_qcom = 0x00800000,
    multisample_buffer_bit0_qcom = 0x01000000,
    multisample_buffer_bit1_qcom = 0x02000000,
    multisample_buffer_bit2_qcom = 0x04000000,
    multisample_buffer_bit3_qcom = 0x08000000,
    multisample_buffer_bit4_qcom = 0x10000000,
    multisample_buffer_bit5_qcom = 0x20000000,
    multisample_buffer_bit6_qcom = 0x40000000,
    multisample_buffer_bit7_qcom = 0x80000000,
  };
  CREATE_ENUM_BITMASK_TYPE(buffer_bit_qcom_mask_t, buffer_bit_qcom_flags_t)

  enum class context_flag_flags_t : unsigned int {
    context_flag_debug_bit_khr = 0x00000002,
  };
  CREATE_ENUM_BITMASK_TYPE(context_flag_mask_t, context_flag_flags_t)

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

  enum class map_buffer_usage_flags_t : unsigned int {
    map_read_bit_ext = 0x0001,
    map_write_bit_ext = 0x0002,
    map_invalidate_range_bit_ext = 0x0004,
    map_invalidate_buffer_bit_ext = 0x0008,
    map_flush_explicit_bit_ext = 0x0010,
    map_unsynchronized_bit_ext = 0x0020,
  };
  CREATE_ENUM_BITMASK_TYPE(map_buffer_usage_mask_t, map_buffer_usage_flags_t)

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
    italic_bit_nv = 0x02,
    glyph_height_bit_nv = 0x02,
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

  enum class pixel_copy_type_t : unsigned int {
    color_ext = 0x1800,
    depth_ext = 0x1801,
    stencil_ext = 0x1802,
  };

  enum class sync_object_flags_t : unsigned int {
    sync_flush_commands_bit_apple = 0x00000001,
  };
  CREATE_ENUM_BITMASK_TYPE(sync_object_mask_t, sync_object_flags_t)

  enum class use_program_stage_flags_t : unsigned int {
    vertex_shader_bit = 0x00000001,
    vertex_shader_bit_ext = 0x00000001,
    fragment_shader_bit = 0x00000002,
    fragment_shader_bit_ext = 0x00000002,
    geometry_shader_bit_oes = 0x00000004,
    geometry_shader_bit = 0x00000004,
    geometry_shader_bit_ext = 0x00000004,
    tess_control_shader_bit_ext = 0x00000008,
    tess_control_shader_bit_oes = 0x00000008,
    tess_control_shader_bit = 0x00000008,
    tess_evaluation_shader_bit = 0x00000010,
    tess_evaluation_shader_bit_oes = 0x00000010,
    tess_evaluation_shader_bit_ext = 0x00000010,
    compute_shader_bit = 0x00000020,
    all_shader_bits = 0xFFFFFFFF,
    all_shader_bits_ext = 0xFFFFFFFF,
  };
  CREATE_ENUM_BITMASK_TYPE(use_program_stage_mask_t, use_program_stage_flags_t)

} // namespace gles2

#endif // #ifndef EXTENSIONS_ENUMS__H
