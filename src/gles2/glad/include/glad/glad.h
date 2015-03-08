
#ifndef __glad_h_

#ifdef __gl_h_
#error OpenGL header already included, remove this include, glad already provides it
#endif

#define __glad_h_
#define __gl_h_

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct gladGLversionStruct {
    int major;
    int minor;
};

extern struct gladGLversionStruct GLVersion;

typedef void* (* GLADloadproc)(const char *name);

#ifndef GLAPI
# if defined(GLAD_GLAPI_EXPORT)
#  if defined(WIN32) || defined(__CYGWIN__)
#   if defined(GLAD_GLAPI_EXPORT_BUILD)
#    if defined(__GNUC__)
#     define GLAPI __attribute__ ((dllexport)) extern
#    else
#     define GLAPI __declspec(dllexport) extern
#    endif
#   else
#    if defined(__GNUC__)
#     define GLAPI __attribute__ ((dllimport)) extern
#    else
#     define GLAPI __declspec(dllimport) extern
#    endif
#   endif
#  elif defined(__GNUC__) && defined(GLAD_GLAPI_EXPORT_BUILD)
#   define GLAPI __attribute__ ((visibility ("default"))) extern
#  else
#   define GLAPI extern
#  endif
# else
#  define GLAPI extern
# endif
#endif

GLAPI int gladLoadGL(void);

GLAPI int gladLoadGLES2Loader(GLADloadproc);

#include <stddef.h>
#include <KHR/khrplatform.h>
#ifndef GLEXT_64_TYPES_DEFINED
/* This code block is duplicated in glxext.h, so must be protected */
#define GLEXT_64_TYPES_DEFINED
/* Define int32_t, int64_t, and uint64_t types for UST/MSC */
/* (as used in the GL_EXT_timer_query extension). */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <inttypes.h>
#elif defined(__sun__) || defined(__digital__)
#include <inttypes.h>
#if defined(__STDC__)
#if defined(__arch64__) || defined(_LP64)
typedef long int int64_t;
typedef unsigned long int uint64_t;
#else
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#endif /* __arch64__ */
#endif /* __STDC__ */
#elif defined( __VMS ) || defined(__sgi)
#include <inttypes.h>
#elif defined(__SCO__) || defined(__USLC__)
#include <stdint.h>
#elif defined(__UNIXOS2__) || defined(__SOL64__)
typedef long int int32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#elif defined(_WIN32) && defined(__GNUC__)
#include <stdint.h>
#elif defined(_WIN32)
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
/* Fallback if nothing above works */
#include <inttypes.h>
#endif
#endif
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLclampx;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void *GLeglImageOES;
typedef char GLchar;
typedef char GLcharARB;
#ifdef __APPLE__
typedef void *GLhandleARB;
#else
typedef unsigned int GLhandleARB;
#endif
typedef unsigned short GLhalfARB;
typedef unsigned short GLhalf;
typedef GLint GLfixed;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;
typedef int64_t GLint64EXT;
typedef uint64_t GLuint64EXT;
typedef struct __GLsync *GLsync;
struct _cl_context;
struct _cl_event;
typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
typedef void (APIENTRY *GLDEBUGPROCAMD)(GLuint id,GLenum category,GLenum severity,GLsizei length,const GLchar *message,void *userParam);
typedef unsigned short GLhalfNV;
typedef GLintptr GLvdpauSurfaceNV;
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_FUNC_ADD 0x8006
#define GL_BLEND_EQUATION 0x8009
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_CONSTANT_COLOR 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_CONSTANT_ALPHA 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_BLEND_COLOR 0x8005
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_STREAM_DRAW 0x88E0
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_FRONT_AND_BACK 0x0408
#define GL_TEXTURE_2D 0x0DE1
#define GL_CULL_FACE 0x0B44
#define GL_BLEND 0x0BE2
#define GL_DITHER 0x0BD0
#define GL_STENCIL_TEST 0x0B90
#define GL_DEPTH_TEST 0x0B71
#define GL_SCISSOR_TEST 0x0C11
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_SAMPLE_COVERAGE 0x80A0
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_CW 0x0900
#define GL_CCW 0x0901
#define GL_LINE_WIDTH 0x0B21
#define GL_ALIASED_POINT_SIZE_RANGE 0x846D
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_DEPTH_RANGE 0x0B70
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_FUNC 0x0B74
#define GL_STENCIL_CLEAR_VALUE 0x0B91
#define GL_STENCIL_FUNC 0x0B92
#define GL_STENCIL_FAIL 0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
#define GL_STENCIL_REF 0x0B97
#define GL_STENCIL_VALUE_MASK 0x0B93
#define GL_STENCIL_WRITEMASK 0x0B98
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_VIEWPORT 0x0BA2
#define GL_SCISSOR_BOX 0x0C10
#define GL_COLOR_CLEAR_VALUE 0x0C22
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_SUBPIXEL_BITS 0x0D50
#define GL_RED_BITS 0x0D52
#define GL_GREEN_BITS 0x0D53
#define GL_BLUE_BITS 0x0D54
#define GL_ALPHA_BITS 0x0D55
#define GL_DEPTH_BITS 0x0D56
#define GL_STENCIL_BITS 0x0D57
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_SAMPLE_BUFFERS 0x80A8
#define GL_SAMPLES 0x80A9
#define GL_SAMPLE_COVERAGE_VALUE 0x80AA
#define GL_SAMPLE_COVERAGE_INVERT 0x80AB
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_DONT_CARE 0x1100
#define GL_FASTEST 0x1101
#define GL_NICEST 0x1102
#define GL_GENERATE_MIPMAP_HINT 0x8192
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_FIXED 0x140C
#define GL_DEPTH_COMPONENT 0x1902
#define GL_ALPHA 0x1906
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_MAX_VERTEX_UNIFORM_VECTORS 0x8DFB
#define GL_MAX_VARYING_VECTORS 0x8DFC
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS 0x8DFD
#define GL_SHADER_TYPE 0x8B4F
#define GL_DELETE_STATUS 0x8B80
#define GL_LINK_STATUS 0x8B82
#define GL_VALIDATE_STATUS 0x8B83
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_INCR 0x1E02
#define GL_DECR 0x1E03
#define GL_INVERT 0x150A
#define GL_INCR_WRAP 0x8507
#define GL_DECR_WRAP 0x8508
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE 0x1702
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MIRRORED_REPEAT 0x8370
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_CUBE 0x8B60
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_COMPILE_STATUS 0x8B81
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_SHADER_COMPILER 0x8DFA
#define GL_SHADER_BINARY_FORMATS 0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS 0x8DF9
#define GL_LOW_FLOAT 0x8DF0
#define GL_MEDIUM_FLOAT 0x8DF1
#define GL_HIGH_FLOAT 0x8DF2
#define GL_LOW_INT 0x8DF3
#define GL_MEDIUM_INT 0x8DF4
#define GL_HIGH_INT 0x8DF5
#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGB565 0x8D62
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_STENCIL_INDEX8 0x8D48
#define GL_RENDERBUFFER_WIDTH 0x8D42
#define GL_RENDERBUFFER_HEIGHT 0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
#define GL_RENDERBUFFER_RED_SIZE 0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_NONE 0
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_RENDERBUFFER_BINDING 0x8CA7
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#ifndef GL_ES_VERSION_2_0
#define GL_ES_VERSION_2_0 1
GLAPI int GLAD_GL_ES_VERSION_2_0;
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum);
GLAPI PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
#define glActiveTexture glad_glActiveTexture
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint, GLuint);
GLAPI PFNGLATTACHSHADERPROC glad_glAttachShader;
#define glAttachShader glad_glAttachShader
typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONPROC)(GLuint, GLuint, const GLchar*);
GLAPI PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation;
#define glBindAttribLocation glad_glBindAttribLocation
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum, GLuint);
GLAPI PFNGLBINDBUFFERPROC glad_glBindBuffer;
#define glBindBuffer glad_glBindBuffer
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum, GLuint);
GLAPI PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer;
#define glBindFramebuffer glad_glBindFramebuffer
typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC)(GLenum, GLuint);
GLAPI PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer;
#define glBindRenderbuffer glad_glBindRenderbuffer
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum, GLuint);
GLAPI PFNGLBINDTEXTUREPROC glad_glBindTexture;
#define glBindTexture glad_glBindTexture
typedef void (APIENTRYP PFNGLBLENDCOLORPROC)(GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLBLENDCOLORPROC glad_glBlendColor;
#define glBlendColor glad_glBlendColor
typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum);
GLAPI PFNGLBLENDEQUATIONPROC glad_glBlendEquation;
#define glBlendEquation glad_glBlendEquation
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum, GLenum);
GLAPI PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate;
#define glBlendEquationSeparate glad_glBlendEquationSeparate
typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum, GLenum);
GLAPI PFNGLBLENDFUNCPROC glad_glBlendFunc;
#define glBlendFunc glad_glBlendFunc
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC)(GLenum, GLenum, GLenum, GLenum);
GLAPI PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate;
#define glBlendFuncSeparate glad_glBlendFuncSeparate
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum, GLsizeiptr, const void*, GLenum);
GLAPI PFNGLBUFFERDATAPROC glad_glBufferData;
#define glBufferData glad_glBufferData
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum, GLintptr, GLsizeiptr, const void*);
GLAPI PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
#define glBufferSubData glad_glBufferSubData
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum);
GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
#define glCheckFramebufferStatus glad_glCheckFramebufferStatus
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield);
GLAPI PFNGLCLEARPROC glad_glClear;
#define glClear glad_glClear
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLCLEARCOLORPROC glad_glClearColor;
#define glClearColor glad_glClearColor
typedef void (APIENTRYP PFNGLCLEARDEPTHFPROC)(GLfloat);
GLAPI PFNGLCLEARDEPTHFPROC glad_glClearDepthf;
#define glClearDepthf glad_glClearDepthf
typedef void (APIENTRYP PFNGLCLEARSTENCILPROC)(GLint);
GLAPI PFNGLCLEARSTENCILPROC glad_glClearStencil;
#define glClearStencil glad_glClearStencil
typedef void (APIENTRYP PFNGLCOLORMASKPROC)(GLboolean, GLboolean, GLboolean, GLboolean);
GLAPI PFNGLCOLORMASKPROC glad_glColorMask;
#define glColorMask glad_glColorMask
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint);
GLAPI PFNGLCOMPILESHADERPROC glad_glCompileShader;
#define glCompileShader glad_glCompileShader
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void*);
GLAPI PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D;
#define glCompressedTexImage2D glad_glCompressedTexImage2D
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void*);
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D;
#define glCompressedTexSubImage2D glad_glCompressedTexSubImage2D
typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DPROC)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint);
GLAPI PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D;
#define glCopyTexImage2D glad_glCopyTexImage2D
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
GLAPI PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D;
#define glCopyTexSubImage2D glad_glCopyTexSubImage2D
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)();
GLAPI PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
#define glCreateProgram glad_glCreateProgram
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum);
GLAPI PFNGLCREATESHADERPROC glad_glCreateShader;
#define glCreateShader glad_glCreateShader
typedef void (APIENTRYP PFNGLCULLFACEPROC)(GLenum);
GLAPI PFNGLCULLFACEPROC glad_glCullFace;
#define glCullFace glad_glCullFace
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei, const GLuint*);
GLAPI PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
#define glDeleteBuffers glad_glDeleteBuffers
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei, const GLuint*);
GLAPI PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
#define glDeleteFramebuffers glad_glDeleteFramebuffers
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint);
GLAPI PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
#define glDeleteProgram glad_glDeleteProgram
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC)(GLsizei, const GLuint*);
GLAPI PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers;
#define glDeleteRenderbuffers glad_glDeleteRenderbuffers
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint);
GLAPI PFNGLDELETESHADERPROC glad_glDeleteShader;
#define glDeleteShader glad_glDeleteShader
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei, const GLuint*);
GLAPI PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
#define glDeleteTextures glad_glDeleteTextures
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum);
GLAPI PFNGLDEPTHFUNCPROC glad_glDepthFunc;
#define glDepthFunc glad_glDepthFunc
typedef void (APIENTRYP PFNGLDEPTHMASKPROC)(GLboolean);
GLAPI PFNGLDEPTHMASKPROC glad_glDepthMask;
#define glDepthMask glad_glDepthMask
typedef void (APIENTRYP PFNGLDEPTHRANGEFPROC)(GLfloat, GLfloat);
GLAPI PFNGLDEPTHRANGEFPROC glad_glDepthRangef;
#define glDepthRangef glad_glDepthRangef
typedef void (APIENTRYP PFNGLDETACHSHADERPROC)(GLuint, GLuint);
GLAPI PFNGLDETACHSHADERPROC glad_glDetachShader;
#define glDetachShader glad_glDetachShader
typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum);
GLAPI PFNGLDISABLEPROC glad_glDisable;
#define glDisable glad_glDisable
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint);
GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;
#define glDisableVertexAttribArray glad_glDisableVertexAttribArray
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum, GLint, GLsizei);
GLAPI PFNGLDRAWARRAYSPROC glad_glDrawArrays;
#define glDrawArrays glad_glDrawArrays
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum, GLsizei, GLenum, const void*);
GLAPI PFNGLDRAWELEMENTSPROC glad_glDrawElements;
#define glDrawElements glad_glDrawElements
typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum);
GLAPI PFNGLENABLEPROC glad_glEnable;
#define glEnable glad_glEnable
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint);
GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
#define glEnableVertexAttribArray glad_glEnableVertexAttribArray
typedef void (APIENTRYP PFNGLFINISHPROC)();
GLAPI PFNGLFINISHPROC glad_glFinish;
#define glFinish glad_glFinish
typedef void (APIENTRYP PFNGLFLUSHPROC)();
GLAPI PFNGLFLUSHPROC glad_glFlush;
#define glFlush glad_glFlush
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum, GLenum, GLenum, GLuint);
GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer;
#define glFramebufferRenderbuffer glad_glFramebufferRenderbuffer
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum, GLenum, GLenum, GLuint, GLint);
GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
#define glFramebufferTexture2D glad_glFramebufferTexture2D
typedef void (APIENTRYP PFNGLFRONTFACEPROC)(GLenum);
GLAPI PFNGLFRONTFACEPROC glad_glFrontFace;
#define glFrontFace glad_glFrontFace
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENBUFFERSPROC glad_glGenBuffers;
#define glGenBuffers glad_glGenBuffers
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum);
GLAPI PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap;
#define glGenerateMipmap glad_glGenerateMipmap
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
#define glGenFramebuffers glad_glGenFramebuffers
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers;
#define glGenRenderbuffers glad_glGenRenderbuffers
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENTEXTURESPROC glad_glGenTextures;
#define glGenTextures glad_glGenTextures
typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC)(GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar*);
GLAPI PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib;
#define glGetActiveAttrib glad_glGetActiveAttrib
typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC)(GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar*);
GLAPI PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform;
#define glGetActiveUniform glad_glGetActiveUniform
typedef void (APIENTRYP PFNGLGETATTACHEDSHADERSPROC)(GLuint, GLsizei, GLsizei*, GLuint*);
GLAPI PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders;
#define glGetAttachedShaders glad_glGetAttachedShaders
typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONPROC)(GLuint, const GLchar*);
GLAPI PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation;
#define glGetAttribLocation glad_glGetAttribLocation
typedef void (APIENTRYP PFNGLGETBOOLEANVPROC)(GLenum, GLboolean*);
GLAPI PFNGLGETBOOLEANVPROC glad_glGetBooleanv;
#define glGetBooleanv glad_glGetBooleanv
typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC)(GLenum, GLenum, GLint*);
GLAPI PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv;
#define glGetBufferParameteriv glad_glGetBufferParameteriv
typedef GLenum (APIENTRYP PFNGLGETERRORPROC)();
GLAPI PFNGLGETERRORPROC glad_glGetError;
#define glGetError glad_glGetError
typedef void (APIENTRYP PFNGLGETFLOATVPROC)(GLenum, GLfloat*);
GLAPI PFNGLGETFLOATVPROC glad_glGetFloatv;
#define glGetFloatv glad_glGetFloatv
typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)(GLenum, GLenum, GLenum, GLint*);
GLAPI PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv;
#define glGetFramebufferAttachmentParameteriv glad_glGetFramebufferAttachmentParameteriv
typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum, GLint*);
GLAPI PFNGLGETINTEGERVPROC glad_glGetIntegerv;
#define glGetIntegerv glad_glGetIntegerv
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
#define glGetProgramiv glad_glGetProgramiv
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
#define glGetProgramInfoLog glad_glGetProgramInfoLog
typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVPROC)(GLenum, GLenum, GLint*);
GLAPI PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv;
#define glGetRenderbufferParameteriv glad_glGetRenderbufferParameteriv
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETSHADERIVPROC glad_glGetShaderiv;
#define glGetShaderiv glad_glGetShaderiv
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;
#define glGetShaderInfoLog glad_glGetShaderInfoLog
typedef void (APIENTRYP PFNGLGETSHADERPRECISIONFORMATPROC)(GLenum, GLenum, GLint*, GLint*);
GLAPI PFNGLGETSHADERPRECISIONFORMATPROC glad_glGetShaderPrecisionFormat;
#define glGetShaderPrecisionFormat glad_glGetShaderPrecisionFormat
typedef void (APIENTRYP PFNGLGETSHADERSOURCEPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource;
#define glGetShaderSource glad_glGetShaderSource
typedef const GLubyte* (APIENTRYP PFNGLGETSTRINGPROC)(GLenum);
GLAPI PFNGLGETSTRINGPROC glad_glGetString;
#define glGetString glad_glGetString
typedef void (APIENTRYP PFNGLGETTEXPARAMETERFVPROC)(GLenum, GLenum, GLfloat*);
GLAPI PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv;
#define glGetTexParameterfv glad_glGetTexParameterfv
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIVPROC)(GLenum, GLenum, GLint*);
GLAPI PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv;
#define glGetTexParameteriv glad_glGetTexParameteriv
typedef void (APIENTRYP PFNGLGETUNIFORMFVPROC)(GLuint, GLint, GLfloat*);
GLAPI PFNGLGETUNIFORMFVPROC glad_glGetUniformfv;
#define glGetUniformfv glad_glGetUniformfv
typedef void (APIENTRYP PFNGLGETUNIFORMIVPROC)(GLuint, GLint, GLint*);
GLAPI PFNGLGETUNIFORMIVPROC glad_glGetUniformiv;
#define glGetUniformiv glad_glGetUniformiv
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint, const GLchar*);
GLAPI PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
#define glGetUniformLocation glad_glGetUniformLocation
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVPROC)(GLuint, GLenum, GLfloat*);
GLAPI PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv;
#define glGetVertexAttribfv glad_glGetVertexAttribfv
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv;
#define glGetVertexAttribiv glad_glGetVertexAttribiv
typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC)(GLuint, GLenum, void**);
GLAPI PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv;
#define glGetVertexAttribPointerv glad_glGetVertexAttribPointerv
typedef void (APIENTRYP PFNGLHINTPROC)(GLenum, GLenum);
GLAPI PFNGLHINTPROC glad_glHint;
#define glHint glad_glHint
typedef GLboolean (APIENTRYP PFNGLISBUFFERPROC)(GLuint);
GLAPI PFNGLISBUFFERPROC glad_glIsBuffer;
#define glIsBuffer glad_glIsBuffer
typedef GLboolean (APIENTRYP PFNGLISENABLEDPROC)(GLenum);
GLAPI PFNGLISENABLEDPROC glad_glIsEnabled;
#define glIsEnabled glad_glIsEnabled
typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFERPROC)(GLuint);
GLAPI PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer;
#define glIsFramebuffer glad_glIsFramebuffer
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPROC)(GLuint);
GLAPI PFNGLISPROGRAMPROC glad_glIsProgram;
#define glIsProgram glad_glIsProgram
typedef GLboolean (APIENTRYP PFNGLISRENDERBUFFERPROC)(GLuint);
GLAPI PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer;
#define glIsRenderbuffer glad_glIsRenderbuffer
typedef GLboolean (APIENTRYP PFNGLISSHADERPROC)(GLuint);
GLAPI PFNGLISSHADERPROC glad_glIsShader;
#define glIsShader glad_glIsShader
typedef GLboolean (APIENTRYP PFNGLISTEXTUREPROC)(GLuint);
GLAPI PFNGLISTEXTUREPROC glad_glIsTexture;
#define glIsTexture glad_glIsTexture
typedef void (APIENTRYP PFNGLLINEWIDTHPROC)(GLfloat);
GLAPI PFNGLLINEWIDTHPROC glad_glLineWidth;
#define glLineWidth glad_glLineWidth
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint);
GLAPI PFNGLLINKPROGRAMPROC glad_glLinkProgram;
#define glLinkProgram glad_glLinkProgram
typedef void (APIENTRYP PFNGLPIXELSTOREIPROC)(GLenum, GLint);
GLAPI PFNGLPIXELSTOREIPROC glad_glPixelStorei;
#define glPixelStorei glad_glPixelStorei
typedef void (APIENTRYP PFNGLPOLYGONOFFSETPROC)(GLfloat, GLfloat);
GLAPI PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset;
#define glPolygonOffset glad_glPolygonOffset
typedef void (APIENTRYP PFNGLREADPIXELSPROC)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*);
GLAPI PFNGLREADPIXELSPROC glad_glReadPixels;
#define glReadPixels glad_glReadPixels
typedef void (APIENTRYP PFNGLRELEASESHADERCOMPILERPROC)();
GLAPI PFNGLRELEASESHADERCOMPILERPROC glad_glReleaseShaderCompiler;
#define glReleaseShaderCompiler glad_glReleaseShaderCompiler
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(GLenum, GLenum, GLsizei, GLsizei);
GLAPI PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage;
#define glRenderbufferStorage glad_glRenderbufferStorage
typedef void (APIENTRYP PFNGLSAMPLECOVERAGEPROC)(GLfloat, GLboolean);
GLAPI PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage;
#define glSampleCoverage glad_glSampleCoverage
typedef void (APIENTRYP PFNGLSCISSORPROC)(GLint, GLint, GLsizei, GLsizei);
GLAPI PFNGLSCISSORPROC glad_glScissor;
#define glScissor glad_glScissor
typedef void (APIENTRYP PFNGLSHADERBINARYPROC)(GLsizei, const GLuint*, GLenum, const void*, GLsizei);
GLAPI PFNGLSHADERBINARYPROC glad_glShaderBinary;
#define glShaderBinary glad_glShaderBinary
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint, GLsizei, const GLchar**, const GLint*);
GLAPI PFNGLSHADERSOURCEPROC glad_glShaderSource;
#define glShaderSource glad_glShaderSource
typedef void (APIENTRYP PFNGLSTENCILFUNCPROC)(GLenum, GLint, GLuint);
GLAPI PFNGLSTENCILFUNCPROC glad_glStencilFunc;
#define glStencilFunc glad_glStencilFunc
typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC)(GLenum, GLenum, GLint, GLuint);
GLAPI PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate;
#define glStencilFuncSeparate glad_glStencilFuncSeparate
typedef void (APIENTRYP PFNGLSTENCILMASKPROC)(GLuint);
GLAPI PFNGLSTENCILMASKPROC glad_glStencilMask;
#define glStencilMask glad_glStencilMask
typedef void (APIENTRYP PFNGLSTENCILMASKSEPARATEPROC)(GLenum, GLuint);
GLAPI PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate;
#define glStencilMaskSeparate glad_glStencilMaskSeparate
typedef void (APIENTRYP PFNGLSTENCILOPPROC)(GLenum, GLenum, GLenum);
GLAPI PFNGLSTENCILOPPROC glad_glStencilOp;
#define glStencilOp glad_glStencilOp
typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEPROC)(GLenum, GLenum, GLenum, GLenum);
GLAPI PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate;
#define glStencilOpSeparate glad_glStencilOpSeparate
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
GLAPI PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
#define glTexImage2D glad_glTexImage2D
typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC)(GLenum, GLenum, GLfloat);
GLAPI PFNGLTEXPARAMETERFPROC glad_glTexParameterf;
#define glTexParameterf glad_glTexParameterf
typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC)(GLenum, GLenum, const GLfloat*);
GLAPI PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv;
#define glTexParameterfv glad_glTexParameterfv
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum, GLenum, GLint);
GLAPI PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
#define glTexParameteri glad_glTexParameteri
typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC)(GLenum, GLenum, const GLint*);
GLAPI PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv;
#define glTexParameteriv glad_glTexParameteriv
typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void*);
GLAPI PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D;
#define glTexSubImage2D glad_glTexSubImage2D
typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint, GLfloat);
GLAPI PFNGLUNIFORM1FPROC glad_glUniform1f;
#define glUniform1f glad_glUniform1f
typedef void (APIENTRYP PFNGLUNIFORM1FVPROC)(GLint, GLsizei, const GLfloat*);
GLAPI PFNGLUNIFORM1FVPROC glad_glUniform1fv;
#define glUniform1fv glad_glUniform1fv
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint, GLint);
GLAPI PFNGLUNIFORM1IPROC glad_glUniform1i;
#define glUniform1i glad_glUniform1i
typedef void (APIENTRYP PFNGLUNIFORM1IVPROC)(GLint, GLsizei, const GLint*);
GLAPI PFNGLUNIFORM1IVPROC glad_glUniform1iv;
#define glUniform1iv glad_glUniform1iv
typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(GLint, GLfloat, GLfloat);
GLAPI PFNGLUNIFORM2FPROC glad_glUniform2f;
#define glUniform2f glad_glUniform2f
typedef void (APIENTRYP PFNGLUNIFORM2FVPROC)(GLint, GLsizei, const GLfloat*);
GLAPI PFNGLUNIFORM2FVPROC glad_glUniform2fv;
#define glUniform2fv glad_glUniform2fv
typedef void (APIENTRYP PFNGLUNIFORM2IPROC)(GLint, GLint, GLint);
GLAPI PFNGLUNIFORM2IPROC glad_glUniform2i;
#define glUniform2i glad_glUniform2i
typedef void (APIENTRYP PFNGLUNIFORM2IVPROC)(GLint, GLsizei, const GLint*);
GLAPI PFNGLUNIFORM2IVPROC glad_glUniform2iv;
#define glUniform2iv glad_glUniform2iv
typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(GLint, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLUNIFORM3FPROC glad_glUniform3f;
#define glUniform3f glad_glUniform3f
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(GLint, GLsizei, const GLfloat*);
GLAPI PFNGLUNIFORM3FVPROC glad_glUniform3fv;
#define glUniform3fv glad_glUniform3fv
typedef void (APIENTRYP PFNGLUNIFORM3IPROC)(GLint, GLint, GLint, GLint);
GLAPI PFNGLUNIFORM3IPROC glad_glUniform3i;
#define glUniform3i glad_glUniform3i
typedef void (APIENTRYP PFNGLUNIFORM3IVPROC)(GLint, GLsizei, const GLint*);
GLAPI PFNGLUNIFORM3IVPROC glad_glUniform3iv;
#define glUniform3iv glad_glUniform3iv
typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLUNIFORM4FPROC glad_glUniform4f;
#define glUniform4f glad_glUniform4f
typedef void (APIENTRYP PFNGLUNIFORM4FVPROC)(GLint, GLsizei, const GLfloat*);
GLAPI PFNGLUNIFORM4FVPROC glad_glUniform4fv;
#define glUniform4fv glad_glUniform4fv
typedef void (APIENTRYP PFNGLUNIFORM4IPROC)(GLint, GLint, GLint, GLint, GLint);
GLAPI PFNGLUNIFORM4IPROC glad_glUniform4i;
#define glUniform4i glad_glUniform4i
typedef void (APIENTRYP PFNGLUNIFORM4IVPROC)(GLint, GLsizei, const GLint*);
GLAPI PFNGLUNIFORM4IVPROC glad_glUniform4iv;
#define glUniform4iv glad_glUniform4iv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv;
#define glUniformMatrix2fv glad_glUniformMatrix2fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv;
#define glUniformMatrix3fv glad_glUniformMatrix3fv
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv;
#define glUniformMatrix4fv glad_glUniformMatrix4fv
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint);
GLAPI PFNGLUSEPROGRAMPROC glad_glUseProgram;
#define glUseProgram glad_glUseProgram
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC)(GLuint);
GLAPI PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram;
#define glValidateProgram glad_glValidateProgram
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FPROC)(GLuint, GLfloat);
GLAPI PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f;
#define glVertexAttrib1f glad_glVertexAttrib1f
typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVPROC)(GLuint, const GLfloat*);
GLAPI PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv;
#define glVertexAttrib1fv glad_glVertexAttrib1fv
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FPROC)(GLuint, GLfloat, GLfloat);
GLAPI PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f;
#define glVertexAttrib2f glad_glVertexAttrib2f
typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVPROC)(GLuint, const GLfloat*);
GLAPI PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv;
#define glVertexAttrib2fv glad_glVertexAttrib2fv
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FPROC)(GLuint, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f;
#define glVertexAttrib3f glad_glVertexAttrib3f
typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVPROC)(GLuint, const GLfloat*);
GLAPI PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv;
#define glVertexAttrib3fv glad_glVertexAttrib3fv
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FPROC)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f;
#define glVertexAttrib4f glad_glVertexAttrib4f
typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVPROC)(GLuint, const GLfloat*);
GLAPI PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv;
#define glVertexAttrib4fv glad_glVertexAttrib4fv
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
GLAPI PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
#define glVertexAttribPointer glad_glVertexAttribPointer
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint, GLint, GLsizei, GLsizei);
GLAPI PFNGLVIEWPORTPROC glad_glViewport;
#define glViewport glad_glViewport
#endif
#define GL_FLOAT_MAT2x3_NV 0x8B65
#define GL_FLOAT_MAT2x4_NV 0x8B66
#define GL_FLOAT_MAT3x2_NV 0x8B67
#define GL_FLOAT_MAT3x4_NV 0x8B68
#define GL_FLOAT_MAT4x2_NV 0x8B69
#define GL_FLOAT_MAT4x3_NV 0x8B6A
#define GL_DEPTH_STENCIL_OES 0x84F9
#define GL_UNSIGNED_INT_24_8_OES 0x84FA
#define GL_DEPTH24_STENCIL8_OES 0x88F0
#define GL_PERFMON_GLOBAL_MODE_QCOM 0x8FA0
#define GL_BLEND_ADVANCED_COHERENT_KHR 0x9285
#define GL_MAX_VIEWPORTS_NV 0x825B
#define GL_VIEWPORT_SUBPIXEL_BITS_NV 0x825C
#define GL_VIEWPORT_BOUNDS_RANGE_NV 0x825D
#define GL_VIEWPORT_INDEX_PROVOKING_VERTEX_NV 0x825F
#define GL_ALL_COMPLETED_NV 0x84F2
#define GL_FENCE_STATUS_NV 0x84F3
#define GL_FENCE_CONDITION_NV 0x84F4
#define GL_TEXTURE_BORDER_COLOR_NV 0x1004
#define GL_CLAMP_TO_BORDER_NV 0x812D
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#define GL_SLUMINANCE_NV 0x8C46
#define GL_SLUMINANCE_ALPHA_NV 0x8C44
#define GL_SRGB8_NV 0x8C41
#define GL_SLUMINANCE8_NV 0x8C47
#define GL_SLUMINANCE8_ALPHA8_NV 0x8C45
#define GL_COMPRESSED_SRGB_S3TC_DXT1_NV 0x8C4C
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV 0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV 0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV 0x8C4F
#define GL_ETC1_SRGB8_NV 0x88EE
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_EXT 0x88FE
#define GL_WRITEONLY_RENDERING_QCOM 0x8823
#define GL_SMAPHS30_PROGRAM_BINARY_DMP 0x9251
#define GL_SMAPHS_PROGRAM_BINARY_DMP 0x9252
#define GL_DMP_PROGRAM_BINARY_DMP 0x9253
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_TRANSLATED_SHADER_SOURCE_LENGTH_ANGLE 0x93A0
#define GL_COVERAGE_COMPONENT_NV 0x8ED0
#define GL_COVERAGE_COMPONENT4_NV 0x8ED1
#define GL_COVERAGE_ATTACHMENT_NV 0x8ED2
#define GL_COVERAGE_BUFFERS_NV 0x8ED3
#define GL_COVERAGE_SAMPLES_NV 0x8ED4
#define GL_COVERAGE_ALL_FRAGMENTS_NV 0x8ED5
#define GL_COVERAGE_EDGE_FRAGMENTS_NV 0x8ED6
#define GL_COVERAGE_AUTOMATIC_NV 0x8ED7
#define GL_COVERAGE_BUFFER_BIT_NV 0x00008000
#define GL_VERTEX_ARRAY_BINDING_OES 0x85B5
#define GL_COLOR_BUFFER_BIT0_QCOM 0x00000001
#define GL_COLOR_BUFFER_BIT1_QCOM 0x00000002
#define GL_COLOR_BUFFER_BIT2_QCOM 0x00000004
#define GL_COLOR_BUFFER_BIT3_QCOM 0x00000008
#define GL_COLOR_BUFFER_BIT4_QCOM 0x00000010
#define GL_COLOR_BUFFER_BIT5_QCOM 0x00000020
#define GL_COLOR_BUFFER_BIT6_QCOM 0x00000040
#define GL_COLOR_BUFFER_BIT7_QCOM 0x00000080
#define GL_DEPTH_BUFFER_BIT0_QCOM 0x00000100
#define GL_DEPTH_BUFFER_BIT1_QCOM 0x00000200
#define GL_DEPTH_BUFFER_BIT2_QCOM 0x00000400
#define GL_DEPTH_BUFFER_BIT3_QCOM 0x00000800
#define GL_DEPTH_BUFFER_BIT4_QCOM 0x00001000
#define GL_DEPTH_BUFFER_BIT5_QCOM 0x00002000
#define GL_DEPTH_BUFFER_BIT6_QCOM 0x00004000
#define GL_DEPTH_BUFFER_BIT7_QCOM 0x00008000
#define GL_STENCIL_BUFFER_BIT0_QCOM 0x00010000
#define GL_STENCIL_BUFFER_BIT1_QCOM 0x00020000
#define GL_STENCIL_BUFFER_BIT2_QCOM 0x00040000
#define GL_STENCIL_BUFFER_BIT3_QCOM 0x00080000
#define GL_STENCIL_BUFFER_BIT4_QCOM 0x00100000
#define GL_STENCIL_BUFFER_BIT5_QCOM 0x00200000
#define GL_STENCIL_BUFFER_BIT6_QCOM 0x00400000
#define GL_STENCIL_BUFFER_BIT7_QCOM 0x00800000
#define GL_MULTISAMPLE_BUFFER_BIT0_QCOM 0x01000000
#define GL_MULTISAMPLE_BUFFER_BIT1_QCOM 0x02000000
#define GL_MULTISAMPLE_BUFFER_BIT2_QCOM 0x04000000
#define GL_MULTISAMPLE_BUFFER_BIT3_QCOM 0x08000000
#define GL_MULTISAMPLE_BUFFER_BIT4_QCOM 0x10000000
#define GL_MULTISAMPLE_BUFFER_BIT5_QCOM 0x20000000
#define GL_MULTISAMPLE_BUFFER_BIT6_QCOM 0x40000000
#define GL_MULTISAMPLE_BUFFER_BIT7_QCOM 0x80000000
#define GL_Z400_BINARY_AMD 0x8740
#define GL_GUILTY_CONTEXT_RESET_EXT 0x8253
#define GL_INNOCENT_CONTEXT_RESET_EXT 0x8254
#define GL_UNKNOWN_CONTEXT_RESET_EXT 0x8255
#define GL_CONTEXT_ROBUST_ACCESS_EXT 0x90F3
#define GL_RESET_NOTIFICATION_STRATEGY_EXT 0x8256
#define GL_LOSE_CONTEXT_ON_RESET_EXT 0x8252
#define GL_NO_RESET_NOTIFICATION_EXT 0x8261
#define GL_CONTEXT_ROBUST_ACCESS 0x90F3
#define GL_LOSE_CONTEXT_ON_RESET 0x8252
#define GL_GUILTY_CONTEXT_RESET 0x8253
#define GL_INNOCENT_CONTEXT_RESET 0x8254
#define GL_UNKNOWN_CONTEXT_RESET 0x8255
#define GL_RESET_NOTIFICATION_STRATEGY 0x8256
#define GL_NO_RESET_NOTIFICATION 0x8261
#define GL_CONTEXT_LOST 0x0507
#define GL_CONTEXT_ROBUST_ACCESS_KHR 0x90F3
#define GL_LOSE_CONTEXT_ON_RESET_KHR 0x8252
#define GL_GUILTY_CONTEXT_RESET_KHR 0x8253
#define GL_INNOCENT_CONTEXT_RESET_KHR 0x8254
#define GL_UNKNOWN_CONTEXT_RESET_KHR 0x8255
#define GL_RESET_NOTIFICATION_STRATEGY_KHR 0x8256
#define GL_NO_RESET_NOTIFICATION_KHR 0x8261
#define GL_CONTEXT_LOST_KHR 0x0507
#define GL_TEXTURE_USAGE_ANGLE 0x93A2
#define GL_FRAMEBUFFER_ATTACHMENT_ANGLE 0x93A3
#define GL_GCCSO_SHADER_BINARY_FJ 0x9260
#define GL_QUERY_COUNTER_BITS_EXT 0x8864
#define GL_CURRENT_QUERY_EXT 0x8865
#define GL_QUERY_RESULT_EXT 0x8866
#define GL_QUERY_RESULT_AVAILABLE_EXT 0x8867
#define GL_TIME_ELAPSED_EXT 0x88BF
#define GL_TIMESTAMP_EXT 0x8E28
#define GL_GPU_DISJOINT_EXT 0x8FBB
#define GL_PALETTE4_RGB8_OES 0x8B90
#define GL_PALETTE4_RGBA8_OES 0x8B91
#define GL_PALETTE4_R5_G6_B5_OES 0x8B92
#define GL_PALETTE4_RGBA4_OES 0x8B93
#define GL_PALETTE4_RGB5_A1_OES 0x8B94
#define GL_PALETTE8_RGB8_OES 0x8B95
#define GL_PALETTE8_RGBA8_OES 0x8B96
#define GL_PALETTE8_R5_G6_B5_OES 0x8B97
#define GL_PALETTE8_RGBA4_OES 0x8B98
#define GL_PALETTE8_RGB5_A1_OES 0x8B99
#define GL_MAX_COLOR_ATTACHMENTS_NV 0x8CDF
#define GL_COLOR_ATTACHMENT0_NV 0x8CE0
#define GL_COLOR_ATTACHMENT1_NV 0x8CE1
#define GL_COLOR_ATTACHMENT2_NV 0x8CE2
#define GL_COLOR_ATTACHMENT3_NV 0x8CE3
#define GL_COLOR_ATTACHMENT4_NV 0x8CE4
#define GL_COLOR_ATTACHMENT5_NV 0x8CE5
#define GL_COLOR_ATTACHMENT6_NV 0x8CE6
#define GL_COLOR_ATTACHMENT7_NV 0x8CE7
#define GL_COLOR_ATTACHMENT8_NV 0x8CE8
#define GL_COLOR_ATTACHMENT9_NV 0x8CE9
#define GL_COLOR_ATTACHMENT10_NV 0x8CEA
#define GL_COLOR_ATTACHMENT11_NV 0x8CEB
#define GL_COLOR_ATTACHMENT12_NV 0x8CEC
#define GL_COLOR_ATTACHMENT13_NV 0x8CED
#define GL_COLOR_ATTACHMENT14_NV 0x8CEE
#define GL_COLOR_ATTACHMENT15_NV 0x8CEF
#define GL_PRIMITIVE_BOUNDING_BOX_EXT 0x92BE
#define GL_BGRA_IMG 0x80E1
#define GL_UNSIGNED_SHORT_4_4_4_4_REV_IMG 0x8365
#define GL_MAX_CLIP_DISTANCES_APPLE 0x0D32
#define GL_CLIP_DISTANCE0_APPLE 0x3000
#define GL_CLIP_DISTANCE1_APPLE 0x3001
#define GL_CLIP_DISTANCE2_APPLE 0x3002
#define GL_CLIP_DISTANCE3_APPLE 0x3003
#define GL_CLIP_DISTANCE4_APPLE 0x3004
#define GL_CLIP_DISTANCE5_APPLE 0x3005
#define GL_CLIP_DISTANCE6_APPLE 0x3006
#define GL_CLIP_DISTANCE7_APPLE 0x3007
#define GL_DEPTH_COMPONENT32_OES 0x81A7
#define GL_R8_SNORM 0x8F94
#define GL_RG8_SNORM 0x8F95
#define GL_RGBA8_SNORM 0x8F97
#define GL_R16_SNORM_EXT 0x8F98
#define GL_RG16_SNORM_EXT 0x8F99
#define GL_RGBA16_SNORM_EXT 0x8F9B
#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR 0x93B0
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR 0x93B1
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR 0x93B2
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR 0x93B3
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR 0x93B4
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR 0x93B5
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR 0x93B6
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR 0x93B7
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR 0x93B8
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR 0x93B9
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR 0x93BA
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
#define GL_GEOMETRY_SHADER_OES 0x8DD9
#define GL_GEOMETRY_SHADER_BIT_OES 0x00000004
#define GL_GEOMETRY_LINKED_VERTICES_OUT_OES 0x8916
#define GL_GEOMETRY_LINKED_INPUT_TYPE_OES 0x8917
#define GL_GEOMETRY_LINKED_OUTPUT_TYPE_OES 0x8918
#define GL_GEOMETRY_SHADER_INVOCATIONS_OES 0x887F
#define GL_LAYER_PROVOKING_VERTEX_OES 0x825E
#define GL_LINES_ADJACENCY_OES 0x000A
#define GL_LINE_STRIP_ADJACENCY_OES 0x000B
#define GL_TRIANGLES_ADJACENCY_OES 0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY_OES 0x000D
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_OES 0x8DDF
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS_OES 0x8A2C
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_OES 0x8A32
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS_OES 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_OES 0x9124
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_OES 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_OES 0x8DE1
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS_OES 0x8E5A
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_OES 0x8C29
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS_OES 0x92CF
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS_OES 0x92D5
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS_OES 0x90CD
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS_OES 0x90D7
#define GL_FIRST_VERTEX_CONVENTION_OES 0x8E4D
#define GL_LAST_VERTEX_CONVENTION_OES 0x8E4E
#define GL_UNDEFINED_VERTEX_OES 0x8260
#define GL_PRIMITIVES_GENERATED_OES 0x8C87
#define GL_FRAMEBUFFER_DEFAULT_LAYERS_OES 0x9312
#define GL_MAX_FRAMEBUFFER_LAYERS_OES 0x9317
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_OES 0x8DA8
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED_OES 0x8DA7
#define GL_REFERENCED_BY_GEOMETRY_SHADER_OES 0x9309
#define GL_PATCHES_OES 0x000E
#define GL_PATCH_VERTICES_OES 0x8E72
#define GL_TESS_CONTROL_OUTPUT_VERTICES_OES 0x8E75
#define GL_TESS_GEN_MODE_OES 0x8E76
#define GL_TESS_GEN_SPACING_OES 0x8E77
#define GL_TESS_GEN_VERTEX_ORDER_OES 0x8E78
#define GL_TESS_GEN_POINT_MODE_OES 0x8E79
#define GL_ISOLINES_OES 0x8E7A
#define GL_QUADS_OES 0x0007
#define GL_FRACTIONAL_ODD_OES 0x8E7B
#define GL_FRACTIONAL_EVEN_OES 0x8E7C
#define GL_MAX_PATCH_VERTICES_OES 0x8E7D
#define GL_MAX_TESS_GEN_LEVEL_OES 0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS_OES 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS_OES 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS_OES 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS_OES 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS_OES 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS_OES 0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS_OES 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS_OES 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS_OES 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS_OES 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS_OES 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS_OES 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS_OES 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS_OES 0x8E1F
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS_OES 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS_OES 0x92CE
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS_OES 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS_OES 0x92D4
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS_OES 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS_OES 0x90CC
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS_OES 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS_OES 0x90D9
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED_OES 0x8221
#define GL_IS_PER_PATCH_OES 0x92E7
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER_OES 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER_OES 0x9308
#define GL_TESS_CONTROL_SHADER_OES 0x8E88
#define GL_TESS_EVALUATION_SHADER_OES 0x8E87
#define GL_TESS_CONTROL_SHADER_BIT_OES 0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT_OES 0x00000010
#define GL_TEXTURE_VIEW_MIN_LEVEL_OES 0x82DB
#define GL_TEXTURE_VIEW_NUM_LEVELS_OES 0x82DC
#define GL_TEXTURE_VIEW_MIN_LAYER_OES 0x82DD
#define GL_TEXTURE_VIEW_NUM_LAYERS_OES 0x82DE
#define GL_TEXTURE_IMMUTABLE_LEVELS 0x82DF
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT 0x8D6C
#define GL_RENDERBUFFER_SAMPLES_EXT 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT 0x8D56
#define GL_MAX_SAMPLES_EXT 0x8D57
#define GL_RENDERBUFFER_SAMPLES_ANGLE 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_ANGLE 0x8D56
#define GL_MAX_SAMPLES_ANGLE 0x8D57
#define GL_TEXTURE_BUFFER_OES 0x8C2A
#define GL_TEXTURE_BUFFER_BINDING_OES 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE_OES 0x8C2B
#define GL_TEXTURE_BINDING_BUFFER_OES 0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_OES 0x8C2D
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT_OES 0x919F
#define GL_SAMPLER_BUFFER_OES 0x8DC2
#define GL_INT_SAMPLER_BUFFER_OES 0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_BUFFER_OES 0x8DD8
#define GL_IMAGE_BUFFER_OES 0x9051
#define GL_INT_IMAGE_BUFFER_OES 0x905C
#define GL_UNSIGNED_INT_IMAGE_BUFFER_OES 0x9067
#define GL_TEXTURE_BUFFER_OFFSET_OES 0x919D
#define GL_TEXTURE_BUFFER_SIZE_OES 0x919E
#define GL_BLEND_ADVANCED_COHERENT_NV 0x9285
#define GL_RGBA16F_EXT 0x881A
#define GL_RGB16F_EXT 0x881B
#define GL_RG16F_EXT 0x822F
#define GL_R16F_EXT 0x822D
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT 0x8211
#define GL_UNSIGNED_NORMALIZED_EXT 0x8C17
#define GL_PROGRAM_BINARY_LENGTH_OES 0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS_OES 0x87FE
#define GL_PROGRAM_BINARY_FORMATS_OES 0x87FF
#define GL_RENDERBUFFER_SAMPLES_APPLE 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_APPLE 0x8D56
#define GL_MAX_SAMPLES_APPLE 0x8D57
#define GL_READ_FRAMEBUFFER_APPLE 0x8CA8
#define GL_DRAW_FRAMEBUFFER_APPLE 0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_APPLE 0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_APPLE 0x8CAA
#define GL_STENCIL_INDEX4_OES 0x8D47
#define GL_COPY_READ_BUFFER_NV 0x8F36
#define GL_COPY_WRITE_BUFFER_NV 0x8F37
#define GL_HALF_FLOAT_OES 0x8D61
#define GL_TEXTURE_VIEW_MIN_LEVEL_EXT 0x82DB
#define GL_TEXTURE_VIEW_NUM_LEVELS_EXT 0x82DC
#define GL_TEXTURE_VIEW_MIN_LAYER_EXT 0x82DD
#define GL_TEXTURE_VIEW_NUM_LAYERS_EXT 0x82DE
#define GL_READ_FRAMEBUFFER_NV 0x8CA8
#define GL_DRAW_FRAMEBUFFER_NV 0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_NV 0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_NV 0x8CAA
#define GL_STENCIL_INDEX1_OES 0x8D46
#define GL_ALPHA_TEST_QCOM 0x0BC0
#define GL_ALPHA_TEST_FUNC_QCOM 0x0BC1
#define GL_ALPHA_TEST_REF_QCOM 0x0BC2
#define GL_PERFQUERY_SINGLE_CONTEXT_INTEL 0x00000000
#define GL_PERFQUERY_GLOBAL_CONTEXT_INTEL 0x00000001
#define GL_PERFQUERY_WAIT_INTEL 0x83FB
#define GL_PERFQUERY_FLUSH_INTEL 0x83FA
#define GL_PERFQUERY_DONOT_FLUSH_INTEL 0x83F9
#define GL_PERFQUERY_COUNTER_EVENT_INTEL 0x94F0
#define GL_PERFQUERY_COUNTER_DURATION_NORM_INTEL 0x94F1
#define GL_PERFQUERY_COUNTER_DURATION_RAW_INTEL 0x94F2
#define GL_PERFQUERY_COUNTER_THROUGHPUT_INTEL 0x94F3
#define GL_PERFQUERY_COUNTER_RAW_INTEL 0x94F4
#define GL_PERFQUERY_COUNTER_TIMESTAMP_INTEL 0x94F5
#define GL_PERFQUERY_COUNTER_DATA_UINT32_INTEL 0x94F8
#define GL_PERFQUERY_COUNTER_DATA_UINT64_INTEL 0x94F9
#define GL_PERFQUERY_COUNTER_DATA_FLOAT_INTEL 0x94FA
#define GL_PERFQUERY_COUNTER_DATA_DOUBLE_INTEL 0x94FB
#define GL_PERFQUERY_COUNTER_DATA_BOOL32_INTEL 0x94FC
#define GL_PERFQUERY_QUERY_NAME_LENGTH_MAX_INTEL 0x94FD
#define GL_PERFQUERY_COUNTER_NAME_LENGTH_MAX_INTEL 0x94FE
#define GL_PERFQUERY_COUNTER_DESC_LENGTH_MAX_INTEL 0x94FF
#define GL_PERFQUERY_GPA_EXTENDED_COUNTERS_INTEL 0x9500
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION 0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM 0x8245
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
#define GL_DEBUG_TYPE_POP_GROUP 0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH 0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH 0x826D
#define GL_BUFFER 0x82E0
#define GL_SHADER 0x82E1
#define GL_PROGRAM 0x82E2
#define GL_VERTEX_ARRAY 0x8074
#define GL_QUERY 0x82E3
#define GL_SAMPLER 0x82E6
#define GL_MAX_LABEL_LENGTH 0x82E8
#define GL_MAX_DEBUG_MESSAGE_LENGTH 0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES 0x9144
#define GL_DEBUG_LOGGED_MESSAGES 0x9145
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR 0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_KHR 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION_KHR 0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM_KHR 0x8245
#define GL_DEBUG_SOURCE_API_KHR 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER_KHR 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY_KHR 0x8249
#define GL_DEBUG_SOURCE_APPLICATION_KHR 0x824A
#define GL_DEBUG_SOURCE_OTHER_KHR 0x824B
#define GL_DEBUG_TYPE_ERROR_KHR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY_KHR 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE_KHR 0x8250
#define GL_DEBUG_TYPE_OTHER_KHR 0x8251
#define GL_DEBUG_TYPE_MARKER_KHR 0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP_KHR 0x8269
#define GL_DEBUG_TYPE_POP_GROUP_KHR 0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION_KHR 0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR 0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH_KHR 0x826D
#define GL_BUFFER_KHR 0x82E0
#define GL_SHADER_KHR 0x82E1
#define GL_PROGRAM_KHR 0x82E2
#define GL_VERTEX_ARRAY_KHR 0x8074
#define GL_QUERY_KHR 0x82E3
#define GL_SAMPLER_KHR 0x82E6
#define GL_MAX_LABEL_LENGTH_KHR 0x82E8
#define GL_MAX_DEBUG_MESSAGE_LENGTH_KHR 0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES_KHR 0x9144
#define GL_DEBUG_LOGGED_MESSAGES_KHR 0x9145
#define GL_DEBUG_SEVERITY_HIGH_KHR 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_KHR 0x9147
#define GL_DEBUG_SEVERITY_LOW_KHR 0x9148
#define GL_DEBUG_OUTPUT_KHR 0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT_KHR 0x00000002
#define GL_STACK_OVERFLOW_KHR 0x0503
#define GL_STACK_UNDERFLOW_KHR 0x0504
#define GL_PROGRAM_PIPELINE 0x82E4
#define GL_DISPLAY_LIST 0x82E7
#define GL_SRGB_EXT 0x8C40
#define GL_SRGB_ALPHA_EXT 0x8C42
#define GL_SRGB8_ALPHA8_EXT 0x8C43
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT 0x8210
#define GL_TEXTURE_BORDER_COLOR_OES 0x1004
#define GL_CLAMP_TO_BORDER_OES 0x812D
#define GL_PACK_REVERSE_ROW_ORDER_ANGLE 0x93A4
#define GL_ANY_SAMPLES_PASSED_EXT 0x8C2F
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT 0x8D6A
#define GL_BINNING_CONTROL_HINT_QCOM 0x8FB0
#define GL_CPU_OPTIMIZED_QCOM 0x8FB1
#define GL_GPU_OPTIMIZED_QCOM 0x8FB2
#define GL_RENDER_DIRECT_TO_FRAMEBUFFER_QCOM 0x8FB3
#define GL_MULTIPLY_KHR 0x9294
#define GL_SCREEN_KHR 0x9295
#define GL_OVERLAY_KHR 0x9296
#define GL_DARKEN_KHR 0x9297
#define GL_LIGHTEN_KHR 0x9298
#define GL_COLORDODGE_KHR 0x9299
#define GL_COLORBURN_KHR 0x929A
#define GL_HARDLIGHT_KHR 0x929B
#define GL_SOFTLIGHT_KHR 0x929C
#define GL_DIFFERENCE_KHR 0x929E
#define GL_EXCLUSION_KHR 0x92A0
#define GL_HSL_HUE_KHR 0x92AD
#define GL_HSL_SATURATION_KHR 0x92AE
#define GL_HSL_COLOR_KHR 0x92AF
#define GL_HSL_LUMINOSITY_KHR 0x92B0
#define GL_FRAMEBUFFER_UNDEFINED_OES 0x8219
#define GL_SYNC_OBJECT_APPLE 0x8A53
#define GL_MAX_SERVER_WAIT_TIMEOUT_APPLE 0x9111
#define GL_OBJECT_TYPE_APPLE 0x9112
#define GL_SYNC_CONDITION_APPLE 0x9113
#define GL_SYNC_STATUS_APPLE 0x9114
#define GL_SYNC_FLAGS_APPLE 0x9115
#define GL_SYNC_FENCE_APPLE 0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE_APPLE 0x9117
#define GL_UNSIGNALED_APPLE 0x9118
#define GL_SIGNALED_APPLE 0x9119
#define GL_ALREADY_SIGNALED_APPLE 0x911A
#define GL_TIMEOUT_EXPIRED_APPLE 0x911B
#define GL_CONDITION_SATISFIED_APPLE 0x911C
#define GL_WAIT_FAILED_APPLE 0x911D
#define GL_SYNC_FLUSH_COMMANDS_BIT_APPLE 0x00000001
#define GL_TIMEOUT_IGNORED_APPLE 0xFFFFFFFFFFFFFFFF
#define GL_SGX_PROGRAM_BINARY_IMG 0x9130
#define GL_FRAGMENT_SHADER_DISCARDS_SAMPLES_EXT 0x8A52
#define GL_PROGRAM_BINARY_ANGLE 0x93A6
#define GL_UNPACK_ROW_LENGTH_EXT 0x0CF2
#define GL_UNPACK_SKIP_ROWS_EXT 0x0CF3
#define GL_UNPACK_SKIP_PIXELS_EXT 0x0CF4
#define GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE 0x83F3
#define GL_BGRA_EXT 0x80E1
#define GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT 0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT 0x8366
#define GL_ETC1_RGB8_OES 0x8D64
#define GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT 0x8A54
#define GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT 0x8A55
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT 0x8A56
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT 0x8A57
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG 0x93F0
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG 0x93F1
#define GL_MAX_SHADER_PIXEL_LOCAL_STORAGE_FAST_SIZE_EXT 0x8F63
#define GL_MAX_SHADER_PIXEL_LOCAL_STORAGE_SIZE_EXT 0x8F67
#define GL_SHADER_PIXEL_LOCAL_STORAGE_EXT 0x8F64
#define GL_SAMPLER_CUBE_SHADOW_NV 0x8DC5
#define GL_COMPRESSED_RGBA_ASTC_3x3x3_OES 0x93C0
#define GL_COMPRESSED_RGBA_ASTC_4x3x3_OES 0x93C1
#define GL_COMPRESSED_RGBA_ASTC_4x4x3_OES 0x93C2
#define GL_COMPRESSED_RGBA_ASTC_4x4x4_OES 0x93C3
#define GL_COMPRESSED_RGBA_ASTC_5x4x4_OES 0x93C4
#define GL_COMPRESSED_RGBA_ASTC_5x5x4_OES 0x93C5
#define GL_COMPRESSED_RGBA_ASTC_5x5x5_OES 0x93C6
#define GL_COMPRESSED_RGBA_ASTC_6x5x5_OES 0x93C7
#define GL_COMPRESSED_RGBA_ASTC_6x6x5_OES 0x93C8
#define GL_COMPRESSED_RGBA_ASTC_6x6x6_OES 0x93C9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES 0x93E0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES 0x93E1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES 0x93E2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES 0x93E3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES 0x93E4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES 0x93E5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES 0x93E6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES 0x93E7
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES 0x93E8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES 0x93E9
#define GL_TEXTURE_MAX_LEVEL_APPLE 0x813D
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET_OES 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET_OES 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS_OES 0x8E5D
#define GL_TEXTURE_COMPARE_MODE_EXT 0x884C
#define GL_TEXTURE_COMPARE_FUNC_EXT 0x884D
#define GL_COMPARE_REF_TO_TEXTURE_EXT 0x884E
#define GL_SAMPLER_2D_SHADOW_EXT 0x8B62
#define GL_SGX_BINARY_IMG 0x8C0A
#define GL_ACTIVE_PROGRAM_EXT 0x8B8D
#define GL_VERTEX_SHADER_BIT_EXT 0x00000001
#define GL_FRAGMENT_SHADER_BIT_EXT 0x00000002
#define GL_ALL_SHADER_BITS_EXT 0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE_EXT 0x8258
#define GL_PROGRAM_PIPELINE_BINDING_EXT 0x825A
#define GL_RENDERBUFFER_SAMPLES_NV 0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_NV 0x8D56
#define GL_MAX_SAMPLES_NV 0x8D57
#define GL_MAX_DRAW_BUFFERS_NV 0x8824
#define GL_DRAW_BUFFER0_NV 0x8825
#define GL_DRAW_BUFFER1_NV 0x8826
#define GL_DRAW_BUFFER2_NV 0x8827
#define GL_DRAW_BUFFER3_NV 0x8828
#define GL_DRAW_BUFFER4_NV 0x8829
#define GL_DRAW_BUFFER5_NV 0x882A
#define GL_DRAW_BUFFER6_NV 0x882B
#define GL_DRAW_BUFFER7_NV 0x882C
#define GL_DRAW_BUFFER8_NV 0x882D
#define GL_DRAW_BUFFER9_NV 0x882E
#define GL_DRAW_BUFFER10_NV 0x882F
#define GL_DRAW_BUFFER11_NV 0x8830
#define GL_DRAW_BUFFER12_NV 0x8831
#define GL_DRAW_BUFFER13_NV 0x8832
#define GL_DRAW_BUFFER14_NV 0x8833
#define GL_DRAW_BUFFER15_NV 0x8834
#define GL_FETCH_PER_SAMPLE_ARM 0x8F65
#define GL_FRAGMENT_SHADER_FRAMEBUFFER_FETCH_MRT_ARM 0x8F66
#define GL_PATH_FORMAT_SVG_NV 0x9070
#define GL_PATH_FORMAT_PS_NV 0x9071
#define GL_STANDARD_FONT_NAME_NV 0x9072
#define GL_SYSTEM_FONT_NAME_NV 0x9073
#define GL_FILE_NAME_NV 0x9074
#define GL_PATH_STROKE_WIDTH_NV 0x9075
#define GL_PATH_END_CAPS_NV 0x9076
#define GL_PATH_INITIAL_END_CAP_NV 0x9077
#define GL_PATH_TERMINAL_END_CAP_NV 0x9078
#define GL_PATH_JOIN_STYLE_NV 0x9079
#define GL_PATH_MITER_LIMIT_NV 0x907A
#define GL_PATH_DASH_CAPS_NV 0x907B
#define GL_PATH_INITIAL_DASH_CAP_NV 0x907C
#define GL_PATH_TERMINAL_DASH_CAP_NV 0x907D
#define GL_PATH_DASH_OFFSET_NV 0x907E
#define GL_PATH_CLIENT_LENGTH_NV 0x907F
#define GL_PATH_FILL_MODE_NV 0x9080
#define GL_PATH_FILL_MASK_NV 0x9081
#define GL_PATH_FILL_COVER_MODE_NV 0x9082
#define GL_PATH_STROKE_COVER_MODE_NV 0x9083
#define GL_PATH_STROKE_MASK_NV 0x9084
#define GL_COUNT_UP_NV 0x9088
#define GL_COUNT_DOWN_NV 0x9089
#define GL_PATH_OBJECT_BOUNDING_BOX_NV 0x908A
#define GL_CONVEX_HULL_NV 0x908B
#define GL_BOUNDING_BOX_NV 0x908D
#define GL_TRANSLATE_X_NV 0x908E
#define GL_TRANSLATE_Y_NV 0x908F
#define GL_TRANSLATE_2D_NV 0x9090
#define GL_TRANSLATE_3D_NV 0x9091
#define GL_AFFINE_2D_NV 0x9092
#define GL_AFFINE_3D_NV 0x9094
#define GL_TRANSPOSE_AFFINE_2D_NV 0x9096
#define GL_TRANSPOSE_AFFINE_3D_NV 0x9098
#define GL_UTF8_NV 0x909A
#define GL_UTF16_NV 0x909B
#define GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV 0x909C
#define GL_PATH_COMMAND_COUNT_NV 0x909D
#define GL_PATH_COORD_COUNT_NV 0x909E
#define GL_PATH_DASH_ARRAY_COUNT_NV 0x909F
#define GL_PATH_COMPUTED_LENGTH_NV 0x90A0
#define GL_PATH_FILL_BOUNDING_BOX_NV 0x90A1
#define GL_PATH_STROKE_BOUNDING_BOX_NV 0x90A2
#define GL_SQUARE_NV 0x90A3
#define GL_ROUND_NV 0x90A4
#define GL_TRIANGULAR_NV 0x90A5
#define GL_BEVEL_NV 0x90A6
#define GL_MITER_REVERT_NV 0x90A7
#define GL_MITER_TRUNCATE_NV 0x90A8
#define GL_SKIP_MISSING_GLYPH_NV 0x90A9
#define GL_USE_MISSING_GLYPH_NV 0x90AA
#define GL_PATH_ERROR_POSITION_NV 0x90AB
#define GL_ACCUM_ADJACENT_PAIRS_NV 0x90AD
#define GL_ADJACENT_PAIRS_NV 0x90AE
#define GL_FIRST_TO_REST_NV 0x90AF
#define GL_PATH_GEN_MODE_NV 0x90B0
#define GL_PATH_GEN_COEFF_NV 0x90B1
#define GL_PATH_GEN_COMPONENTS_NV 0x90B3
#define GL_PATH_STENCIL_FUNC_NV 0x90B7
#define GL_PATH_STENCIL_REF_NV 0x90B8
#define GL_PATH_STENCIL_VALUE_MASK_NV 0x90B9
#define GL_PATH_STENCIL_DEPTH_OFFSET_FACTOR_NV 0x90BD
#define GL_PATH_STENCIL_DEPTH_OFFSET_UNITS_NV 0x90BE
#define GL_PATH_COVER_DEPTH_FUNC_NV 0x90BF
#define GL_PATH_DASH_OFFSET_RESET_NV 0x90B4
#define GL_MOVE_TO_RESETS_NV 0x90B5
#define GL_MOVE_TO_CONTINUES_NV 0x90B6
#define GL_CLOSE_PATH_NV 0x00
#define GL_MOVE_TO_NV 0x02
#define GL_RELATIVE_MOVE_TO_NV 0x03
#define GL_LINE_TO_NV 0x04
#define GL_RELATIVE_LINE_TO_NV 0x05
#define GL_HORIZONTAL_LINE_TO_NV 0x06
#define GL_RELATIVE_HORIZONTAL_LINE_TO_NV 0x07
#define GL_VERTICAL_LINE_TO_NV 0x08
#define GL_RELATIVE_VERTICAL_LINE_TO_NV 0x09
#define GL_QUADRATIC_CURVE_TO_NV 0x0A
#define GL_RELATIVE_QUADRATIC_CURVE_TO_NV 0x0B
#define GL_CUBIC_CURVE_TO_NV 0x0C
#define GL_RELATIVE_CUBIC_CURVE_TO_NV 0x0D
#define GL_SMOOTH_QUADRATIC_CURVE_TO_NV 0x0E
#define GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV 0x0F
#define GL_SMOOTH_CUBIC_CURVE_TO_NV 0x10
#define GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV 0x11
#define GL_SMALL_CCW_ARC_TO_NV 0x12
#define GL_RELATIVE_SMALL_CCW_ARC_TO_NV 0x13
#define GL_SMALL_CW_ARC_TO_NV 0x14
#define GL_RELATIVE_SMALL_CW_ARC_TO_NV 0x15
#define GL_LARGE_CCW_ARC_TO_NV 0x16
#define GL_RELATIVE_LARGE_CCW_ARC_TO_NV 0x17
#define GL_LARGE_CW_ARC_TO_NV 0x18
#define GL_RELATIVE_LARGE_CW_ARC_TO_NV 0x19
#define GL_RESTART_PATH_NV 0xF0
#define GL_DUP_FIRST_CUBIC_CURVE_TO_NV 0xF2
#define GL_DUP_LAST_CUBIC_CURVE_TO_NV 0xF4
#define GL_RECT_NV 0xF6
#define GL_CIRCULAR_CCW_ARC_TO_NV 0xF8
#define GL_CIRCULAR_CW_ARC_TO_NV 0xFA
#define GL_CIRCULAR_TANGENT_ARC_TO_NV 0xFC
#define GL_ARC_TO_NV 0xFE
#define GL_RELATIVE_ARC_TO_NV 0xFF
#define GL_BOLD_BIT_NV 0x01
#define GL_ITALIC_BIT_NV 0x02
#define GL_GLYPH_WIDTH_BIT_NV 0x01
#define GL_GLYPH_HEIGHT_BIT_NV 0x02
#define GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV 0x04
#define GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV 0x08
#define GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV 0x10
#define GL_GLYPH_VERTICAL_BEARING_X_BIT_NV 0x20
#define GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV 0x40
#define GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV 0x80
#define GL_GLYPH_HAS_KERNING_BIT_NV 0x100
#define GL_FONT_X_MIN_BOUNDS_BIT_NV 0x00010000
#define GL_FONT_Y_MIN_BOUNDS_BIT_NV 0x00020000
#define GL_FONT_X_MAX_BOUNDS_BIT_NV 0x00040000
#define GL_FONT_Y_MAX_BOUNDS_BIT_NV 0x00080000
#define GL_FONT_UNITS_PER_EM_BIT_NV 0x00100000
#define GL_FONT_ASCENDER_BIT_NV 0x00200000
#define GL_FONT_DESCENDER_BIT_NV 0x00400000
#define GL_FONT_HEIGHT_BIT_NV 0x00800000
#define GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV 0x01000000
#define GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV 0x02000000
#define GL_FONT_UNDERLINE_POSITION_BIT_NV 0x04000000
#define GL_FONT_UNDERLINE_THICKNESS_BIT_NV 0x08000000
#define GL_FONT_HAS_KERNING_BIT_NV 0x10000000
#define GL_ROUNDED_RECT_NV 0xE8
#define GL_RELATIVE_ROUNDED_RECT_NV 0xE9
#define GL_ROUNDED_RECT2_NV 0xEA
#define GL_RELATIVE_ROUNDED_RECT2_NV 0xEB
#define GL_ROUNDED_RECT4_NV 0xEC
#define GL_RELATIVE_ROUNDED_RECT4_NV 0xED
#define GL_ROUNDED_RECT8_NV 0xEE
#define GL_RELATIVE_ROUNDED_RECT8_NV 0xEF
#define GL_RELATIVE_RECT_NV 0xF7
#define GL_FONT_GLYPHS_AVAILABLE_NV 0x9368
#define GL_FONT_TARGET_UNAVAILABLE_NV 0x9369
#define GL_FONT_UNAVAILABLE_NV 0x936A
#define GL_FONT_UNINTELLIGIBLE_NV 0x936B
#define GL_CONIC_CURVE_TO_NV 0x1A
#define GL_RELATIVE_CONIC_CURVE_TO_NV 0x1B
#define GL_FONT_NUM_GLYPH_INDICES_BIT_NV 0x20000000
#define GL_STANDARD_FONT_FORMAT_NV 0x936C
#define GL_2_BYTES_NV 0x1407
#define GL_3_BYTES_NV 0x1408
#define GL_4_BYTES_NV 0x1409
#define GL_EYE_LINEAR_NV 0x2400
#define GL_OBJECT_LINEAR_NV 0x2401
#define GL_CONSTANT_NV 0x8576
#define GL_PATH_FOG_GEN_MODE_NV 0x90AC
#define GL_PRIMARY_COLOR 0x8577
#define GL_PRIMARY_COLOR_NV 0x852C
#define GL_SECONDARY_COLOR_NV 0x852D
#define GL_PATH_GEN_COLOR_FORMAT_NV 0x90B2
#define GL_PATH_PROJECTION_NV 0x1701
#define GL_PATH_MODELVIEW_NV 0x1700
#define GL_PATH_MODELVIEW_STACK_DEPTH_NV 0x0BA3
#define GL_PATH_MODELVIEW_MATRIX_NV 0x0BA6
#define GL_PATH_MAX_MODELVIEW_STACK_DEPTH_NV 0x0D36
#define GL_PATH_TRANSPOSE_MODELVIEW_MATRIX_NV 0x84E3
#define GL_PATH_PROJECTION_STACK_DEPTH_NV 0x0BA4
#define GL_PATH_PROJECTION_MATRIX_NV 0x0BA7
#define GL_PATH_MAX_PROJECTION_STACK_DEPTH_NV 0x0D38
#define GL_PATH_TRANSPOSE_PROJECTION_MATRIX_NV 0x84E4
#define GL_FRAGMENT_INPUT_NV 0x936D
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY_OES 0x9102
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY_OES 0x9105
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910D
#define GL_UNSIGNED_INT_10_10_10_2_OES 0x8DF6
#define GL_INT_10_10_10_2_OES 0x8DF7
#define GL_RGB_422_APPLE 0x8A1F
#define GL_UNSIGNED_SHORT_8_8_APPLE 0x85BA
#define GL_UNSIGNED_SHORT_8_8_REV_APPLE 0x85BB
#define GL_RGB_RAW_422_APPLE 0x8A51
#define GL_SHADER_BINARY_DMP 0x9250
#define GL_BLEND_OVERLAP_NV 0x9281
#define GL_BLEND_PREMULTIPLIED_SRC_NV 0x9280
#define GL_BLUE_NV 0x1905
#define GL_COLORBURN_NV 0x929A
#define GL_COLORDODGE_NV 0x9299
#define GL_CONJOINT_NV 0x9284
#define GL_CONTRAST_NV 0x92A1
#define GL_DARKEN_NV 0x9297
#define GL_DIFFERENCE_NV 0x929E
#define GL_DISJOINT_NV 0x9283
#define GL_DST_ATOP_NV 0x928F
#define GL_DST_IN_NV 0x928B
#define GL_DST_NV 0x9287
#define GL_DST_OUT_NV 0x928D
#define GL_DST_OVER_NV 0x9289
#define GL_EXCLUSION_NV 0x92A0
#define GL_GREEN_NV 0x1904
#define GL_HARDLIGHT_NV 0x929B
#define GL_HARDMIX_NV 0x92A9
#define GL_HSL_COLOR_NV 0x92AF
#define GL_HSL_HUE_NV 0x92AD
#define GL_HSL_LUMINOSITY_NV 0x92B0
#define GL_HSL_SATURATION_NV 0x92AE
#define GL_INVERT_OVG_NV 0x92B4
#define GL_INVERT_RGB_NV 0x92A3
#define GL_LIGHTEN_NV 0x9298
#define GL_LINEARBURN_NV 0x92A5
#define GL_LINEARDODGE_NV 0x92A4
#define GL_LINEARLIGHT_NV 0x92A7
#define GL_MINUS_CLAMPED_NV 0x92B3
#define GL_MINUS_NV 0x929F
#define GL_MULTIPLY_NV 0x9294
#define GL_OVERLAY_NV 0x9296
#define GL_PINLIGHT_NV 0x92A8
#define GL_PLUS_CLAMPED_ALPHA_NV 0x92B2
#define GL_PLUS_CLAMPED_NV 0x92B1
#define GL_PLUS_DARKER_NV 0x9292
#define GL_PLUS_NV 0x9291
#define GL_RED_NV 0x1903
#define GL_SCREEN_NV 0x9295
#define GL_SOFTLIGHT_NV 0x929C
#define GL_SRC_ATOP_NV 0x928E
#define GL_SRC_IN_NV 0x928A
#define GL_SRC_NV 0x9286
#define GL_SRC_OUT_NV 0x928C
#define GL_SRC_OVER_NV 0x9288
#define GL_UNCORRELATED_NV 0x9282
#define GL_VIVIDLIGHT_NV 0x92A6
#define GL_XOR_NV 0x1506
#define GL_MAX_COLOR_ATTACHMENTS_EXT 0x8CDF
#define GL_MAX_DRAW_BUFFERS_EXT 0x8824
#define GL_DRAW_BUFFER0_EXT 0x8825
#define GL_DRAW_BUFFER1_EXT 0x8826
#define GL_DRAW_BUFFER2_EXT 0x8827
#define GL_DRAW_BUFFER3_EXT 0x8828
#define GL_DRAW_BUFFER4_EXT 0x8829
#define GL_DRAW_BUFFER5_EXT 0x882A
#define GL_DRAW_BUFFER6_EXT 0x882B
#define GL_DRAW_BUFFER7_EXT 0x882C
#define GL_DRAW_BUFFER8_EXT 0x882D
#define GL_DRAW_BUFFER9_EXT 0x882E
#define GL_DRAW_BUFFER10_EXT 0x882F
#define GL_DRAW_BUFFER11_EXT 0x8830
#define GL_DRAW_BUFFER12_EXT 0x8831
#define GL_DRAW_BUFFER13_EXT 0x8832
#define GL_DRAW_BUFFER14_EXT 0x8833
#define GL_DRAW_BUFFER15_EXT 0x8834
#define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT 0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT 0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT 0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT 0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT 0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT 0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT 0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT 0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT 0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT 0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT 0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT 0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT 0x8CED
#define GL_COLOR_ATTACHMENT14_EXT 0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT 0x8CEF
#define GL_PROGRAM_PIPELINE_OBJECT_EXT 0x8A4F
#define GL_PROGRAM_OBJECT_EXT 0x8B40
#define GL_SHADER_OBJECT_EXT 0x8B48
#define GL_BUFFER_OBJECT_EXT 0x9151
#define GL_QUERY_OBJECT_EXT 0x9153
#define GL_VERTEX_ARRAY_OBJECT_EXT 0x9154
#define GL_TRANSFORM_FEEDBACK 0x8E22
#define GL_QUERY_WAIT_NV 0x8E13
#define GL_QUERY_NO_WAIT_NV 0x8E14
#define GL_QUERY_BY_REGION_WAIT_NV 0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT_NV 0x8E16
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#define GL_MIN_EXT 0x8007
#define GL_MAX_EXT 0x8008
#define GL_FUNC_ADD_EXT 0x8006
#define GL_BLEND_EQUATION_EXT 0x8009
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9102
#define GL_MULTISAMPLES_NV 0x9371
#define GL_SUPERSAMPLE_SCALE_X_NV 0x9372
#define GL_SUPERSAMPLE_SCALE_Y_NV 0x9373
#define GL_CONFORMANT_NV 0x9374
#define GL_DEPTH_COMPONENT24_OES 0x81A6
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_NV 0x88FE
#define GL_TEXTURE_IMMUTABLE_FORMAT_EXT 0x912F
#define GL_ALPHA8_EXT 0x803C
#define GL_LUMINANCE8_EXT 0x8040
#define GL_LUMINANCE8_ALPHA8_EXT 0x8045
#define GL_RGBA32F_EXT 0x8814
#define GL_RGB32F_EXT 0x8815
#define GL_ALPHA32F_EXT 0x8816
#define GL_LUMINANCE32F_EXT 0x8818
#define GL_LUMINANCE_ALPHA32F_EXT 0x8819
#define GL_ALPHA16F_EXT 0x881C
#define GL_LUMINANCE16F_EXT 0x881E
#define GL_LUMINANCE_ALPHA16F_EXT 0x881F
#define GL_RGB10_A2_EXT 0x8059
#define GL_RGB10_EXT 0x8052
#define GL_BGRA8_EXT 0x93A1
#define GL_R8_EXT 0x8229
#define GL_RG8_EXT 0x822B
#define GL_R32F_EXT 0x822E
#define GL_RG32F_EXT 0x8230
#define GL_ALPHA8_OES 0x803C
#define GL_DEPTH_COMPONENT16_OES 0x81A5
#define GL_LUMINANCE4_ALPHA4_OES 0x8043
#define GL_LUMINANCE8_ALPHA8_OES 0x8045
#define GL_LUMINANCE8_OES 0x8040
#define GL_RGBA4_OES 0x8056
#define GL_RGB5_A1_OES 0x8057
#define GL_RGB565_OES 0x8D62
#define GL_RGB8_OES 0x8051
#define GL_RGBA8_OES 0x8058
#define GL_FRAMEBUFFER_SRGB_EXT 0x8DB9
#define GL_3DC_X_AMD 0x87F9
#define GL_3DC_XY_AMD 0x87FA
#define GL_SAMPLE_SHADING_OES 0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE_OES 0x8C37
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG 0x9137
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG 0x9138
#define GL_MAP_READ_BIT_EXT 0x0001
#define GL_MAP_WRITE_BIT_EXT 0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT_EXT 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT_EXT 0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT_EXT 0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT_EXT 0x0020
#define GL_WRITE_ONLY_OES 0x88B9
#define GL_BUFFER_ACCESS_OES 0x88BB
#define GL_BUFFER_MAPPED_OES 0x88BC
#define GL_BUFFER_MAP_POINTER_OES 0x88BD
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#define GL_TEXTURE_BINDING_EXTERNAL_OES 0x8D67
#define GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES 0x8D68
#define GL_SAMPLER_EXTERNAL_OES 0x8D66
#define GL_COUNTER_TYPE_AMD 0x8BC0
#define GL_COUNTER_RANGE_AMD 0x8BC1
#define GL_UNSIGNED_INT64_AMD 0x8BC2
#define GL_PERCENTAGE_AMD 0x8BC3
#define GL_PERFMON_RESULT_AVAILABLE_AMD 0x8BC4
#define GL_PERFMON_RESULT_SIZE_AMD 0x8BC5
#define GL_PERFMON_RESULT_AMD 0x8BC6
#define GL_SHADER_BINARY_VIV 0x8FC4
#define GL_SAMPLER_2D_ARRAY_SHADOW_NV 0x8DC4
#define GL_RENDERBUFFER_SAMPLES_IMG 0x9133
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_IMG 0x9134
#define GL_MAX_SAMPLES_IMG 0x9135
#define GL_TEXTURE_SAMPLES_IMG 0x9136
#define GL_DEPTH_COMPONENT16_NONLINEAR_NV 0x8E2C
#define GL_TEXTURE_CUBE_MAP_ARRAY_OES 0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY_OES 0x900A
#define GL_SAMPLER_CUBE_MAP_ARRAY_OES 0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_OES 0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY_OES 0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_OES 0x900F
#define GL_IMAGE_CUBE_MAP_ARRAY_OES 0x9054
#define GL_INT_IMAGE_CUBE_MAP_ARRAY_OES 0x905F
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY_OES 0x906A
#define GL_MALI_SHADER_BINARY_ARM 0x8F60
#define GL_COLOR_ATTACHMENT_EXT 0x90F0
#define GL_MULTIVIEW_EXT 0x90F1
#define GL_DRAW_BUFFER_EXT 0x0C01
#define GL_READ_BUFFER_EXT 0x0C02
#define GL_MAX_MULTIVIEW_BUFFERS_EXT 0x90F2
#define GL_STENCIL_INDEX_OES 0x1901
#define GL_STENCIL_INDEX8_OES 0x8D48
#define GL_TEXTURE_WIDTH_QCOM 0x8BD2
#define GL_TEXTURE_HEIGHT_QCOM 0x8BD3
#define GL_TEXTURE_DEPTH_QCOM 0x8BD4
#define GL_TEXTURE_INTERNAL_FORMAT_QCOM 0x8BD5
#define GL_TEXTURE_FORMAT_QCOM 0x8BD6
#define GL_TEXTURE_TYPE_QCOM 0x8BD7
#define GL_TEXTURE_IMAGE_VALID_QCOM 0x8BD8
#define GL_TEXTURE_NUM_LEVELS_QCOM 0x8BD9
#define GL_TEXTURE_TARGET_QCOM 0x8BDA
#define GL_TEXTURE_OBJECT_VALID_QCOM 0x8BDB
#define GL_STATE_RESTORE 0x8BDC
#define GL_GEOMETRY_SHADER_EXT 0x8DD9
#define GL_GEOMETRY_SHADER_BIT_EXT 0x00000004
#define GL_GEOMETRY_LINKED_VERTICES_OUT_EXT 0x8916
#define GL_GEOMETRY_LINKED_INPUT_TYPE_EXT 0x8917
#define GL_GEOMETRY_LINKED_OUTPUT_TYPE_EXT 0x8918
#define GL_GEOMETRY_SHADER_INVOCATIONS_EXT 0x887F
#define GL_LAYER_PROVOKING_VERTEX_EXT 0x825E
#define GL_LINES_ADJACENCY_EXT 0x000A
#define GL_LINE_STRIP_ADJACENCY_EXT 0x000B
#define GL_TRIANGLES_ADJACENCY_EXT 0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY_EXT 0x000D
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT 0x8DDF
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS_EXT 0x8A2C
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_EXT 0x8A32
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS_EXT 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_EXT 0x9124
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_EXT 0x8DE1
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS_EXT 0x8E5A
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_EXT 0x8C29
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS_EXT 0x92CF
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS_EXT 0x92D5
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS_EXT 0x90CD
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS_EXT 0x90D7
#define GL_FIRST_VERTEX_CONVENTION_EXT 0x8E4D
#define GL_LAST_VERTEX_CONVENTION_EXT 0x8E4E
#define GL_UNDEFINED_VERTEX_EXT 0x8260
#define GL_PRIMITIVES_GENERATED_EXT 0x8C87
#define GL_FRAMEBUFFER_DEFAULT_LAYERS_EXT 0x9312
#define GL_MAX_FRAMEBUFFER_LAYERS_EXT 0x9317
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT 0x8DA8
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT 0x8DA7
#define GL_REFERENCED_BY_GEOMETRY_SHADER_EXT 0x9309
#define GL_R16_EXT 0x822A
#define GL_RG16_EXT 0x822C
#define GL_RGBA16_EXT 0x805B
#define GL_RGB16_EXT 0x8054
#define GL_RGB16_SNORM_EXT 0x8F9A
#define GL_RED_EXT 0x1903
#define GL_RG_EXT 0x8227
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES 0x8B8B
#define GL_TEXTURE_BORDER_COLOR_EXT 0x1004
#define GL_CLAMP_TO_BORDER_EXT 0x812D
#define GL_READ_FRAMEBUFFER_ANGLE 0x8CA8
#define GL_DRAW_FRAMEBUFFER_ANGLE 0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_ANGLE 0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_ANGLE 0x8CAA
#define GL_TEXTURE_WRAP_R_OES 0x8072
#define GL_TEXTURE_3D_OES 0x806F
#define GL_TEXTURE_BINDING_3D_OES 0x806A
#define GL_MAX_3D_TEXTURE_SIZE_OES 0x8073
#define GL_SAMPLER_3D_OES 0x8B5F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES 0x8CD4
#define GL_PATCHES_EXT 0x000E
#define GL_PATCH_VERTICES_EXT 0x8E72
#define GL_TESS_CONTROL_OUTPUT_VERTICES_EXT 0x8E75
#define GL_TESS_GEN_MODE_EXT 0x8E76
#define GL_TESS_GEN_SPACING_EXT 0x8E77
#define GL_TESS_GEN_VERTEX_ORDER_EXT 0x8E78
#define GL_TESS_GEN_POINT_MODE_EXT 0x8E79
#define GL_ISOLINES_EXT 0x8E7A
#define GL_QUADS_EXT 0x0007
#define GL_FRACTIONAL_ODD_EXT 0x8E7B
#define GL_FRACTIONAL_EVEN_EXT 0x8E7C
#define GL_MAX_PATCH_VERTICES_EXT 0x8E7D
#define GL_MAX_TESS_GEN_LEVEL_EXT 0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS_EXT 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS_EXT 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS_EXT 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS_EXT 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS_EXT 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS_EXT 0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS_EXT 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS_EXT 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS_EXT 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS_EXT 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS_EXT 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS_EXT 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS_EXT 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS_EXT 0x8E1F
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS_EXT 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS_EXT 0x92CE
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS_EXT 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS_EXT 0x92D4
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS_EXT 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS_EXT 0x90CC
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS_EXT 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS_EXT 0x90D9
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
#define GL_IS_PER_PATCH_EXT 0x92E7
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER_EXT 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER_EXT 0x9308
#define GL_TESS_CONTROL_SHADER_EXT 0x8E88
#define GL_TESS_EVALUATION_SHADER_EXT 0x8E87
#define GL_TESS_CONTROL_SHADER_BIT_EXT 0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT_EXT 0x00000010
#define GL_PRIMITIVE_BOUNDING_BOX_OES 0x92BE
#define GL_READ_BUFFER_NV 0x0C02
#define GL_TEXTURE_CUBE_MAP_ARRAY_EXT 0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY_EXT 0x900A
#define GL_SAMPLER_CUBE_MAP_ARRAY_EXT 0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_EXT 0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY_EXT 0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_EXT 0x900F
#define GL_IMAGE_CUBE_MAP_ARRAY_EXT 0x9054
#define GL_INT_IMAGE_CUBE_MAP_ARRAY_EXT 0x905F
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY_EXT 0x906A
#define GL_CONTEXT_RELEASE_BEHAVIOR 0x82FB
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82FC
#define GL_CONTEXT_RELEASE_BEHAVIOR_KHR 0x82FB
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR 0x82FC
#define GL_UNSIGNED_INT_2_10_10_10_REV_EXT 0x8368
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE 0x88FE
#define GL_COLOR_EXT 0x1800
#define GL_DEPTH_EXT 0x1801
#define GL_STENCIL_EXT 0x1802
#define GL_TEXTURE_SRGB_DECODE_EXT 0x8A48
#define GL_DECODE_EXT 0x8A49
#define GL_SKIP_DECODE_EXT 0x8A4A
#define GL_MALI_PROGRAM_BINARY_ARM 0x8F61
#define GL_ATC_RGB_AMD 0x8C92
#define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE
#define GL_UNSIGNED_INT_10F_11F_11F_REV_APPLE 0x8C3B
#define GL_UNSIGNED_INT_5_9_9_9_REV_APPLE 0x8C3E
#define GL_R11F_G11F_B10F_APPLE 0x8C3A
#define GL_RGB9_E5_APPLE 0x8C3D
#define GL_TEXTURE_BUFFER_EXT 0x8C2A
#define GL_TEXTURE_BUFFER_BINDING_EXT 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE_EXT 0x8C2B
#define GL_TEXTURE_BINDING_BUFFER_EXT 0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_EXT 0x8C2D
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT_EXT 0x919F
#define GL_SAMPLER_BUFFER_EXT 0x8DC2
#define GL_INT_SAMPLER_BUFFER_EXT 0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT 0x8DD8
#define GL_IMAGE_BUFFER_EXT 0x9051
#define GL_INT_IMAGE_BUFFER_EXT 0x905C
#define GL_UNSIGNED_INT_IMAGE_BUFFER_EXT 0x9067
#define GL_TEXTURE_BUFFER_OFFSET_EXT 0x919D
#define GL_TEXTURE_BUFFER_SIZE_EXT 0x919E
#ifndef GL_NV_non_square_matrices
#define GL_NV_non_square_matrices 1
GLAPI int GLAD_GL_NV_non_square_matrices;
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3FVNVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX2X3FVNVPROC glad_glUniformMatrix2x3fvNV;
#define glUniformMatrix2x3fvNV glad_glUniformMatrix2x3fvNV
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2FVNVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX3X2FVNVPROC glad_glUniformMatrix3x2fvNV;
#define glUniformMatrix3x2fvNV glad_glUniformMatrix3x2fvNV
typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4FVNVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX2X4FVNVPROC glad_glUniformMatrix2x4fvNV;
#define glUniformMatrix2x4fvNV glad_glUniformMatrix2x4fvNV
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2FVNVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX4X2FVNVPROC glad_glUniformMatrix4x2fvNV;
#define glUniformMatrix4x2fvNV glad_glUniformMatrix4x2fvNV
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4FVNVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX3X4FVNVPROC glad_glUniformMatrix3x4fvNV;
#define glUniformMatrix3x4fvNV glad_glUniformMatrix3x4fvNV
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3FVNVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLUNIFORMMATRIX4X3FVNVPROC glad_glUniformMatrix4x3fvNV;
#define glUniformMatrix4x3fvNV glad_glUniformMatrix4x3fvNV
#endif
#ifndef GL_OES_packed_depth_stencil
#define GL_OES_packed_depth_stencil 1
GLAPI int GLAD_GL_OES_packed_depth_stencil;
#endif
#ifndef GL_QCOM_perfmon_global_mode
#define GL_QCOM_perfmon_global_mode 1
GLAPI int GLAD_GL_QCOM_perfmon_global_mode;
#endif
#ifndef GL_NV_draw_instanced
#define GL_NV_draw_instanced 1
GLAPI int GLAD_GL_NV_draw_instanced;
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDNVPROC)(GLenum, GLint, GLsizei, GLsizei);
GLAPI PFNGLDRAWARRAYSINSTANCEDNVPROC glad_glDrawArraysInstancedNV;
#define glDrawArraysInstancedNV glad_glDrawArraysInstancedNV
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDNVPROC)(GLenum, GLsizei, GLenum, const void*, GLsizei);
GLAPI PFNGLDRAWELEMENTSINSTANCEDNVPROC glad_glDrawElementsInstancedNV;
#define glDrawElementsInstancedNV glad_glDrawElementsInstancedNV
#endif
#ifndef GL_OES_texture_half_float_linear
#define GL_OES_texture_half_float_linear 1
GLAPI int GLAD_GL_OES_texture_half_float_linear;
#endif
#ifndef GL_KHR_blend_equation_advanced_coherent
#define GL_KHR_blend_equation_advanced_coherent 1
GLAPI int GLAD_GL_KHR_blend_equation_advanced_coherent;
#endif
#ifndef GL_NV_viewport_array
#define GL_NV_viewport_array 1
GLAPI int GLAD_GL_NV_viewport_array;
typedef void (APIENTRYP PFNGLVIEWPORTARRAYVNVPROC)(GLuint, GLsizei, const GLfloat*);
GLAPI PFNGLVIEWPORTARRAYVNVPROC glad_glViewportArrayvNV;
#define glViewportArrayvNV glad_glViewportArrayvNV
typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFNVPROC)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLVIEWPORTINDEXEDFNVPROC glad_glViewportIndexedfNV;
#define glViewportIndexedfNV glad_glViewportIndexedfNV
typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFVNVPROC)(GLuint, const GLfloat*);
GLAPI PFNGLVIEWPORTINDEXEDFVNVPROC glad_glViewportIndexedfvNV;
#define glViewportIndexedfvNV glad_glViewportIndexedfvNV
typedef void (APIENTRYP PFNGLSCISSORARRAYVNVPROC)(GLuint, GLsizei, const GLint*);
GLAPI PFNGLSCISSORARRAYVNVPROC glad_glScissorArrayvNV;
#define glScissorArrayvNV glad_glScissorArrayvNV
typedef void (APIENTRYP PFNGLSCISSORINDEXEDNVPROC)(GLuint, GLint, GLint, GLsizei, GLsizei);
GLAPI PFNGLSCISSORINDEXEDNVPROC glad_glScissorIndexedNV;
#define glScissorIndexedNV glad_glScissorIndexedNV
typedef void (APIENTRYP PFNGLSCISSORINDEXEDVNVPROC)(GLuint, const GLint*);
GLAPI PFNGLSCISSORINDEXEDVNVPROC glad_glScissorIndexedvNV;
#define glScissorIndexedvNV glad_glScissorIndexedvNV
typedef void (APIENTRYP PFNGLDEPTHRANGEARRAYFVNVPROC)(GLuint, GLsizei, const GLfloat*);
GLAPI PFNGLDEPTHRANGEARRAYFVNVPROC glad_glDepthRangeArrayfvNV;
#define glDepthRangeArrayfvNV glad_glDepthRangeArrayfvNV
typedef void (APIENTRYP PFNGLDEPTHRANGEINDEXEDFNVPROC)(GLuint, GLfloat, GLfloat);
GLAPI PFNGLDEPTHRANGEINDEXEDFNVPROC glad_glDepthRangeIndexedfNV;
#define glDepthRangeIndexedfNV glad_glDepthRangeIndexedfNV
typedef void (APIENTRYP PFNGLGETFLOATI_VNVPROC)(GLenum, GLuint, GLfloat*);
GLAPI PFNGLGETFLOATI_VNVPROC glad_glGetFloati_vNV;
#define glGetFloati_vNV glad_glGetFloati_vNV
typedef void (APIENTRYP PFNGLENABLEINVPROC)(GLenum, GLuint);
GLAPI PFNGLENABLEINVPROC glad_glEnableiNV;
#define glEnableiNV glad_glEnableiNV
typedef void (APIENTRYP PFNGLDISABLEINVPROC)(GLenum, GLuint);
GLAPI PFNGLDISABLEINVPROC glad_glDisableiNV;
#define glDisableiNV glad_glDisableiNV
typedef GLboolean (APIENTRYP PFNGLISENABLEDINVPROC)(GLenum, GLuint);
GLAPI PFNGLISENABLEDINVPROC glad_glIsEnablediNV;
#define glIsEnablediNV glad_glIsEnablediNV
#endif
#ifndef GL_NV_fence
#define GL_NV_fence 1
GLAPI int GLAD_GL_NV_fence;
typedef void (APIENTRYP PFNGLDELETEFENCESNVPROC)(GLsizei, const GLuint*);
GLAPI PFNGLDELETEFENCESNVPROC glad_glDeleteFencesNV;
#define glDeleteFencesNV glad_glDeleteFencesNV
typedef void (APIENTRYP PFNGLGENFENCESNVPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENFENCESNVPROC glad_glGenFencesNV;
#define glGenFencesNV glad_glGenFencesNV
typedef GLboolean (APIENTRYP PFNGLISFENCENVPROC)(GLuint);
GLAPI PFNGLISFENCENVPROC glad_glIsFenceNV;
#define glIsFenceNV glad_glIsFenceNV
typedef GLboolean (APIENTRYP PFNGLTESTFENCENVPROC)(GLuint);
GLAPI PFNGLTESTFENCENVPROC glad_glTestFenceNV;
#define glTestFenceNV glad_glTestFenceNV
typedef void (APIENTRYP PFNGLGETFENCEIVNVPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETFENCEIVNVPROC glad_glGetFenceivNV;
#define glGetFenceivNV glad_glGetFenceivNV
typedef void (APIENTRYP PFNGLFINISHFENCENVPROC)(GLuint);
GLAPI PFNGLFINISHFENCENVPROC glad_glFinishFenceNV;
#define glFinishFenceNV glad_glFinishFenceNV
typedef void (APIENTRYP PFNGLSETFENCENVPROC)(GLuint, GLenum);
GLAPI PFNGLSETFENCENVPROC glad_glSetFenceNV;
#define glSetFenceNV glad_glSetFenceNV
#endif
#ifndef GL_NV_texture_border_clamp
#define GL_NV_texture_border_clamp 1
GLAPI int GLAD_GL_NV_texture_border_clamp;
#endif
#ifndef GL_IMG_texture_compression_pvrtc
#define GL_IMG_texture_compression_pvrtc 1
GLAPI int GLAD_GL_IMG_texture_compression_pvrtc;
#endif
#ifndef GL_NV_sRGB_formats
#define GL_NV_sRGB_formats 1
GLAPI int GLAD_GL_NV_sRGB_formats;
#endif
#ifndef GL_EXT_texture_compression_s3tc
#define GL_EXT_texture_compression_s3tc 1
GLAPI int GLAD_GL_EXT_texture_compression_s3tc;
#endif
#ifndef GL_EXT_base_instance
#define GL_EXT_base_instance 1
GLAPI int GLAD_GL_EXT_base_instance;
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEEXTPROC)(GLenum, GLint, GLsizei, GLsizei, GLuint);
GLAPI PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEEXTPROC glad_glDrawArraysInstancedBaseInstanceEXT;
#define glDrawArraysInstancedBaseInstanceEXT glad_glDrawArraysInstancedBaseInstanceEXT
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEEXTPROC)(GLenum, GLsizei, GLenum, const void*, GLsizei, GLuint);
GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEEXTPROC glad_glDrawElementsInstancedBaseInstanceEXT;
#define glDrawElementsInstancedBaseInstanceEXT glad_glDrawElementsInstancedBaseInstanceEXT
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEEXTPROC)(GLenum, GLsizei, GLenum, const void*, GLsizei, GLint, GLuint);
GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEEXTPROC glad_glDrawElementsInstancedBaseVertexBaseInstanceEXT;
#define glDrawElementsInstancedBaseVertexBaseInstanceEXT glad_glDrawElementsInstancedBaseVertexBaseInstanceEXT
#endif
#ifndef GL_EXT_instanced_arrays
#define GL_EXT_instanced_arrays 1
GLAPI int GLAD_GL_EXT_instanced_arrays;
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDEXTPROC)(GLenum, GLint, GLsizei, GLsizei);
GLAPI PFNGLDRAWARRAYSINSTANCEDEXTPROC glad_glDrawArraysInstancedEXT;
#define glDrawArraysInstancedEXT glad_glDrawArraysInstancedEXT
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDEXTPROC)(GLenum, GLsizei, GLenum, const void*, GLsizei);
GLAPI PFNGLDRAWELEMENTSINSTANCEDEXTPROC glad_glDrawElementsInstancedEXT;
#define glDrawElementsInstancedEXT glad_glDrawElementsInstancedEXT
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISOREXTPROC)(GLuint, GLuint);
GLAPI PFNGLVERTEXATTRIBDIVISOREXTPROC glad_glVertexAttribDivisorEXT;
#define glVertexAttribDivisorEXT glad_glVertexAttribDivisorEXT
#endif
#ifndef GL_QCOM_writeonly_rendering
#define GL_QCOM_writeonly_rendering 1
GLAPI int GLAD_GL_QCOM_writeonly_rendering;
#endif
#ifndef GL_DMP_program_binary
#define GL_DMP_program_binary 1
GLAPI int GLAD_GL_DMP_program_binary;
#endif
#ifndef GL_EXT_draw_buffers_indexed
#define GL_EXT_draw_buffers_indexed 1
GLAPI int GLAD_GL_EXT_draw_buffers_indexed;
typedef void (APIENTRYP PFNGLENABLEIEXTPROC)(GLenum, GLuint);
GLAPI PFNGLENABLEIEXTPROC glad_glEnableiEXT;
#define glEnableiEXT glad_glEnableiEXT
typedef void (APIENTRYP PFNGLDISABLEIEXTPROC)(GLenum, GLuint);
GLAPI PFNGLDISABLEIEXTPROC glad_glDisableiEXT;
#define glDisableiEXT glad_glDisableiEXT
typedef void (APIENTRYP PFNGLBLENDEQUATIONIEXTPROC)(GLuint, GLenum);
GLAPI PFNGLBLENDEQUATIONIEXTPROC glad_glBlendEquationiEXT;
#define glBlendEquationiEXT glad_glBlendEquationiEXT
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIEXTPROC)(GLuint, GLenum, GLenum);
GLAPI PFNGLBLENDEQUATIONSEPARATEIEXTPROC glad_glBlendEquationSeparateiEXT;
#define glBlendEquationSeparateiEXT glad_glBlendEquationSeparateiEXT
typedef void (APIENTRYP PFNGLBLENDFUNCIEXTPROC)(GLuint, GLenum, GLenum);
GLAPI PFNGLBLENDFUNCIEXTPROC glad_glBlendFunciEXT;
#define glBlendFunciEXT glad_glBlendFunciEXT
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIEXTPROC)(GLuint, GLenum, GLenum, GLenum, GLenum);
GLAPI PFNGLBLENDFUNCSEPARATEIEXTPROC glad_glBlendFuncSeparateiEXT;
#define glBlendFuncSeparateiEXT glad_glBlendFuncSeparateiEXT
typedef void (APIENTRYP PFNGLCOLORMASKIEXTPROC)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean);
GLAPI PFNGLCOLORMASKIEXTPROC glad_glColorMaskiEXT;
#define glColorMaskiEXT glad_glColorMaskiEXT
typedef GLboolean (APIENTRYP PFNGLISENABLEDIEXTPROC)(GLenum, GLuint);
GLAPI PFNGLISENABLEDIEXTPROC glad_glIsEnablediEXT;
#define glIsEnablediEXT glad_glIsEnablediEXT
#endif
#ifndef GL_NV_read_depth_stencil
#define GL_NV_read_depth_stencil 1
GLAPI int GLAD_GL_NV_read_depth_stencil;
#endif
#ifndef GL_ANGLE_translated_shader_source
#define GL_ANGLE_translated_shader_source 1
GLAPI int GLAD_GL_ANGLE_translated_shader_source;
typedef void (APIENTRYP PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC glad_glGetTranslatedShaderSourceANGLE;
#define glGetTranslatedShaderSourceANGLE glad_glGetTranslatedShaderSourceANGLE
#endif
#ifndef GL_NV_coverage_sample
#define GL_NV_coverage_sample 1
GLAPI int GLAD_GL_NV_coverage_sample;
typedef void (APIENTRYP PFNGLCOVERAGEMASKNVPROC)(GLboolean);
GLAPI PFNGLCOVERAGEMASKNVPROC glad_glCoverageMaskNV;
#define glCoverageMaskNV glad_glCoverageMaskNV
typedef void (APIENTRYP PFNGLCOVERAGEOPERATIONNVPROC)(GLenum);
GLAPI PFNGLCOVERAGEOPERATIONNVPROC glad_glCoverageOperationNV;
#define glCoverageOperationNV glad_glCoverageOperationNV
#endif
#ifndef GL_OES_vertex_array_object
#define GL_OES_vertex_array_object 1
GLAPI int GLAD_GL_OES_vertex_array_object;
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYOESPROC)(GLuint);
GLAPI PFNGLBINDVERTEXARRAYOESPROC glad_glBindVertexArrayOES;
#define glBindVertexArrayOES glad_glBindVertexArrayOES
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSOESPROC)(GLsizei, const GLuint*);
GLAPI PFNGLDELETEVERTEXARRAYSOESPROC glad_glDeleteVertexArraysOES;
#define glDeleteVertexArraysOES glad_glDeleteVertexArraysOES
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSOESPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENVERTEXARRAYSOESPROC glad_glGenVertexArraysOES;
#define glGenVertexArraysOES glad_glGenVertexArraysOES
typedef GLboolean (APIENTRYP PFNGLISVERTEXARRAYOESPROC)(GLuint);
GLAPI PFNGLISVERTEXARRAYOESPROC glad_glIsVertexArrayOES;
#define glIsVertexArrayOES glad_glIsVertexArrayOES
#endif
#ifndef GL_QCOM_tiled_rendering
#define GL_QCOM_tiled_rendering 1
GLAPI int GLAD_GL_QCOM_tiled_rendering;
typedef void (APIENTRYP PFNGLSTARTTILINGQCOMPROC)(GLuint, GLuint, GLuint, GLuint, GLbitfield);
GLAPI PFNGLSTARTTILINGQCOMPROC glad_glStartTilingQCOM;
#define glStartTilingQCOM glad_glStartTilingQCOM
typedef void (APIENTRYP PFNGLENDTILINGQCOMPROC)(GLbitfield);
GLAPI PFNGLENDTILINGQCOMPROC glad_glEndTilingQCOM;
#define glEndTilingQCOM glad_glEndTilingQCOM
#endif
#ifndef GL_AMD_program_binary_Z400
#define GL_AMD_program_binary_Z400 1
GLAPI int GLAD_GL_AMD_program_binary_Z400;
#endif
#ifndef GL_APPLE_color_buffer_packed_float
#define GL_APPLE_color_buffer_packed_float 1
GLAPI int GLAD_GL_APPLE_color_buffer_packed_float;
#endif
#ifndef GL_EXT_robustness
#define GL_EXT_robustness 1
GLAPI int GLAD_GL_EXT_robustness;
typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSEXTPROC)();
GLAPI PFNGLGETGRAPHICSRESETSTATUSEXTPROC glad_glGetGraphicsResetStatusEXT;
#define glGetGraphicsResetStatusEXT glad_glGetGraphicsResetStatusEXT
typedef void (APIENTRYP PFNGLREADNPIXELSEXTPROC)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void*);
GLAPI PFNGLREADNPIXELSEXTPROC glad_glReadnPixelsEXT;
#define glReadnPixelsEXT glad_glReadnPixelsEXT
typedef void (APIENTRYP PFNGLGETNUNIFORMFVEXTPROC)(GLuint, GLint, GLsizei, GLfloat*);
GLAPI PFNGLGETNUNIFORMFVEXTPROC glad_glGetnUniformfvEXT;
#define glGetnUniformfvEXT glad_glGetnUniformfvEXT
typedef void (APIENTRYP PFNGLGETNUNIFORMIVEXTPROC)(GLuint, GLint, GLsizei, GLint*);
GLAPI PFNGLGETNUNIFORMIVEXTPROC glad_glGetnUniformivEXT;
#define glGetnUniformivEXT glad_glGetnUniformivEXT
#endif
#ifndef GL_OES_fbo_render_mipmap
#define GL_OES_fbo_render_mipmap 1
GLAPI int GLAD_GL_OES_fbo_render_mipmap;
#endif
#ifndef GL_KHR_robustness
#define GL_KHR_robustness 1
GLAPI int GLAD_GL_KHR_robustness;
typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSPROC)();
GLAPI PFNGLGETGRAPHICSRESETSTATUSPROC glad_glGetGraphicsResetStatus;
#define glGetGraphicsResetStatus glad_glGetGraphicsResetStatus
typedef void (APIENTRYP PFNGLREADNPIXELSPROC)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void*);
GLAPI PFNGLREADNPIXELSPROC glad_glReadnPixels;
#define glReadnPixels glad_glReadnPixels
typedef void (APIENTRYP PFNGLGETNUNIFORMFVPROC)(GLuint, GLint, GLsizei, GLfloat*);
GLAPI PFNGLGETNUNIFORMFVPROC glad_glGetnUniformfv;
#define glGetnUniformfv glad_glGetnUniformfv
typedef void (APIENTRYP PFNGLGETNUNIFORMIVPROC)(GLuint, GLint, GLsizei, GLint*);
GLAPI PFNGLGETNUNIFORMIVPROC glad_glGetnUniformiv;
#define glGetnUniformiv glad_glGetnUniformiv
typedef void (APIENTRYP PFNGLGETNUNIFORMUIVPROC)(GLuint, GLint, GLsizei, GLuint*);
GLAPI PFNGLGETNUNIFORMUIVPROC glad_glGetnUniformuiv;
#define glGetnUniformuiv glad_glGetnUniformuiv
typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSKHRPROC)();
GLAPI PFNGLGETGRAPHICSRESETSTATUSKHRPROC glad_glGetGraphicsResetStatusKHR;
#define glGetGraphicsResetStatusKHR glad_glGetGraphicsResetStatusKHR
typedef void (APIENTRYP PFNGLREADNPIXELSKHRPROC)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void*);
GLAPI PFNGLREADNPIXELSKHRPROC glad_glReadnPixelsKHR;
#define glReadnPixelsKHR glad_glReadnPixelsKHR
typedef void (APIENTRYP PFNGLGETNUNIFORMFVKHRPROC)(GLuint, GLint, GLsizei, GLfloat*);
GLAPI PFNGLGETNUNIFORMFVKHRPROC glad_glGetnUniformfvKHR;
#define glGetnUniformfvKHR glad_glGetnUniformfvKHR
typedef void (APIENTRYP PFNGLGETNUNIFORMIVKHRPROC)(GLuint, GLint, GLsizei, GLint*);
GLAPI PFNGLGETNUNIFORMIVKHRPROC glad_glGetnUniformivKHR;
#define glGetnUniformivKHR glad_glGetnUniformivKHR
typedef void (APIENTRYP PFNGLGETNUNIFORMUIVKHRPROC)(GLuint, GLint, GLsizei, GLuint*);
GLAPI PFNGLGETNUNIFORMUIVKHRPROC glad_glGetnUniformuivKHR;
#define glGetnUniformuivKHR glad_glGetnUniformuivKHR
#endif
#ifndef GL_OES_compressed_ETC1_RGB8_sub_texture
#define GL_OES_compressed_ETC1_RGB8_sub_texture 1
GLAPI int GLAD_GL_OES_compressed_ETC1_RGB8_sub_texture;
#endif
#ifndef GL_ANGLE_texture_usage
#define GL_ANGLE_texture_usage 1
GLAPI int GLAD_GL_ANGLE_texture_usage;
#endif
#ifndef GL_EXT_gpu_shader5
#define GL_EXT_gpu_shader5 1
GLAPI int GLAD_GL_EXT_gpu_shader5;
#endif
#ifndef GL_OES_shader_io_blocks
#define GL_OES_shader_io_blocks 1
GLAPI int GLAD_GL_OES_shader_io_blocks;
#endif
#ifndef GL_FJ_shader_binary_GCCSO
#define GL_FJ_shader_binary_GCCSO 1
GLAPI int GLAD_GL_FJ_shader_binary_GCCSO;
#endif
#ifndef GL_EXT_disjoint_timer_query
#define GL_EXT_disjoint_timer_query 1
GLAPI int GLAD_GL_EXT_disjoint_timer_query;
typedef void (APIENTRYP PFNGLGENQUERIESEXTPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENQUERIESEXTPROC glad_glGenQueriesEXT;
#define glGenQueriesEXT glad_glGenQueriesEXT
typedef void (APIENTRYP PFNGLDELETEQUERIESEXTPROC)(GLsizei, const GLuint*);
GLAPI PFNGLDELETEQUERIESEXTPROC glad_glDeleteQueriesEXT;
#define glDeleteQueriesEXT glad_glDeleteQueriesEXT
typedef GLboolean (APIENTRYP PFNGLISQUERYEXTPROC)(GLuint);
GLAPI PFNGLISQUERYEXTPROC glad_glIsQueryEXT;
#define glIsQueryEXT glad_glIsQueryEXT
typedef void (APIENTRYP PFNGLBEGINQUERYEXTPROC)(GLenum, GLuint);
GLAPI PFNGLBEGINQUERYEXTPROC glad_glBeginQueryEXT;
#define glBeginQueryEXT glad_glBeginQueryEXT
typedef void (APIENTRYP PFNGLENDQUERYEXTPROC)(GLenum);
GLAPI PFNGLENDQUERYEXTPROC glad_glEndQueryEXT;
#define glEndQueryEXT glad_glEndQueryEXT
typedef void (APIENTRYP PFNGLQUERYCOUNTEREXTPROC)(GLuint, GLenum);
GLAPI PFNGLQUERYCOUNTEREXTPROC glad_glQueryCounterEXT;
#define glQueryCounterEXT glad_glQueryCounterEXT
typedef void (APIENTRYP PFNGLGETQUERYIVEXTPROC)(GLenum, GLenum, GLint*);
GLAPI PFNGLGETQUERYIVEXTPROC glad_glGetQueryivEXT;
#define glGetQueryivEXT glad_glGetQueryivEXT
typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVEXTPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETQUERYOBJECTIVEXTPROC glad_glGetQueryObjectivEXT;
#define glGetQueryObjectivEXT glad_glGetQueryObjectivEXT
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVEXTPROC)(GLuint, GLenum, GLuint*);
GLAPI PFNGLGETQUERYOBJECTUIVEXTPROC glad_glGetQueryObjectuivEXT;
#define glGetQueryObjectuivEXT glad_glGetQueryObjectuivEXT
typedef void (APIENTRYP PFNGLGETQUERYOBJECTI64VEXTPROC)(GLuint, GLenum, GLint64*);
GLAPI PFNGLGETQUERYOBJECTI64VEXTPROC glad_glGetQueryObjecti64vEXT;
#define glGetQueryObjecti64vEXT glad_glGetQueryObjecti64vEXT
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VEXTPROC)(GLuint, GLenum, GLuint64*);
GLAPI PFNGLGETQUERYOBJECTUI64VEXTPROC glad_glGetQueryObjectui64vEXT;
#define glGetQueryObjectui64vEXT glad_glGetQueryObjectui64vEXT
#endif
#ifndef GL_OES_compressed_paletted_texture
#define GL_OES_compressed_paletted_texture 1
GLAPI int GLAD_GL_OES_compressed_paletted_texture;
#endif
#ifndef GL_EXT_shader_texture_lod
#define GL_EXT_shader_texture_lod 1
GLAPI int GLAD_GL_EXT_shader_texture_lod;
#endif
#ifndef GL_NV_read_buffer_front
#define GL_NV_read_buffer_front 1
GLAPI int GLAD_GL_NV_read_buffer_front;
#endif
#ifndef GL_OES_texture_float
#define GL_OES_texture_float 1
GLAPI int GLAD_GL_OES_texture_float;
#endif
#ifndef GL_OES_texture_float_linear
#define GL_OES_texture_float_linear 1
GLAPI int GLAD_GL_OES_texture_float_linear;
#endif
#ifndef GL_NV_fbo_color_attachments
#define GL_NV_fbo_color_attachments 1
GLAPI int GLAD_GL_NV_fbo_color_attachments;
#endif
#ifndef GL_EXT_primitive_bounding_box
#define GL_EXT_primitive_bounding_box 1
GLAPI int GLAD_GL_EXT_primitive_bounding_box;
typedef void (APIENTRYP PFNGLPRIMITIVEBOUNDINGBOXEXTPROC)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLPRIMITIVEBOUNDINGBOXEXTPROC glad_glPrimitiveBoundingBoxEXT;
#define glPrimitiveBoundingBoxEXT glad_glPrimitiveBoundingBoxEXT
#endif
#ifndef GL_IMG_read_format
#define GL_IMG_read_format 1
GLAPI int GLAD_GL_IMG_read_format;
#endif
#ifndef GL_EXT_shader_integer_mix
#define GL_EXT_shader_integer_mix 1
GLAPI int GLAD_GL_EXT_shader_integer_mix;
#endif
#ifndef GL_ANDROID_extension_pack_es31a
#define GL_ANDROID_extension_pack_es31a 1
GLAPI int GLAD_GL_ANDROID_extension_pack_es31a;
#endif
#ifndef GL_NV_texture_compression_s3tc_update
#define GL_NV_texture_compression_s3tc_update 1
GLAPI int GLAD_GL_NV_texture_compression_s3tc_update;
#endif
#ifndef GL_OES_fragment_precision_high
#define GL_OES_fragment_precision_high 1
GLAPI int GLAD_GL_OES_fragment_precision_high;
#endif
#ifndef GL_EXT_multi_draw_arrays
#define GL_EXT_multi_draw_arrays 1
GLAPI int GLAD_GL_EXT_multi_draw_arrays;
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSEXTPROC)(GLenum, const GLint*, const GLsizei*, GLsizei);
GLAPI PFNGLMULTIDRAWARRAYSEXTPROC glad_glMultiDrawArraysEXT;
#define glMultiDrawArraysEXT glad_glMultiDrawArraysEXT
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSEXTPROC)(GLenum, const GLsizei*, GLenum, const void**, GLsizei);
GLAPI PFNGLMULTIDRAWELEMENTSEXTPROC glad_glMultiDrawElementsEXT;
#define glMultiDrawElementsEXT glad_glMultiDrawElementsEXT
#endif
#ifndef GL_OES_texture_npot
#define GL_OES_texture_npot 1
GLAPI int GLAD_GL_OES_texture_npot;
#endif
#ifndef GL_EXT_texture_compression_dxt1
#define GL_EXT_texture_compression_dxt1 1
GLAPI int GLAD_GL_EXT_texture_compression_dxt1;
#endif
#ifndef GL_APPLE_clip_distance
#define GL_APPLE_clip_distance 1
GLAPI int GLAD_GL_APPLE_clip_distance;
#endif
#ifndef GL_QCOM_driver_control
#define GL_QCOM_driver_control 1
GLAPI int GLAD_GL_QCOM_driver_control;
typedef void (APIENTRYP PFNGLGETDRIVERCONTROLSQCOMPROC)(GLint*, GLsizei, GLuint*);
GLAPI PFNGLGETDRIVERCONTROLSQCOMPROC glad_glGetDriverControlsQCOM;
#define glGetDriverControlsQCOM glad_glGetDriverControlsQCOM
typedef void (APIENTRYP PFNGLGETDRIVERCONTROLSTRINGQCOMPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETDRIVERCONTROLSTRINGQCOMPROC glad_glGetDriverControlStringQCOM;
#define glGetDriverControlStringQCOM glad_glGetDriverControlStringQCOM
typedef void (APIENTRYP PFNGLENABLEDRIVERCONTROLQCOMPROC)(GLuint);
GLAPI PFNGLENABLEDRIVERCONTROLQCOMPROC glad_glEnableDriverControlQCOM;
#define glEnableDriverControlQCOM glad_glEnableDriverControlQCOM
typedef void (APIENTRYP PFNGLDISABLEDRIVERCONTROLQCOMPROC)(GLuint);
GLAPI PFNGLDISABLEDRIVERCONTROLQCOMPROC glad_glDisableDriverControlQCOM;
#define glDisableDriverControlQCOM glad_glDisableDriverControlQCOM
#endif
#ifndef GL_ANGLE_depth_texture
#define GL_ANGLE_depth_texture 1
GLAPI int GLAD_GL_ANGLE_depth_texture;
#endif
#ifndef GL_KHR_robust_buffer_access_behavior
#define GL_KHR_robust_buffer_access_behavior 1
GLAPI int GLAD_GL_KHR_robust_buffer_access_behavior;
#endif
#ifndef GL_EXT_render_snorm
#define GL_EXT_render_snorm 1
GLAPI int GLAD_GL_EXT_render_snorm;
#endif
#ifndef GL_KHR_texture_compression_astc_ldr
#define GL_KHR_texture_compression_astc_ldr 1
GLAPI int GLAD_GL_KHR_texture_compression_astc_ldr;
#endif
#ifndef GL_EXT_debug_marker
#define GL_EXT_debug_marker 1
GLAPI int GLAD_GL_EXT_debug_marker;
typedef void (APIENTRYP PFNGLINSERTEVENTMARKEREXTPROC)(GLsizei, const GLchar*);
GLAPI PFNGLINSERTEVENTMARKEREXTPROC glad_glInsertEventMarkerEXT;
#define glInsertEventMarkerEXT glad_glInsertEventMarkerEXT
typedef void (APIENTRYP PFNGLPUSHGROUPMARKEREXTPROC)(GLsizei, const GLchar*);
GLAPI PFNGLPUSHGROUPMARKEREXTPROC glad_glPushGroupMarkerEXT;
#define glPushGroupMarkerEXT glad_glPushGroupMarkerEXT
typedef void (APIENTRYP PFNGLPOPGROUPMARKEREXTPROC)();
GLAPI PFNGLPOPGROUPMARKEREXTPROC glad_glPopGroupMarkerEXT;
#define glPopGroupMarkerEXT glad_glPopGroupMarkerEXT
#endif
#ifndef GL_OES_geometry_shader
#define GL_OES_geometry_shader 1
GLAPI int GLAD_GL_OES_geometry_shader;
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREOESPROC)(GLenum, GLenum, GLuint, GLint);
GLAPI PFNGLFRAMEBUFFERTEXTUREOESPROC glad_glFramebufferTextureOES;
#define glFramebufferTextureOES glad_glFramebufferTextureOES
#endif
#ifndef GL_OES_tessellation_shader
#define GL_OES_tessellation_shader 1
GLAPI int GLAD_GL_OES_tessellation_shader;
typedef void (APIENTRYP PFNGLPATCHPARAMETERIOESPROC)(GLenum, GLint);
GLAPI PFNGLPATCHPARAMETERIOESPROC glad_glPatchParameteriOES;
#define glPatchParameteriOES glad_glPatchParameteriOES
#endif
#ifndef GL_OES_texture_view
#define GL_OES_texture_view 1
GLAPI int GLAD_GL_OES_texture_view;
typedef void (APIENTRYP PFNGLTEXTUREVIEWOESPROC)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint);
GLAPI PFNGLTEXTUREVIEWOESPROC glad_glTextureViewOES;
#define glTextureViewOES glad_glTextureViewOES
#endif
#ifndef GL_EXT_multisampled_render_to_texture
#define GL_EXT_multisampled_render_to_texture 1
GLAPI int GLAD_GL_EXT_multisampled_render_to_texture;
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glad_glRenderbufferStorageMultisampleEXT;
#define glRenderbufferStorageMultisampleEXT glad_glRenderbufferStorageMultisampleEXT
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei);
GLAPI PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glad_glFramebufferTexture2DMultisampleEXT;
#define glFramebufferTexture2DMultisampleEXT glad_glFramebufferTexture2DMultisampleEXT
#endif
#ifndef GL_ANGLE_framebuffer_multisample
#define GL_ANGLE_framebuffer_multisample 1
GLAPI int GLAD_GL_ANGLE_framebuffer_multisample;
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEANGLEPROC glad_glRenderbufferStorageMultisampleANGLE;
#define glRenderbufferStorageMultisampleANGLE glad_glRenderbufferStorageMultisampleANGLE
#endif
#ifndef GL_OES_texture_buffer
#define GL_OES_texture_buffer 1
GLAPI int GLAD_GL_OES_texture_buffer;
typedef void (APIENTRYP PFNGLTEXBUFFEROESPROC)(GLenum, GLenum, GLuint);
GLAPI PFNGLTEXBUFFEROESPROC glad_glTexBufferOES;
#define glTexBufferOES glad_glTexBufferOES
typedef void (APIENTRYP PFNGLTEXBUFFERRANGEOESPROC)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr);
GLAPI PFNGLTEXBUFFERRANGEOESPROC glad_glTexBufferRangeOES;
#define glTexBufferRangeOES glad_glTexBufferRangeOES
#endif
#ifndef GL_NV_blend_equation_advanced_coherent
#define GL_NV_blend_equation_advanced_coherent 1
GLAPI int GLAD_GL_NV_blend_equation_advanced_coherent;
#endif
#ifndef GL_EXT_tessellation_point_size
#define GL_EXT_tessellation_point_size 1
GLAPI int GLAD_GL_EXT_tessellation_point_size;
#endif
#ifndef GL_EXT_color_buffer_half_float
#define GL_EXT_color_buffer_half_float 1
GLAPI int GLAD_GL_EXT_color_buffer_half_float;
#endif
#ifndef GL_OES_get_program_binary
#define GL_OES_get_program_binary 1
GLAPI int GLAD_GL_OES_get_program_binary;
typedef void (APIENTRYP PFNGLGETPROGRAMBINARYOESPROC)(GLuint, GLsizei, GLsizei*, GLenum*, void*);
GLAPI PFNGLGETPROGRAMBINARYOESPROC glad_glGetProgramBinaryOES;
#define glGetProgramBinaryOES glad_glGetProgramBinaryOES
typedef void (APIENTRYP PFNGLPROGRAMBINARYOESPROC)(GLuint, GLenum, const void*, GLint);
GLAPI PFNGLPROGRAMBINARYOESPROC glad_glProgramBinaryOES;
#define glProgramBinaryOES glad_glProgramBinaryOES
#endif
#ifndef GL_OES_geometry_point_size
#define GL_OES_geometry_point_size 1
GLAPI int GLAD_GL_OES_geometry_point_size;
#endif
#ifndef GL_APPLE_framebuffer_multisample
#define GL_APPLE_framebuffer_multisample 1
GLAPI int GLAD_GL_APPLE_framebuffer_multisample;
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEAPPLEPROC glad_glRenderbufferStorageMultisampleAPPLE;
#define glRenderbufferStorageMultisampleAPPLE glad_glRenderbufferStorageMultisampleAPPLE
typedef void (APIENTRYP PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC)();
GLAPI PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC glad_glResolveMultisampleFramebufferAPPLE;
#define glResolveMultisampleFramebufferAPPLE glad_glResolveMultisampleFramebufferAPPLE
#endif
#ifndef GL_OES_stencil4
#define GL_OES_stencil4 1
GLAPI int GLAD_GL_OES_stencil4;
#endif
#ifndef GL_NV_copy_buffer
#define GL_NV_copy_buffer 1
GLAPI int GLAD_GL_NV_copy_buffer;
typedef void (APIENTRYP PFNGLCOPYBUFFERSUBDATANVPROC)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr);
GLAPI PFNGLCOPYBUFFERSUBDATANVPROC glad_glCopyBufferSubDataNV;
#define glCopyBufferSubDataNV glad_glCopyBufferSubDataNV
#endif
#ifndef GL_OES_texture_half_float
#define GL_OES_texture_half_float 1
GLAPI int GLAD_GL_OES_texture_half_float;
#endif
#ifndef GL_EXT_texture_view
#define GL_EXT_texture_view 1
GLAPI int GLAD_GL_EXT_texture_view;
typedef void (APIENTRYP PFNGLTEXTUREVIEWEXTPROC)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint);
GLAPI PFNGLTEXTUREVIEWEXTPROC glad_glTextureViewEXT;
#define glTextureViewEXT glad_glTextureViewEXT
#endif
#ifndef GL_OES_vertex_half_float
#define GL_OES_vertex_half_float 1
GLAPI int GLAD_GL_OES_vertex_half_float;
#endif
#ifndef GL_NV_framebuffer_blit
#define GL_NV_framebuffer_blit 1
GLAPI int GLAD_GL_NV_framebuffer_blit;
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFERNVPROC)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
GLAPI PFNGLBLITFRAMEBUFFERNVPROC glad_glBlitFramebufferNV;
#define glBlitFramebufferNV glad_glBlitFramebufferNV
#endif
#ifndef GL_OES_stencil1
#define GL_OES_stencil1 1
GLAPI int GLAD_GL_OES_stencil1;
#endif
#ifndef GL_QCOM_alpha_test
#define GL_QCOM_alpha_test 1
GLAPI int GLAD_GL_QCOM_alpha_test;
typedef void (APIENTRYP PFNGLALPHAFUNCQCOMPROC)(GLenum, GLclampf);
GLAPI PFNGLALPHAFUNCQCOMPROC glad_glAlphaFuncQCOM;
#define glAlphaFuncQCOM glad_glAlphaFuncQCOM
#endif
#ifndef GL_EXT_shader_implicit_conversions
#define GL_EXT_shader_implicit_conversions 1
GLAPI int GLAD_GL_EXT_shader_implicit_conversions;
#endif
#ifndef GL_INTEL_performance_query
#define GL_INTEL_performance_query 1
GLAPI int GLAD_GL_INTEL_performance_query;
typedef void (APIENTRYP PFNGLBEGINPERFQUERYINTELPROC)(GLuint);
GLAPI PFNGLBEGINPERFQUERYINTELPROC glad_glBeginPerfQueryINTEL;
#define glBeginPerfQueryINTEL glad_glBeginPerfQueryINTEL
typedef void (APIENTRYP PFNGLCREATEPERFQUERYINTELPROC)(GLuint, GLuint*);
GLAPI PFNGLCREATEPERFQUERYINTELPROC glad_glCreatePerfQueryINTEL;
#define glCreatePerfQueryINTEL glad_glCreatePerfQueryINTEL
typedef void (APIENTRYP PFNGLDELETEPERFQUERYINTELPROC)(GLuint);
GLAPI PFNGLDELETEPERFQUERYINTELPROC glad_glDeletePerfQueryINTEL;
#define glDeletePerfQueryINTEL glad_glDeletePerfQueryINTEL
typedef void (APIENTRYP PFNGLENDPERFQUERYINTELPROC)(GLuint);
GLAPI PFNGLENDPERFQUERYINTELPROC glad_glEndPerfQueryINTEL;
#define glEndPerfQueryINTEL glad_glEndPerfQueryINTEL
typedef void (APIENTRYP PFNGLGETFIRSTPERFQUERYIDINTELPROC)(GLuint*);
GLAPI PFNGLGETFIRSTPERFQUERYIDINTELPROC glad_glGetFirstPerfQueryIdINTEL;
#define glGetFirstPerfQueryIdINTEL glad_glGetFirstPerfQueryIdINTEL
typedef void (APIENTRYP PFNGLGETNEXTPERFQUERYIDINTELPROC)(GLuint, GLuint*);
GLAPI PFNGLGETNEXTPERFQUERYIDINTELPROC glad_glGetNextPerfQueryIdINTEL;
#define glGetNextPerfQueryIdINTEL glad_glGetNextPerfQueryIdINTEL
typedef void (APIENTRYP PFNGLGETPERFCOUNTERINFOINTELPROC)(GLuint, GLuint, GLuint, GLchar*, GLuint, GLchar*, GLuint*, GLuint*, GLuint*, GLuint*, GLuint64*);
GLAPI PFNGLGETPERFCOUNTERINFOINTELPROC glad_glGetPerfCounterInfoINTEL;
#define glGetPerfCounterInfoINTEL glad_glGetPerfCounterInfoINTEL
typedef void (APIENTRYP PFNGLGETPERFQUERYDATAINTELPROC)(GLuint, GLuint, GLsizei, GLvoid*, GLuint*);
GLAPI PFNGLGETPERFQUERYDATAINTELPROC glad_glGetPerfQueryDataINTEL;
#define glGetPerfQueryDataINTEL glad_glGetPerfQueryDataINTEL
typedef void (APIENTRYP PFNGLGETPERFQUERYIDBYNAMEINTELPROC)(GLchar*, GLuint*);
GLAPI PFNGLGETPERFQUERYIDBYNAMEINTELPROC glad_glGetPerfQueryIdByNameINTEL;
#define glGetPerfQueryIdByNameINTEL glad_glGetPerfQueryIdByNameINTEL
typedef void (APIENTRYP PFNGLGETPERFQUERYINFOINTELPROC)(GLuint, GLuint, GLchar*, GLuint*, GLuint*, GLuint*, GLuint*);
GLAPI PFNGLGETPERFQUERYINFOINTELPROC glad_glGetPerfQueryInfoINTEL;
#define glGetPerfQueryInfoINTEL glad_glGetPerfQueryInfoINTEL
#endif
#ifndef GL_KHR_debug
#define GL_KHR_debug 1
GLAPI int GLAD_GL_KHR_debug;
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC)(GLenum, GLenum, GLenum, GLsizei, const GLuint*, GLboolean);
GLAPI PFNGLDEBUGMESSAGECONTROLPROC glad_glDebugMessageControl;
#define glDebugMessageControl glad_glDebugMessageControl
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTPROC)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*);
GLAPI PFNGLDEBUGMESSAGEINSERTPROC glad_glDebugMessageInsert;
#define glDebugMessageInsert glad_glDebugMessageInsert
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC, const void*);
GLAPI PFNGLDEBUGMESSAGECALLBACKPROC glad_glDebugMessageCallback;
#define glDebugMessageCallback glad_glDebugMessageCallback
typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGPROC)(GLuint, GLsizei, GLenum*, GLenum*, GLuint*, GLenum*, GLsizei*, GLchar*);
GLAPI PFNGLGETDEBUGMESSAGELOGPROC glad_glGetDebugMessageLog;
#define glGetDebugMessageLog glad_glGetDebugMessageLog
typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC)(GLenum, GLuint, GLsizei, const GLchar*);
GLAPI PFNGLPUSHDEBUGGROUPPROC glad_glPushDebugGroup;
#define glPushDebugGroup glad_glPushDebugGroup
typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC)();
GLAPI PFNGLPOPDEBUGGROUPPROC glad_glPopDebugGroup;
#define glPopDebugGroup glad_glPopDebugGroup
typedef void (APIENTRYP PFNGLOBJECTLABELPROC)(GLenum, GLuint, GLsizei, const GLchar*);
GLAPI PFNGLOBJECTLABELPROC glad_glObjectLabel;
#define glObjectLabel glad_glObjectLabel
typedef void (APIENTRYP PFNGLGETOBJECTLABELPROC)(GLenum, GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETOBJECTLABELPROC glad_glGetObjectLabel;
#define glGetObjectLabel glad_glGetObjectLabel
typedef void (APIENTRYP PFNGLOBJECTPTRLABELPROC)(const void*, GLsizei, const GLchar*);
GLAPI PFNGLOBJECTPTRLABELPROC glad_glObjectPtrLabel;
#define glObjectPtrLabel glad_glObjectPtrLabel
typedef void (APIENTRYP PFNGLGETOBJECTPTRLABELPROC)(const void*, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETOBJECTPTRLABELPROC glad_glGetObjectPtrLabel;
#define glGetObjectPtrLabel glad_glGetObjectPtrLabel
typedef void (APIENTRYP PFNGLGETPOINTERVPROC)(GLenum, void**);
GLAPI PFNGLGETPOINTERVPROC glad_glGetPointerv;
#define glGetPointerv glad_glGetPointerv
typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLKHRPROC)(GLenum, GLenum, GLenum, GLsizei, const GLuint*, GLboolean);
GLAPI PFNGLDEBUGMESSAGECONTROLKHRPROC glad_glDebugMessageControlKHR;
#define glDebugMessageControlKHR glad_glDebugMessageControlKHR
typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTKHRPROC)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*);
GLAPI PFNGLDEBUGMESSAGEINSERTKHRPROC glad_glDebugMessageInsertKHR;
#define glDebugMessageInsertKHR glad_glDebugMessageInsertKHR
typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKKHRPROC)(GLDEBUGPROCKHR, const void*);
GLAPI PFNGLDEBUGMESSAGECALLBACKKHRPROC glad_glDebugMessageCallbackKHR;
#define glDebugMessageCallbackKHR glad_glDebugMessageCallbackKHR
typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGKHRPROC)(GLuint, GLsizei, GLenum*, GLenum*, GLuint*, GLenum*, GLsizei*, GLchar*);
GLAPI PFNGLGETDEBUGMESSAGELOGKHRPROC glad_glGetDebugMessageLogKHR;
#define glGetDebugMessageLogKHR glad_glGetDebugMessageLogKHR
typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPKHRPROC)(GLenum, GLuint, GLsizei, const GLchar*);
GLAPI PFNGLPUSHDEBUGGROUPKHRPROC glad_glPushDebugGroupKHR;
#define glPushDebugGroupKHR glad_glPushDebugGroupKHR
typedef void (APIENTRYP PFNGLPOPDEBUGGROUPKHRPROC)();
GLAPI PFNGLPOPDEBUGGROUPKHRPROC glad_glPopDebugGroupKHR;
#define glPopDebugGroupKHR glad_glPopDebugGroupKHR
typedef void (APIENTRYP PFNGLOBJECTLABELKHRPROC)(GLenum, GLuint, GLsizei, const GLchar*);
GLAPI PFNGLOBJECTLABELKHRPROC glad_glObjectLabelKHR;
#define glObjectLabelKHR glad_glObjectLabelKHR
typedef void (APIENTRYP PFNGLGETOBJECTLABELKHRPROC)(GLenum, GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETOBJECTLABELKHRPROC glad_glGetObjectLabelKHR;
#define glGetObjectLabelKHR glad_glGetObjectLabelKHR
typedef void (APIENTRYP PFNGLOBJECTPTRLABELKHRPROC)(const void*, GLsizei, const GLchar*);
GLAPI PFNGLOBJECTPTRLABELKHRPROC glad_glObjectPtrLabelKHR;
#define glObjectPtrLabelKHR glad_glObjectPtrLabelKHR
typedef void (APIENTRYP PFNGLGETOBJECTPTRLABELKHRPROC)(const void*, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETOBJECTPTRLABELKHRPROC glad_glGetObjectPtrLabelKHR;
#define glGetObjectPtrLabelKHR glad_glGetObjectPtrLabelKHR
typedef void (APIENTRYP PFNGLGETPOINTERVKHRPROC)(GLenum, void**);
GLAPI PFNGLGETPOINTERVKHRPROC glad_glGetPointervKHR;
#define glGetPointervKHR glad_glGetPointervKHR
#endif
#ifndef GL_EXT_sRGB
#define GL_EXT_sRGB 1
GLAPI int GLAD_GL_EXT_sRGB;
#endif
#ifndef GL_OES_texture_border_clamp
#define GL_OES_texture_border_clamp 1
GLAPI int GLAD_GL_OES_texture_border_clamp;
typedef void (APIENTRYP PFNGLTEXPARAMETERIIVOESPROC)(GLenum, GLenum, const GLint*);
GLAPI PFNGLTEXPARAMETERIIVOESPROC glad_glTexParameterIivOES;
#define glTexParameterIivOES glad_glTexParameterIivOES
typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVOESPROC)(GLenum, GLenum, const GLuint*);
GLAPI PFNGLTEXPARAMETERIUIVOESPROC glad_glTexParameterIuivOES;
#define glTexParameterIuivOES glad_glTexParameterIuivOES
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVOESPROC)(GLenum, GLenum, GLint*);
GLAPI PFNGLGETTEXPARAMETERIIVOESPROC glad_glGetTexParameterIivOES;
#define glGetTexParameterIivOES glad_glGetTexParameterIivOES
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVOESPROC)(GLenum, GLenum, GLuint*);
GLAPI PFNGLGETTEXPARAMETERIUIVOESPROC glad_glGetTexParameterIuivOES;
#define glGetTexParameterIuivOES glad_glGetTexParameterIuivOES
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVOESPROC)(GLuint, GLenum, const GLint*);
GLAPI PFNGLSAMPLERPARAMETERIIVOESPROC glad_glSamplerParameterIivOES;
#define glSamplerParameterIivOES glad_glSamplerParameterIivOES
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVOESPROC)(GLuint, GLenum, const GLuint*);
GLAPI PFNGLSAMPLERPARAMETERIUIVOESPROC glad_glSamplerParameterIuivOES;
#define glSamplerParameterIuivOES glad_glSamplerParameterIuivOES
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVOESPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETSAMPLERPARAMETERIIVOESPROC glad_glGetSamplerParameterIivOES;
#define glGetSamplerParameterIivOES glad_glGetSamplerParameterIivOES
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVOESPROC)(GLuint, GLenum, GLuint*);
GLAPI PFNGLGETSAMPLERPARAMETERIUIVOESPROC glad_glGetSamplerParameterIuivOES;
#define glGetSamplerParameterIuivOES glad_glGetSamplerParameterIuivOES
#endif
#ifndef GL_ANGLE_pack_reverse_row_order
#define GL_ANGLE_pack_reverse_row_order 1
GLAPI int GLAD_GL_ANGLE_pack_reverse_row_order;
#endif
#ifndef GL_EXT_occlusion_query_boolean
#define GL_EXT_occlusion_query_boolean 1
GLAPI int GLAD_GL_EXT_occlusion_query_boolean;
#endif
#ifndef GL_NV_bindless_texture
#define GL_NV_bindless_texture 1
GLAPI int GLAD_GL_NV_bindless_texture;
typedef GLuint64 (APIENTRYP PFNGLGETTEXTUREHANDLENVPROC)(GLuint);
GLAPI PFNGLGETTEXTUREHANDLENVPROC glad_glGetTextureHandleNV;
#define glGetTextureHandleNV glad_glGetTextureHandleNV
typedef GLuint64 (APIENTRYP PFNGLGETTEXTURESAMPLERHANDLENVPROC)(GLuint, GLuint);
GLAPI PFNGLGETTEXTURESAMPLERHANDLENVPROC glad_glGetTextureSamplerHandleNV;
#define glGetTextureSamplerHandleNV glad_glGetTextureSamplerHandleNV
typedef void (APIENTRYP PFNGLMAKETEXTUREHANDLERESIDENTNVPROC)(GLuint64);
GLAPI PFNGLMAKETEXTUREHANDLERESIDENTNVPROC glad_glMakeTextureHandleResidentNV;
#define glMakeTextureHandleResidentNV glad_glMakeTextureHandleResidentNV
typedef void (APIENTRYP PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC)(GLuint64);
GLAPI PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC glad_glMakeTextureHandleNonResidentNV;
#define glMakeTextureHandleNonResidentNV glad_glMakeTextureHandleNonResidentNV
typedef GLuint64 (APIENTRYP PFNGLGETIMAGEHANDLENVPROC)(GLuint, GLint, GLboolean, GLint, GLenum);
GLAPI PFNGLGETIMAGEHANDLENVPROC glad_glGetImageHandleNV;
#define glGetImageHandleNV glad_glGetImageHandleNV
typedef void (APIENTRYP PFNGLMAKEIMAGEHANDLERESIDENTNVPROC)(GLuint64, GLenum);
GLAPI PFNGLMAKEIMAGEHANDLERESIDENTNVPROC glad_glMakeImageHandleResidentNV;
#define glMakeImageHandleResidentNV glad_glMakeImageHandleResidentNV
typedef void (APIENTRYP PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC)(GLuint64);
GLAPI PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC glad_glMakeImageHandleNonResidentNV;
#define glMakeImageHandleNonResidentNV glad_glMakeImageHandleNonResidentNV
typedef void (APIENTRYP PFNGLUNIFORMHANDLEUI64NVPROC)(GLint, GLuint64);
GLAPI PFNGLUNIFORMHANDLEUI64NVPROC glad_glUniformHandleui64NV;
#define glUniformHandleui64NV glad_glUniformHandleui64NV
typedef void (APIENTRYP PFNGLUNIFORMHANDLEUI64VNVPROC)(GLint, GLsizei, const GLuint64*);
GLAPI PFNGLUNIFORMHANDLEUI64VNVPROC glad_glUniformHandleui64vNV;
#define glUniformHandleui64vNV glad_glUniformHandleui64vNV
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC)(GLuint, GLint, GLuint64);
GLAPI PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC glad_glProgramUniformHandleui64NV;
#define glProgramUniformHandleui64NV glad_glProgramUniformHandleui64NV
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC)(GLuint, GLint, GLsizei, const GLuint64*);
GLAPI PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC glad_glProgramUniformHandleui64vNV;
#define glProgramUniformHandleui64vNV glad_glProgramUniformHandleui64vNV
typedef GLboolean (APIENTRYP PFNGLISTEXTUREHANDLERESIDENTNVPROC)(GLuint64);
GLAPI PFNGLISTEXTUREHANDLERESIDENTNVPROC glad_glIsTextureHandleResidentNV;
#define glIsTextureHandleResidentNV glad_glIsTextureHandleResidentNV
typedef GLboolean (APIENTRYP PFNGLISIMAGEHANDLERESIDENTNVPROC)(GLuint64);
GLAPI PFNGLISIMAGEHANDLERESIDENTNVPROC glad_glIsImageHandleResidentNV;
#define glIsImageHandleResidentNV glad_glIsImageHandleResidentNV
#endif
#ifndef GL_EXT_multi_draw_indirect
#define GL_EXT_multi_draw_indirect 1
GLAPI int GLAD_GL_EXT_multi_draw_indirect;
typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSINDIRECTEXTPROC)(GLenum, const void*, GLsizei, GLsizei);
GLAPI PFNGLMULTIDRAWARRAYSINDIRECTEXTPROC glad_glMultiDrawArraysIndirectEXT;
#define glMultiDrawArraysIndirectEXT glad_glMultiDrawArraysIndirectEXT
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSINDIRECTEXTPROC)(GLenum, GLenum, const void*, GLsizei, GLsizei);
GLAPI PFNGLMULTIDRAWELEMENTSINDIRECTEXTPROC glad_glMultiDrawElementsIndirectEXT;
#define glMultiDrawElementsIndirectEXT glad_glMultiDrawElementsIndirectEXT
#endif
#ifndef GL_OES_tessellation_point_size
#define GL_OES_tessellation_point_size 1
GLAPI int GLAD_GL_OES_tessellation_point_size;
#endif
#ifndef GL_OES_depth_texture
#define GL_OES_depth_texture 1
GLAPI int GLAD_GL_OES_depth_texture;
#endif
#ifndef GL_QCOM_binning_control
#define GL_QCOM_binning_control 1
GLAPI int GLAD_GL_QCOM_binning_control;
#endif
#ifndef GL_KHR_texture_compression_astc_hdr
#define GL_KHR_texture_compression_astc_hdr 1
GLAPI int GLAD_GL_KHR_texture_compression_astc_hdr;
#endif
#ifndef GL_KHR_blend_equation_advanced
#define GL_KHR_blend_equation_advanced 1
GLAPI int GLAD_GL_KHR_blend_equation_advanced;
typedef void (APIENTRYP PFNGLBLENDBARRIERKHRPROC)();
GLAPI PFNGLBLENDBARRIERKHRPROC glad_glBlendBarrierKHR;
#define glBlendBarrierKHR glad_glBlendBarrierKHR
#endif
#ifndef GL_OES_surfaceless_context
#define GL_OES_surfaceless_context 1
GLAPI int GLAD_GL_OES_surfaceless_context;
#endif
#ifndef GL_APPLE_sync
#define GL_APPLE_sync 1
GLAPI int GLAD_GL_APPLE_sync;
typedef GLsync (APIENTRYP PFNGLFENCESYNCAPPLEPROC)(GLenum, GLbitfield);
GLAPI PFNGLFENCESYNCAPPLEPROC glad_glFenceSyncAPPLE;
#define glFenceSyncAPPLE glad_glFenceSyncAPPLE
typedef GLboolean (APIENTRYP PFNGLISSYNCAPPLEPROC)(GLsync);
GLAPI PFNGLISSYNCAPPLEPROC glad_glIsSyncAPPLE;
#define glIsSyncAPPLE glad_glIsSyncAPPLE
typedef void (APIENTRYP PFNGLDELETESYNCAPPLEPROC)(GLsync);
GLAPI PFNGLDELETESYNCAPPLEPROC glad_glDeleteSyncAPPLE;
#define glDeleteSyncAPPLE glad_glDeleteSyncAPPLE
typedef GLenum (APIENTRYP PFNGLCLIENTWAITSYNCAPPLEPROC)(GLsync, GLbitfield, GLuint64);
GLAPI PFNGLCLIENTWAITSYNCAPPLEPROC glad_glClientWaitSyncAPPLE;
#define glClientWaitSyncAPPLE glad_glClientWaitSyncAPPLE
typedef void (APIENTRYP PFNGLWAITSYNCAPPLEPROC)(GLsync, GLbitfield, GLuint64);
GLAPI PFNGLWAITSYNCAPPLEPROC glad_glWaitSyncAPPLE;
#define glWaitSyncAPPLE glad_glWaitSyncAPPLE
typedef void (APIENTRYP PFNGLGETINTEGER64VAPPLEPROC)(GLenum, GLint64*);
GLAPI PFNGLGETINTEGER64VAPPLEPROC glad_glGetInteger64vAPPLE;
#define glGetInteger64vAPPLE glad_glGetInteger64vAPPLE
typedef void (APIENTRYP PFNGLGETSYNCIVAPPLEPROC)(GLsync, GLenum, GLsizei, GLsizei*, GLint*);
GLAPI PFNGLGETSYNCIVAPPLEPROC glad_glGetSyncivAPPLE;
#define glGetSyncivAPPLE glad_glGetSyncivAPPLE
#endif
#ifndef GL_IMG_program_binary
#define GL_IMG_program_binary 1
GLAPI int GLAD_GL_IMG_program_binary;
#endif
#ifndef GL_EXT_geometry_point_size
#define GL_EXT_geometry_point_size 1
GLAPI int GLAD_GL_EXT_geometry_point_size;
#endif
#ifndef GL_EXT_shader_framebuffer_fetch
#define GL_EXT_shader_framebuffer_fetch 1
GLAPI int GLAD_GL_EXT_shader_framebuffer_fetch;
#endif
#ifndef GL_ANGLE_program_binary
#define GL_ANGLE_program_binary 1
GLAPI int GLAD_GL_ANGLE_program_binary;
#endif
#ifndef GL_EXT_unpack_subimage
#define GL_EXT_unpack_subimage 1
GLAPI int GLAD_GL_EXT_unpack_subimage;
#endif
#ifndef GL_ANGLE_texture_compression_dxt3
#define GL_ANGLE_texture_compression_dxt3 1
GLAPI int GLAD_GL_ANGLE_texture_compression_dxt3;
#endif
#ifndef GL_ANGLE_texture_compression_dxt5
#define GL_ANGLE_texture_compression_dxt5 1
GLAPI int GLAD_GL_ANGLE_texture_compression_dxt5;
#endif
#ifndef GL_EXT_read_format_bgra
#define GL_EXT_read_format_bgra 1
GLAPI int GLAD_GL_EXT_read_format_bgra;
#endif
#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_OES_compressed_ETC1_RGB8_texture 1
GLAPI int GLAD_GL_OES_compressed_ETC1_RGB8_texture;
#endif
#ifndef GL_OES_copy_image
#define GL_OES_copy_image 1
GLAPI int GLAD_GL_OES_copy_image;
typedef void (APIENTRYP PFNGLCOPYIMAGESUBDATAOESPROC)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei);
GLAPI PFNGLCOPYIMAGESUBDATAOESPROC glad_glCopyImageSubDataOES;
#define glCopyImageSubDataOES glad_glCopyImageSubDataOES
#endif
#ifndef GL_EXT_pvrtc_sRGB
#define GL_EXT_pvrtc_sRGB 1
GLAPI int GLAD_GL_EXT_pvrtc_sRGB;
#endif
#ifndef GL_QCOM_extended_get2
#define GL_QCOM_extended_get2 1
GLAPI int GLAD_GL_QCOM_extended_get2;
typedef void (APIENTRYP PFNGLEXTGETSHADERSQCOMPROC)(GLuint*, GLint, GLint*);
GLAPI PFNGLEXTGETSHADERSQCOMPROC glad_glExtGetShadersQCOM;
#define glExtGetShadersQCOM glad_glExtGetShadersQCOM
typedef void (APIENTRYP PFNGLEXTGETPROGRAMSQCOMPROC)(GLuint*, GLint, GLint*);
GLAPI PFNGLEXTGETPROGRAMSQCOMPROC glad_glExtGetProgramsQCOM;
#define glExtGetProgramsQCOM glad_glExtGetProgramsQCOM
typedef GLboolean (APIENTRYP PFNGLEXTISPROGRAMBINARYQCOMPROC)(GLuint);
GLAPI PFNGLEXTISPROGRAMBINARYQCOMPROC glad_glExtIsProgramBinaryQCOM;
#define glExtIsProgramBinaryQCOM glad_glExtIsProgramBinaryQCOM
typedef void (APIENTRYP PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC)(GLuint, GLenum, GLchar*, GLint*);
GLAPI PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC glad_glExtGetProgramBinarySourceQCOM;
#define glExtGetProgramBinarySourceQCOM glad_glExtGetProgramBinarySourceQCOM
#endif
#ifndef GL_EXT_shader_pixel_local_storage
#define GL_EXT_shader_pixel_local_storage 1
GLAPI int GLAD_GL_EXT_shader_pixel_local_storage;
#endif
#ifndef GL_NV_shadow_samplers_cube
#define GL_NV_shadow_samplers_cube 1
GLAPI int GLAD_GL_NV_shadow_samplers_cube;
#endif
#ifndef GL_OES_texture_compression_astc
#define GL_OES_texture_compression_astc 1
GLAPI int GLAD_GL_OES_texture_compression_astc;
#endif
#ifndef GL_APPLE_texture_max_level
#define GL_APPLE_texture_max_level 1
GLAPI int GLAD_GL_APPLE_texture_max_level;
#endif
#ifndef GL_OES_shader_multisample_interpolation
#define GL_OES_shader_multisample_interpolation 1
GLAPI int GLAD_GL_OES_shader_multisample_interpolation;
#endif
#ifndef GL_EXT_shadow_samplers
#define GL_EXT_shadow_samplers 1
GLAPI int GLAD_GL_EXT_shadow_samplers;
#endif
#ifndef GL_IMG_shader_binary
#define GL_IMG_shader_binary 1
GLAPI int GLAD_GL_IMG_shader_binary;
#endif
#ifndef GL_EXT_draw_elements_base_vertex
#define GL_EXT_draw_elements_base_vertex 1
GLAPI int GLAD_GL_EXT_draw_elements_base_vertex;
typedef void (APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXEXTPROC)(GLenum, GLsizei, GLenum, const void*, GLint);
GLAPI PFNGLDRAWELEMENTSBASEVERTEXEXTPROC glad_glDrawElementsBaseVertexEXT;
#define glDrawElementsBaseVertexEXT glad_glDrawElementsBaseVertexEXT
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXEXTPROC)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void*, GLint);
GLAPI PFNGLDRAWRANGEELEMENTSBASEVERTEXEXTPROC glad_glDrawRangeElementsBaseVertexEXT;
#define glDrawRangeElementsBaseVertexEXT glad_glDrawRangeElementsBaseVertexEXT
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXEXTPROC)(GLenum, GLsizei, GLenum, const void*, GLsizei, GLint);
GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXEXTPROC glad_glDrawElementsInstancedBaseVertexEXT;
#define glDrawElementsInstancedBaseVertexEXT glad_glDrawElementsInstancedBaseVertexEXT
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC)(GLenum, const GLsizei*, GLenum, const void**, GLsizei, const GLint*);
GLAPI PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC glad_glMultiDrawElementsBaseVertexEXT;
#define glMultiDrawElementsBaseVertexEXT glad_glMultiDrawElementsBaseVertexEXT
#endif
#ifndef GL_OES_depth32
#define GL_OES_depth32 1
GLAPI int GLAD_GL_OES_depth32;
#endif
#ifndef GL_EXT_separate_shader_objects
#define GL_EXT_separate_shader_objects 1
GLAPI int GLAD_GL_EXT_separate_shader_objects;
typedef void (APIENTRYP PFNGLUSESHADERPROGRAMEXTPROC)(GLenum, GLuint);
GLAPI PFNGLUSESHADERPROGRAMEXTPROC glad_glUseShaderProgramEXT;
#define glUseShaderProgramEXT glad_glUseShaderProgramEXT
typedef void (APIENTRYP PFNGLACTIVEPROGRAMEXTPROC)(GLuint);
GLAPI PFNGLACTIVEPROGRAMEXTPROC glad_glActiveProgramEXT;
#define glActiveProgramEXT glad_glActiveProgramEXT
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROGRAMEXTPROC)(GLenum, const GLchar*);
GLAPI PFNGLCREATESHADERPROGRAMEXTPROC glad_glCreateShaderProgramEXT;
#define glCreateShaderProgramEXT glad_glCreateShaderProgramEXT
typedef void (APIENTRYP PFNGLACTIVESHADERPROGRAMEXTPROC)(GLuint, GLuint);
GLAPI PFNGLACTIVESHADERPROGRAMEXTPROC glad_glActiveShaderProgramEXT;
#define glActiveShaderProgramEXT glad_glActiveShaderProgramEXT
typedef void (APIENTRYP PFNGLBINDPROGRAMPIPELINEEXTPROC)(GLuint);
GLAPI PFNGLBINDPROGRAMPIPELINEEXTPROC glad_glBindProgramPipelineEXT;
#define glBindProgramPipelineEXT glad_glBindProgramPipelineEXT
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROGRAMVEXTPROC)(GLenum, GLsizei, const GLchar**);
GLAPI PFNGLCREATESHADERPROGRAMVEXTPROC glad_glCreateShaderProgramvEXT;
#define glCreateShaderProgramvEXT glad_glCreateShaderProgramvEXT
typedef void (APIENTRYP PFNGLDELETEPROGRAMPIPELINESEXTPROC)(GLsizei, const GLuint*);
GLAPI PFNGLDELETEPROGRAMPIPELINESEXTPROC glad_glDeleteProgramPipelinesEXT;
#define glDeleteProgramPipelinesEXT glad_glDeleteProgramPipelinesEXT
typedef void (APIENTRYP PFNGLGENPROGRAMPIPELINESEXTPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENPROGRAMPIPELINESEXTPROC glad_glGenProgramPipelinesEXT;
#define glGenProgramPipelinesEXT glad_glGenProgramPipelinesEXT
typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC glad_glGetProgramPipelineInfoLogEXT;
#define glGetProgramPipelineInfoLogEXT glad_glGetProgramPipelineInfoLogEXT
typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEIVEXTPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETPROGRAMPIPELINEIVEXTPROC glad_glGetProgramPipelineivEXT;
#define glGetProgramPipelineivEXT glad_glGetProgramPipelineivEXT
typedef GLboolean (APIENTRYP PFNGLISPROGRAMPIPELINEEXTPROC)(GLuint);
GLAPI PFNGLISPROGRAMPIPELINEEXTPROC glad_glIsProgramPipelineEXT;
#define glIsProgramPipelineEXT glad_glIsProgramPipelineEXT
typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIEXTPROC)(GLuint, GLenum, GLint);
GLAPI PFNGLPROGRAMPARAMETERIEXTPROC glad_glProgramParameteriEXT;
#define glProgramParameteriEXT glad_glProgramParameteriEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FEXTPROC)(GLuint, GLint, GLfloat);
GLAPI PFNGLPROGRAMUNIFORM1FEXTPROC glad_glProgramUniform1fEXT;
#define glProgramUniform1fEXT glad_glProgramUniform1fEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FVEXTPROC)(GLuint, GLint, GLsizei, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORM1FVEXTPROC glad_glProgramUniform1fvEXT;
#define glProgramUniform1fvEXT glad_glProgramUniform1fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IEXTPROC)(GLuint, GLint, GLint);
GLAPI PFNGLPROGRAMUNIFORM1IEXTPROC glad_glProgramUniform1iEXT;
#define glProgramUniform1iEXT glad_glProgramUniform1iEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IVEXTPROC)(GLuint, GLint, GLsizei, const GLint*);
GLAPI PFNGLPROGRAMUNIFORM1IVEXTPROC glad_glProgramUniform1ivEXT;
#define glProgramUniform1ivEXT glad_glProgramUniform1ivEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FEXTPROC)(GLuint, GLint, GLfloat, GLfloat);
GLAPI PFNGLPROGRAMUNIFORM2FEXTPROC glad_glProgramUniform2fEXT;
#define glProgramUniform2fEXT glad_glProgramUniform2fEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FVEXTPROC)(GLuint, GLint, GLsizei, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORM2FVEXTPROC glad_glProgramUniform2fvEXT;
#define glProgramUniform2fvEXT glad_glProgramUniform2fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IEXTPROC)(GLuint, GLint, GLint, GLint);
GLAPI PFNGLPROGRAMUNIFORM2IEXTPROC glad_glProgramUniform2iEXT;
#define glProgramUniform2iEXT glad_glProgramUniform2iEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IVEXTPROC)(GLuint, GLint, GLsizei, const GLint*);
GLAPI PFNGLPROGRAMUNIFORM2IVEXTPROC glad_glProgramUniform2ivEXT;
#define glProgramUniform2ivEXT glad_glProgramUniform2ivEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FEXTPROC)(GLuint, GLint, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLPROGRAMUNIFORM3FEXTPROC glad_glProgramUniform3fEXT;
#define glProgramUniform3fEXT glad_glProgramUniform3fEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FVEXTPROC)(GLuint, GLint, GLsizei, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORM3FVEXTPROC glad_glProgramUniform3fvEXT;
#define glProgramUniform3fvEXT glad_glProgramUniform3fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IEXTPROC)(GLuint, GLint, GLint, GLint, GLint);
GLAPI PFNGLPROGRAMUNIFORM3IEXTPROC glad_glProgramUniform3iEXT;
#define glProgramUniform3iEXT glad_glProgramUniform3iEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IVEXTPROC)(GLuint, GLint, GLsizei, const GLint*);
GLAPI PFNGLPROGRAMUNIFORM3IVEXTPROC glad_glProgramUniform3ivEXT;
#define glProgramUniform3ivEXT glad_glProgramUniform3ivEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FEXTPROC)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLPROGRAMUNIFORM4FEXTPROC glad_glProgramUniform4fEXT;
#define glProgramUniform4fEXT glad_glProgramUniform4fEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FVEXTPROC)(GLuint, GLint, GLsizei, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORM4FVEXTPROC glad_glProgramUniform4fvEXT;
#define glProgramUniform4fvEXT glad_glProgramUniform4fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IEXTPROC)(GLuint, GLint, GLint, GLint, GLint, GLint);
GLAPI PFNGLPROGRAMUNIFORM4IEXTPROC glad_glProgramUniform4iEXT;
#define glProgramUniform4iEXT glad_glProgramUniform4iEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IVEXTPROC)(GLuint, GLint, GLsizei, const GLint*);
GLAPI PFNGLPROGRAMUNIFORM4IVEXTPROC glad_glProgramUniform4ivEXT;
#define glProgramUniform4ivEXT glad_glProgramUniform4ivEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC glad_glProgramUniformMatrix2fvEXT;
#define glProgramUniformMatrix2fvEXT glad_glProgramUniformMatrix2fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC glad_glProgramUniformMatrix3fvEXT;
#define glProgramUniformMatrix3fvEXT glad_glProgramUniformMatrix3fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC glad_glProgramUniformMatrix4fvEXT;
#define glProgramUniformMatrix4fvEXT glad_glProgramUniformMatrix4fvEXT
typedef void (APIENTRYP PFNGLUSEPROGRAMSTAGESEXTPROC)(GLuint, GLbitfield, GLuint);
GLAPI PFNGLUSEPROGRAMSTAGESEXTPROC glad_glUseProgramStagesEXT;
#define glUseProgramStagesEXT glad_glUseProgramStagesEXT
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPIPELINEEXTPROC)(GLuint);
GLAPI PFNGLVALIDATEPROGRAMPIPELINEEXTPROC glad_glValidateProgramPipelineEXT;
#define glValidateProgramPipelineEXT glad_glValidateProgramPipelineEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIEXTPROC)(GLuint, GLint, GLuint);
GLAPI PFNGLPROGRAMUNIFORM1UIEXTPROC glad_glProgramUniform1uiEXT;
#define glProgramUniform1uiEXT glad_glProgramUniform1uiEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIEXTPROC)(GLuint, GLint, GLuint, GLuint);
GLAPI PFNGLPROGRAMUNIFORM2UIEXTPROC glad_glProgramUniform2uiEXT;
#define glProgramUniform2uiEXT glad_glProgramUniform2uiEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIEXTPROC)(GLuint, GLint, GLuint, GLuint, GLuint);
GLAPI PFNGLPROGRAMUNIFORM3UIEXTPROC glad_glProgramUniform3uiEXT;
#define glProgramUniform3uiEXT glad_glProgramUniform3uiEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIEXTPROC)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint);
GLAPI PFNGLPROGRAMUNIFORM4UIEXTPROC glad_glProgramUniform4uiEXT;
#define glProgramUniform4uiEXT glad_glProgramUniform4uiEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIVEXTPROC)(GLuint, GLint, GLsizei, const GLuint*);
GLAPI PFNGLPROGRAMUNIFORM1UIVEXTPROC glad_glProgramUniform1uivEXT;
#define glProgramUniform1uivEXT glad_glProgramUniform1uivEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIVEXTPROC)(GLuint, GLint, GLsizei, const GLuint*);
GLAPI PFNGLPROGRAMUNIFORM2UIVEXTPROC glad_glProgramUniform2uivEXT;
#define glProgramUniform2uivEXT glad_glProgramUniform2uivEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIVEXTPROC)(GLuint, GLint, GLsizei, const GLuint*);
GLAPI PFNGLPROGRAMUNIFORM3UIVEXTPROC glad_glProgramUniform3uivEXT;
#define glProgramUniform3uivEXT glad_glProgramUniform3uivEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIVEXTPROC)(GLuint, GLint, GLsizei, const GLuint*);
GLAPI PFNGLPROGRAMUNIFORM4UIVEXTPROC glad_glProgramUniform4uivEXT;
#define glProgramUniform4uivEXT glad_glProgramUniform4uivEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC glad_glProgramUniformMatrix2x3fvEXT;
#define glProgramUniformMatrix2x3fvEXT glad_glProgramUniformMatrix2x3fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC glad_glProgramUniformMatrix3x2fvEXT;
#define glProgramUniformMatrix3x2fvEXT glad_glProgramUniformMatrix3x2fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC glad_glProgramUniformMatrix2x4fvEXT;
#define glProgramUniformMatrix2x4fvEXT glad_glProgramUniformMatrix2x4fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC glad_glProgramUniformMatrix4x2fvEXT;
#define glProgramUniformMatrix4x2fvEXT glad_glProgramUniformMatrix4x2fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC glad_glProgramUniformMatrix3x4fvEXT;
#define glProgramUniformMatrix3x4fvEXT glad_glProgramUniformMatrix3x4fvEXT
typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
GLAPI PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC glad_glProgramUniformMatrix4x3fvEXT;
#define glProgramUniformMatrix4x3fvEXT glad_glProgramUniformMatrix4x3fvEXT
#endif
#ifndef GL_EXT_shader_io_blocks
#define GL_EXT_shader_io_blocks 1
GLAPI int GLAD_GL_EXT_shader_io_blocks;
#endif
#ifndef GL_NV_framebuffer_multisample
#define GL_NV_framebuffer_multisample 1
GLAPI int GLAD_GL_NV_framebuffer_multisample;
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLENVPROC)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLENVPROC glad_glRenderbufferStorageMultisampleNV;
#define glRenderbufferStorageMultisampleNV glad_glRenderbufferStorageMultisampleNV
#endif
#ifndef GL_NV_draw_buffers
#define GL_NV_draw_buffers 1
GLAPI int GLAD_GL_NV_draw_buffers;
typedef void (APIENTRYP PFNGLDRAWBUFFERSNVPROC)(GLsizei, const GLenum*);
GLAPI PFNGLDRAWBUFFERSNVPROC glad_glDrawBuffersNV;
#define glDrawBuffersNV glad_glDrawBuffersNV
#endif
#ifndef GL_ARM_shader_framebuffer_fetch
#define GL_ARM_shader_framebuffer_fetch 1
GLAPI int GLAD_GL_ARM_shader_framebuffer_fetch;
#endif
#ifndef GL_NV_path_rendering
#define GL_NV_path_rendering 1
GLAPI int GLAD_GL_NV_path_rendering;
typedef GLuint (APIENTRYP PFNGLGENPATHSNVPROC)(GLsizei);
GLAPI PFNGLGENPATHSNVPROC glad_glGenPathsNV;
#define glGenPathsNV glad_glGenPathsNV
typedef void (APIENTRYP PFNGLDELETEPATHSNVPROC)(GLuint, GLsizei);
GLAPI PFNGLDELETEPATHSNVPROC glad_glDeletePathsNV;
#define glDeletePathsNV glad_glDeletePathsNV
typedef GLboolean (APIENTRYP PFNGLISPATHNVPROC)(GLuint);
GLAPI PFNGLISPATHNVPROC glad_glIsPathNV;
#define glIsPathNV glad_glIsPathNV
typedef void (APIENTRYP PFNGLPATHCOMMANDSNVPROC)(GLuint, GLsizei, const GLubyte*, GLsizei, GLenum, const void*);
GLAPI PFNGLPATHCOMMANDSNVPROC glad_glPathCommandsNV;
#define glPathCommandsNV glad_glPathCommandsNV
typedef void (APIENTRYP PFNGLPATHCOORDSNVPROC)(GLuint, GLsizei, GLenum, const void*);
GLAPI PFNGLPATHCOORDSNVPROC glad_glPathCoordsNV;
#define glPathCoordsNV glad_glPathCoordsNV
typedef void (APIENTRYP PFNGLPATHSUBCOMMANDSNVPROC)(GLuint, GLsizei, GLsizei, GLsizei, const GLubyte*, GLsizei, GLenum, const void*);
GLAPI PFNGLPATHSUBCOMMANDSNVPROC glad_glPathSubCommandsNV;
#define glPathSubCommandsNV glad_glPathSubCommandsNV
typedef void (APIENTRYP PFNGLPATHSUBCOORDSNVPROC)(GLuint, GLsizei, GLsizei, GLenum, const void*);
GLAPI PFNGLPATHSUBCOORDSNVPROC glad_glPathSubCoordsNV;
#define glPathSubCoordsNV glad_glPathSubCoordsNV
typedef void (APIENTRYP PFNGLPATHSTRINGNVPROC)(GLuint, GLenum, GLsizei, const void*);
GLAPI PFNGLPATHSTRINGNVPROC glad_glPathStringNV;
#define glPathStringNV glad_glPathStringNV
typedef void (APIENTRYP PFNGLPATHGLYPHSNVPROC)(GLuint, GLenum, const void*, GLbitfield, GLsizei, GLenum, const void*, GLenum, GLuint, GLfloat);
GLAPI PFNGLPATHGLYPHSNVPROC glad_glPathGlyphsNV;
#define glPathGlyphsNV glad_glPathGlyphsNV
typedef void (APIENTRYP PFNGLPATHGLYPHRANGENVPROC)(GLuint, GLenum, const void*, GLbitfield, GLuint, GLsizei, GLenum, GLuint, GLfloat);
GLAPI PFNGLPATHGLYPHRANGENVPROC glad_glPathGlyphRangeNV;
#define glPathGlyphRangeNV glad_glPathGlyphRangeNV
typedef void (APIENTRYP PFNGLWEIGHTPATHSNVPROC)(GLuint, GLsizei, const GLuint*, const GLfloat*);
GLAPI PFNGLWEIGHTPATHSNVPROC glad_glWeightPathsNV;
#define glWeightPathsNV glad_glWeightPathsNV
typedef void (APIENTRYP PFNGLCOPYPATHNVPROC)(GLuint, GLuint);
GLAPI PFNGLCOPYPATHNVPROC glad_glCopyPathNV;
#define glCopyPathNV glad_glCopyPathNV
typedef void (APIENTRYP PFNGLINTERPOLATEPATHSNVPROC)(GLuint, GLuint, GLuint, GLfloat);
GLAPI PFNGLINTERPOLATEPATHSNVPROC glad_glInterpolatePathsNV;
#define glInterpolatePathsNV glad_glInterpolatePathsNV
typedef void (APIENTRYP PFNGLTRANSFORMPATHNVPROC)(GLuint, GLuint, GLenum, const GLfloat*);
GLAPI PFNGLTRANSFORMPATHNVPROC glad_glTransformPathNV;
#define glTransformPathNV glad_glTransformPathNV
typedef void (APIENTRYP PFNGLPATHPARAMETERIVNVPROC)(GLuint, GLenum, const GLint*);
GLAPI PFNGLPATHPARAMETERIVNVPROC glad_glPathParameterivNV;
#define glPathParameterivNV glad_glPathParameterivNV
typedef void (APIENTRYP PFNGLPATHPARAMETERINVPROC)(GLuint, GLenum, GLint);
GLAPI PFNGLPATHPARAMETERINVPROC glad_glPathParameteriNV;
#define glPathParameteriNV glad_glPathParameteriNV
typedef void (APIENTRYP PFNGLPATHPARAMETERFVNVPROC)(GLuint, GLenum, const GLfloat*);
GLAPI PFNGLPATHPARAMETERFVNVPROC glad_glPathParameterfvNV;
#define glPathParameterfvNV glad_glPathParameterfvNV
typedef void (APIENTRYP PFNGLPATHPARAMETERFNVPROC)(GLuint, GLenum, GLfloat);
GLAPI PFNGLPATHPARAMETERFNVPROC glad_glPathParameterfNV;
#define glPathParameterfNV glad_glPathParameterfNV
typedef void (APIENTRYP PFNGLPATHDASHARRAYNVPROC)(GLuint, GLsizei, const GLfloat*);
GLAPI PFNGLPATHDASHARRAYNVPROC glad_glPathDashArrayNV;
#define glPathDashArrayNV glad_glPathDashArrayNV
typedef void (APIENTRYP PFNGLPATHSTENCILFUNCNVPROC)(GLenum, GLint, GLuint);
GLAPI PFNGLPATHSTENCILFUNCNVPROC glad_glPathStencilFuncNV;
#define glPathStencilFuncNV glad_glPathStencilFuncNV
typedef void (APIENTRYP PFNGLPATHSTENCILDEPTHOFFSETNVPROC)(GLfloat, GLfloat);
GLAPI PFNGLPATHSTENCILDEPTHOFFSETNVPROC glad_glPathStencilDepthOffsetNV;
#define glPathStencilDepthOffsetNV glad_glPathStencilDepthOffsetNV
typedef void (APIENTRYP PFNGLSTENCILFILLPATHNVPROC)(GLuint, GLenum, GLuint);
GLAPI PFNGLSTENCILFILLPATHNVPROC glad_glStencilFillPathNV;
#define glStencilFillPathNV glad_glStencilFillPathNV
typedef void (APIENTRYP PFNGLSTENCILSTROKEPATHNVPROC)(GLuint, GLint, GLuint);
GLAPI PFNGLSTENCILSTROKEPATHNVPROC glad_glStencilStrokePathNV;
#define glStencilStrokePathNV glad_glStencilStrokePathNV
typedef void (APIENTRYP PFNGLSTENCILFILLPATHINSTANCEDNVPROC)(GLsizei, GLenum, const void*, GLuint, GLenum, GLuint, GLenum, const GLfloat*);
GLAPI PFNGLSTENCILFILLPATHINSTANCEDNVPROC glad_glStencilFillPathInstancedNV;
#define glStencilFillPathInstancedNV glad_glStencilFillPathInstancedNV
typedef void (APIENTRYP PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC)(GLsizei, GLenum, const void*, GLuint, GLint, GLuint, GLenum, const GLfloat*);
GLAPI PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC glad_glStencilStrokePathInstancedNV;
#define glStencilStrokePathInstancedNV glad_glStencilStrokePathInstancedNV
typedef void (APIENTRYP PFNGLPATHCOVERDEPTHFUNCNVPROC)(GLenum);
GLAPI PFNGLPATHCOVERDEPTHFUNCNVPROC glad_glPathCoverDepthFuncNV;
#define glPathCoverDepthFuncNV glad_glPathCoverDepthFuncNV
typedef void (APIENTRYP PFNGLCOVERFILLPATHNVPROC)(GLuint, GLenum);
GLAPI PFNGLCOVERFILLPATHNVPROC glad_glCoverFillPathNV;
#define glCoverFillPathNV glad_glCoverFillPathNV
typedef void (APIENTRYP PFNGLCOVERSTROKEPATHNVPROC)(GLuint, GLenum);
GLAPI PFNGLCOVERSTROKEPATHNVPROC glad_glCoverStrokePathNV;
#define glCoverStrokePathNV glad_glCoverStrokePathNV
typedef void (APIENTRYP PFNGLCOVERFILLPATHINSTANCEDNVPROC)(GLsizei, GLenum, const void*, GLuint, GLenum, GLenum, const GLfloat*);
GLAPI PFNGLCOVERFILLPATHINSTANCEDNVPROC glad_glCoverFillPathInstancedNV;
#define glCoverFillPathInstancedNV glad_glCoverFillPathInstancedNV
typedef void (APIENTRYP PFNGLCOVERSTROKEPATHINSTANCEDNVPROC)(GLsizei, GLenum, const void*, GLuint, GLenum, GLenum, const GLfloat*);
GLAPI PFNGLCOVERSTROKEPATHINSTANCEDNVPROC glad_glCoverStrokePathInstancedNV;
#define glCoverStrokePathInstancedNV glad_glCoverStrokePathInstancedNV
typedef void (APIENTRYP PFNGLGETPATHPARAMETERIVNVPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETPATHPARAMETERIVNVPROC glad_glGetPathParameterivNV;
#define glGetPathParameterivNV glad_glGetPathParameterivNV
typedef void (APIENTRYP PFNGLGETPATHPARAMETERFVNVPROC)(GLuint, GLenum, GLfloat*);
GLAPI PFNGLGETPATHPARAMETERFVNVPROC glad_glGetPathParameterfvNV;
#define glGetPathParameterfvNV glad_glGetPathParameterfvNV
typedef void (APIENTRYP PFNGLGETPATHCOMMANDSNVPROC)(GLuint, GLubyte*);
GLAPI PFNGLGETPATHCOMMANDSNVPROC glad_glGetPathCommandsNV;
#define glGetPathCommandsNV glad_glGetPathCommandsNV
typedef void (APIENTRYP PFNGLGETPATHCOORDSNVPROC)(GLuint, GLfloat*);
GLAPI PFNGLGETPATHCOORDSNVPROC glad_glGetPathCoordsNV;
#define glGetPathCoordsNV glad_glGetPathCoordsNV
typedef void (APIENTRYP PFNGLGETPATHDASHARRAYNVPROC)(GLuint, GLfloat*);
GLAPI PFNGLGETPATHDASHARRAYNVPROC glad_glGetPathDashArrayNV;
#define glGetPathDashArrayNV glad_glGetPathDashArrayNV
typedef void (APIENTRYP PFNGLGETPATHMETRICSNVPROC)(GLbitfield, GLsizei, GLenum, const void*, GLuint, GLsizei, GLfloat*);
GLAPI PFNGLGETPATHMETRICSNVPROC glad_glGetPathMetricsNV;
#define glGetPathMetricsNV glad_glGetPathMetricsNV
typedef void (APIENTRYP PFNGLGETPATHMETRICRANGENVPROC)(GLbitfield, GLuint, GLsizei, GLsizei, GLfloat*);
GLAPI PFNGLGETPATHMETRICRANGENVPROC glad_glGetPathMetricRangeNV;
#define glGetPathMetricRangeNV glad_glGetPathMetricRangeNV
typedef void (APIENTRYP PFNGLGETPATHSPACINGNVPROC)(GLenum, GLsizei, GLenum, const void*, GLuint, GLfloat, GLfloat, GLenum, GLfloat*);
GLAPI PFNGLGETPATHSPACINGNVPROC glad_glGetPathSpacingNV;
#define glGetPathSpacingNV glad_glGetPathSpacingNV
typedef GLboolean (APIENTRYP PFNGLISPOINTINFILLPATHNVPROC)(GLuint, GLuint, GLfloat, GLfloat);
GLAPI PFNGLISPOINTINFILLPATHNVPROC glad_glIsPointInFillPathNV;
#define glIsPointInFillPathNV glad_glIsPointInFillPathNV
typedef GLboolean (APIENTRYP PFNGLISPOINTINSTROKEPATHNVPROC)(GLuint, GLfloat, GLfloat);
GLAPI PFNGLISPOINTINSTROKEPATHNVPROC glad_glIsPointInStrokePathNV;
#define glIsPointInStrokePathNV glad_glIsPointInStrokePathNV
typedef GLfloat (APIENTRYP PFNGLGETPATHLENGTHNVPROC)(GLuint, GLsizei, GLsizei);
GLAPI PFNGLGETPATHLENGTHNVPROC glad_glGetPathLengthNV;
#define glGetPathLengthNV glad_glGetPathLengthNV
typedef GLboolean (APIENTRYP PFNGLPOINTALONGPATHNVPROC)(GLuint, GLsizei, GLsizei, GLfloat, GLfloat*, GLfloat*, GLfloat*, GLfloat*);
GLAPI PFNGLPOINTALONGPATHNVPROC glad_glPointAlongPathNV;
#define glPointAlongPathNV glad_glPointAlongPathNV
typedef void (APIENTRYP PFNGLMATRIXLOAD3X2FNVPROC)(GLenum, const GLfloat*);
GLAPI PFNGLMATRIXLOAD3X2FNVPROC glad_glMatrixLoad3x2fNV;
#define glMatrixLoad3x2fNV glad_glMatrixLoad3x2fNV
typedef void (APIENTRYP PFNGLMATRIXLOAD3X3FNVPROC)(GLenum, const GLfloat*);
GLAPI PFNGLMATRIXLOAD3X3FNVPROC glad_glMatrixLoad3x3fNV;
#define glMatrixLoad3x3fNV glad_glMatrixLoad3x3fNV
typedef void (APIENTRYP PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC)(GLenum, const GLfloat*);
GLAPI PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC glad_glMatrixLoadTranspose3x3fNV;
#define glMatrixLoadTranspose3x3fNV glad_glMatrixLoadTranspose3x3fNV
typedef void (APIENTRYP PFNGLMATRIXMULT3X2FNVPROC)(GLenum, const GLfloat*);
GLAPI PFNGLMATRIXMULT3X2FNVPROC glad_glMatrixMult3x2fNV;
#define glMatrixMult3x2fNV glad_glMatrixMult3x2fNV
typedef void (APIENTRYP PFNGLMATRIXMULT3X3FNVPROC)(GLenum, const GLfloat*);
GLAPI PFNGLMATRIXMULT3X3FNVPROC glad_glMatrixMult3x3fNV;
#define glMatrixMult3x3fNV glad_glMatrixMult3x3fNV
typedef void (APIENTRYP PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC)(GLenum, const GLfloat*);
GLAPI PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC glad_glMatrixMultTranspose3x3fNV;
#define glMatrixMultTranspose3x3fNV glad_glMatrixMultTranspose3x3fNV
typedef void (APIENTRYP PFNGLSTENCILTHENCOVERFILLPATHNVPROC)(GLuint, GLenum, GLuint, GLenum);
GLAPI PFNGLSTENCILTHENCOVERFILLPATHNVPROC glad_glStencilThenCoverFillPathNV;
#define glStencilThenCoverFillPathNV glad_glStencilThenCoverFillPathNV
typedef void (APIENTRYP PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC)(GLuint, GLint, GLuint, GLenum);
GLAPI PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC glad_glStencilThenCoverStrokePathNV;
#define glStencilThenCoverStrokePathNV glad_glStencilThenCoverStrokePathNV
typedef void (APIENTRYP PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC)(GLsizei, GLenum, const void*, GLuint, GLenum, GLuint, GLenum, GLenum, const GLfloat*);
GLAPI PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC glad_glStencilThenCoverFillPathInstancedNV;
#define glStencilThenCoverFillPathInstancedNV glad_glStencilThenCoverFillPathInstancedNV
typedef void (APIENTRYP PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC)(GLsizei, GLenum, const void*, GLuint, GLint, GLuint, GLenum, GLenum, const GLfloat*);
GLAPI PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC glad_glStencilThenCoverStrokePathInstancedNV;
#define glStencilThenCoverStrokePathInstancedNV glad_glStencilThenCoverStrokePathInstancedNV
typedef GLenum (APIENTRYP PFNGLPATHGLYPHINDEXRANGENVPROC)(GLenum, const void*, GLbitfield, GLuint, GLfloat, GLuint*);
GLAPI PFNGLPATHGLYPHINDEXRANGENVPROC glad_glPathGlyphIndexRangeNV;
#define glPathGlyphIndexRangeNV glad_glPathGlyphIndexRangeNV
typedef GLenum (APIENTRYP PFNGLPATHGLYPHINDEXARRAYNVPROC)(GLuint, GLenum, const void*, GLbitfield, GLuint, GLsizei, GLuint, GLfloat);
GLAPI PFNGLPATHGLYPHINDEXARRAYNVPROC glad_glPathGlyphIndexArrayNV;
#define glPathGlyphIndexArrayNV glad_glPathGlyphIndexArrayNV
typedef GLenum (APIENTRYP PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC)(GLuint, GLenum, GLsizeiptr, const void*, GLsizei, GLuint, GLsizei, GLuint, GLfloat);
GLAPI PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC glad_glPathMemoryGlyphIndexArrayNV;
#define glPathMemoryGlyphIndexArrayNV glad_glPathMemoryGlyphIndexArrayNV
typedef void (APIENTRYP PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC)(GLuint, GLint, GLenum, GLint, const GLfloat*);
GLAPI PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC glad_glProgramPathFragmentInputGenNV;
#define glProgramPathFragmentInputGenNV glad_glProgramPathFragmentInputGenNV
typedef void (APIENTRYP PFNGLGETPROGRAMRESOURCEFVNVPROC)(GLuint, GLenum, GLuint, GLsizei, const GLenum*, GLsizei, GLsizei*, GLfloat*);
GLAPI PFNGLGETPROGRAMRESOURCEFVNVPROC glad_glGetProgramResourcefvNV;
#define glGetProgramResourcefvNV glad_glGetProgramResourcefvNV
typedef void (APIENTRYP PFNGLPATHCOLORGENNVPROC)(GLenum, GLenum, GLenum, const GLfloat*);
GLAPI PFNGLPATHCOLORGENNVPROC glad_glPathColorGenNV;
#define glPathColorGenNV glad_glPathColorGenNV
typedef void (APIENTRYP PFNGLPATHTEXGENNVPROC)(GLenum, GLenum, GLint, const GLfloat*);
GLAPI PFNGLPATHTEXGENNVPROC glad_glPathTexGenNV;
#define glPathTexGenNV glad_glPathTexGenNV
typedef void (APIENTRYP PFNGLPATHFOGGENNVPROC)(GLenum);
GLAPI PFNGLPATHFOGGENNVPROC glad_glPathFogGenNV;
#define glPathFogGenNV glad_glPathFogGenNV
typedef void (APIENTRYP PFNGLGETPATHCOLORGENIVNVPROC)(GLenum, GLenum, GLint*);
GLAPI PFNGLGETPATHCOLORGENIVNVPROC glad_glGetPathColorGenivNV;
#define glGetPathColorGenivNV glad_glGetPathColorGenivNV
typedef void (APIENTRYP PFNGLGETPATHCOLORGENFVNVPROC)(GLenum, GLenum, GLfloat*);
GLAPI PFNGLGETPATHCOLORGENFVNVPROC glad_glGetPathColorGenfvNV;
#define glGetPathColorGenfvNV glad_glGetPathColorGenfvNV
typedef void (APIENTRYP PFNGLGETPATHTEXGENIVNVPROC)(GLenum, GLenum, GLint*);
GLAPI PFNGLGETPATHTEXGENIVNVPROC glad_glGetPathTexGenivNV;
#define glGetPathTexGenivNV glad_glGetPathTexGenivNV
typedef void (APIENTRYP PFNGLGETPATHTEXGENFVNVPROC)(GLenum, GLenum, GLfloat*);
GLAPI PFNGLGETPATHTEXGENFVNVPROC glad_glGetPathTexGenfvNV;
#define glGetPathTexGenfvNV glad_glGetPathTexGenfvNV
#endif
#ifndef GL_OES_texture_storage_multisample_2d_array
#define GL_OES_texture_storage_multisample_2d_array 1
GLAPI int GLAD_GL_OES_texture_storage_multisample_2d_array;
typedef void (APIENTRYP PFNGLTEXSTORAGE3DMULTISAMPLEOESPROC)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean);
GLAPI PFNGLTEXSTORAGE3DMULTISAMPLEOESPROC glad_glTexStorage3DMultisampleOES;
#define glTexStorage3DMultisampleOES glad_glTexStorage3DMultisampleOES
#endif
#ifndef GL_OES_draw_buffers_indexed
#define GL_OES_draw_buffers_indexed 1
GLAPI int GLAD_GL_OES_draw_buffers_indexed;
typedef void (APIENTRYP PFNGLENABLEIOESPROC)(GLenum, GLuint);
GLAPI PFNGLENABLEIOESPROC glad_glEnableiOES;
#define glEnableiOES glad_glEnableiOES
typedef void (APIENTRYP PFNGLDISABLEIOESPROC)(GLenum, GLuint);
GLAPI PFNGLDISABLEIOESPROC glad_glDisableiOES;
#define glDisableiOES glad_glDisableiOES
typedef void (APIENTRYP PFNGLBLENDEQUATIONIOESPROC)(GLuint, GLenum);
GLAPI PFNGLBLENDEQUATIONIOESPROC glad_glBlendEquationiOES;
#define glBlendEquationiOES glad_glBlendEquationiOES
typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIOESPROC)(GLuint, GLenum, GLenum);
GLAPI PFNGLBLENDEQUATIONSEPARATEIOESPROC glad_glBlendEquationSeparateiOES;
#define glBlendEquationSeparateiOES glad_glBlendEquationSeparateiOES
typedef void (APIENTRYP PFNGLBLENDFUNCIOESPROC)(GLuint, GLenum, GLenum);
GLAPI PFNGLBLENDFUNCIOESPROC glad_glBlendFunciOES;
#define glBlendFunciOES glad_glBlendFunciOES
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIOESPROC)(GLuint, GLenum, GLenum, GLenum, GLenum);
GLAPI PFNGLBLENDFUNCSEPARATEIOESPROC glad_glBlendFuncSeparateiOES;
#define glBlendFuncSeparateiOES glad_glBlendFuncSeparateiOES
typedef void (APIENTRYP PFNGLCOLORMASKIOESPROC)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean);
GLAPI PFNGLCOLORMASKIOESPROC glad_glColorMaskiOES;
#define glColorMaskiOES glad_glColorMaskiOES
typedef GLboolean (APIENTRYP PFNGLISENABLEDIOESPROC)(GLenum, GLuint);
GLAPI PFNGLISENABLEDIOESPROC glad_glIsEnablediOES;
#define glIsEnablediOES glad_glIsEnablediOES
#endif
#ifndef GL_OES_EGL_image
#define GL_OES_EGL_image 1
GLAPI int GLAD_GL_OES_EGL_image;
typedef void (APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum, GLeglImageOES);
GLAPI PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glad_glEGLImageTargetTexture2DOES;
#define glEGLImageTargetTexture2DOES glad_glEGLImageTargetTexture2DOES
typedef void (APIENTRYP PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC)(GLenum, GLeglImageOES);
GLAPI PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC glad_glEGLImageTargetRenderbufferStorageOES;
#define glEGLImageTargetRenderbufferStorageOES glad_glEGLImageTargetRenderbufferStorageOES
#endif
#ifndef GL_OES_vertex_type_10_10_10_2
#define GL_OES_vertex_type_10_10_10_2 1
GLAPI int GLAD_GL_OES_vertex_type_10_10_10_2;
#endif
#ifndef GL_APPLE_rgb_422
#define GL_APPLE_rgb_422 1
GLAPI int GLAD_GL_APPLE_rgb_422;
#endif
#ifndef GL_DMP_shader_binary
#define GL_DMP_shader_binary 1
GLAPI int GLAD_GL_DMP_shader_binary;
#endif
#ifndef GL_NV_texture_npot_2D_mipmap
#define GL_NV_texture_npot_2D_mipmap 1
GLAPI int GLAD_GL_NV_texture_npot_2D_mipmap;
#endif
#ifndef GL_OES_sample_variables
#define GL_OES_sample_variables 1
GLAPI int GLAD_GL_OES_sample_variables;
#endif
#ifndef GL_NV_explicit_attrib_location
#define GL_NV_explicit_attrib_location 1
GLAPI int GLAD_GL_NV_explicit_attrib_location;
#endif
#ifndef GL_NV_blend_equation_advanced
#define GL_NV_blend_equation_advanced 1
GLAPI int GLAD_GL_NV_blend_equation_advanced;
typedef void (APIENTRYP PFNGLBLENDPARAMETERINVPROC)(GLenum, GLint);
GLAPI PFNGLBLENDPARAMETERINVPROC glad_glBlendParameteriNV;
#define glBlendParameteriNV glad_glBlendParameteriNV
typedef void (APIENTRYP PFNGLBLENDBARRIERNVPROC)();
GLAPI PFNGLBLENDBARRIERNVPROC glad_glBlendBarrierNV;
#define glBlendBarrierNV glad_glBlendBarrierNV
#endif
#ifndef GL_EXT_draw_buffers
#define GL_EXT_draw_buffers 1
GLAPI int GLAD_GL_EXT_draw_buffers;
typedef void (APIENTRYP PFNGLDRAWBUFFERSEXTPROC)(GLsizei, const GLenum*);
GLAPI PFNGLDRAWBUFFERSEXTPROC glad_glDrawBuffersEXT;
#define glDrawBuffersEXT glad_glDrawBuffersEXT
#endif
#ifndef GL_EXT_debug_label
#define GL_EXT_debug_label 1
GLAPI int GLAD_GL_EXT_debug_label;
typedef void (APIENTRYP PFNGLLABELOBJECTEXTPROC)(GLenum, GLuint, GLsizei, const GLchar*);
GLAPI PFNGLLABELOBJECTEXTPROC glad_glLabelObjectEXT;
#define glLabelObjectEXT glad_glLabelObjectEXT
typedef void (APIENTRYP PFNGLGETOBJECTLABELEXTPROC)(GLenum, GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETOBJECTLABELEXTPROC glad_glGetObjectLabelEXT;
#define glGetObjectLabelEXT glad_glGetObjectLabelEXT
#endif
#ifndef GL_NV_read_stencil
#define GL_NV_read_stencil 1
GLAPI int GLAD_GL_NV_read_stencil;
#endif
#ifndef GL_NV_conditional_render
#define GL_NV_conditional_render 1
GLAPI int GLAD_GL_NV_conditional_render;
typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERNVPROC)(GLuint, GLenum);
GLAPI PFNGLBEGINCONDITIONALRENDERNVPROC glad_glBeginConditionalRenderNV;
#define glBeginConditionalRenderNV glad_glBeginConditionalRenderNV
typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERNVPROC)();
GLAPI PFNGLENDCONDITIONALRENDERNVPROC glad_glEndConditionalRenderNV;
#define glEndConditionalRenderNV glad_glEndConditionalRenderNV
#endif
#ifndef GL_EXT_texture_filter_anisotropic
#define GL_EXT_texture_filter_anisotropic 1
GLAPI int GLAD_GL_EXT_texture_filter_anisotropic;
#endif
#ifndef GL_EXT_blend_minmax
#define GL_EXT_blend_minmax 1
GLAPI int GLAD_GL_EXT_blend_minmax;
typedef void (APIENTRYP PFNGLBLENDEQUATIONEXTPROC)(GLenum);
GLAPI PFNGLBLENDEQUATIONEXTPROC glad_glBlendEquationEXT;
#define glBlendEquationEXT glad_glBlendEquationEXT
#endif
#ifndef GL_NV_internalformat_sample_query
#define GL_NV_internalformat_sample_query 1
GLAPI int GLAD_GL_NV_internalformat_sample_query;
typedef void (APIENTRYP PFNGLGETINTERNALFORMATSAMPLEIVNVPROC)(GLenum, GLenum, GLsizei, GLenum, GLsizei, GLint*);
GLAPI PFNGLGETINTERNALFORMATSAMPLEIVNVPROC glad_glGetInternalformatSampleivNV;
#define glGetInternalformatSampleivNV glad_glGetInternalformatSampleivNV
#endif
#ifndef GL_OES_depth24
#define GL_OES_depth24 1
GLAPI int GLAD_GL_OES_depth24;
#endif
#ifndef GL_NV_instanced_arrays
#define GL_NV_instanced_arrays 1
GLAPI int GLAD_GL_NV_instanced_arrays;
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORNVPROC)(GLuint, GLuint);
GLAPI PFNGLVERTEXATTRIBDIVISORNVPROC glad_glVertexAttribDivisorNV;
#define glVertexAttribDivisorNV glad_glVertexAttribDivisorNV
#endif
#ifndef GL_EXT_texture_storage
#define GL_EXT_texture_storage 1
GLAPI int GLAD_GL_EXT_texture_storage;
typedef void (APIENTRYP PFNGLTEXSTORAGE1DEXTPROC)(GLenum, GLsizei, GLenum, GLsizei);
GLAPI PFNGLTEXSTORAGE1DEXTPROC glad_glTexStorage1DEXT;
#define glTexStorage1DEXT glad_glTexStorage1DEXT
typedef void (APIENTRYP PFNGLTEXSTORAGE2DEXTPROC)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI PFNGLTEXSTORAGE2DEXTPROC glad_glTexStorage2DEXT;
#define glTexStorage2DEXT glad_glTexStorage2DEXT
typedef void (APIENTRYP PFNGLTEXSTORAGE3DEXTPROC)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
GLAPI PFNGLTEXSTORAGE3DEXTPROC glad_glTexStorage3DEXT;
#define glTexStorage3DEXT glad_glTexStorage3DEXT
typedef void (APIENTRYP PFNGLTEXTURESTORAGE1DEXTPROC)(GLuint, GLenum, GLsizei, GLenum, GLsizei);
GLAPI PFNGLTEXTURESTORAGE1DEXTPROC glad_glTextureStorage1DEXT;
#define glTextureStorage1DEXT glad_glTextureStorage1DEXT
typedef void (APIENTRYP PFNGLTEXTURESTORAGE2DEXTPROC)(GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI PFNGLTEXTURESTORAGE2DEXTPROC glad_glTextureStorage2DEXT;
#define glTextureStorage2DEXT glad_glTextureStorage2DEXT
typedef void (APIENTRYP PFNGLTEXTURESTORAGE3DEXTPROC)(GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
GLAPI PFNGLTEXTURESTORAGE3DEXTPROC glad_glTextureStorage3DEXT;
#define glTextureStorage3DEXT glad_glTextureStorage3DEXT
#endif
#ifndef GL_OES_gpu_shader5
#define GL_OES_gpu_shader5 1
GLAPI int GLAD_GL_OES_gpu_shader5;
#endif
#ifndef GL_OES_required_internalformat
#define GL_OES_required_internalformat 1
GLAPI int GLAD_GL_OES_required_internalformat;
#endif
#ifndef GL_EXT_sRGB_write_control
#define GL_EXT_sRGB_write_control 1
GLAPI int GLAD_GL_EXT_sRGB_write_control;
#endif
#ifndef GL_EXT_copy_image
#define GL_EXT_copy_image 1
GLAPI int GLAD_GL_EXT_copy_image;
typedef void (APIENTRYP PFNGLCOPYIMAGESUBDATAEXTPROC)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei);
GLAPI PFNGLCOPYIMAGESUBDATAEXTPROC glad_glCopyImageSubDataEXT;
#define glCopyImageSubDataEXT glad_glCopyImageSubDataEXT
#endif
#ifndef GL_AMD_compressed_3DC_texture
#define GL_AMD_compressed_3DC_texture 1
GLAPI int GLAD_GL_AMD_compressed_3DC_texture;
#endif
#ifndef GL_OES_element_index_uint
#define GL_OES_element_index_uint 1
GLAPI int GLAD_GL_OES_element_index_uint;
#endif
#ifndef GL_OES_sample_shading
#define GL_OES_sample_shading 1
GLAPI int GLAD_GL_OES_sample_shading;
typedef void (APIENTRYP PFNGLMINSAMPLESHADINGOESPROC)(GLfloat);
GLAPI PFNGLMINSAMPLESHADINGOESPROC glad_glMinSampleShadingOES;
#define glMinSampleShadingOES glad_glMinSampleShadingOES
#endif
#ifndef GL_IMG_texture_compression_pvrtc2
#define GL_IMG_texture_compression_pvrtc2 1
GLAPI int GLAD_GL_IMG_texture_compression_pvrtc2;
#endif
#ifndef GL_OES_rgb8_rgba8
#define GL_OES_rgb8_rgba8 1
GLAPI int GLAD_GL_OES_rgb8_rgba8;
#endif
#ifndef GL_EXT_map_buffer_range
#define GL_EXT_map_buffer_range 1
GLAPI int GLAD_GL_EXT_map_buffer_range;
typedef void* (APIENTRYP PFNGLMAPBUFFERRANGEEXTPROC)(GLenum, GLintptr, GLsizeiptr, GLbitfield);
GLAPI PFNGLMAPBUFFERRANGEEXTPROC glad_glMapBufferRangeEXT;
#define glMapBufferRangeEXT glad_glMapBufferRangeEXT
typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC)(GLenum, GLintptr, GLsizeiptr);
GLAPI PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC glad_glFlushMappedBufferRangeEXT;
#define glFlushMappedBufferRangeEXT glad_glFlushMappedBufferRangeEXT
#endif
#ifndef GL_OES_mapbuffer
#define GL_OES_mapbuffer 1
GLAPI int GLAD_GL_OES_mapbuffer;
typedef void* (APIENTRYP PFNGLMAPBUFFEROESPROC)(GLenum, GLenum);
GLAPI PFNGLMAPBUFFEROESPROC glad_glMapBufferOES;
#define glMapBufferOES glad_glMapBufferOES
typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFEROESPROC)(GLenum);
GLAPI PFNGLUNMAPBUFFEROESPROC glad_glUnmapBufferOES;
#define glUnmapBufferOES glad_glUnmapBufferOES
typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVOESPROC)(GLenum, GLenum, void**);
GLAPI PFNGLGETBUFFERPOINTERVOESPROC glad_glGetBufferPointervOES;
#define glGetBufferPointervOES glad_glGetBufferPointervOES
#endif
#ifndef GL_ARM_shader_framebuffer_fetch_depth_stencil
#define GL_ARM_shader_framebuffer_fetch_depth_stencil 1
GLAPI int GLAD_GL_ARM_shader_framebuffer_fetch_depth_stencil;
#endif
#ifndef GL_NV_image_formats
#define GL_NV_image_formats 1
GLAPI int GLAD_GL_NV_image_formats;
#endif
#ifndef GL_OES_EGL_image_external
#define GL_OES_EGL_image_external 1
GLAPI int GLAD_GL_OES_EGL_image_external;
#endif
#ifndef GL_APPLE_texture_format_BGRA8888
#define GL_APPLE_texture_format_BGRA8888 1
GLAPI int GLAD_GL_APPLE_texture_format_BGRA8888;
#endif
#ifndef GL_AMD_performance_monitor
#define GL_AMD_performance_monitor 1
GLAPI int GLAD_GL_AMD_performance_monitor;
typedef void (APIENTRYP PFNGLGETPERFMONITORGROUPSAMDPROC)(GLint*, GLsizei, GLuint*);
GLAPI PFNGLGETPERFMONITORGROUPSAMDPROC glad_glGetPerfMonitorGroupsAMD;
#define glGetPerfMonitorGroupsAMD glad_glGetPerfMonitorGroupsAMD
typedef void (APIENTRYP PFNGLGETPERFMONITORCOUNTERSAMDPROC)(GLuint, GLint*, GLint*, GLsizei, GLuint*);
GLAPI PFNGLGETPERFMONITORCOUNTERSAMDPROC glad_glGetPerfMonitorCountersAMD;
#define glGetPerfMonitorCountersAMD glad_glGetPerfMonitorCountersAMD
typedef void (APIENTRYP PFNGLGETPERFMONITORGROUPSTRINGAMDPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETPERFMONITORGROUPSTRINGAMDPROC glad_glGetPerfMonitorGroupStringAMD;
#define glGetPerfMonitorGroupStringAMD glad_glGetPerfMonitorGroupStringAMD
typedef void (APIENTRYP PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC)(GLuint, GLuint, GLsizei, GLsizei*, GLchar*);
GLAPI PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC glad_glGetPerfMonitorCounterStringAMD;
#define glGetPerfMonitorCounterStringAMD glad_glGetPerfMonitorCounterStringAMD
typedef void (APIENTRYP PFNGLGETPERFMONITORCOUNTERINFOAMDPROC)(GLuint, GLuint, GLenum, void*);
GLAPI PFNGLGETPERFMONITORCOUNTERINFOAMDPROC glad_glGetPerfMonitorCounterInfoAMD;
#define glGetPerfMonitorCounterInfoAMD glad_glGetPerfMonitorCounterInfoAMD
typedef void (APIENTRYP PFNGLGENPERFMONITORSAMDPROC)(GLsizei, GLuint*);
GLAPI PFNGLGENPERFMONITORSAMDPROC glad_glGenPerfMonitorsAMD;
#define glGenPerfMonitorsAMD glad_glGenPerfMonitorsAMD
typedef void (APIENTRYP PFNGLDELETEPERFMONITORSAMDPROC)(GLsizei, GLuint*);
GLAPI PFNGLDELETEPERFMONITORSAMDPROC glad_glDeletePerfMonitorsAMD;
#define glDeletePerfMonitorsAMD glad_glDeletePerfMonitorsAMD
typedef void (APIENTRYP PFNGLSELECTPERFMONITORCOUNTERSAMDPROC)(GLuint, GLboolean, GLuint, GLint, GLuint*);
GLAPI PFNGLSELECTPERFMONITORCOUNTERSAMDPROC glad_glSelectPerfMonitorCountersAMD;
#define glSelectPerfMonitorCountersAMD glad_glSelectPerfMonitorCountersAMD
typedef void (APIENTRYP PFNGLBEGINPERFMONITORAMDPROC)(GLuint);
GLAPI PFNGLBEGINPERFMONITORAMDPROC glad_glBeginPerfMonitorAMD;
#define glBeginPerfMonitorAMD glad_glBeginPerfMonitorAMD
typedef void (APIENTRYP PFNGLENDPERFMONITORAMDPROC)(GLuint);
GLAPI PFNGLENDPERFMONITORAMDPROC glad_glEndPerfMonitorAMD;
#define glEndPerfMonitorAMD glad_glEndPerfMonitorAMD
typedef void (APIENTRYP PFNGLGETPERFMONITORCOUNTERDATAAMDPROC)(GLuint, GLenum, GLsizei, GLuint*, GLint*);
GLAPI PFNGLGETPERFMONITORCOUNTERDATAAMDPROC glad_glGetPerfMonitorCounterDataAMD;
#define glGetPerfMonitorCounterDataAMD glad_glGetPerfMonitorCounterDataAMD
#endif
#ifndef GL_VIV_shader_binary
#define GL_VIV_shader_binary 1
GLAPI int GLAD_GL_VIV_shader_binary;
#endif
#ifndef GL_NV_shadow_samplers_array
#define GL_NV_shadow_samplers_array 1
GLAPI int GLAD_GL_NV_shadow_samplers_array;
#endif
#ifndef GL_IMG_multisampled_render_to_texture
#define GL_IMG_multisampled_render_to_texture 1
GLAPI int GLAD_GL_IMG_multisampled_render_to_texture;
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
GLAPI PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC glad_glRenderbufferStorageMultisampleIMG;
#define glRenderbufferStorageMultisampleIMG glad_glRenderbufferStorageMultisampleIMG
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei);
GLAPI PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC glad_glFramebufferTexture2DMultisampleIMG;
#define glFramebufferTexture2DMultisampleIMG glad_glFramebufferTexture2DMultisampleIMG
#endif
#ifndef GL_NV_depth_nonlinear
#define GL_NV_depth_nonlinear 1
GLAPI int GLAD_GL_NV_depth_nonlinear;
#endif
#ifndef GL_OES_draw_elements_base_vertex
#define GL_OES_draw_elements_base_vertex 1
GLAPI int GLAD_GL_OES_draw_elements_base_vertex;
typedef void (APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXOESPROC)(GLenum, GLsizei, GLenum, const void*, GLint);
GLAPI PFNGLDRAWELEMENTSBASEVERTEXOESPROC glad_glDrawElementsBaseVertexOES;
#define glDrawElementsBaseVertexOES glad_glDrawElementsBaseVertexOES
typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXOESPROC)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void*, GLint);
GLAPI PFNGLDRAWRANGEELEMENTSBASEVERTEXOESPROC glad_glDrawRangeElementsBaseVertexOES;
#define glDrawRangeElementsBaseVertexOES glad_glDrawRangeElementsBaseVertexOES
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXOESPROC)(GLenum, GLsizei, GLenum, const void*, GLsizei, GLint);
GLAPI PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXOESPROC glad_glDrawElementsInstancedBaseVertexOES;
#define glDrawElementsInstancedBaseVertexOES glad_glDrawElementsInstancedBaseVertexOES
typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXOESPROC)(GLenum, const GLsizei*, GLenum, const void**, GLsizei, const GLint*);
GLAPI PFNGLMULTIDRAWELEMENTSBASEVERTEXOESPROC glad_glMultiDrawElementsBaseVertexOES;
#define glMultiDrawElementsBaseVertexOES glad_glMultiDrawElementsBaseVertexOES
#endif
#ifndef GL_NV_shader_noperspective_interpolation
#define GL_NV_shader_noperspective_interpolation 1
GLAPI int GLAD_GL_NV_shader_noperspective_interpolation;
#endif
#ifndef GL_EXT_texture_format_BGRA8888
#define GL_EXT_texture_format_BGRA8888 1
GLAPI int GLAD_GL_EXT_texture_format_BGRA8888;
#endif
#ifndef GL_OES_texture_cube_map_array
#define GL_OES_texture_cube_map_array 1
GLAPI int GLAD_GL_OES_texture_cube_map_array;
#endif
#ifndef GL_APPLE_copy_texture_levels
#define GL_APPLE_copy_texture_levels 1
GLAPI int GLAD_GL_APPLE_copy_texture_levels;
typedef void (APIENTRYP PFNGLCOPYTEXTURELEVELSAPPLEPROC)(GLuint, GLuint, GLint, GLsizei);
GLAPI PFNGLCOPYTEXTURELEVELSAPPLEPROC glad_glCopyTextureLevelsAPPLE;
#define glCopyTextureLevelsAPPLE glad_glCopyTextureLevelsAPPLE
#endif
#ifndef GL_ARM_mali_shader_binary
#define GL_ARM_mali_shader_binary 1
GLAPI int GLAD_GL_ARM_mali_shader_binary;
#endif
#ifndef GL_EXT_multiview_draw_buffers
#define GL_EXT_multiview_draw_buffers 1
GLAPI int GLAD_GL_EXT_multiview_draw_buffers;
typedef void (APIENTRYP PFNGLREADBUFFERINDEXEDEXTPROC)(GLenum, GLint);
GLAPI PFNGLREADBUFFERINDEXEDEXTPROC glad_glReadBufferIndexedEXT;
#define glReadBufferIndexedEXT glad_glReadBufferIndexedEXT
typedef void (APIENTRYP PFNGLDRAWBUFFERSINDEXEDEXTPROC)(GLint, const GLenum*, const GLint*);
GLAPI PFNGLDRAWBUFFERSINDEXEDEXTPROC glad_glDrawBuffersIndexedEXT;
#define glDrawBuffersIndexedEXT glad_glDrawBuffersIndexedEXT
typedef void (APIENTRYP PFNGLGETINTEGERI_VEXTPROC)(GLenum, GLuint, GLint*);
GLAPI PFNGLGETINTEGERI_VEXTPROC glad_glGetIntegeri_vEXT;
#define glGetIntegeri_vEXT glad_glGetIntegeri_vEXT
#endif
#ifndef GL_OES_texture_stencil8
#define GL_OES_texture_stencil8 1
GLAPI int GLAD_GL_OES_texture_stencil8;
#endif
#ifndef GL_QCOM_extended_get
#define GL_QCOM_extended_get 1
GLAPI int GLAD_GL_QCOM_extended_get;
typedef void (APIENTRYP PFNGLEXTGETTEXTURESQCOMPROC)(GLuint*, GLint, GLint*);
GLAPI PFNGLEXTGETTEXTURESQCOMPROC glad_glExtGetTexturesQCOM;
#define glExtGetTexturesQCOM glad_glExtGetTexturesQCOM
typedef void (APIENTRYP PFNGLEXTGETBUFFERSQCOMPROC)(GLuint*, GLint, GLint*);
GLAPI PFNGLEXTGETBUFFERSQCOMPROC glad_glExtGetBuffersQCOM;
#define glExtGetBuffersQCOM glad_glExtGetBuffersQCOM
typedef void (APIENTRYP PFNGLEXTGETRENDERBUFFERSQCOMPROC)(GLuint*, GLint, GLint*);
GLAPI PFNGLEXTGETRENDERBUFFERSQCOMPROC glad_glExtGetRenderbuffersQCOM;
#define glExtGetRenderbuffersQCOM glad_glExtGetRenderbuffersQCOM
typedef void (APIENTRYP PFNGLEXTGETFRAMEBUFFERSQCOMPROC)(GLuint*, GLint, GLint*);
GLAPI PFNGLEXTGETFRAMEBUFFERSQCOMPROC glad_glExtGetFramebuffersQCOM;
#define glExtGetFramebuffersQCOM glad_glExtGetFramebuffersQCOM
typedef void (APIENTRYP PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC)(GLuint, GLenum, GLint, GLenum, GLint*);
GLAPI PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC glad_glExtGetTexLevelParameterivQCOM;
#define glExtGetTexLevelParameterivQCOM glad_glExtGetTexLevelParameterivQCOM
typedef void (APIENTRYP PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC)(GLenum, GLenum, GLint);
GLAPI PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC glad_glExtTexObjectStateOverrideiQCOM;
#define glExtTexObjectStateOverrideiQCOM glad_glExtTexObjectStateOverrideiQCOM
typedef void (APIENTRYP PFNGLEXTGETTEXSUBIMAGEQCOMPROC)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, void*);
GLAPI PFNGLEXTGETTEXSUBIMAGEQCOMPROC glad_glExtGetTexSubImageQCOM;
#define glExtGetTexSubImageQCOM glad_glExtGetTexSubImageQCOM
typedef void (APIENTRYP PFNGLEXTGETBUFFERPOINTERVQCOMPROC)(GLenum, void**);
GLAPI PFNGLEXTGETBUFFERPOINTERVQCOMPROC glad_glExtGetBufferPointervQCOM;
#define glExtGetBufferPointervQCOM glad_glExtGetBufferPointervQCOM
#endif
#ifndef GL_EXT_geometry_shader
#define GL_EXT_geometry_shader 1
GLAPI int GLAD_GL_EXT_geometry_shader;
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREEXTPROC)(GLenum, GLenum, GLuint, GLint);
GLAPI PFNGLFRAMEBUFFERTEXTUREEXTPROC glad_glFramebufferTextureEXT;
#define glFramebufferTextureEXT glad_glFramebufferTextureEXT
#endif
#ifndef GL_EXT_texture_norm16
#define GL_EXT_texture_norm16 1
GLAPI int GLAD_GL_EXT_texture_norm16;
#endif
#ifndef GL_EXT_texture_rg
#define GL_EXT_texture_rg 1
GLAPI int GLAD_GL_EXT_texture_rg;
#endif
#ifndef GL_OES_standard_derivatives
#define GL_OES_standard_derivatives 1
GLAPI int GLAD_GL_OES_standard_derivatives;
#endif
#ifndef GL_EXT_texture_border_clamp
#define GL_EXT_texture_border_clamp 1
GLAPI int GLAD_GL_EXT_texture_border_clamp;
typedef void (APIENTRYP PFNGLTEXPARAMETERIIVEXTPROC)(GLenum, GLenum, const GLint*);
GLAPI PFNGLTEXPARAMETERIIVEXTPROC glad_glTexParameterIivEXT;
#define glTexParameterIivEXT glad_glTexParameterIivEXT
typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVEXTPROC)(GLenum, GLenum, const GLuint*);
GLAPI PFNGLTEXPARAMETERIUIVEXTPROC glad_glTexParameterIuivEXT;
#define glTexParameterIuivEXT glad_glTexParameterIuivEXT
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVEXTPROC)(GLenum, GLenum, GLint*);
GLAPI PFNGLGETTEXPARAMETERIIVEXTPROC glad_glGetTexParameterIivEXT;
#define glGetTexParameterIivEXT glad_glGetTexParameterIivEXT
typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVEXTPROC)(GLenum, GLenum, GLuint*);
GLAPI PFNGLGETTEXPARAMETERIUIVEXTPROC glad_glGetTexParameterIuivEXT;
#define glGetTexParameterIuivEXT glad_glGetTexParameterIuivEXT
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVEXTPROC)(GLuint, GLenum, const GLint*);
GLAPI PFNGLSAMPLERPARAMETERIIVEXTPROC glad_glSamplerParameterIivEXT;
#define glSamplerParameterIivEXT glad_glSamplerParameterIivEXT
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVEXTPROC)(GLuint, GLenum, const GLuint*);
GLAPI PFNGLSAMPLERPARAMETERIUIVEXTPROC glad_glSamplerParameterIuivEXT;
#define glSamplerParameterIuivEXT glad_glSamplerParameterIuivEXT
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVEXTPROC)(GLuint, GLenum, GLint*);
GLAPI PFNGLGETSAMPLERPARAMETERIIVEXTPROC glad_glGetSamplerParameterIivEXT;
#define glGetSamplerParameterIivEXT glad_glGetSamplerParameterIivEXT
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVEXTPROC)(GLuint, GLenum, GLuint*);
GLAPI PFNGLGETSAMPLERPARAMETERIUIVEXTPROC glad_glGetSamplerParameterIuivEXT;
#define glGetSamplerParameterIuivEXT glad_glGetSamplerParameterIuivEXT
#endif
#ifndef GL_OES_shader_image_atomic
#define GL_OES_shader_image_atomic 1
GLAPI int GLAD_GL_OES_shader_image_atomic;
#endif
#ifndef GL_ANGLE_framebuffer_blit
#define GL_ANGLE_framebuffer_blit 1
GLAPI int GLAD_GL_ANGLE_framebuffer_blit;
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFERANGLEPROC)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
GLAPI PFNGLBLITFRAMEBUFFERANGLEPROC glad_glBlitFramebufferANGLE;
#define glBlitFramebufferANGLE glad_glBlitFramebufferANGLE
#endif
#ifndef GL_EXT_draw_instanced
#define GL_EXT_draw_instanced 1
GLAPI int GLAD_GL_EXT_draw_instanced;
#endif
#ifndef GL_OES_texture_3D
#define GL_OES_texture_3D 1
GLAPI int GLAD_GL_OES_texture_3D;
typedef void (APIENTRYP PFNGLTEXIMAGE3DOESPROC)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
GLAPI PFNGLTEXIMAGE3DOESPROC glad_glTexImage3DOES;
#define glTexImage3DOES glad_glTexImage3DOES
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DOESPROC)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void*);
GLAPI PFNGLTEXSUBIMAGE3DOESPROC glad_glTexSubImage3DOES;
#define glTexSubImage3DOES glad_glTexSubImage3DOES
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DOESPROC)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
GLAPI PFNGLCOPYTEXSUBIMAGE3DOESPROC glad_glCopyTexSubImage3DOES;
#define glCopyTexSubImage3DOES glad_glCopyTexSubImage3DOES
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DOESPROC)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void*);
GLAPI PFNGLCOMPRESSEDTEXIMAGE3DOESPROC glad_glCompressedTexImage3DOES;
#define glCompressedTexImage3DOES glad_glCompressedTexImage3DOES
typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void*);
GLAPI PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC glad_glCompressedTexSubImage3DOES;
#define glCompressedTexSubImage3DOES glad_glCompressedTexSubImage3DOES
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DOESPROC)(GLenum, GLenum, GLenum, GLuint, GLint, GLint);
GLAPI PFNGLFRAMEBUFFERTEXTURE3DOESPROC glad_glFramebufferTexture3DOES;
#define glFramebufferTexture3DOES glad_glFramebufferTexture3DOES
#endif
#ifndef GL_ARM_rgba8
#define GL_ARM_rgba8 1
GLAPI int GLAD_GL_ARM_rgba8;
#endif
#ifndef GL_EXT_tessellation_shader
#define GL_EXT_tessellation_shader 1
GLAPI int GLAD_GL_EXT_tessellation_shader;
typedef void (APIENTRYP PFNGLPATCHPARAMETERIEXTPROC)(GLenum, GLint);
GLAPI PFNGLPATCHPARAMETERIEXTPROC glad_glPatchParameteriEXT;
#define glPatchParameteriEXT glad_glPatchParameteriEXT
#endif
#ifndef GL_OES_primitive_bounding_box
#define GL_OES_primitive_bounding_box 1
GLAPI int GLAD_GL_OES_primitive_bounding_box;
typedef void (APIENTRYP PFNGLPRIMITIVEBOUNDINGBOXOESPROC)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
GLAPI PFNGLPRIMITIVEBOUNDINGBOXOESPROC glad_glPrimitiveBoundingBoxOES;
#define glPrimitiveBoundingBoxOES glad_glPrimitiveBoundingBoxOES
#endif
#ifndef GL_NV_read_buffer
#define GL_NV_read_buffer 1
GLAPI int GLAD_GL_NV_read_buffer;
typedef void (APIENTRYP PFNGLREADBUFFERNVPROC)(GLenum);
GLAPI PFNGLREADBUFFERNVPROC glad_glReadBufferNV;
#define glReadBufferNV glad_glReadBufferNV
#endif
#ifndef GL_NV_generate_mipmap_sRGB
#define GL_NV_generate_mipmap_sRGB 1
GLAPI int GLAD_GL_NV_generate_mipmap_sRGB;
#endif
#ifndef GL_EXT_texture_cube_map_array
#define GL_EXT_texture_cube_map_array 1
GLAPI int GLAD_GL_EXT_texture_cube_map_array;
#endif
#ifndef GL_OES_EGL_image_external_essl3
#define GL_OES_EGL_image_external_essl3 1
GLAPI int GLAD_GL_OES_EGL_image_external_essl3;
#endif
#ifndef GL_KHR_context_flush_control
#define GL_KHR_context_flush_control 1
GLAPI int GLAD_GL_KHR_context_flush_control;
#endif
#ifndef GL_EXT_texture_type_2_10_10_10_REV
#define GL_EXT_texture_type_2_10_10_10_REV 1
GLAPI int GLAD_GL_EXT_texture_type_2_10_10_10_REV;
#endif
#ifndef GL_ANGLE_instanced_arrays
#define GL_ANGLE_instanced_arrays 1
GLAPI int GLAD_GL_ANGLE_instanced_arrays;
typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDANGLEPROC)(GLenum, GLint, GLsizei, GLsizei);
GLAPI PFNGLDRAWARRAYSINSTANCEDANGLEPROC glad_glDrawArraysInstancedANGLE;
#define glDrawArraysInstancedANGLE glad_glDrawArraysInstancedANGLE
typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDANGLEPROC)(GLenum, GLsizei, GLenum, const void*, GLsizei);
GLAPI PFNGLDRAWELEMENTSINSTANCEDANGLEPROC glad_glDrawElementsInstancedANGLE;
#define glDrawElementsInstancedANGLE glad_glDrawElementsInstancedANGLE
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORANGLEPROC)(GLuint, GLuint);
GLAPI PFNGLVERTEXATTRIBDIVISORANGLEPROC glad_glVertexAttribDivisorANGLE;
#define glVertexAttribDivisorANGLE glad_glVertexAttribDivisorANGLE
#endif
#ifndef GL_EXT_discard_framebuffer
#define GL_EXT_discard_framebuffer 1
GLAPI int GLAD_GL_EXT_discard_framebuffer;
typedef void (APIENTRYP PFNGLDISCARDFRAMEBUFFEREXTPROC)(GLenum, GLsizei, const GLenum*);
GLAPI PFNGLDISCARDFRAMEBUFFEREXTPROC glad_glDiscardFramebufferEXT;
#define glDiscardFramebufferEXT glad_glDiscardFramebufferEXT
#endif
#ifndef GL_EXT_texture_sRGB_decode
#define GL_EXT_texture_sRGB_decode 1
GLAPI int GLAD_GL_EXT_texture_sRGB_decode;
#endif
#ifndef GL_NV_read_depth
#define GL_NV_read_depth 1
GLAPI int GLAD_GL_NV_read_depth;
#endif
#ifndef GL_ARM_mali_program_binary
#define GL_ARM_mali_program_binary 1
GLAPI int GLAD_GL_ARM_mali_program_binary;
#endif
#ifndef GL_AMD_compressed_ATC_texture
#define GL_AMD_compressed_ATC_texture 1
GLAPI int GLAD_GL_AMD_compressed_ATC_texture;
#endif
#ifndef GL_APPLE_texture_packed_float
#define GL_APPLE_texture_packed_float 1
GLAPI int GLAD_GL_APPLE_texture_packed_float;
#endif
#ifndef GL_EXT_texture_buffer
#define GL_EXT_texture_buffer 1
GLAPI int GLAD_GL_EXT_texture_buffer;
typedef void (APIENTRYP PFNGLTEXBUFFEREXTPROC)(GLenum, GLenum, GLuint);
GLAPI PFNGLTEXBUFFEREXTPROC glad_glTexBufferEXT;
#define glTexBufferEXT glad_glTexBufferEXT
typedef void (APIENTRYP PFNGLTEXBUFFERRANGEEXTPROC)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr);
GLAPI PFNGLTEXBUFFERRANGEEXTPROC glad_glTexBufferRangeEXT;
#define glTexBufferRangeEXT glad_glTexBufferRangeEXT
#endif

#ifdef __cplusplus
}
#endif

#endif
