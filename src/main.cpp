#include "GL/glew.h"
#include "SDL.h"
#include "SDL_image.h"

char const *VERTEX_SHADER_SOURCE = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord;

    out vec3 vertexColor;
    out vec2 texCoord;

    void main() {
        gl_Position = vec4(aPos, 1.0);
        vertexColor = aColor;
        texCoord = aTexCoord;
    }
)";

char const *FRAGMENT_SHADER_SOURCE = R"(
    #version 330 core
    in vec3 vertexColor;
    in vec2 texCoord;

    out vec4 fragColor;

    uniform sampler2D uTex;

    void main() {
        fragColor = texture(uTex, texCoord);
    }
)";

SDL_Surface *load_rgb24_image_flipped(char const* path) {
    SDL_Surface *texture_surface = IMG_Load(path);
    if (texture_surface == nullptr) {
        SDL_Log("Failed to load texture: %s", SDL_GetError());
        return nullptr;
    }

    int height = texture_surface->h;
    int width = texture_surface->w;

    SDL_Surface *ret = SDL_CreateRGBSurfaceWithFormat(0, width, height, SDL_BITSPERPIXEL(SDL_PIXELFORMAT_RGB24), SDL_PIXELFORMAT_RGB24);
    if (ret == nullptr) {
        SDL_Log("Failed to create RGB surface: %s", SDL_GetError());
        SDL_FreeSurface(texture_surface);
        return nullptr;
    }

    SDL_Rect src_rect = {0, 0, width, 1};
    SDL_Rect dest_rect = {0, height - 1, width, 1};

    for (int i = 0; i < height; ++i) {
        SDL_BlitSurface(texture_surface, &src_rect, ret, &dest_rect);
        ++src_rect.y;
        --dest_rect.y;
    }

    SDL_FreeSurface(texture_surface);

    if (SDL_SetSurfaceRLE(ret, 0) != 0) {
        SDL_Log("Error setting surface RLE: %s", SDL_GetError());
        SDL_FreeSurface(ret);
        return nullptr;
    }

    return ret;
}


int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_Log("Error initializing SDL: %s", SDL_GetError());
        return 1;
    }

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0) {
        SDL_Log("Error setting OpenGL major version: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0) {
        SDL_Log("Error setting OpenGL minor version: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    auto img_flags = IMG_INIT_JPG;
    if (IMG_Init(img_flags) != img_flags) {
        SDL_Log("Error initializing SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 400, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        SDL_Log("Error creating SDL window: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }


    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        SDL_Log("Error creating OpenGL context: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    GLenum glew_init_result = glewInit();
    if (glew_init_result != GLEW_OK) {
        SDL_Log("Error initializing GLEW: %s", glewGetErrorString(glew_init_result));
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragment_shader);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    SDL_Surface *rgb_img = load_rgb24_image_flipped("assets/wall.jpg");
    GLuint tex;
    glGenTextures(1, &tex);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgb_img->w, rgb_img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_img->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(rgb_img);


    GLfloat vertices[] = {
            // positions          // colors           // texture coords
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };

    GLuint indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);

    GLuint buffers[2];
    glGenBuffers(2, buffers);

    GLuint vbo = buffers[0];
    GLuint ebo = buffers[1];

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (void*)(0*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "uTex"), 0);

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    running = false;
                    break;
                }
            }
        }

        glBindVertexArray(vao);
        glUseProgram(shader_program);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        SDL_GL_SwapWindow(window);

        SDL_Delay(1000/60);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
