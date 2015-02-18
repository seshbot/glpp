// manually generated
// this file is included after 'GLES2/gl2ext.h' to ensure only the extensions outlined
// on https://code.google.com/p/angleproject/wiki/ExtensionSupport are enabled

//
// First unset ALL known ES2 extensions
//

#ifndef ANGLE_EXTENSION_MACROS__H
#define ANGLE_EXTENSION_MACROS__H

#if ANGLE_BUILD


// some ANGLE extensions dont appear to conform to the spec
// e.g., GL_KHR_debug - spec says in GLES2 all commands must end in _KHR but ANGLEs' do not
// e.g., GL_EXT_separate_shader_objects - ANGLE does not provide all the required commands (e.g., glProgramUniformMatrix4x2fvEXT)


#ifdef GL_NV_non_square_matrices
#undef GL_NV_non_square_matrices
#endif

#ifdef GL_OES_packed_depth_stencil
#undef GL_OES_packed_depth_stencil
#endif

#ifdef GL_QCOM_perfmon_global_mode
#undef GL_QCOM_perfmon_global_mode
#endif

#ifdef GL_QCOM_tiled_rendering
#undef GL_QCOM_tiled_rendering
#endif

#ifdef GL_OES_texture_half_float_linear
#undef GL_OES_texture_half_float_linear
#endif

#ifdef GL_KHR_blend_equation_advanced_coherent
#undef GL_KHR_blend_equation_advanced_coherent
#endif

#ifdef GL_NV_fence
#undef GL_NV_fence
#endif

#ifdef GL_NV_texture_border_clamp
#undef GL_NV_texture_border_clamp
#endif

#ifdef GL_IMG_texture_compression_pvrtc
#undef GL_IMG_texture_compression_pvrtc
#endif

#ifdef GL_OES_EGL_image
#undef GL_OES_EGL_image
#endif

#ifdef GL_NV_sRGB_formats
#undef GL_NV_sRGB_formats
#endif

#ifdef GL_EXT_texture_compression_s3tc
#undef GL_EXT_texture_compression_s3tc
#endif

#ifdef GL_OES_texture_half_float
#undef GL_OES_texture_half_float
#endif

#ifdef GL_EXT_sRGB
#undef GL_EXT_sRGB
#endif

#ifdef GL_QCOM_writeonly_rendering
#undef GL_QCOM_writeonly_rendering
#endif

#ifdef GL_DMP_program_binary
#undef GL_DMP_program_binary
#endif

#ifdef GL_EXT_draw_buffers_indexed
#undef GL_EXT_draw_buffers_indexed
#endif

#ifdef GL_NV_read_depth_stencil
#undef GL_NV_read_depth_stencil
#endif

#ifdef GL_ANGLE_translated_shader_source
#undef GL_ANGLE_translated_shader_source
#endif

#ifdef GL_EXT_multisampled_render_to_texture
#undef GL_EXT_multisampled_render_to_texture
#endif

#ifdef GL_NV_coverage_sample
#undef GL_NV_coverage_sample
#endif

#ifdef GL_OES_vertex_array_object
#undef GL_OES_vertex_array_object
#endif

#ifdef GL_AMD_program_binary_Z400
#undef GL_AMD_program_binary_Z400
#endif

#ifdef GL_APPLE_color_buffer_packed_float
#undef GL_APPLE_color_buffer_packed_float
#endif

#ifdef GL_EXT_robustness
#undef GL_EXT_robustness
#endif

#ifdef GL_OES_fbo_render_mipmap
#undef GL_OES_fbo_render_mipmap
#endif

#ifdef GL_KHR_robustness
#undef GL_KHR_robustness
#endif

#ifdef GL_OES_compressed_ETC1_RGB8_sub_texture
#undef GL_OES_compressed_ETC1_RGB8_sub_texture
#endif

#ifdef GL_ANGLE_texture_usage
#undef GL_ANGLE_texture_usage
#endif

#ifdef GL_EXT_gpu_shader5
#undef GL_EXT_gpu_shader5
#endif

#ifdef GL_OES_shader_io_blocks
#undef GL_OES_shader_io_blocks
#endif

#ifdef GL_FJ_shader_binary_GCCSO
#undef GL_FJ_shader_binary_GCCSO
#endif

#ifdef GL_EXT_disjoint_timer_query
#undef GL_EXT_disjoint_timer_query
#endif

#ifdef GL_OES_compressed_paletted_texture
#undef GL_OES_compressed_paletted_texture
#endif

#ifdef GL_EXT_shader_texture_lod
#undef GL_EXT_shader_texture_lod
#endif

#ifdef GL_OES_standard_derivatives
#undef GL_OES_standard_derivatives
#endif

#ifdef GL_OES_texture_float
#undef GL_OES_texture_float
#endif

#ifdef GL_NV_read_buffer_front
#undef GL_NV_read_buffer_front
#endif

#ifdef GL_NV_fbo_color_attachments
#undef GL_NV_fbo_color_attachments
#endif

#ifdef GL_EXT_draw_elements_base_vertex
#undef GL_EXT_draw_elements_base_vertex
#endif

#ifdef GL_EXT_primitive_bounding_box
#undef GL_EXT_primitive_bounding_box
#endif

#ifdef GL_IMG_read_format
#undef GL_IMG_read_format
#endif

#ifdef GL_OES_sample_shading
#undef GL_OES_sample_shading
#endif

#ifdef GL_ANDROID_extension_pack_es31a
#undef GL_ANDROID_extension_pack_es31a
#endif

#ifdef GL_EXT_draw_instanced
#undef GL_EXT_draw_instanced
#endif

#ifdef GL_NV_texture_compression_s3tc_update
#undef GL_NV_texture_compression_s3tc_update
#endif

#ifdef GL_OES_texture_float_linear
#undef GL_OES_texture_float_linear
#endif

#ifdef GL_OES_fragment_precision_high
#undef GL_OES_fragment_precision_high
#endif

#ifdef GL_EXT_multi_draw_arrays
#undef GL_EXT_multi_draw_arrays
#endif

#ifdef GL_OES_texture_npot
#undef GL_OES_texture_npot
#endif

#ifdef GL_EXT_texture_compression_dxt1
#undef GL_EXT_texture_compression_dxt1
#endif

#ifdef GL_APPLE_clip_distance
#undef GL_APPLE_clip_distance
#endif

#ifdef GL_ANGLE_depth_texture
#undef GL_ANGLE_depth_texture
#endif

#ifdef GL_KHR_robust_buffer_access_behavior
#undef GL_KHR_robust_buffer_access_behavior
#endif

#ifdef GL_EXT_render_snorm
#undef GL_EXT_render_snorm
#endif

#ifdef GL_KHR_texture_compression_astc_ldr
#undef GL_KHR_texture_compression_astc_ldr
#endif

#ifdef GL_EXT_debug_marker
#undef GL_EXT_debug_marker
#endif

#ifdef GL_OES_geometry_shader
#undef GL_OES_geometry_shader
#endif

#ifdef GL_OES_tessellation_shader
#undef GL_OES_tessellation_shader
#endif

#ifdef GL_OES_texture_view
#undef GL_OES_texture_view
#endif

#ifdef GL_OES_texture_compression_astc
#undef GL_OES_texture_compression_astc
#endif

#ifdef GL_EXT_blend_minmax
#undef GL_EXT_blend_minmax
#endif

#ifdef GL_ARM_shader_framebuffer_fetch_depth_stencil
#undef GL_ARM_shader_framebuffer_fetch_depth_stencil
#endif

#ifdef GL_OES_texture_buffer
#undef GL_OES_texture_buffer
#endif

#ifdef GL_EXT_tessellation_point_size
#undef GL_EXT_tessellation_point_size
#endif

#ifdef GL_EXT_color_buffer_half_float
#undef GL_EXT_color_buffer_half_float
#endif

#ifdef GL_OES_get_program_binary
#undef GL_OES_get_program_binary
#endif

#ifdef GL_OES_geometry_point_size
#undef GL_OES_geometry_point_size
#endif

#ifdef GL_APPLE_framebuffer_multisample
#undef GL_APPLE_framebuffer_multisample
#endif

#ifdef GL_OES_stencil4
#undef GL_OES_stencil4
#endif

#ifdef GL_NV_copy_buffer
#undef GL_NV_copy_buffer
#endif

#ifdef GL_EXT_base_instance
#undef GL_EXT_base_instance
#endif

#ifdef GL_EXT_texture_view
#undef GL_EXT_texture_view
#endif

#ifdef GL_OES_vertex_half_float
#undef GL_OES_vertex_half_float
#endif

#ifdef GL_NV_framebuffer_blit
#undef GL_NV_framebuffer_blit
#endif

#ifdef GL_EXT_geometry_point_size
#undef GL_EXT_geometry_point_size
#endif

#ifdef GL_OES_stencil1
#undef GL_OES_stencil1
#endif

#ifdef GL_QCOM_alpha_test
#undef GL_QCOM_alpha_test
#endif

#ifdef GL_EXT_shader_implicit_conversions
#undef GL_EXT_shader_implicit_conversions
#endif

#ifdef GL_INTEL_performance_query
#undef GL_INTEL_performance_query
#endif

#ifdef GL_KHR_debug
#undef GL_KHR_debug
#endif

#ifdef GL_OES_rgb8_rgba8
#undef GL_OES_rgb8_rgba8
#endif

#ifdef GL_OES_texture_border_clamp
#undef GL_OES_texture_border_clamp
#endif

#ifdef GL_ANGLE_pack_reverse_row_order
#undef GL_ANGLE_pack_reverse_row_order
#endif

#ifdef GL_EXT_occlusion_query_boolean
#undef GL_EXT_occlusion_query_boolean
#endif

#ifdef GL_NV_bindless_texture
#undef GL_NV_bindless_texture
#endif

#ifdef GL_EXT_multi_draw_indirect
#undef GL_EXT_multi_draw_indirect
#endif

#ifdef GL_OES_tessellation_point_size
#undef GL_OES_tessellation_point_size
#endif

#ifdef GL_OES_depth_texture
#undef GL_OES_depth_texture
#endif

#ifdef GL_QCOM_binning_control
#undef GL_QCOM_binning_control
#endif

#ifdef GL_KHR_texture_compression_astc_hdr
#undef GL_KHR_texture_compression_astc_hdr
#endif

#ifdef GL_EXT_instanced_arrays
#undef GL_EXT_instanced_arrays
#endif

#ifdef GL_KHR_blend_equation_advanced
#undef GL_KHR_blend_equation_advanced
#endif

#ifdef GL_OES_surfaceless_context
#undef GL_OES_surfaceless_context
#endif

#ifdef GL_APPLE_sync
#undef GL_APPLE_sync
#endif

#ifdef GL_IMG_program_binary
#undef GL_IMG_program_binary
#endif

#ifdef GL_EXT_texture_cube_map_array
#undef GL_EXT_texture_cube_map_array
#endif

#ifdef GL_QCOM_driver_control
#undef GL_QCOM_driver_control
#endif

#ifdef GL_DMP_shader_binary
#undef GL_DMP_shader_binary
#endif

#ifdef GL_ANGLE_program_binary
#undef GL_ANGLE_program_binary
#endif

#ifdef GL_EXT_unpack_subimage
#undef GL_EXT_unpack_subimage
#endif

#ifdef GL_ANGLE_texture_compression_dxt3
#undef GL_ANGLE_texture_compression_dxt3
#endif

#ifdef GL_ANGLE_texture_compression_dxt5
#undef GL_ANGLE_texture_compression_dxt5
#endif

#ifdef GL_EXT_read_format_bgra
#undef GL_EXT_read_format_bgra
#endif

#ifdef GL_OES_compressed_ETC1_RGB8_texture
#undef GL_OES_compressed_ETC1_RGB8_texture
#endif

#ifdef GL_OES_copy_image
#undef GL_OES_copy_image
#endif

#ifdef GL_EXT_pvrtc_sRGB
#undef GL_EXT_pvrtc_sRGB
#endif

#ifdef GL_QCOM_extended_get2
#undef GL_QCOM_extended_get2
#endif

#ifdef GL_EXT_shader_pixel_local_storage
#undef GL_EXT_shader_pixel_local_storage
#endif

#ifdef GL_NV_shadow_samplers_cube
#undef GL_NV_shadow_samplers_cube
#endif

#ifdef GL_APPLE_texture_max_level
#undef GL_APPLE_texture_max_level
#endif

#ifdef GL_EXT_shader_framebuffer_fetch
#undef GL_EXT_shader_framebuffer_fetch
#endif

#ifdef GL_OES_shader_multisample_interpolation
#undef GL_OES_shader_multisample_interpolation
#endif

#ifdef GL_EXT_shadow_samplers
#undef GL_EXT_shadow_samplers
#endif

#ifdef GL_IMG_shader_binary
#undef GL_IMG_shader_binary
#endif

#ifdef GL_NV_viewport_array
#undef GL_NV_viewport_array
#endif

#ifdef GL_OES_depth32
#undef GL_OES_depth32
#endif

#ifdef GL_EXT_separate_shader_objects
#undef GL_EXT_separate_shader_objects
#endif

#ifdef GL_EXT_shader_io_blocks
#undef GL_EXT_shader_io_blocks
#endif

#ifdef GL_NV_framebuffer_multisample
#undef GL_NV_framebuffer_multisample
#endif

#ifdef GL_EXT_geometry_shader
#undef GL_EXT_geometry_shader
#endif

#ifdef GL_NV_read_stencil
#undef GL_NV_read_stencil
#endif

#ifdef GL_NV_path_rendering
#undef GL_NV_path_rendering
#endif

#ifdef GL_OES_texture_storage_multisample_2d_array
#undef GL_OES_texture_storage_multisample_2d_array
#endif

#ifdef GL_OES_draw_buffers_indexed
#undef GL_OES_draw_buffers_indexed
#endif

#ifdef GL_VIV_shader_binary
#undef GL_VIV_shader_binary
#endif

#ifdef GL_NV_draw_buffers
#undef GL_NV_draw_buffers
#endif

#ifdef GL_OES_vertex_type_10_10_10_2
#undef GL_OES_vertex_type_10_10_10_2
#endif

#ifdef GL_APPLE_rgb_422
#undef GL_APPLE_rgb_422
#endif

#ifdef GL_EXT_texture_sRGB_decode
#undef GL_EXT_texture_sRGB_decode
#endif

#ifdef GL_NV_texture_npot_2D_mipmap
#undef GL_NV_texture_npot_2D_mipmap
#endif

#ifdef GL_OES_sample_variables
#undef GL_OES_sample_variables
#endif

#ifdef GL_NV_blend_equation_advanced
#undef GL_NV_blend_equation_advanced
#endif

#ifdef GL_EXT_draw_buffers
#undef GL_EXT_draw_buffers
#endif

#ifdef GL_EXT_debug_label
#undef GL_EXT_debug_label
#endif

#ifdef GL_ARM_shader_framebuffer_fetch
#undef GL_ARM_shader_framebuffer_fetch
#endif

#ifdef GL_NV_conditional_render
#undef GL_NV_conditional_render
#endif

#ifdef GL_EXT_texture_filter_anisotropic
#undef GL_EXT_texture_filter_anisotropic
#endif

#ifdef GL_ANGLE_framebuffer_multisample
#undef GL_ANGLE_framebuffer_multisample
#endif

#ifdef GL_NV_internalformat_sample_query
#undef GL_NV_internalformat_sample_query
#endif

#ifdef GL_OES_depth24
#undef GL_OES_depth24
#endif

#ifdef GL_NV_instanced_arrays
#undef GL_NV_instanced_arrays
#endif

#ifdef GL_EXT_texture_storage
#undef GL_EXT_texture_storage
#endif

#ifdef GL_OES_gpu_shader5
#undef GL_OES_gpu_shader5
#endif

#ifdef GL_OES_required_internalformat
#undef GL_OES_required_internalformat
#endif

#ifdef GL_EXT_sRGB_write_control
#undef GL_EXT_sRGB_write_control
#endif

#ifdef GL_EXT_copy_image
#undef GL_EXT_copy_image
#endif

#ifdef GL_AMD_compressed_3DC_texture
#undef GL_AMD_compressed_3DC_texture
#endif

#ifdef GL_OES_element_index_uint
#undef GL_OES_element_index_uint
#endif

#ifdef GL_IMG_texture_compression_pvrtc2
#undef GL_IMG_texture_compression_pvrtc2
#endif

#ifdef GL_EXT_map_buffer_range
#undef GL_EXT_map_buffer_range
#endif

#ifdef GL_OES_mapbuffer
#undef GL_OES_mapbuffer
#endif

#ifdef GL_NV_draw_instanced
#undef GL_NV_draw_instanced
#endif

#ifdef GL_NV_explicit_attrib_location
#undef GL_NV_explicit_attrib_location
#endif

#ifdef GL_NV_image_formats
#undef GL_NV_image_formats
#endif

#ifdef GL_OES_EGL_image_external
#undef GL_OES_EGL_image_external
#endif

#ifdef GL_APPLE_texture_format_BGRA8888
#undef GL_APPLE_texture_format_BGRA8888
#endif

#ifdef GL_AMD_performance_monitor
#undef GL_AMD_performance_monitor
#endif

#ifdef GL_NV_shadow_samplers_array
#undef GL_NV_shadow_samplers_array
#endif

#ifdef GL_IMG_multisampled_render_to_texture
#undef GL_IMG_multisampled_render_to_texture
#endif

#ifdef GL_NV_depth_nonlinear
#undef GL_NV_depth_nonlinear
#endif

#ifdef GL_OES_draw_elements_base_vertex
#undef GL_OES_draw_elements_base_vertex
#endif

#ifdef GL_NV_shader_noperspective_interpolation
#undef GL_NV_shader_noperspective_interpolation
#endif

#ifdef GL_EXT_texture_format_BGRA8888
#undef GL_EXT_texture_format_BGRA8888
#endif

#ifdef GL_OES_texture_cube_map_array
#undef GL_OES_texture_cube_map_array
#endif

#ifdef GL_APPLE_copy_texture_levels
#undef GL_APPLE_copy_texture_levels
#endif

#ifdef GL_ARM_mali_shader_binary
#undef GL_ARM_mali_shader_binary
#endif

#ifdef GL_EXT_multiview_draw_buffers
#undef GL_EXT_multiview_draw_buffers
#endif

#ifdef GL_OES_texture_stencil8
#undef GL_OES_texture_stencil8
#endif

#ifdef GL_QCOM_extended_get
#undef GL_QCOM_extended_get
#endif

#ifdef GL_EXT_shader_integer_mix
#undef GL_EXT_shader_integer_mix
#endif

#ifdef GL_EXT_texture_norm16
#undef GL_EXT_texture_norm16
#endif

#ifdef GL_EXT_texture_rg
#undef GL_EXT_texture_rg
#endif

#ifdef GL_EXT_texture_border_clamp
#undef GL_EXT_texture_border_clamp
#endif

#ifdef GL_OES_shader_image_atomic
#undef GL_OES_shader_image_atomic
#endif

#ifdef GL_ANGLE_framebuffer_blit
#undef GL_ANGLE_framebuffer_blit
#endif

#ifdef GL_NV_blend_equation_advanced_coherent
#undef GL_NV_blend_equation_advanced_coherent
#endif

#ifdef GL_OES_texture_3D
#undef GL_OES_texture_3D
#endif

#ifdef GL_ARM_rgba8
#undef GL_ARM_rgba8
#endif

#ifdef GL_EXT_tessellation_shader
#undef GL_EXT_tessellation_shader
#endif

#ifdef GL_OES_primitive_bounding_box
#undef GL_OES_primitive_bounding_box
#endif

#ifdef GL_NV_read_buffer
#undef GL_NV_read_buffer
#endif

#ifdef GL_NV_generate_mipmap_sRGB
#undef GL_NV_generate_mipmap_sRGB
#endif

#ifdef GL_ARM_mali_program_binary
#undef GL_ARM_mali_program_binary
#endif

#ifdef GL_OES_EGL_image_external_essl3
#undef GL_OES_EGL_image_external_essl3
#endif

#ifdef GL_AMD_compressed_ATC_texture
#undef GL_AMD_compressed_ATC_texture
#endif

#ifdef GL_EXT_texture_type_2_10_10_10_REV
#undef GL_EXT_texture_type_2_10_10_10_REV
#endif

#ifdef GL_ANGLE_instanced_arrays
#undef GL_ANGLE_instanced_arrays
#endif

#ifdef GL_EXT_discard_framebuffer
#undef GL_EXT_discard_framebuffer
#endif

#ifdef GL_NV_read_depth
#undef GL_NV_read_depth
#endif

#ifdef GL_KHR_context_flush_control
#undef GL_KHR_context_flush_control
#endif

#ifdef GL_APPLE_texture_packed_float
#undef GL_APPLE_texture_packed_float
#endif

#ifdef GL_EXT_texture_buffer
#undef GL_EXT_texture_buffer
#endif

//
// re-enable the known ANGLE extensions
//

#define GL_OES_texture_npot 1

#define GL_EXT_texture_compression_dxt1 1

#define EGL_ANGLE_d3d_share_handle_client_buffer 1

#define GL_OES_element_index_uint 1

#define GL_ANGLE_texture_compression_dxt5 1

#define GL_OES_texture_half_float_linear 1

#define GL_OES_standard_derivatives 1

#define GL_EXT_read_format_bgra 1

#define GL_ANGLE_depth_texture 1

#define GL_OES_texture_half_float 1

#define GL_OES_rgb8_rgba8 1

#define GL_OES_packed_depth_stencil 1

#define EGL_ANGLE_surface_d3d_texture_2d_share_handle 1

#define GL_OES_get_program_binary 1

#define GL_ANGLE_translated_shader_source 1

#define GL_EXT_texture_format_BGRA8888 1

#define GL_OES_texture_float 1

#define EGL_NV_post_sub_buffer 1

#define GL_ANGLE_texture_compression_dxt3 1

#define GL_ANGLE_framebuffer_blit 1

#define GL_EXT_robustness 1

#define GL_ANGLE_pack_reverse_row_order 1

#define GL_EXT_occlusion_query_boolean 1

#define GL_ANGLE_texture_usage 1

#define EGL_ANGLE_query_surface_pointer 1

#define EGL_EXT_create_context_robustness 1

#define GL_NV_fence 1

#define EGL_ANGLE_software_display 1

#define GL_OES_texture_float_linear 1

#define GL_EXT_texture_filter_anisotropic 1

#define GL_ANGLE_framebuffer_multisample 1

// although this is listed in the ANGLE page, the DLL does not have the relevant symbols
// #define GL_EXT_texture_storage 1

#define GL_ANGLE_instanced_arrays 1

#endif // ANGLE_BUILD
#endif // #ifndef ANGLE_EXTENSION_MACROS__H
