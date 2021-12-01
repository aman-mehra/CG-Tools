//Assignment 02: Trandformations, viewing and projection

#include "utils.h"
#include <math.h>

int width = 640, height=640;

void createCubeObject(unsigned int &, unsigned int &);
void setupModelTransformation(unsigned int &);
void setupViewTransformation(unsigned int &);
void setupProjectionTransformation(unsigned int &, int, int);

int main(int, char**)
{
    // Setup window
    GLFWwindow *window = setupWindow(width, height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
    glUseProgram(shaderProgram);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    setupModelTransformation(shaderProgram);
    setupViewTransformation(shaderProgram);
    setupProjectionTransformation(shaderProgram, width , height);

    createCubeObject(shaderProgram, VAO);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glUseProgram(shaderProgram);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Information");                          
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO); 
        
        glDrawArrays(GL_TRIANGLES, 0, 6*2*3);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    // Cleanup
    cleanup(window);

    return 0;
}

void createCubeObject(unsigned int &program, unsigned int &cube_VAO)
{
    glUseProgram(program);

    //Bind shader variables
    int vVertex_attrib = glGetAttribLocation(program, "vVertex");
    if(vVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vVertex\n");
        exit(0);
    }
    int vColor_attrib = glGetAttribLocation(program, "vColor");
    if(vColor_attrib == -1) {
        fprintf(stderr, "Could not bind location: vColor\n");
        exit(0);
    }

    //Cube data
    GLfloat cube_vertices[] = {10, 10, 10, -10, 10, 10, -10, -10, 10, 10, -10, 10, //Front
                   10, 10, -10, -10, 10, -10, -10, -10, -10, 10, -10, -10}; //Back
    GLushort cube_indices[] = {0, 2, 3, 0, 1, 2, //Front
                4, 7, 6, 4, 6, 5, //Back
                5, 2, 1, 5, 6, 2, //Left
                4, 3, 7, 4, 0, 3, //Right
                1, 0, 4, 1, 4, 5, //Top
                2, 7, 3, 2, 6, 7}; //Bottom
    GLfloat cube_colors[] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1}; //Unique face colors

    //Generate VAO object
    glGenVertexArrays(1, &cube_VAO);
    glBindVertexArray(cube_VAO);

    //Create VBOs for the VAO
    //Position information (data + format)
    int nVertices = 6*2*3; //(6 faces) * (2 triangles each) * (3 vertices each)
    GLfloat *expanded_vertices = new GLfloat[nVertices*3];
    for(int i=0; i<nVertices; i++) {
        expanded_vertices[i*3] = cube_vertices[cube_indices[i]*3];
        expanded_vertices[i*3 + 1] = cube_vertices[cube_indices[i]*3+1];
        expanded_vertices[i*3 + 2] = cube_vertices[cube_indices[i]*3+2];
    }
    GLuint vertex_VBO;
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vVertex_attrib);
    glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []expanded_vertices;

    //Color - one for each face
    GLfloat *expanded_colors = new GLfloat[nVertices*3];
    for(int i=0; i<nVertices; i++) {
        int color_index = i / 6;
        expanded_colors[i*3] = cube_colors[color_index*3];
        expanded_colors[i*3+1] = cube_colors[color_index*3+1];
        expanded_colors[i*3+2] = cube_colors[color_index*3+2];
    }
    GLuint color_VBO;
    glGenBuffers(1, &color_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
    glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vColor_attrib);
    glVertexAttribPointer(vColor_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []expanded_colors;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
}


void setupModelTransformation(unsigned int &program)
{

    glm::vec3 axis(1,2,2);


    // Rotation using change of basis matrix
    glm::vec3 up(0.0,0.0,1.0); 
    glm::vec3 w = glm::normalize(axis);
    glm::vec3 u = glm::normalize(glm::cross(up,w));
    glm::vec3 v = glm::normalize(glm::cross(w,u));


    GLfloat change_of_basis[] = {
        u.x, v.x, w.x, 0.0f, // first column
        u.y, v.y, w.y, 0.0f, // second column
        u.z, v.z, w.z, 0.0f,// third column
        0.0f, 0.0f, 0.0f, 1.0f
        };

    glm::mat4 base_change_mat = glm::make_mat4(change_of_basis);

    float pi = 3.14159265;
    float degrees = 30.0f; // Anticlockwise - +ve
    float radians = degrees*(pi/180.0); 

    GLfloat rot_z[] = {
        cos(radians), sin(radians), 0.0f, 0.0f, // first column
        -sin(radians),  cos(radians), 0.0f, 0.0f, // second column
        0.0f, 0.0f, 1.0f, 0.0f,// third column
        0.0f, 0.0f,0.0f, 1.0f
        };

    glm::mat4 mat_z = glm::make_mat4(rot_z);

    
    // Rotation by chaining rotation matrices about x, y and z axes

    float sin_x = axis.y/sqrt(pow(axis.y,2) + pow(axis.z,2)); // AntiClockwise
    float cos_x = axis.z/sqrt(pow(axis.y,2) + pow(axis.z,2));

    float sin_y = -axis.x/sqrt(pow(axis.x,2) + pow(axis.y,2) + pow(axis.z,2)); // Clockwise
    float cos_y = sqrt(pow(axis.y,2) + pow(axis.z,2))/sqrt(pow(axis.x,2) + pow(axis.y,2) + pow(axis.z,2));

    GLfloat rot_x[] = {
        1.0f, 0.0f, 0.0f, 0.0f, // first column
        0.0f, cos_x, sin_x, 0.0f, // second column
        0.0f, -sin_x, cos_x, 0.0f,// third column
        0.0f, 0.0f,0.0f, 1.0f
        };

    GLfloat rot_x_inv[] = {
        1.0f, 0.0f, 0.0f, 0.0f, // first column
        0.0f, cos_x, -sin_x, 0.0f, // second column
        0.0f, sin_x, cos_x, 0.0f,// third column
        0.0f, 0.0f,0.0f, 1.0f
        };

    GLfloat rot_y[] = {
        cos_y, 0.0f, -sin_y, 0.0f, // first column
        0.0f, 1.0f, 0.f, 0.0f, // second column
        sin_y, 0.0f, cos_y, 0.0f,// third column
        0.0f, 0.0f,0.0f, 1.0f
        };

    GLfloat rot_y_inv[] = {
        cos_y, 0.0f, sin_y, 0.0f, // first column
        0.0f, 1.0f, 0.f, 0.0f, // second column
        -sin_y, 0.0f, cos_y, 0.0f,// third column
        0.0f, 0.0f,0.0f, 1.0f
        };

    glm::mat4 mat_x = glm::make_mat4(rot_x);
    glm::mat4 mat_y = glm::make_mat4(rot_y);
    glm::mat4 mat_y_inv = glm::make_mat4(rot_y_inv);
    glm::mat4 mat_x_inv = glm::make_mat4(rot_x_inv);


    //Modelling transformations (Model -> World coordinates)
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates
    //TODO: Q1 - Apply modelling transformations here.

    // Rotation Method 1 - change of basis matrix
    model = glm::inverse(base_change_mat)*mat_z*base_change_mat;//glm::transpose(base_change_mat);

    // Rotation method 2 - chaining rotation matrices
    // model = mat_x_inv*mat_y_inv*mat_z*mat_y*mat_x;

    //TODO: Reset modelling transformations to Identity. Uncomment line below before attempting Q4!
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates

    //Pass on the modelling matrix to the vertex shader
    glUseProgram(program);
    int vModel_uniform = glGetUniformLocation(program, "vModel");
    if(vModel_uniform == -1){
        fprintf(stderr, "Could not bind location: vModel\n");
        exit(0);
    }
    glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(model));
}


glm::mat4 get_view_transform(glm::vec3 eye, glm::vec3 center, glm::vec3 up){
    glm::vec3 eye_offset = center-eye;
    glm::vec3 w = glm::normalize(eye-center);
    glm::vec3 u = glm::normalize(glm::cross(up,w));
    glm::vec3 v = glm::normalize(glm::cross(w,u));

    GLfloat view_transform[] = {
        u.x, v.x, w.x, 0.0f, // first column
        u.y, v.y, w.y, 0.0f, // second column
        u.z, v.z, w.z, 0.0f,// third column
        0.0f,0.0f,0.0f,1.0f
        };  

    GLfloat view_offset[] = {
        1.0f, 0.0f, 0.0f, 0.0f, // first column
        0.0f, 1.0f, 0.0f, 0.0f, // second column
        0.0f, 0.0f, 1.0f, 0.0f,// third column
        eye_offset.x, eye_offset.y, eye_offset.z, 1.0f
        };

    return glm::make_mat4(view_transform)*glm::make_mat4(view_offset);
}

void setupViewTransformation(unsigned int &program)
{
    //Viewing transformations (World -> Camera coordinates
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 100.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); // eye, center , up
    //Camera at (0, 0, 100) looking down the negative Z-axis in a right handed coordinate system

    //TODO: Q3 - Apply viewing transformations here.

    glm::vec3 eye(50.0,100.0,20.0); 
    glm::vec3 center(0.0,0.0,0.0); 
    glm::vec3 up(0.0,0.0,1.0); 

    view = get_view_transform(eye,center,up);
    // view = glm::lookAt(glm::vec3(50.0, 100.0, 20.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));

    //TODO: Q4 - Apply appropriate viewing transformations here to generate 1-point, 2-point, and 3-point perspective views.
    up = glm::vec3(0.0,1.0,0.0);
    // One Point Perspective 
    eye = glm::vec3(0.0, 0.0, 100.0);
    view = get_view_transform(eye,center,up);
    // Two Point Perspective 
    eye = glm::vec3(100.0, 0.0, 100.0);
    view = get_view_transform(eye,center,up);
    // Three Point Perspective (Birds Eye View)
    eye = glm::vec3(100.0, 100.0, 100.0);
    view = get_view_transform(eye,center,up);
    // Three Point Perspective (Rats Eye View)
    eye = glm::vec3(100.0, -100.0, 100.0);
    view = get_view_transform(eye,center,up);



    //Pass-on the viewing matrix to the vertex shader
    glUseProgram(program);
    int vView_uniform = glGetUniformLocation(program, "vView");
    if(vView_uniform == -1){
        fprintf(stderr, "Could not bind location: vView\n");
        exit(0);
    }
    glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(view));
}

void setupProjectionTransformation(unsigned int &program, int screen_width, int screen_height)
{
    //Projection transformation
    float aspect = (float)screen_width/(float)screen_height;

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)screen_width/(GLfloat)screen_height, 0.1f, 1000.0f);

    //Pass on the projection matrix to the vertex shader
    glUseProgram(program);
    int vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if(vProjection_uniform == -1){
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projection));
}
