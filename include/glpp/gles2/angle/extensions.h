/**
 * This file was autogenerated by glgen (https://github.com/seshbot/glgen) on 2015-06-16
 * Command line: src/gen.py assets/gl.xml -p assets/gl-patch.xml -o ../glpp/ --includesubdir include/glpp --sourcesubdir src --namespace gles2:angle --force --cpp --es2only --extensionsimportfile assets/extensions_angle_dx11.txt --extensionsoutputfilename extensions
 */

#ifndef GLES2_ANGLE_EXTENSIONS__H
#define GLES2_ANGLE_EXTENSIONS__H


#include "../../types.h"
#include "../enums.h"
#include "../extensions_enums.h"

namespace gles2 {
namespace angle {

   /**
    * command aliases
    */
  
   /**
    * Required by extensions:
    *  - GL_ANGLE_framebuffer_blit (gles2)
    */
    void blit_framebuffer(gl::int_t srcX0, gl::int_t srcY0, gl::int_t srcX1, gl::int_t srcY1, gl::int_t dstX0, gl::int_t dstY0, gl::int_t dstX1, gl::int_t dstY1, gl::bitmask<clear_buffer_flags_t> mask, gl::enum_t filter);

   /**
    * Required by extensions:
    *  - GL_ANGLE_framebuffer_multisample (gles2)
    */
    void renderbuffer_storage_multisample(renderbuffer_target_t target, gl::sizei_t samples, internal_format_t internalformat, gl::sizei_t width, gl::sizei_t height);

   /**
    * Required by extensions:
    *  - GL_ANGLE_instanced_arrays (gles2)
    */
    void draw_arrays_instanced(primitive_type_t mode, gl::int_t first, gl::sizei_t count, gl::sizei_t primcount);

   /**
    * Required by extensions:
    *  - GL_ANGLE_instanced_arrays (gles2)
    */
    void draw_elements_instanced(primitive_type_t mode, gl::sizei_t count, draw_elements_type_t type, const  void * indices, gl::sizei_t primcount);

   /**
    * Required by extensions:
    *  - GL_ANGLE_instanced_arrays (gles2)
    */
    void vertex_attrib_divisor(gl::uint_t index, gl::uint_t divisor);

   /**
    * Required by extensions:
    *  - GL_ANGLE_translated_shader_source (gles2)
    */
    void get_translated_shader_source(gl::uint_t shader, gl::sizei_t bufsize, gl::sizei_t * length, gl::char_t * source);

   /**
    * Required by extensions:
    *  - GL_EXT_direct_state_access (gl)
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void texture_storage_1d(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width);

   /**
    * Required by extensions:
    *  - GL_EXT_direct_state_access (gl)
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void texture_storage_2d(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height);

   /**
    * Required by extensions:
    *  - GL_EXT_direct_state_access (gl)
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void texture_storage_3d(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height, gl::sizei_t depth);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void begin_query(gl::enum_t target, gl::uint_t id);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void delete_queries(gl::sizei_t n, const  gl::uint_t * ids);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void end_query(gl::enum_t target);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void gen_queries(gl::sizei_t n, gl::uint_t * ids);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void get_query_objectuiv(gl::uint_t id, gl::enum_t pname, gl::uint_t * params);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void get_queryiv(gl::enum_t target, gl::enum_t pname, gl::int_t * params);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    gl::boolean_t is_query(gl::uint_t id);

   /**
    * Required by extensions:
    *  - GL_EXT_draw_buffers (gles2)
    */
    void draw_buffers(gl::sizei_t n, const  gl::enum_t * bufs);

   /**
    * Required by extensions:
    *  - GL_EXT_map_buffer_range (gles1,gles2)
    */
    void flush_mapped_buffer_range(buffer_target_arb_t target, gl::intptr_t offset, gl::sizeiptr_t length);

   /**
    * Required by extensions:
    *  - GL_EXT_map_buffer_range (gles1,gles2)
    */
    void * map_buffer_range(buffer_target_arb_t target, gl::intptr_t offset, gl::sizeiptr_t length, gl::bitfield_t access);

   /**
    * Required by extensions:
    *  - GL_EXT_robustness (gles1,gles2)
    */
    gl::enum_t get_graphics_reset_status();

   /**
    * Required by extensions:
    *  - GL_EXT_robustness (gles1,gles2)
    */
    void getn_uniformfv(gl::uint_t program, gl::int_t location, gl::sizei_t bufSize, gl::float_t * params);

   /**
    * Required by extensions:
    *  - GL_EXT_robustness (gles1,gles2)
    */
    void getn_uniformiv(gl::uint_t program, gl::int_t location, gl::sizei_t bufSize, gl::int_t * params);

   /**
    * Required by extensions:
    *  - GL_EXT_robustness (gles1,gles2)
    */
    void readn_pixels(gl::int_t x, gl::int_t y, gl::sizei_t width, gl::sizei_t height, gl::enum_t format, gl::enum_t type, gl::sizei_t bufSize, void * data);

   /**
    * Required by extensions:
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void tex_storage_1d(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width);

   /**
    * Required by extensions:
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void tex_storage_2d(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height);

   /**
    * Required by extensions:
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void tex_storage_3d(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height, gl::sizei_t depth);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void delete_fences(gl::sizei_t n, const  gl::uint_t * fences);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void finish_fence(gl::uint_t fence);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void gen_fences(gl::sizei_t n, gl::uint_t * fences);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void get_fenceiv(gl::uint_t fence, gl::enum_t pname, gl::int_t * params);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    gl::boolean_t is_fence(gl::uint_t fence);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void set_fence(gl::uint_t fence, gl::enum_t condition);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    gl::boolean_t test_fence(gl::uint_t fence);

   /**
    * Required by extensions:
    *  - GL_OES_get_program_binary (gles2)
    */
    void get_program_binary(gl::uint_t program, gl::sizei_t bufSize, gl::sizei_t * length, gl::enum_t * binaryFormat, void * binary);

   /**
    * Required by extensions:
    *  - GL_OES_get_program_binary (gles2)
    */
    void program_binary(gl::uint_t program, gl::enum_t binaryFormat, const  void * binary, gl::int_t length);

   /**
    * Required by extensions:
    *  - GL_OES_mapbuffer (gles1,gles2)
    */
    void get_buffer_pointerv(buffer_target_arb_t target, gl::enum_t pname, void ** params);

   /**
    * Required by extensions:
    *  - GL_OES_mapbuffer (gles1,gles2)
    */
    void * map_buffer(buffer_target_arb_t target, gl::enum_t access);

   /**
    * Required by extensions:
    *  - GL_OES_mapbuffer (gles1,gles2)
    */
    gl::boolean_t unmap_buffer(buffer_target_arb_t target);


   /**
    * non-aliased commands
    */
  
   /**
    * Required by extensions:
    *  - GL_ANGLE_framebuffer_blit (gles2)
    */
    void blit_framebuffer_angle(gl::int_t srcX0, gl::int_t srcY0, gl::int_t srcX1, gl::int_t srcY1, gl::int_t dstX0, gl::int_t dstY0, gl::int_t dstX1, gl::int_t dstY1, gl::bitmask<clear_buffer_flags_t> mask, gl::enum_t filter);

   /**
    * Required by extensions:
    *  - GL_ANGLE_framebuffer_multisample (gles2)
    */
    void renderbuffer_storage_multisample_angle(renderbuffer_target_t target, gl::sizei_t samples, internal_format_t internalformat, gl::sizei_t width, gl::sizei_t height);

   /**
    * Required by extensions:
    *  - GL_ANGLE_instanced_arrays (gles2)
    */
    void draw_arrays_instanced_angle(primitive_type_t mode, gl::int_t first, gl::sizei_t count, gl::sizei_t primcount);

   /**
    * Required by extensions:
    *  - GL_ANGLE_instanced_arrays (gles2)
    */
    void draw_elements_instanced_angle(primitive_type_t mode, gl::sizei_t count, draw_elements_type_t type, const  void * indices, gl::sizei_t primcount);

   /**
    * Required by extensions:
    *  - GL_ANGLE_instanced_arrays (gles2)
    */
    void vertex_attrib_divisor_angle(gl::uint_t index, gl::uint_t divisor);

   /**
    * Required by extensions:
    *  - GL_ANGLE_translated_shader_source (gles2)
    */
    void get_translated_shader_source_angle(gl::uint_t shader, gl::sizei_t bufsize, gl::sizei_t * length, gl::char_t * source);

   /**
    * Required by extensions:
    *  - GL_EXT_direct_state_access (gl)
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void texture_storage_1dext(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width);

   /**
    * Required by extensions:
    *  - GL_EXT_direct_state_access (gl)
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void texture_storage_2dext(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height);

   /**
    * Required by extensions:
    *  - GL_EXT_direct_state_access (gl)
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void texture_storage_3dext(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height, gl::sizei_t depth);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void begin_query_ext(gl::enum_t target, gl::uint_t id);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void delete_queries_ext(gl::sizei_t n, const  gl::uint_t * ids);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void end_query_ext(gl::enum_t target);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void gen_queries_ext(gl::sizei_t n, gl::uint_t * ids);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void get_query_objectuiv_ext(gl::uint_t id, gl::enum_t pname, gl::uint_t * params);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    void get_queryiv_ext(gl::enum_t target, gl::enum_t pname, gl::int_t * params);

   /**
    * Required by extensions:
    *  - GL_EXT_disjoint_timer_query (gles2)
    *  - GL_EXT_occlusion_query_boolean (gles2)
    */
    gl::boolean_t is_query_ext(gl::uint_t id);

   /**
    * Required by extensions:
    *  - GL_EXT_draw_buffers (gles2)
    */
    void draw_buffers_ext(gl::sizei_t n, const  gl::enum_t * bufs);

   /**
    * Required by extensions:
    *  - GL_EXT_map_buffer_range (gles1,gles2)
    */
    void flush_mapped_buffer_range_ext(buffer_target_arb_t target, gl::intptr_t offset, gl::sizeiptr_t length);

   /**
    * Required by extensions:
    *  - GL_EXT_map_buffer_range (gles1,gles2)
    */
    void * map_buffer_range_ext(buffer_target_arb_t target, gl::intptr_t offset, gl::sizeiptr_t length, gl::bitfield_t access);

   /**
    * Required by extensions:
    *  - GL_EXT_robustness (gles1,gles2)
    */
    gl::enum_t get_graphics_reset_status_ext();

   /**
    * Required by extensions:
    *  - GL_EXT_robustness (gles1,gles2)
    */
    void getn_uniformfv_ext(gl::uint_t program, gl::int_t location, gl::sizei_t bufSize, gl::float_t * params);

   /**
    * Required by extensions:
    *  - GL_EXT_robustness (gles1,gles2)
    */
    void getn_uniformiv_ext(gl::uint_t program, gl::int_t location, gl::sizei_t bufSize, gl::int_t * params);

   /**
    * Required by extensions:
    *  - GL_EXT_robustness (gles1,gles2)
    */
    void readn_pixels_ext(gl::int_t x, gl::int_t y, gl::sizei_t width, gl::sizei_t height, gl::enum_t format, gl::enum_t type, gl::sizei_t bufSize, void * data);

   /**
    * Required by extensions:
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void tex_storage_1dext(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width);

   /**
    * Required by extensions:
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void tex_storage_2dext(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height);

   /**
    * Required by extensions:
    *  - GL_EXT_texture_storage (gles1,gles2)
    */
    void tex_storage_3dext(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height, gl::sizei_t depth);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void delete_fences_nv(gl::sizei_t n, const  gl::uint_t * fences);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void finish_fence_nv(gl::uint_t fence);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void gen_fences_nv(gl::sizei_t n, gl::uint_t * fences);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void get_fenceiv_nv(gl::uint_t fence, gl::enum_t pname, gl::int_t * params);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    gl::boolean_t is_fence_nv(gl::uint_t fence);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    void set_fence_nv(gl::uint_t fence, gl::enum_t condition);

   /**
    * Required by extensions:
    *  - GL_NV_fence (gl,gles1,gles2)
    */
    gl::boolean_t test_fence_nv(gl::uint_t fence);

   /**
    * Required by extensions:
    *  - GL_OES_get_program_binary (gles2)
    */
    void get_program_binary_oes(gl::uint_t program, gl::sizei_t bufSize, gl::sizei_t * length, gl::enum_t * binaryFormat, void * binary);

   /**
    * Required by extensions:
    *  - GL_OES_get_program_binary (gles2)
    */
    void program_binary_oes(gl::uint_t program, gl::enum_t binaryFormat, const  void * binary, gl::int_t length);

   /**
    * Required by extensions:
    *  - GL_OES_mapbuffer (gles1,gles2)
    */
    void get_buffer_pointerv_oes(buffer_target_arb_t target, gl::enum_t pname, void ** params);

   /**
    * Required by extensions:
    *  - GL_OES_mapbuffer (gles1,gles2)
    */
    void * map_buffer_oes(buffer_target_arb_t target, gl::enum_t access);

   /**
    * Required by extensions:
    *  - GL_OES_mapbuffer (gles1,gles2)
    */
    gl::boolean_t unmap_buffer_oes(buffer_target_arb_t target);

} // namespace angle
} // namespace gles2

#endif // #ifndef GLES2_ANGLE_EXTENSIONS__H
