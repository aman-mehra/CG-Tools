//////////////////////////////////////////////////////////////////////////////
////Run command - export MESA_GL_VERSION_OVERRIDE=3.3 before runnning code////
//////////////////////////////////////////////////////////////////////////////
#include "utils.h"
#include <math.h>
#include <cstdlib>
#include <ctime>

#define DRAW_CUBIC_BEZIER 1 // Use to switch Linear and Cubic bezier curves
#define SAMPLES_PER_BEZIER 40 //Sample each Bezier curve as N=10 segments and draw as connected lines

// GLobal variables
std::vector<float> controlPoints;
std::vector<float> linearBezier;
std::vector<float> cubicBezier;
int width = 640, height = 640; 
bool controlPointsUpdated = false;

// To compute random numbers within a range
float get_random_magnitude(float offset, float multiplier){
    float rand_mag =  static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // between 0 and 1
    rand_mag = offset + rand_mag*multiplier;
    return rand_mag;
}

void calculatePiecewiseLinearBezier()
{
    // Since linearBezier is just a polyline, we can just copy the control points and plot.
    // However to show how a piecewise parametric curve needs to be plotted, we sample t and 
    // evaluate all linear bezier curves.
    // linearBezier.assign(controlPoints.begin(), controlPoints.end());

    linearBezier.clear();
    int sz  = controlPoints.size(); // Contains 3 points/vertex. Ignore Z
    float x[2], y[2];
    float delta_t = 1.0/(SAMPLES_PER_BEZIER - 1.0);
    float t;
    for(int i=0; i<(sz-3); i+=3) {
        x[0] = controlPoints[i];
        y[0] = controlPoints[i+1];
        x[1] = controlPoints[i+3];
        y[1] = controlPoints[i+4];
        linearBezier.push_back(x[0]);
        linearBezier.push_back(y[0]);
        linearBezier.push_back(0.0);
        std::cout << x[0] << " " << x[1] << "\n";
        t = 0.0;
        for (float j=1; j<(SAMPLES_PER_BEZIER-1); j++)
        {
        t += delta_t;
        linearBezier.push_back(x[0] + t*(x[1] - x[0]));
        linearBezier.push_back(y[0] + t*(y[1] - y[0]));
        linearBezier.push_back(0.0);
        }
        // No need to add the last point for this segment, since it will be added as first point in next.
    }
    // However, add last point of entire piecewise curve here (i.e, the last control point)
    linearBezier.push_back(x[1]);
    linearBezier.push_back(y[1]);
    linearBezier.push_back(0.0);
}

void calculatePiecewiseCubicBezier()
{
    srand(1);
    cubicBezier.clear();
    int sz  = controlPoints.size(); // Contains 3 points/vertex. Ignore Z
    float x[4], y[4]; // stores actual plus phantom control points
    float delta_t = 1.0/(SAMPLES_PER_BEZIER - 1.0);
    float t;
    
    /*
    To ensure C1 continuity at the interpolation control points.
    To so at a point P1 which is common to bezier curves B0 and B1, ensure that the tangent at P1 for both B1 and B2 has the same 
    magnitude and direction. 
    */

    float prev_dir_x,prev_dir_y; 
    float prev_mag_fraction;
    float prev_length;

    float dir_x,dir_y;
    float mag_fraction;
    float length;
    float sign;

    float lc;
    float lc_length;

    for(int i=0; i<(sz-3); i+=3) {

        x[0] = controlPoints[i];
        y[0] = controlPoints[i+1];
        x[3] = controlPoints[i+3];
        y[3] = controlPoints[i+4];

        // generates a pseudo random number between -0.7 and 0.7 (other 2 control points will be further away from the line joining P0 and P3)
        // The vector Pi - P0 is likely to be closer (cosine dist) to the perpendicular to P3-P0 than the line itself
        lc = get_random_magnitude(-0.7,1.4);  
        length = sqrt(pow(x[0]-x[3],2) + pow(y[0]-y[3],2));

        // linear combination of normal and tangent vectors
        dir_x = (1-lc)*(y[3]-y[0])/length + lc*(x[3]-x[0])/length;
        dir_y = (1-lc)*(x[0]-x[3])/length + lc*(y[3]-y[0])/length;

        // Normalizing the computed vector and representing it as a unit vector.
        lc_length = sqrt(pow(dir_x,2) + pow(dir_y,2));
        dir_x = dir_x/lc_length;
        dir_y = dir_y/lc_length;

        // switching direction of computed vector to further enhance diversity
        sign =  2*(get_random_magnitude(-1,2) > 0)-1;

        // Prev vector initializations for first bezier curve
        if (i==0){
            prev_length = length;
            prev_dir_x = dir_x;
            prev_dir_y = dir_y;
            prev_mag_fraction = get_random_magnitude(0.35,0.5); // generates a pseudo random number between 0.35 and 0.85
        }

        mag_fraction = get_random_magnitude(0.35,0.5); // generates a pseudo random number between 0.35 and 0.85

        // Computing phantom points
        x[1] = x[0] + prev_mag_fraction*prev_length*prev_dir_x;
        y[1] = y[0] + prev_mag_fraction*prev_length*prev_dir_y;
        x[2] = x[3] + sign*mag_fraction*length*dir_x;
        y[2] = y[3] + sign*mag_fraction*length*dir_y;
        
        // std::cout <<x[0]<<"::"<<y[0]<<"|"<<x[1]<<"::"<<y[1]<<"|"<<x[2]<<"::"<<y[2]<<"|"<<x[3]<<"::"<<y[3]<<"\n";
        // std::cout << "Line ::"<<x[3]-x[0]<<"|"<<y[3]-y[0]<<"|"<<length<<"\n";
        // std::cout << "Unit Tangent :: "<<(x[3]-x[0])/length<<"|"<<(y[3]-y[0])/length<<"\n";
        // std::cout << "Unit Normal :: "<<dir_x<<"|"<<dir_y<<"\n";
        // std::cout << "Normal :: "<<mag_fraction*dir_x*length<<"|"<<mag_fraction*dir_y*length<<"|"<<mag_fraction<<"\n";

        // std::cout << "Prev Normal :: "<<prev_mag_fraction*prev_length*prev_dir_x<<"|"<<prev_mag_fraction*prev_length*prev_dir_y<<"\n";
        // std::cout << "Cur Normal :: "<<mag_fraction*prev_length*dir_x<<"|"<<mag_fraction*prev_length*dir_y<<"\n";
        

        cubicBezier.push_back(x[0]);
        cubicBezier.push_back(y[0]);
        cubicBezier.push_back(0.0);

        // cubicBezier.push_back(x[1]);
        // cubicBezier.push_back(y[1]);
        // cubicBezier.push_back(0.0);

        // cubicBezier.push_back(x[2]);
        // cubicBezier.push_back(y[2]);
        // cubicBezier.push_back(0.0);

        // Sampling bezier curve
        t = 0.0;
        for (float j=1; j<(SAMPLES_PER_BEZIER-1); j++)
        {
            t += delta_t;
            // std::cout << t << "::" << pow(1-t,3) << "::" << pow(1-t,2)*t << "::" << (1-t)*pow(t,2) << "::" << pow(t,3) << "::" <<  pow(1-t,3) + 3*pow(1-t,2)*t + 3*(1-t)*pow(t,2) + pow(t,3) << "\n";
            cubicBezier.push_back(pow(1-t,3)*x[0] + 3*pow(1-t,2)*t*x[1] + 3*(1-t)*pow(t,2)*x[2] + pow(t,3)*x[3]);
            cubicBezier.push_back(pow(1-t,3)*y[0] + 3*pow(1-t,2)*t*y[1] + 3*(1-t)*pow(t,2)*y[2] + pow(t,3)*y[3]);
            cubicBezier.push_back(0.0);
        }

        // Updating prev variables to ensure C1 continuity and control points.
        prev_length = length;
        prev_dir_x = -dir_x*sign;
        prev_dir_y = -dir_y*sign;
        prev_mag_fraction = mag_fraction;

        // No need to add the last point for this segment, since it will be added as first point in next.
    }
    // However, add last point of entire piecewise curve here (i.e, the last control point)
    cubicBezier.push_back(x[3]);
    cubicBezier.push_back(y[3]);
    cubicBezier.push_back(0.0);
}

int main(int, char* argv[])
{

    GLFWwindow* window = setupWindow(width, height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
	glUseProgram(shaderProgram);

    // Create VBOs, VAOs
    unsigned int VBO_controlPoints, VBO_linearBezier, VBO_cubicBezier;
    unsigned int VAO_controlPoints, VAO_linearBezier, VAO_cubicBezier;
    glGenBuffers(1, &VBO_controlPoints);
    glGenVertexArrays(1, &VAO_controlPoints);
    // glGenBuffers(1, &VBO_linearBezier);
    // glGenVertexArrays(1, &VAO_linearBezier);
    glGenBuffers(1, &VBO_cubicBezier);
    glGenVertexArrays(1, &VAO_cubicBezier);
    //TODO:

    int button_status = 0;

    //Display loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        showOptionsDialog(controlPoints, io);
        ImGui::Render();

        // Add a new point on mouse click
        float x,y ;
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        if(io.MouseClicked[0] && !ImGui::IsAnyItemActive()){
            x = io.MousePos.x;
            y = io.MousePos.y;
            addPoints(controlPoints, x, y, width, height);
            controlPointsUpdated = true;
         }

        if(controlPointsUpdated) {
            // Update VAO/VBO for control points (since we added a new point)
            glBindVertexArray(VAO_controlPoints);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_controlPoints);
            glBufferData(GL_ARRAY_BUFFER, controlPoints.size()*sizeof(GLfloat), &controlPoints[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)

            // Update VAO/VBO for piecewise linear Bezier curve (since we added a new point)
            // calculatePiecewiseLinearBezier();
            // glBindVertexArray(VAO_linearBezier);
            // glBindBuffer(GL_ARRAY_BUFFER, VBO_linearBezier);
            // glBufferData(GL_ARRAY_BUFFER, linearBezier.size()*sizeof(GLfloat), &linearBezier[0], GL_DYNAMIC_DRAW);
            // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            // glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)

            // Update VAO/VBO for piecewise cubic Bezier curve
            calculatePiecewiseCubicBezier();
            glBindVertexArray(VAO_cubicBezier);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_cubicBezier);
            glBufferData(GL_ARRAY_BUFFER, cubicBezier.size()*sizeof(GLfloat), &cubicBezier[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)
        
            controlPointsUpdated = false; // Finish all VAO/VBO updates before setting this to false.
        }

        glUseProgram(shaderProgram);

        // Draw control points
        glBindVertexArray(VAO_controlPoints);
		glDrawArrays(GL_POINTS, 0, controlPoints.size()/3); // Draw points

#if DRAW_CUBIC_BEZIER
        // TODO:
        glBindVertexArray(VAO_cubicBezier);
        glDrawArrays(GL_LINE_STRIP, 0, cubicBezier.size()/3); //Draw lines
#else
        // Draw linear Bezier
        glBindVertexArray(VAO_linearBezier);
        glDrawArrays(GL_LINE_STRIP, 0, linearBezier.size()/3); //Draw lines
#endif

        glUseProgram(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    // Delete VBO buffers
    glDeleteBuffers(1, &VBO_controlPoints);
    // glDeleteBuffers(1, &VBO_linearBezier);
    glDeleteBuffers(1, &VBO_cubicBezier);
    //TODO:

    // Cleanup
    cleanup(window);
    return 0;
}
