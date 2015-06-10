/**
 * This file was autogenerated by glgen (https://github.com/seshbot/glgen) on 2015-06-11
 * Command line: src/gen.py assets/gl.xml -p assets/gl-patch.xml -o ../glpp/ --includesubdir include/glpp --sourcesubdir src --namespace gles2:angle --force --cpp --es2only --extensionsimportfile assets/extensions_angle_dx11.txt --extensionsoutputfilename extensions
 */

#include <glad/glad.h>
#include <stdexcept>
#include <glpp/gles2/angle/extensions.h>


namespace gles2 {
namespace angle {

   /**
    * command aliases
    */
  
    void blit_framebuffer(gl::int_t srcX0, gl::int_t srcY0, gl::int_t srcX1, gl::int_t srcY1, gl::int_t dstX0, gl::int_t dstY0, gl::int_t dstX1, gl::int_t dstY1, gl::bitmask<clear_buffer_flags_t> mask, gl::enum_t filter) {
      if ( GLAD_GL_ANGLE_framebuffer_blit ) {
        glBlitFramebufferANGLE(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, static_cast<GLbitfield>(mask.value), filter);
      }
      else {
        throw std::runtime_error("OpenGL command 'glBlitFramebufferANGLE' not available on this platform (extensions: GL_ANGLE_framebuffer_blit)");
      }
    }

    void renderbuffer_storage_multisample(renderbuffer_target_t target, gl::sizei_t samples, internal_format_t internalformat, gl::sizei_t width, gl::sizei_t height) {
      if ( GLAD_GL_ANGLE_framebuffer_multisample ) {
        glRenderbufferStorageMultisampleANGLE(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width, height);
      }
      else {
        throw std::runtime_error("OpenGL command 'glRenderbufferStorageMultisampleANGLE' not available on this platform (extensions: GL_ANGLE_framebuffer_multisample)");
      }
    }

    void draw_arrays_instanced(primitive_type_t mode, gl::int_t first, gl::sizei_t count, gl::sizei_t primcount) {
      if ( GLAD_GL_ANGLE_instanced_arrays ) {
        glDrawArraysInstancedANGLE(static_cast<GLenum>(mode), first, count, primcount);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDrawArraysInstancedANGLE' not available on this platform (extensions: GL_ANGLE_instanced_arrays)");
      }
    }

    void draw_elements_instanced(primitive_type_t mode, gl::sizei_t count, draw_elements_type_t type, const  void * indices, gl::sizei_t primcount) {
      if ( GLAD_GL_ANGLE_instanced_arrays ) {
        glDrawElementsInstancedANGLE(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, primcount);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDrawElementsInstancedANGLE' not available on this platform (extensions: GL_ANGLE_instanced_arrays)");
      }
    }

    void vertex_attrib_divisor(gl::uint_t index, gl::uint_t divisor) {
      if ( GLAD_GL_ANGLE_instanced_arrays ) {
        glVertexAttribDivisorANGLE(index, divisor);
      }
      else {
        throw std::runtime_error("OpenGL command 'glVertexAttribDivisorANGLE' not available on this platform (extensions: GL_ANGLE_instanced_arrays)");
      }
    }

    void get_translated_shader_source(gl::uint_t shader, gl::sizei_t bufsize, gl::sizei_t * length, gl::char_t * source) {
      if ( GLAD_GL_ANGLE_translated_shader_source ) {
        glGetTranslatedShaderSourceANGLE(shader, bufsize, length, source);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetTranslatedShaderSourceANGLE' not available on this platform (extensions: GL_ANGLE_translated_shader_source)");
      }
    }

    void texture_storage_1d(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTextureStorage1DEXT(texture, target, levels, internalformat, width);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTextureStorage1DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void texture_storage_2d(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTextureStorage2DEXT(texture, target, levels, internalformat, width, height);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTextureStorage2DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void texture_storage_3d(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height, gl::sizei_t depth) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTextureStorage3DEXT(texture, target, levels, internalformat, width, height, depth);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTextureStorage3DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void begin_query(gl::enum_t target, gl::uint_t id) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glBeginQueryEXT(target, id);
      }
      else {
        throw std::runtime_error("OpenGL command 'glBeginQueryEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void delete_queries(gl::sizei_t n, const  gl::uint_t * ids) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glDeleteQueriesEXT(n, ids);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDeleteQueriesEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void end_query(gl::enum_t target) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glEndQueryEXT(target);
      }
      else {
        throw std::runtime_error("OpenGL command 'glEndQueryEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void gen_queries(gl::sizei_t n, gl::uint_t * ids) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glGenQueriesEXT(n, ids);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGenQueriesEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void get_query_objectuiv(gl::uint_t id, gl::enum_t pname, gl::uint_t * params) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glGetQueryObjectuivEXT(id, pname, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetQueryObjectuivEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void get_queryiv(gl::enum_t target, gl::enum_t pname, gl::int_t * params) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glGetQueryivEXT(target, pname, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetQueryivEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    gl::boolean_t is_query(gl::uint_t id) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        return glIsQueryEXT(id);
      }
      else {
        throw std::runtime_error("OpenGL command 'glIsQueryEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void draw_buffers(gl::sizei_t n, const  gl::enum_t * bufs) {
      if ( GLAD_GL_EXT_draw_buffers ) {
        glDrawBuffersEXT(n, bufs);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDrawBuffersEXT' not available on this platform (extensions: GL_EXT_draw_buffers)");
      }
    }

    void flush_mapped_buffer_range(buffer_target_arb_t target, gl::intptr_t offset, gl::sizeiptr_t length) {
      if ( GLAD_GL_EXT_map_buffer_range ) {
        glFlushMappedBufferRangeEXT(static_cast<GLenum>(target), offset, length);
      }
      else {
        throw std::runtime_error("OpenGL command 'glFlushMappedBufferRangeEXT' not available on this platform (extensions: GL_EXT_map_buffer_range)");
      }
    }

    void * map_buffer_range(buffer_target_arb_t target, gl::intptr_t offset, gl::sizeiptr_t length, gl::bitfield_t access) {
      if ( GLAD_GL_EXT_map_buffer_range ) {
        return glMapBufferRangeEXT(static_cast<GLenum>(target), offset, length, access);
      }
      else {
        throw std::runtime_error("OpenGL command 'glMapBufferRangeEXT' not available on this platform (extensions: GL_EXT_map_buffer_range)");
      }
    }

    gl::enum_t get_graphics_reset_status() {
      if ( GLAD_GL_EXT_robustness ) {
        return glGetGraphicsResetStatusEXT();
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetGraphicsResetStatusEXT' not available on this platform (extensions: GL_EXT_robustness)");
      }
    }

    void getn_uniformfv(gl::uint_t program, gl::int_t location, gl::sizei_t bufSize, gl::float_t * params) {
      if ( GLAD_GL_EXT_robustness ) {
        glGetnUniformfvEXT(program, location, bufSize, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetnUniformfvEXT' not available on this platform (extensions: GL_EXT_robustness)");
      }
    }

    void getn_uniformiv(gl::uint_t program, gl::int_t location, gl::sizei_t bufSize, gl::int_t * params) {
      if ( GLAD_GL_EXT_robustness ) {
        glGetnUniformivEXT(program, location, bufSize, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetnUniformivEXT' not available on this platform (extensions: GL_EXT_robustness)");
      }
    }

    void readn_pixels(gl::int_t x, gl::int_t y, gl::sizei_t width, gl::sizei_t height, gl::enum_t format, gl::enum_t type, gl::sizei_t bufSize, void * data) {
      if ( GLAD_GL_EXT_robustness ) {
        glReadnPixelsEXT(x, y, width, height, format, type, bufSize, data);
      }
      else {
        throw std::runtime_error("OpenGL command 'glReadnPixelsEXT' not available on this platform (extensions: GL_EXT_robustness)");
      }
    }

    void tex_storage_1d(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTexStorage1DEXT(target, levels, internalformat, width);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTexStorage1DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void tex_storage_2d(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTexStorage2DEXT(target, levels, internalformat, width, height);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTexStorage2DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void tex_storage_3d(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height, gl::sizei_t depth) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTexStorage3DEXT(target, levels, internalformat, width, height, depth);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTexStorage3DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void delete_fences(gl::sizei_t n, const  gl::uint_t * fences) {
      if ( GLAD_GL_NV_fence ) {
        glDeleteFencesNV(n, fences);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDeleteFencesNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void finish_fence(gl::uint_t fence) {
      if ( GLAD_GL_NV_fence ) {
        glFinishFenceNV(fence);
      }
      else {
        throw std::runtime_error("OpenGL command 'glFinishFenceNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void gen_fences(gl::sizei_t n, gl::uint_t * fences) {
      if ( GLAD_GL_NV_fence ) {
        glGenFencesNV(n, fences);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGenFencesNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void get_fenceiv(gl::uint_t fence, gl::enum_t pname, gl::int_t * params) {
      if ( GLAD_GL_NV_fence ) {
        glGetFenceivNV(fence, pname, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetFenceivNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    gl::boolean_t is_fence(gl::uint_t fence) {
      if ( GLAD_GL_NV_fence ) {
        return glIsFenceNV(fence);
      }
      else {
        throw std::runtime_error("OpenGL command 'glIsFenceNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void set_fence(gl::uint_t fence, gl::enum_t condition) {
      if ( GLAD_GL_NV_fence ) {
        glSetFenceNV(fence, condition);
      }
      else {
        throw std::runtime_error("OpenGL command 'glSetFenceNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    gl::boolean_t test_fence(gl::uint_t fence) {
      if ( GLAD_GL_NV_fence ) {
        return glTestFenceNV(fence);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTestFenceNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void get_program_binary(gl::uint_t program, gl::sizei_t bufSize, gl::sizei_t * length, gl::enum_t * binaryFormat, void * binary) {
      if ( GLAD_GL_OES_get_program_binary ) {
        glGetProgramBinaryOES(program, bufSize, length, binaryFormat, binary);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetProgramBinaryOES' not available on this platform (extensions: GL_OES_get_program_binary)");
      }
    }

    void program_binary(gl::uint_t program, gl::enum_t binaryFormat, const  void * binary, gl::int_t length) {
      if ( GLAD_GL_OES_get_program_binary ) {
        glProgramBinaryOES(program, binaryFormat, binary, length);
      }
      else {
        throw std::runtime_error("OpenGL command 'glProgramBinaryOES' not available on this platform (extensions: GL_OES_get_program_binary)");
      }
    }

    void get_buffer_pointerv(buffer_target_arb_t target, gl::enum_t pname, void ** params) {
      if ( GLAD_GL_OES_mapbuffer ) {
        glGetBufferPointervOES(static_cast<GLenum>(target), pname, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetBufferPointervOES' not available on this platform (extensions: GL_OES_mapbuffer)");
      }
    }

    void * map_buffer(buffer_target_arb_t target, gl::enum_t access) {
      if ( GLAD_GL_OES_mapbuffer ) {
        return glMapBufferOES(static_cast<GLenum>(target), access);
      }
      else {
        throw std::runtime_error("OpenGL command 'glMapBufferOES' not available on this platform (extensions: GL_OES_mapbuffer)");
      }
    }

    gl::boolean_t unmap_buffer(buffer_target_arb_t target) {
      if ( GLAD_GL_OES_mapbuffer ) {
        return glUnmapBufferOES(static_cast<GLenum>(target));
      }
      else {
        throw std::runtime_error("OpenGL command 'glUnmapBufferOES' not available on this platform (extensions: GL_OES_mapbuffer)");
      }
    }


   /**
    * non-aliased commands
    */
  
    void blit_framebuffer_angle(gl::int_t srcX0, gl::int_t srcY0, gl::int_t srcX1, gl::int_t srcY1, gl::int_t dstX0, gl::int_t dstY0, gl::int_t dstX1, gl::int_t dstY1, gl::bitmask<clear_buffer_flags_t> mask, gl::enum_t filter) {
      if ( GLAD_GL_ANGLE_framebuffer_blit ) {
        glBlitFramebufferANGLE(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, static_cast<GLbitfield>(mask.value), filter);
      }
      else {
        throw std::runtime_error("OpenGL command 'glBlitFramebufferANGLE' not available on this platform (extensions: GL_ANGLE_framebuffer_blit)");
      }
    }

    void renderbuffer_storage_multisample_angle(renderbuffer_target_t target, gl::sizei_t samples, internal_format_t internalformat, gl::sizei_t width, gl::sizei_t height) {
      if ( GLAD_GL_ANGLE_framebuffer_multisample ) {
        glRenderbufferStorageMultisampleANGLE(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width, height);
      }
      else {
        throw std::runtime_error("OpenGL command 'glRenderbufferStorageMultisampleANGLE' not available on this platform (extensions: GL_ANGLE_framebuffer_multisample)");
      }
    }

    void draw_arrays_instanced_angle(primitive_type_t mode, gl::int_t first, gl::sizei_t count, gl::sizei_t primcount) {
      if ( GLAD_GL_ANGLE_instanced_arrays ) {
        glDrawArraysInstancedANGLE(static_cast<GLenum>(mode), first, count, primcount);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDrawArraysInstancedANGLE' not available on this platform (extensions: GL_ANGLE_instanced_arrays)");
      }
    }

    void draw_elements_instanced_angle(primitive_type_t mode, gl::sizei_t count, draw_elements_type_t type, const  void * indices, gl::sizei_t primcount) {
      if ( GLAD_GL_ANGLE_instanced_arrays ) {
        glDrawElementsInstancedANGLE(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), indices, primcount);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDrawElementsInstancedANGLE' not available on this platform (extensions: GL_ANGLE_instanced_arrays)");
      }
    }

    void vertex_attrib_divisor_angle(gl::uint_t index, gl::uint_t divisor) {
      if ( GLAD_GL_ANGLE_instanced_arrays ) {
        glVertexAttribDivisorANGLE(index, divisor);
      }
      else {
        throw std::runtime_error("OpenGL command 'glVertexAttribDivisorANGLE' not available on this platform (extensions: GL_ANGLE_instanced_arrays)");
      }
    }

    void get_translated_shader_source_angle(gl::uint_t shader, gl::sizei_t bufsize, gl::sizei_t * length, gl::char_t * source) {
      if ( GLAD_GL_ANGLE_translated_shader_source ) {
        glGetTranslatedShaderSourceANGLE(shader, bufsize, length, source);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetTranslatedShaderSourceANGLE' not available on this platform (extensions: GL_ANGLE_translated_shader_source)");
      }
    }

    void texture_storage_1dext(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTextureStorage1DEXT(texture, target, levels, internalformat, width);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTextureStorage1DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void texture_storage_2dext(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTextureStorage2DEXT(texture, target, levels, internalformat, width, height);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTextureStorage2DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void texture_storage_3dext(gl::uint_t texture, gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height, gl::sizei_t depth) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTextureStorage3DEXT(texture, target, levels, internalformat, width, height, depth);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTextureStorage3DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void begin_query_ext(gl::enum_t target, gl::uint_t id) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glBeginQueryEXT(target, id);
      }
      else {
        throw std::runtime_error("OpenGL command 'glBeginQueryEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void delete_queries_ext(gl::sizei_t n, const  gl::uint_t * ids) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glDeleteQueriesEXT(n, ids);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDeleteQueriesEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void end_query_ext(gl::enum_t target) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glEndQueryEXT(target);
      }
      else {
        throw std::runtime_error("OpenGL command 'glEndQueryEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void gen_queries_ext(gl::sizei_t n, gl::uint_t * ids) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glGenQueriesEXT(n, ids);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGenQueriesEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void get_query_objectuiv_ext(gl::uint_t id, gl::enum_t pname, gl::uint_t * params) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glGetQueryObjectuivEXT(id, pname, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetQueryObjectuivEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void get_queryiv_ext(gl::enum_t target, gl::enum_t pname, gl::int_t * params) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        glGetQueryivEXT(target, pname, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetQueryivEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    gl::boolean_t is_query_ext(gl::uint_t id) {
      if ( GLAD_GL_EXT_disjoint_timer_query || GLAD_GL_EXT_occlusion_query_boolean ) {
        return glIsQueryEXT(id);
      }
      else {
        throw std::runtime_error("OpenGL command 'glIsQueryEXT' not available on this platform (extensions: GL_EXT_disjoint_timer_query, GL_EXT_occlusion_query_boolean)");
      }
    }

    void draw_buffers_ext(gl::sizei_t n, const  gl::enum_t * bufs) {
      if ( GLAD_GL_EXT_draw_buffers ) {
        glDrawBuffersEXT(n, bufs);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDrawBuffersEXT' not available on this platform (extensions: GL_EXT_draw_buffers)");
      }
    }

    void flush_mapped_buffer_range_ext(buffer_target_arb_t target, gl::intptr_t offset, gl::sizeiptr_t length) {
      if ( GLAD_GL_EXT_map_buffer_range ) {
        glFlushMappedBufferRangeEXT(static_cast<GLenum>(target), offset, length);
      }
      else {
        throw std::runtime_error("OpenGL command 'glFlushMappedBufferRangeEXT' not available on this platform (extensions: GL_EXT_map_buffer_range)");
      }
    }

    void * map_buffer_range_ext(buffer_target_arb_t target, gl::intptr_t offset, gl::sizeiptr_t length, gl::bitfield_t access) {
      if ( GLAD_GL_EXT_map_buffer_range ) {
        return glMapBufferRangeEXT(static_cast<GLenum>(target), offset, length, access);
      }
      else {
        throw std::runtime_error("OpenGL command 'glMapBufferRangeEXT' not available on this platform (extensions: GL_EXT_map_buffer_range)");
      }
    }

    gl::enum_t get_graphics_reset_status_ext() {
      if ( GLAD_GL_EXT_robustness ) {
        return glGetGraphicsResetStatusEXT();
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetGraphicsResetStatusEXT' not available on this platform (extensions: GL_EXT_robustness)");
      }
    }

    void getn_uniformfv_ext(gl::uint_t program, gl::int_t location, gl::sizei_t bufSize, gl::float_t * params) {
      if ( GLAD_GL_EXT_robustness ) {
        glGetnUniformfvEXT(program, location, bufSize, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetnUniformfvEXT' not available on this platform (extensions: GL_EXT_robustness)");
      }
    }

    void getn_uniformiv_ext(gl::uint_t program, gl::int_t location, gl::sizei_t bufSize, gl::int_t * params) {
      if ( GLAD_GL_EXT_robustness ) {
        glGetnUniformivEXT(program, location, bufSize, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetnUniformivEXT' not available on this platform (extensions: GL_EXT_robustness)");
      }
    }

    void readn_pixels_ext(gl::int_t x, gl::int_t y, gl::sizei_t width, gl::sizei_t height, gl::enum_t format, gl::enum_t type, gl::sizei_t bufSize, void * data) {
      if ( GLAD_GL_EXT_robustness ) {
        glReadnPixelsEXT(x, y, width, height, format, type, bufSize, data);
      }
      else {
        throw std::runtime_error("OpenGL command 'glReadnPixelsEXT' not available on this platform (extensions: GL_EXT_robustness)");
      }
    }

    void tex_storage_1dext(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTexStorage1DEXT(target, levels, internalformat, width);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTexStorage1DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void tex_storage_2dext(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTexStorage2DEXT(target, levels, internalformat, width, height);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTexStorage2DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void tex_storage_3dext(gl::enum_t target, gl::sizei_t levels, gl::enum_t internalformat, gl::sizei_t width, gl::sizei_t height, gl::sizei_t depth) {
      if ( GLAD_GL_EXT_texture_storage ) {
        glTexStorage3DEXT(target, levels, internalformat, width, height, depth);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTexStorage3DEXT' not available on this platform (extensions: GL_EXT_texture_storage)");
      }
    }

    void delete_fences_nv(gl::sizei_t n, const  gl::uint_t * fences) {
      if ( GLAD_GL_NV_fence ) {
        glDeleteFencesNV(n, fences);
      }
      else {
        throw std::runtime_error("OpenGL command 'glDeleteFencesNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void finish_fence_nv(gl::uint_t fence) {
      if ( GLAD_GL_NV_fence ) {
        glFinishFenceNV(fence);
      }
      else {
        throw std::runtime_error("OpenGL command 'glFinishFenceNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void gen_fences_nv(gl::sizei_t n, gl::uint_t * fences) {
      if ( GLAD_GL_NV_fence ) {
        glGenFencesNV(n, fences);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGenFencesNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void get_fenceiv_nv(gl::uint_t fence, gl::enum_t pname, gl::int_t * params) {
      if ( GLAD_GL_NV_fence ) {
        glGetFenceivNV(fence, pname, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetFenceivNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    gl::boolean_t is_fence_nv(gl::uint_t fence) {
      if ( GLAD_GL_NV_fence ) {
        return glIsFenceNV(fence);
      }
      else {
        throw std::runtime_error("OpenGL command 'glIsFenceNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void set_fence_nv(gl::uint_t fence, gl::enum_t condition) {
      if ( GLAD_GL_NV_fence ) {
        glSetFenceNV(fence, condition);
      }
      else {
        throw std::runtime_error("OpenGL command 'glSetFenceNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    gl::boolean_t test_fence_nv(gl::uint_t fence) {
      if ( GLAD_GL_NV_fence ) {
        return glTestFenceNV(fence);
      }
      else {
        throw std::runtime_error("OpenGL command 'glTestFenceNV' not available on this platform (extensions: GL_NV_fence)");
      }
    }

    void get_program_binary_oes(gl::uint_t program, gl::sizei_t bufSize, gl::sizei_t * length, gl::enum_t * binaryFormat, void * binary) {
      if ( GLAD_GL_OES_get_program_binary ) {
        glGetProgramBinaryOES(program, bufSize, length, binaryFormat, binary);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetProgramBinaryOES' not available on this platform (extensions: GL_OES_get_program_binary)");
      }
    }

    void program_binary_oes(gl::uint_t program, gl::enum_t binaryFormat, const  void * binary, gl::int_t length) {
      if ( GLAD_GL_OES_get_program_binary ) {
        glProgramBinaryOES(program, binaryFormat, binary, length);
      }
      else {
        throw std::runtime_error("OpenGL command 'glProgramBinaryOES' not available on this platform (extensions: GL_OES_get_program_binary)");
      }
    }

    void get_buffer_pointerv_oes(buffer_target_arb_t target, gl::enum_t pname, void ** params) {
      if ( GLAD_GL_OES_mapbuffer ) {
        glGetBufferPointervOES(static_cast<GLenum>(target), pname, params);
      }
      else {
        throw std::runtime_error("OpenGL command 'glGetBufferPointervOES' not available on this platform (extensions: GL_OES_mapbuffer)");
      }
    }

    void * map_buffer_oes(buffer_target_arb_t target, gl::enum_t access) {
      if ( GLAD_GL_OES_mapbuffer ) {
        return glMapBufferOES(static_cast<GLenum>(target), access);
      }
      else {
        throw std::runtime_error("OpenGL command 'glMapBufferOES' not available on this platform (extensions: GL_OES_mapbuffer)");
      }
    }

    gl::boolean_t unmap_buffer_oes(buffer_target_arb_t target) {
      if ( GLAD_GL_OES_mapbuffer ) {
        return glUnmapBufferOES(static_cast<GLenum>(target));
      }
      else {
        throw std::runtime_error("OpenGL command 'glUnmapBufferOES' not available on this platform (extensions: GL_OES_mapbuffer)");
      }
    }

} // namespace angle
} // namespace gles2
