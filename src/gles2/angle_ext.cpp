// ANGLE's eglGetProcAddress() only returns the address of extension functions, 
// which while technically conforming to the spec does not work with glad (which uses
// the getprocaddress function to load ALL functions)

#include <string.h>

// this should be the only place we need to include these (otherwise use the glad header)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

namespace gles2 {
  using gl_proc = void(*)();
  using get_gl_proc_func = gl_proc(*)(const char *);

  namespace {
     struct proc_name_address_t {
        const char * name;
        gl_proc address;
     };

     // TODO: generate this list from glgen
     gl_proc get_proc_address_impl(const char* proc_name) {
        static const proc_name_address_t proc_info[] = {
          { "glActiveTexture", reinterpret_cast<gl_proc>(glActiveTexture) },
          { "glAttachShader", reinterpret_cast<gl_proc>(glAttachShader) },
          { "glBindAttribLocation", reinterpret_cast<gl_proc>(glBindAttribLocation) },
          { "glBindBuffer", reinterpret_cast<gl_proc>(glBindBuffer) },
          { "glBindFramebuffer", reinterpret_cast<gl_proc>(glBindFramebuffer) },
          { "glBindRenderbuffer", reinterpret_cast<gl_proc>(glBindRenderbuffer) },
          { "glBindTexture", reinterpret_cast<gl_proc>(glBindTexture) },
          { "glBlendColor", reinterpret_cast<gl_proc>(glBlendColor) },
          { "glBlendEquation", reinterpret_cast<gl_proc>(glBlendEquation) },
          { "glBlendEquationSeparate", reinterpret_cast<gl_proc>(glBlendEquationSeparate) },
          { "glBlendFunc", reinterpret_cast<gl_proc>(glBlendFunc) },
          { "glBlendFuncSeparate", reinterpret_cast<gl_proc>(glBlendFuncSeparate) },
          { "glBufferData", reinterpret_cast<gl_proc>(glBufferData) },
          { "glBufferSubData", reinterpret_cast<gl_proc>(glBufferSubData) },
          { "glCheckFramebufferStatus", reinterpret_cast<gl_proc>(glCheckFramebufferStatus) },
          { "glClear", reinterpret_cast<gl_proc>(glClear) },
          { "glClearColor", reinterpret_cast<gl_proc>(glClearColor) },
          { "glClearDepthf", reinterpret_cast<gl_proc>(glClearDepthf) },
          { "glClearStencil", reinterpret_cast<gl_proc>(glClearStencil) },
          { "glColorMask", reinterpret_cast<gl_proc>(glColorMask) },
          { "glCompileShader", reinterpret_cast<gl_proc>(glCompileShader) },
          { "glCompressedTexImage2D", reinterpret_cast<gl_proc>(glCompressedTexImage2D) },
          { "glCompressedTexSubImage2D", reinterpret_cast<gl_proc>(glCompressedTexSubImage2D) },
          { "glCopyTexImage2D", reinterpret_cast<gl_proc>(glCopyTexImage2D) },
          { "glCopyTexSubImage2D", reinterpret_cast<gl_proc>(glCopyTexSubImage2D) },
          { "glCreateProgram", reinterpret_cast<gl_proc>(glCreateProgram) },
          { "glCreateShader", reinterpret_cast<gl_proc>(glCreateShader) },
          { "glCullFace", reinterpret_cast<gl_proc>(glCullFace) },
          { "glDeleteBuffers", reinterpret_cast<gl_proc>(glDeleteBuffers) },
          { "glDeleteFramebuffers", reinterpret_cast<gl_proc>(glDeleteFramebuffers) },
          { "glDeleteProgram", reinterpret_cast<gl_proc>(glDeleteProgram) },
          { "glDeleteRenderbuffers", reinterpret_cast<gl_proc>(glDeleteRenderbuffers) },
          { "glDeleteShader", reinterpret_cast<gl_proc>(glDeleteShader) },
          { "glDeleteTextures", reinterpret_cast<gl_proc>(glDeleteTextures) },
          { "glDepthFunc", reinterpret_cast<gl_proc>(glDepthFunc) },
          { "glDepthMask", reinterpret_cast<gl_proc>(glDepthMask) },
          { "glDepthRangef", reinterpret_cast<gl_proc>(glDepthRangef) },
          { "glDetachShader", reinterpret_cast<gl_proc>(glDetachShader) },
          { "glDisable", reinterpret_cast<gl_proc>(glDisable) },
          { "glDisableVertexAttribArray", reinterpret_cast<gl_proc>(glDisableVertexAttribArray) },
          { "glDrawArrays", reinterpret_cast<gl_proc>(glDrawArrays) },
          { "glDrawElements", reinterpret_cast<gl_proc>(glDrawElements) },
          { "glEnable", reinterpret_cast<gl_proc>(glEnable) },
          { "glEnableVertexAttribArray", reinterpret_cast<gl_proc>(glEnableVertexAttribArray) },
          { "glFinish", reinterpret_cast<gl_proc>(glFinish) },
          { "glFlush", reinterpret_cast<gl_proc>(glFlush) },
          { "glFramebufferRenderbuffer", reinterpret_cast<gl_proc>(glFramebufferRenderbuffer) },
          { "glFramebufferTexture2D", reinterpret_cast<gl_proc>(glFramebufferTexture2D) },
          { "glFrontFace", reinterpret_cast<gl_proc>(glFrontFace) },
          { "glGenBuffers", reinterpret_cast<gl_proc>(glGenBuffers) },
          { "glGenFramebuffers", reinterpret_cast<gl_proc>(glGenFramebuffers) },
          { "glGenRenderbuffers", reinterpret_cast<gl_proc>(glGenRenderbuffers) },
          { "glGenTextures", reinterpret_cast<gl_proc>(glGenTextures) },
          { "glGenerateMipmap", reinterpret_cast<gl_proc>(glGenerateMipmap) },
          { "glGetActiveAttrib", reinterpret_cast<gl_proc>(glGetActiveAttrib) },
          { "glGetActiveUniform", reinterpret_cast<gl_proc>(glGetActiveUniform) },
          { "glGetAttachedShaders", reinterpret_cast<gl_proc>(glGetAttachedShaders) },
          { "glGetAttribLocation", reinterpret_cast<gl_proc>(glGetAttribLocation) },
          { "glGetBooleanv", reinterpret_cast<gl_proc>(glGetBooleanv) },
          { "glGetBufferParameteriv", reinterpret_cast<gl_proc>(glGetBufferParameteriv) },
          { "glGetError", reinterpret_cast<gl_proc>(glGetError) },
          { "glGetFloatv", reinterpret_cast<gl_proc>(glGetFloatv) },
          { "glGetFramebufferAttachmentParameteriv", reinterpret_cast<gl_proc>(glGetFramebufferAttachmentParameteriv) },
          { "glGetIntegerv", reinterpret_cast<gl_proc>(glGetIntegerv) },
          { "glGetProgramInfoLog", reinterpret_cast<gl_proc>(glGetProgramInfoLog) },
          { "glGetProgramiv", reinterpret_cast<gl_proc>(glGetProgramiv) },
          { "glGetRenderbufferParameteriv", reinterpret_cast<gl_proc>(glGetRenderbufferParameteriv) },
          { "glGetShaderInfoLog", reinterpret_cast<gl_proc>(glGetShaderInfoLog) },
          { "glGetShaderPrecisionFormat", reinterpret_cast<gl_proc>(glGetShaderPrecisionFormat) },
          { "glGetShaderSource", reinterpret_cast<gl_proc>(glGetShaderSource) },
          { "glGetShaderiv", reinterpret_cast<gl_proc>(glGetShaderiv) },
          { "glGetString", reinterpret_cast<gl_proc>(glGetString) },
          { "glGetTexParameterfv", reinterpret_cast<gl_proc>(glGetTexParameterfv) },
          { "glGetTexParameteriv", reinterpret_cast<gl_proc>(glGetTexParameteriv) },
          { "glGetUniformLocation", reinterpret_cast<gl_proc>(glGetUniformLocation) },
          { "glGetUniformfv", reinterpret_cast<gl_proc>(glGetUniformfv) },
          { "glGetUniformiv", reinterpret_cast<gl_proc>(glGetUniformiv) },
          { "glGetVertexAttribPointerv", reinterpret_cast<gl_proc>(glGetVertexAttribPointerv) },
          { "glGetVertexAttribfv", reinterpret_cast<gl_proc>(glGetVertexAttribfv) },
          { "glGetVertexAttribiv", reinterpret_cast<gl_proc>(glGetVertexAttribiv) },
          { "glHint", reinterpret_cast<gl_proc>(glHint) },
          { "glIsBuffer", reinterpret_cast<gl_proc>(glIsBuffer) },
          { "glIsEnabled", reinterpret_cast<gl_proc>(glIsEnabled) },
          { "glIsFramebuffer", reinterpret_cast<gl_proc>(glIsFramebuffer) },
          { "glIsProgram", reinterpret_cast<gl_proc>(glIsProgram) },
          { "glIsRenderbuffer", reinterpret_cast<gl_proc>(glIsRenderbuffer) },
          { "glIsShader", reinterpret_cast<gl_proc>(glIsShader) },
          { "glIsTexture", reinterpret_cast<gl_proc>(glIsTexture) },
          { "glLineWidth", reinterpret_cast<gl_proc>(glLineWidth) },
          { "glLinkProgram", reinterpret_cast<gl_proc>(glLinkProgram) },
          { "glPixelStorei", reinterpret_cast<gl_proc>(glPixelStorei) },
          { "glPolygonOffset", reinterpret_cast<gl_proc>(glPolygonOffset) },
          { "glReadPixels", reinterpret_cast<gl_proc>(glReadPixels) },
          { "glReleaseShaderCompiler", reinterpret_cast<gl_proc>(glReleaseShaderCompiler) },
          { "glRenderbufferStorage", reinterpret_cast<gl_proc>(glRenderbufferStorage) },
          { "glSampleCoverage", reinterpret_cast<gl_proc>(glSampleCoverage) },
          { "glScissor", reinterpret_cast<gl_proc>(glScissor) },
          { "glShaderBinary", reinterpret_cast<gl_proc>(glShaderBinary) },
          { "glShaderSource", reinterpret_cast<gl_proc>(glShaderSource) },
          { "glStencilFunc", reinterpret_cast<gl_proc>(glStencilFunc) },
          { "glStencilFuncSeparate", reinterpret_cast<gl_proc>(glStencilFuncSeparate) },
          { "glStencilMask", reinterpret_cast<gl_proc>(glStencilMask) },
          { "glStencilMaskSeparate", reinterpret_cast<gl_proc>(glStencilMaskSeparate) },
          { "glStencilOp", reinterpret_cast<gl_proc>(glStencilOp) },
          { "glStencilOpSeparate", reinterpret_cast<gl_proc>(glStencilOpSeparate) },
          { "glTexImage2D", reinterpret_cast<gl_proc>(glTexImage2D) },
          { "glTexParameterf", reinterpret_cast<gl_proc>(glTexParameterf) },
          { "glTexParameterfv", reinterpret_cast<gl_proc>(glTexParameterfv) },
          { "glTexParameteri", reinterpret_cast<gl_proc>(glTexParameteri) },
          { "glTexParameteriv", reinterpret_cast<gl_proc>(glTexParameteriv) },
          { "glTexSubImage2D", reinterpret_cast<gl_proc>(glTexSubImage2D) },
          { "glUniform1f", reinterpret_cast<gl_proc>(glUniform1f) },
          { "glUniform1fv", reinterpret_cast<gl_proc>(glUniform1fv) },
          { "glUniform1i", reinterpret_cast<gl_proc>(glUniform1i) },
          { "glUniform1iv", reinterpret_cast<gl_proc>(glUniform1iv) },
          { "glUniform2f", reinterpret_cast<gl_proc>(glUniform2f) },
          { "glUniform2fv", reinterpret_cast<gl_proc>(glUniform2fv) },
          { "glUniform2i", reinterpret_cast<gl_proc>(glUniform2i) },
          { "glUniform2iv", reinterpret_cast<gl_proc>(glUniform2iv) },
          { "glUniform3f", reinterpret_cast<gl_proc>(glUniform3f) },
          { "glUniform3fv", reinterpret_cast<gl_proc>(glUniform3fv) },
          { "glUniform3i", reinterpret_cast<gl_proc>(glUniform3i) },
          { "glUniform3iv", reinterpret_cast<gl_proc>(glUniform3iv) },
          { "glUniform4f", reinterpret_cast<gl_proc>(glUniform4f) },
          { "glUniform4fv", reinterpret_cast<gl_proc>(glUniform4fv) },
          { "glUniform4i", reinterpret_cast<gl_proc>(glUniform4i) },
          { "glUniform4iv", reinterpret_cast<gl_proc>(glUniform4iv) },
          { "glUniformMatrix2fv", reinterpret_cast<gl_proc>(glUniformMatrix2fv) },
          { "glUniformMatrix3fv", reinterpret_cast<gl_proc>(glUniformMatrix3fv) },
          { "glUniformMatrix4fv", reinterpret_cast<gl_proc>(glUniformMatrix4fv) },
          { "glUseProgram", reinterpret_cast<gl_proc>(glUseProgram) },
          { "glValidateProgram", reinterpret_cast<gl_proc>(glValidateProgram) },
          { "glVertexAttrib1f", reinterpret_cast<gl_proc>(glVertexAttrib1f) },
          { "glVertexAttrib1fv", reinterpret_cast<gl_proc>(glVertexAttrib1fv) },
          { "glVertexAttrib2f", reinterpret_cast<gl_proc>(glVertexAttrib2f) },
          { "glVertexAttrib2fv", reinterpret_cast<gl_proc>(glVertexAttrib2fv) },
          { "glVertexAttrib3f", reinterpret_cast<gl_proc>(glVertexAttrib3f) },
          { "glVertexAttrib3fv", reinterpret_cast<gl_proc>(glVertexAttrib3fv) },
          { "glVertexAttrib4f", reinterpret_cast<gl_proc>(glVertexAttrib4f) },
          { "glVertexAttrib4fv", reinterpret_cast<gl_proc>(glVertexAttrib4fv) },
          { "glVertexAttribPointer", reinterpret_cast<gl_proc>(glVertexAttribPointer) },
          { "glViewport", reinterpret_cast<gl_proc>(glViewport) },
        };

        for (auto & inf : proc_info) {
          if (strcmp(proc_name, inf.name) == 0)
            return inf.address;
        }
        
        return nullptr;
     }

    bool is_core_proc(const char* proc_name) {
      return nullptr != get_proc_address_impl(proc_name);
    }

    gl_proc get_core_proc_address(const char* proc_name) {
      return get_proc_address_impl(proc_name);
    }
  }

  namespace angle {
    gl_proc get_proc_address(const char* proc_name) {
      if (is_core_proc(proc_name))
        return get_proc_address_impl(proc_name);

      return eglGetProcAddress(proc_name);
    }
  }
}