#include <assert.h>
#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "util.h"

FT_Library freetype;
FT_Face face;
GLFWwindow *window;

const char *default_font_path = (
    "/usr/share/fonts/truetype/dejavu/DejaVuSansCondensed.ttf"
);

void init_freetype(const char *font_path)
{
    FT_Error error;

    error = FT_Init_FreeType(&freetype);
    abort_on_error("FreeType", error);

    error = FT_New_Face(freetype, font_path, 0, &face);

    abort_on_error("FreeType", error);

    error = FT_Set_Char_Size(face, 0, 16*64, 300, 300);
    abort_on_error("FreeType", error);
}

void freetype_render_glyph(unsigned charcode)
{
    FT_Error error;
    unsigned glyph_index;

    glyph_index = FT_Get_Char_Index(face, charcode);
    assert(glyph_index != 0);

    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_BITMAP);
    abort_on_error("FreeType", error);

    #ifdef FT_CONFIG_OPTION_SUBPIXEL_RENDERING
    printf("Subpixel rendering is enabled.\n");
    #endif

    if(face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
    {
        printf("Glyph is not bitmap. Rendering...\n");
        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        abort_on_error("FreeType", error);
    }
}

void init_gl(int width, int height, const char *caption)
{
    int fb_width;
    int fb_height;

    assert(glfwInit());

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, caption, NULL, NULL);
    assert(window);

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glEnable(GL_TEXTURE_2D);
}

void update_viewport_and_projection()
{
    int fb_width;
    int fb_height;
    float ratio;

    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    ratio = fb_width / (float)(fb_height);

    glViewport(0, 0, fb_width, fb_height);
    assert(glGetError() == GL_NO_ERROR);

    glClear(GL_COLOR_BUFFER_BIT);
    assert(glGetError() == GL_NO_ERROR);

    glMatrixMode(GL_PROJECTION);
    abort_on_error("OpenGL", glGetError());

    glLoadIdentity();
    abort_on_error("OpenGL", glGetError());

    glOrtho(-ratio, ratio, -1, 1, 1, -1);
    abort_on_error("OpenGL", glGetError());
}

void update_window()
{
    glfwSwapBuffers(window);
    abort_on_error("OpenGL", glGetError());

    glfwPollEvents();
    abort_on_error("OpenGL", glGetError());
}

void draw_triangle()
{
    glMatrixMode(GL_MODELVIEW);
    abort_on_error("OpenGL", glGetError());

    glLoadIdentity();
    abort_on_error("OpenGL", glGetError());

    glRotatef((float)(glfwGetTime() * 50), 0, 0, 1);
    abort_on_error("OpenGL", glGetError());

    glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
    glEnd();
    abort_on_error("OpenGL", glGetError());
}

void draw_quad()
{
    glMatrixMode(GL_MODELVIEW);
    abort_on_error("OpenGL", glGetError());

    glLoadIdentity();
    abort_on_error("OpenGL", glGetError());

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-0.5, -0.5);
        glTexCoord2f(1, 0); glVertex2f(0.5, -0.5);
        glTexCoord2f(1, 1); glVertex2f(0.5, 0.5);
        glTexCoord2f(0, 1); glVertex2f(-0.5, 0.5);
    glEnd();
}

void gl_render_glyph()
{
    FT_Bitmap *bitmap = &face->glyph->bitmap;

    unsigned width = bitmap->width;
    unsigned height = bitmap->rows;
    const void *pixels = bitmap->buffer;

    unsigned texture_id;

    glGenTextures(1, &texture_id);
    abort_on_error("OpenGL", glGetError());

    glBindTexture(GL_TEXTURE_2D, texture_id);
    abort_on_error("OpenGL", glGetError());

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels
    );
    abort_on_error("OpenGL", glGetError());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    abort_on_error("OpenGL", glGetError());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    abort_on_error("OpenGL", glGetError());

    draw_quad();

    glDeleteTextures(1, &texture_id);
    abort_on_error("OpenGL", glGetError());
}

int main(int argc, char *argv[])
{
    init_freetype(
        argc >= 2
            ? argv[1]
            : default_font_path
    );

    freetype_render_glyph(0x41);

    init_gl(800, 600, "Text Test");

    while(!glfwWindowShouldClose(window))
    {
        update_viewport_and_projection();

        gl_render_glyph();

        update_window();
    }

    return 0;
}
