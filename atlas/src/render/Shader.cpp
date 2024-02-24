#include "atlas/render/Shader.h"

#include <algorithm>
#include <iostream>
#include <fstream>

Shader::~Shader() {
    glDeleteProgram(glID);
}

//====
// OPENGL STUFF
//====

bool Shader::addSourceAndCompile(const std::string &path, GLenum type) {
    auto source = readFile(path);
    if (source.empty()) {
        ++failed;
        return false;
    }

    std::string errors;
    bool compiled = compileShader(source, type, errors);

    if (!compiled) {
        std::cout
            << "ERROR: (compilation) `" << path << "'" << std::endl
            << errors;

#if DEBUG
        // generate debugging files.
        std::ofstream out{path + ".out"};
        out << source;
        out.close();
#endif
        ++failed;
    }

    return compiled;
}

bool Shader::compileShader(const std::string &source, GLenum type, std::string &errors) {
    GLuint shader = glCreateShader(type);
    const GLchar *sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, 0);

    glCompileShader(shader);
    int isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint size;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
        std::vector<GLchar> infoLog(size);
        glGetShaderInfoLog(shader, size, &size, infoLog.data());

        errors = std::string(infoLog.data());

        glDeleteShader(shader);
        return false;
    }

    errors = "";

    shaders.push_back(shader);
    return true;
}

bool Shader::link() {
    if (failed) {
        std::cout << "--- Linking failed due to " << failed << " compiler error(s)." << std::endl;
        return false;
    }

    GLuint program = glCreateProgram();

    for (auto shader: shaders)
        glAttachShader(program, shader);

    glLinkProgram(program);
    int isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

    if (isLinked == GL_FALSE) {
        GLint size;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);
        std::vector<GLchar> infoLog(size);
        glGetProgramInfoLog(program, size, &size, infoLog.data());

        std::cout << "Failed to link program! Error:\n" << infoLog.data() << std::endl;

        glDeleteProgram(program);
        return false;
    }

    for (auto shader: shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    glID = program;

    return true;
}

void Shader::use() {
    glUseProgram(glID);
}

void Shader::setColor(const std::string& name, Color color) {
    int location = glGetUniformLocation(glID, name.c_str());
    glUniform4fv(location, 1, (float*)&color);
}

void Shader::setInt(const std::string &name, int value) {
    int location = glGetUniformLocation(glID, name.c_str());
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string &name, float value) {
    int location = glGetUniformLocation(glID, name.c_str());
    glUniform1f(location, value);
}

void Shader::setSize(const std::string &name, Size s) {
    int location = glGetUniformLocation(glID, name.c_str());
    glUniform2uiv(location, 1, (unsigned *)&s);
}

void Shader::set(const std::string &name, unsigned u) {
    int location = glGetUniformLocation(glID, name.c_str());
    glUniform1ui(location, u);
}

//====
// PREPROCESSOR STUFF
//====

bool Shader::startsWith(const std::string& str, const std::string& prefix, size_t& idx) {
    int i{};
    while (i < str.length() && std::isspace(str[i])) ++i;

    if (i + prefix.length() > str.length()) {
        idx = i;
        return false;
    }

    idx = i + prefix.length();
    return str.substr(i, prefix.length()) == prefix;
}

bool Shader::parseInclude(const std::string &dir, const std::string &str, std::string &out, bool &error, size_t line) {
    size_t start;
    if (!startsWith(str, dir, start)) {
        out = "";
        error = false;
        return false;
    }

    auto from = str.substr(start);

    size_t where;
    if (!startsWith(from, "\"", where)) {
        out = std::to_string(line) + "," + std::to_string(start + where) + ": primary argument of `" + dir + "' must be a string";
        error = true;
        return true;
    }

    std::stringstream result{};
    size_t i = where;
    bool escape{};
    while (true) {
        if (i >= from.length()) {
            out = std::to_string(line) + "," + std::to_string(start + i) + ": premature end of line in string literal";
            error = true;
            return true;
        }

        char c = from[i];

        if (escape) {
            switch (c) {
                case 'n': result << '\n';
                    break;
                case 'a': result << '\a';
                    break;
                case '\\': result << '\\';
                    break;
                case '\"': result << '\"';
                    break;
                case '\t': result << '\t';
                    break;
                default:
                    out =  std::to_string(line) + "," + std::to_string(start + i) + ": use of unknown esapce sequence `" + c + "' in include";
                    error = true;
                    return true;
            }

            escape = false;
            continue;
        }

        if (c == '\\') {
            escape = true;
            ++i;
            continue;
        }

        if (c == '\"') {
            ++i;
            break;
        }

        result << c;
        ++i;
    }

    error = false;
    out = result.str();
    return true;
}

std::string Shader::readFile(const std::string &path, std::vector<std::string> &includes) {
    includes.emplace_back(path);

    std::stringstream stream{};
    std::ifstream input{path};

    std::string line;
    int line_no{};
    while (std::getline(input, line)) {
        ++line_no;

        std::string out;
        bool error;
        bool isInclude = parseInclude("#include", line, out, error, line_no);

        if (isInclude) {
            if (error) {
                std::cout << "WARN: (preprocess) `" << path << "' " << out << std::endl;
            } else {
                if (std::find(includes.begin(), includes.end(), out) != includes.end()) {
                    continue;
                }

                includes.emplace_back(out);
                stream
                    << "// begin include " << out << std::endl
                    << readFile(out, includes) << std::endl
                    << "// end include " << out << std::endl;
            }

            continue;
        }

        stream << line << std::endl;
    }

    input.close();
    return stream.str();
}

std::string Shader::readFile(const std::string &path) {
    std::vector<std::string> includes{};
    return readFile(path, includes);
}
