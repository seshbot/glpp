GLpp OpenGL 2.0 ES wrapper library
=============

This library helps C++ programmers avoid some of the general problems when programming with OpenGL:

 - wrappers around OpenGL abstractions that create and validate their own resources
 - run-time checking of VBO descriptors against the size of the data contained in that buffer
 - strongly typed 'VBO buffer descriptor' abstraction allows VBO buffers to be specified in a safer manner 
 - a 'render pass' abstraction that takes care of binding the correct program before setting uniforms and attributes
 - an FBO abstraction that works with the render pass to allow trivial implementation of multiple shader passes

In addition it contains:
 - uses Google's [ANGLE](https://code.google.com/p/angleproject/) OpenGL 2.0 ES impelmentation to ensure Windows compatability without requiring updated OpenGL driver installation
 - uses [SOIL2](https://bitbucket.org/SpartanJ/soil2) to allow images to be read/written to
 - uses modern C++ idioms such as providing lambdas for 'active' uniform values, movable value semantics for tracking expensive resources and <code>initializer_list</code> initialization for data buffers

Motivation
-------
GLpp is different to other C++ graphics libraries in that it works directly in terms of OpenGL programs, shaders, VBOs, FBOs etc. This means that it is much easier to leverage the underlying OpenGL implementation directly instead of learning a separate technology.

GLpp also provides several higher-level abstractions (such as the <code>pass</code> class) that allow the composition of higher-level rendering activities.
    
    gl::context context { key_handler }; // key handler is a std::function
    
    //
    // raw data
    //
    static const float screen_verts[] = {
      -1., 1., 0., 1.,
      1., 1., 1., 1.,
      -1., -1., 0., 0.,
      1., -1., 1., 0.,
    };

    static const unsigned short screen_indices[] = {
      0, 2, 1,
      1, 2, 3,
    };

    //
    // set up shader passes
    //
    auto prg_bg = gl::program(gl::shader("bg.vert"),gl::shader("bg.frag"));
    auto prg_post = gl::program(gl::shader("post.vert"),gl::shader("post.frag"));

    auto bg_pass_buff = gl::describe_buffer({ screen_verts, screen_indices })
      .add(prg_bg.attrib("position"), 2)
      .add(prg_bg.attrib("tex_coords"), 2).build();

    auto post_pass_buff = gl::describe_buffer({ screen_verts, screen_indices })
      .add(prg_post.attrib("position"), 2)
      .add(prg_post.attrib("tex_coords"), 2).build();

    gl::texture_t bg_tex{ "bg.png" };
    auto bg_pass = prg_bg.pass()
      .with(bg_pass_buff)
      .set_uniform("texture", bg_tex);
      
    // its very simple to pass an FBO texture to post-processing passes
    auto fbo = gl::frame_buffer_t{ context.win().frame_buffer_dims() };
      
    auto post_pass = prg_post.pass()
      .with(post_pass_buff)
      .set_uniform("texture", fbo->texture())
      .set_uniform("time", [](gl::uniform & u) { u.set((float)gl::get_time()); });
      
    
    while (!context.win().closing())
    {
      GL_VERIFY(glClearColor(1.f, 0.f, 1.f, 1.f));
      GL_VERIFY(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
      
      auto dims = context.win().frame_buffer_dims();
      GL_VERIFY(glViewport(0, 0, dims.x, dims.y));

      bg_pass.draw_to(*fbo, gl::DrawMode::Triangles);
      post_pass.draw(gl::DrawMode::Triangles);

      // other conveniences exist, for example:
      // fbo->texture().save("test.png");

      context.win().swap();
    }

Building
--------
To compile you should be able to write: 

    git clone https://github.com/seshbot/glpp
    cd glpp
    mkdir build && cd build
    cmake ..

    ./glapp
