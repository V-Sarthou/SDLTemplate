#include <glad/glad.h>
#include <SDL.h>

#include <stdlib.h>
#include <stdio.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

int g_should_quit = 0;

typedef struct
{
  GLenum shader_type;
  const GLchar *shader_source;
} shader_info;

const char *get_shader_type_name(GLenum shader_type)
{
  static const char vertex_shader_type_name[] = "vertex";
  static const char fragment_shader_type_name[] = "fragment";
  static const char unknown_shader_type_name[] = "";

  switch (shader_type)
  {
  case GL_VERTEX_SHADER:
    return vertex_shader_type_name;
    break;
  case GL_FRAGMENT_SHADER:
    return fragment_shader_type_name;
    break;
  default:
    return unknown_shader_type_name;
    break;
  }
  return unknown_shader_type_name;
}

GLuint buildShader(int num_shaders, const shader_info *shaders)
{
  const GLuint program_id = glCreateProgram();

  GLuint *const shader_ids = malloc(num_shaders * sizeof(GLuint));

  for (int i = 0; i < num_shaders; i++)
  {
    GLuint shader_id = glCreateShader(shaders[i].shader_type);
    glShaderSource(shader_id, 1, &shaders[i].shader_source, NULL);
    glCompileShader(shader_id);

    GLint compile_success = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);
    if (compile_success == GL_FALSE)
    {
      GLint log_length = 0;
      glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

      GLchar *const infoLog = malloc(log_length * sizeof(GLchar));
      glGetShaderInfoLog(shader_id, log_length, NULL, infoLog);
      SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "error: %s shader compilation failed:\n%s\n",
                      get_shader_type_name(shaders[i].shader_type), infoLog);
      free(infoLog);
    }
    else
    {
      glAttachShader(program_id, shader_id);
    }
    shader_ids[i] = shader_id;
  }

  glLinkProgram(program_id);

  GLint link_success = GL_FALSE;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_success);
  if (link_success == GL_FALSE)
  {
    GLint log_length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

    GLchar *const infoLog = malloc(log_length * sizeof(GLchar));
    glGetProgramInfoLog(program_id, log_length, NULL, infoLog);
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "error: shader program link failed:\n%s\n", infoLog);
    free(infoLog);
  };

  for (int i = 0; i < num_shaders; i++)
    glDeleteShader(shader_ids[i]);

  free(shader_ids);

  return program_id;
}

void process_event(const SDL_Event *event)
{
  switch (event->type)
  {
    case SDL_QUIT:
      g_should_quit = 1;
      break;
    case SDL_WINDOWEVENT:
    {
      switch (event->window.event)
      {
      case SDL_WINDOWEVENT_RESIZED:
        glViewport(0, 0, event->window.data1, event->window.data2);
        break;
      }
    }
      break;
  }
}

int main(int argc, char **argv)
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
  {
    fprintf(stderr, "Unable to initialize SDL:  %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);

  SDL_Window *window = SDL_CreateWindow("OpenGL Test", 0, 0, display_mode.w, display_mode.h,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (window == NULL)
  {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL2 window\n");
    return EXIT_FAILURE;
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  const SDL_GLContext context = SDL_GL_CreateContext(window);

  if (!gladLoadGL())
  {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize OpenGL context\n");
    return EXIT_FAILURE;
  }

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "OpenGL: %s\nVendor: %s\nRenderer: %s\nGLSL: %s\n",
    glGetString(GL_VERSION),
    glGetString(GL_VENDOR),
    glGetString(GL_RENDERER),
    glGetString(GL_SHADING_LANGUAGE_VERSION));

  static const shader_info shaders[] =
  {
    {
      GL_VERTEX_SHADER,
      "#version 460\n"
      "out vec2 texCoords;\n"
      "void main()\n"
      "{\n"
      "  texCoords = 2.0 * vec2(gl_VertexID & 1, gl_VertexID >> 1);\n"
      "  gl_Position = vec4(texCoords * 2.0 - 1.0, 0.0, 1.0);\n"
      "}\n"
    },
    {
      GL_FRAGMENT_SHADER,
      "#version 460\n"
      "in vec2 texCoords;\n"
      "out vec4 outputColor;\n"
      "void main()\n"
      "{\n"
      "  outputColor = vec4(texCoords, 0.0, 1.0);\n"
      "}\n"
    },
  };
  static const int num_shaders = ARRAY_SIZE(shaders);
  const GLuint shader_program = buildShader(num_shaders, shaders);

  glViewport(0, 0, display_mode.w, display_mode.h);

  glUseProgram(shader_program);

  // Needed for standard GL in core profile
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  SDL_Event sdl_event;
  while (!g_should_quit)
  {
    SDL_WaitEvent(&sdl_event);
    process_event(&sdl_event);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
