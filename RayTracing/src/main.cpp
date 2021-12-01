// Assignment 03: Raytracing

#include "imgui_setup.h"
#include "camera.h"
#include "renderengine.h"
#include "world.h"
#include "material.h"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include "lightsource.h"
#include "pointlightsource.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../depends/stb/stb_image.h"
#include "../depends/stb/stb_image_write.h"

#define RENDER_BATCH_COLUMNS 20 // Number of columns to render in a single go. Increase to gain some display/render speed!

Camera *camera;
RenderEngine *engine;

int screen_width = 800, screen_height = 600; // This is window size, used to display scaled raytraced image.
int image_width = 1920, image_height = 1080; // This is raytraced image size. Change it if needed.
GLuint texImage;

World createScene1(World& world){

}

int main(int, char**)
{
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // Setup raytracer camera. This is used to spawn rays.
    Vector3D camera_position(0, 0, 10);
    Vector3D camera_target(0, 0, 0); //Looking down -Z axis
    Vector3D camera_up(0, 1, 0);
    float camera_fovy =  45;
    camera = new Camera(camera_position, camera_target, camera_up, camera_fovy, image_width, image_height);

    //Create a world
    World *world = new World;
    world->setAmbient(Color(1));
    world->setBackground(Color(0.1, 0.3, 0.7));
    
    // //////////////////////////////////////////////////////////////////////////////////////////
    // Scene 1
    // //////////////////////////////////////////////////////////////////////////////////////////
    Material *m1 = new Material(world);
    m1->color = Color(0.1, 0.7, 0.5);
    m1->ka = 0.1;m1->kd = 0.9;m1->ks = 0.4;m1->kr=0.1;m1->n = 2;

    Material *m2 = new Material(world);
    m2->color = Color(0.8, 0.2, 0.3);
    m2->ka = 0.1;m2->kd = 0.1;m2->ks = 0.9;m2->n = 2;

    Material *m3 = new Material(world);
    m3->color = Color(0.1, 0.7, 0.0);
    m3->ka = 0.1;m3->kd = 0.9;m3->ks = 0.7;m3->kr=0.8;m3->n = 4;

    Material *m4 = new Material(world);
    m4->color = Color(0.9, 0.4, 0);
    m4->ka = 0.1;m4->kd = 0.4;m4->ks = 0.2;m4->n = 8;m4->kt = 0.9;m4->eta=2.6;

    Material *m5 = new Material(world);
    m5->color = Color(0.9, 0.4, 0);
    m5->ka = 0.1;m5->kd = 0.4;m5->ks = 0.2;m5->n = 8;m5->kt = 0.9;m5->eta=2.6;m5->ar=10;m5->ag=10;m5->ab=10;


    Material *ground = new Material(world);
    ground->color = Color(0.1, 0.5, 0.5);
    ground->ka = 0.1;ground->kd = 0.5;ground->kr = 0.2;

    Material *wall_ref = new Material(world);
    wall_ref->color = Color(0.47, 0.32, 0.66);
    wall_ref->ka = 0.1;wall_ref->kd = 0.2;wall_ref->ks = 0.3;wall_ref->kr = 0.2;

    Material *wall_reg = new Material(world);
    wall_reg->color = Color(0.6, 0.6, 0.1);
    wall_reg->ka = 0.3;wall_reg->kd = 0.3;wall_reg->ks = 0.7;wall_reg->n = 4;
    
    Object *sphere1 = new Sphere(Vector3D(2, 0, -9), 3, m1);
    world->addObject(sphere1);

    Object *sphere2 = new Sphere(Vector3D(5, -4, -6), 1, m2);
    world->addObject(sphere2);

    Object *sphere3 = new Sphere(Vector3D(5, 3, -1), 1, m2);
    world->addObject(sphere3);

    Object *sphere4 = new Sphere(Vector3D(5, -4, -4), 0.5, m4);
    world->addObject(sphere4);

    Object *sphere5 = new Sphere(Vector3D(-3, 4, -9), 0.5, m4);
    world->addObject(sphere5);

    // triangle
    Object *traingle1 = new Triangle(Vector3D(-3, 1, -9), Vector3D(-3, 4, -5), Vector3D(-5, 1, -5), m3);
    world->addObject(traingle1);

    // Ground
    Object *ground_traingle1 = new Triangle(Vector3D(-6, 4, -30), Vector3D(13, 4, -30), Vector3D(13, 4, 5), ground);
    world->addObject(ground_traingle1);
    Object *ground_traingle2 = new Triangle(Vector3D(-6, 4, -30), Vector3D(-6, 4, 5), Vector3D(13, 4, 5), ground);
    world->addObject(ground_traingle2);

    // Left wall
    Object *left_traingle1 = new Triangle(Vector3D(-6, 4, -30), Vector3D(-6, -10, -30), Vector3D(-6, 4, 5), wall_ref);
    world->addObject(left_traingle1);
    Object *left_traingle2 = new Triangle(Vector3D(-6, -10, -30), Vector3D(-6, -10, 5), Vector3D(-6, 4, 5), wall_ref);
    world->addObject(left_traingle2);

    // Right wall
    Object *right_traingle1 = new Triangle(Vector3D(13, 4, -30), Vector3D(13, -10, -30), Vector3D(13, 4, 5), wall_reg);
    world->addObject(right_traingle1);
    Object *right_traingle2 = new Triangle(Vector3D(13, -10, -30), Vector3D(13, -10, 5), Vector3D(13, 4, 5), wall_reg);
    world->addObject(right_traingle2);

    // Back wall
    Object *back_traingle1 = new Triangle(Vector3D(13, 4, -30), Vector3D(13, -10, -30), Vector3D(-6, 4, -30), wall_reg);
    world->addObject(back_traingle1);
    Object *back_traingle2 = new Triangle(Vector3D(13, -10, -30), Vector3D(-6, 4, -30),  Vector3D(-6, -10, -30), wall_reg);
    world->addObject(back_traingle2);

    LightSource *light1 = new PointLightSource(world, Vector3D(7, -8, 4), Color(1, 1, 1));
    world->addLight(light1);

    LightSource *light2 = new PointLightSource(world, Vector3D(12, -10, 4), Color(1, 1, 1));
    world->addLight(light2);

    LightSource *light3 = new PointLightSource(world, Vector3D(-4, -1, 4), Color(1, 1, 1));
    world->addLight(light3);
    // //////////////////////////////////////////////////////////////////////////////////////////


    // //////////////////////////////////////////////////////////////////////////////////////////
    // Scene 2
    // Material *m_new = new Material(world);
    // m_new->color = Color(0.9, 0.4, 0.1);
    // m_new->ka = 0.1;m_new->kd = 0.4;m_new->ks = 0.3;m_new->n = 4;m_new->kt = 0.9;m_new->eta=1.7;

    // Material *m_new2 = new Material(world);
    // m_new2->color = Color(0.9, 0.4, 0.1);
    // m_new2->ka = 0.1;m_new2->kd = 0.4;m_new2->ks = 0.3;m_new2->kr=0.6;m_new2->n = 4;

    // LightSource *light1_2 = new PointLightSource(world, Vector3D(20, 0, 0), Color(1, 1, 1));
    // world->addLight(light1_2);

    // LightSource *light2_2 = new PointLightSource(world, Vector3D(-20, 0, 0), Color(1, 1, 1));
    // world->addLight(light2_2);

    // LightSource *light3_2 = new PointLightSource(world, Vector3D(0, -20, 3), Color(1, 1, 1));
    // world->addLight(light3_2);

    // LightSource *light4_2 = new PointLightSource(world, Vector3D(0, 20, 3), Color(1, 1, 1));
    // world->addLight(light4_2);

    // Object *sphere1_2 = new Sphere(Vector3D(0, 0, 0), 3, m_new);
    // world->addObject(sphere1_2);

    // Object *sphere2_2 = new Sphere(Vector3D(-1, -1, -3), 1, m_new2);
    // world->addObject(sphere2_2);
    // //////////////////////////////////////////////////////////////////////////////////////////


    engine = new RenderEngine(world, camera);

    //Initialise texture
    glGenTextures(1, &texImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool render_status;
        for(int i=0; i<RENDER_BATCH_COLUMNS; i++) 
            render_status = engine->renderLoop(); // RenderLoop() raytraces 1 column of pixels at a time.
        if(!render_status)
        {
            // Update texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texImage);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
        } 

        ImGui::Begin("Lumina", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Size: %d x %d", image_width, image_height);
        if(ImGui::Button("Save")){
          char filename[] = "img.png";
          stbi_write_png(filename, image_width, image_height, 3, camera->getBitmap(),0);        
        }
        //Display render view - fit to width
        int win_w, win_h;
        glfwGetWindowSize(window, &win_w, &win_h);
        float image_aspect = (float)image_width/(float)image_height;
        float frac = 0.95; // ensure no horizontal scrolling
        ImGui::Image((void*)(intptr_t)texImage, ImVec2(frac*win_w, frac*win_w/image_aspect), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteTextures(1, &texImage);

    cleanup(window);

    return 0;
}
