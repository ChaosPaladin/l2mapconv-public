#include "pch.h"

#include <rendering/Shader.h>

namespace rendering {

Shader::Shader(Context &context, const std::string &vertex,
               const std::string &fragment, const std::string &geometry)
    : m_context{context} {

  auto shaders = std::vector{compile(vertex, GL_VERTEX_SHADER),
                             compile(fragment, GL_FRAGMENT_SHADER)};

  if (!geometry.empty()) {
    shaders.push_back(compile(geometry, GL_GEOMETRY_SHADER));
  }

  GL_CALL(m_program = glCreateProgram());

  for (const auto shader : shaders) {
    GL_CALL(glAttachShader(m_program, shader));
  }

  GL_CALL(glLinkProgram(m_program));

  check_for_errors(m_program);
  GL_CALL(glValidateProgram(m_program));

  for (const auto shader : shaders) {
    GL_CALL(glDeleteShader(shader));
  }
}

Shader::~Shader() {
  unbind();
  GL_CALL(glDeleteProgram(m_program));
}

Shader::Shader(Shader &&other) noexcept
    : m_context{other.m_context}, m_program{other.m_program},
      m_uniform_locations{std::move(other.m_uniform_locations)} {

  other.m_program = 0;
}

auto Shader::operator=(Shader &&other) noexcept -> Shader & {
  m_context = other.m_context;
  m_program = other.m_program;
  m_uniform_locations = std::move(other.m_uniform_locations);

  other.m_program = 0;

  return *this;
}

void Shader::bind() const {
  if (m_context.shader.program != m_program) {
    GL_CALL(glUseProgram(m_program));
    m_context.shader.program = m_program;
  }
}

void Shader::unbind() const {
  if (m_context.shader.program == m_program) {
    GL_CALL(glUseProgram(0));
    m_context.shader.program = 0;
  }
}

void Shader::load(const std::string &name, float value, bool required) const {
  bind();
  const auto location = uniform_location(name, required);
  GL_CALL(glUniform1f(location, value));
}

void Shader::load(const std::string &name, int value, bool required) const {
  bind();
  const auto location = uniform_location(name, required);
  GL_CALL(glUniform1i(location, value));
}

void Shader::load(const std::string &name, const glm::vec2 &value,
                  bool required) const {

  bind();
  const auto location = uniform_location(name, required);
  GL_CALL(glUniform2f(location, value.x, value.y));
}

void Shader::load(const std::string &name, const glm::vec3 &value,
                  bool required) const {

  bind();
  const auto location = uniform_location(name, required);
  GL_CALL(glUniform3f(location, value.x, value.y, value.z));
}

void Shader::load(const std::string &name, const glm::vec4 &value,
                  bool required) const {

  bind();
  const auto location = uniform_location(name, required);
  GL_CALL(glUniform4f(location, value.x, value.y, value.z, value.w));
}

void Shader::load(const std::string &name, const glm::mat4 &value,
                  bool required) const {

  bind();
  const auto location = uniform_location(name, required);
  GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)));
}

auto Shader::compile(const std::string &source, GLenum type) const
    -> unsigned int {

  GL_CALL(const auto shader = glCreateShader(type));
  const auto *raw_source = source.c_str();

  GL_CALL(glShaderSource(shader, 1, &raw_source, nullptr));
  GL_CALL(glCompileShader(shader));

  check_for_errors(shader);

  return shader;
}

auto Shader::uniform_location(const std::string &name, bool required) const
    -> int {

  const auto pair = m_uniform_locations.find(name);

  if (pair != m_uniform_locations.end()) {
    return pair->second;
  }

  GL_CALL(const auto location = glGetUniformLocation(m_program, name.c_str()));

  ASSERT(!required || location != -1, "Rendering",
         "Uniform not found: " << name);

  m_uniform_locations[name] = location;
  return location;
}

void Shader::check_for_errors(unsigned int handle) const {
  GL_CALL(const auto is_shader = glIsShader(handle));

  int status = GL_TRUE;

  if (is_shader == GL_TRUE) {
    GL_CALL(glGetShaderiv(handle, GL_COMPILE_STATUS, &status));
  } else {
    GL_CALL(glGetProgramiv(handle, GL_LINK_STATUS, &status));
  }

  if (status == GL_FALSE) {
    int log_length = 0;

    if (is_shader == GL_TRUE) {
      GL_CALL(glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length));
    } else {
      GL_CALL(glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length));
    }

    ASSERT(log_length != 0, "Rendering", "Log can't be empty");

    std::string info_log;
    info_log.resize(static_cast<std::string::size_type>(log_length - 1));

    if (is_shader == GL_TRUE) {
      GL_CALL(glGetShaderInfoLog(handle, log_length, nullptr, info_log.data()));
      GL_CALL(glDeleteShader(handle));

      ASSERT(false, "Rendering",
             "Shader compilation error: " << std::endl
                                          << info_log);
    } else {
      GL_CALL(
          glGetProgramInfoLog(handle, log_length, nullptr, info_log.data()));
      GL_CALL(glDeleteProgram(handle));

      ASSERT(false, "Rendering",
             "Shader linking error: " << std::endl
                                      << info_log);
    }
  }
}

} // namespace rendering
