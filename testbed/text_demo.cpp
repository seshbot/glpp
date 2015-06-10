#include <glpp/glpp.h>

#ifdef _MSC_VER
#  include <glpp/gles2.h>
   namespace gl { using namespace gles2; }
#else
#  include <glpp/gl2.h>
   namespace gl { using namespace gl2; }
#endif


static auto VERT_SHADER_SOURCE = R"(
#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

attribute mediump vec2 pos;
attribute lowp vec4 col;

varying mediump vec3 frag_pos;
varying lowp vec4 frag_col;

void main() {
   gl_Position = vec4(pos, 0., 1.);
   frag_col = col;
}
)";

static auto FRAG_SHADER_SOURCE = R"(
#ifndef GL_ES
#define highp
#define mediump
#define lowp
#endif

varying lowp vec4 frag_col;

void main() {
   gl_FragColor = frag_col;
}
)";

static auto SAMPLE_TEXT = R"(a line of text
another line of text with some data
whats going on another line of text with some data
another line but something
what is going on here
something should really happen here
this is weird)";


void text_demo() {
   auto key_handler = [&](glpp::context & ctx, glpp::Key key, int scancode, glpp::KeyAction action, int mods) {
      if (key == glpp::KEY_ESCAPE && action == glpp::KEY_ACTION_PRESS)
         ctx.win().set_should_close();
   };

   glpp::init();
   glpp::context context(key_handler);

   glpp::program prg(
      glpp::shader::create_from_source(VERT_SHADER_SOURCE, glpp::shader::Vertex),
      glpp::shader::create_from_source(FRAG_SHADER_SOURCE, glpp::shader::Fragment)
      );

   auto text_buffer = glpp::describe_debug_text_buffer(SAMPLE_TEXT, -.95f, .95f, .0025f)
      .attrib("pos", 2)
      .skip_bytes(4 * 2) // skip [z, col] words
      .build(prg);

   while (!context.win().closing()) {
      prg.use();
      gl::vertex_attrib_4f(prg.attrib("col").location(), .8f, .8f, .8f, 1.f);
      glpp::draw(text_buffer, glpp::DrawMode::Triangles);
      context.win().swap();
   }

   glpp::shutdown();
}