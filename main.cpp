#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>


using namespace std;

struct Engine {
    GLFWwindow* window;
    int WIDTH = 800, HEIGHT = 600;

    Engine() {
        // wake up GLFW
        if (!glfwInit()) {
            cerr << "glfw didn't start" << endl;
            exit(EXIT_FAILURE);
        }

        // ask the operating system for a window
        window = glfwCreateWindow(WIDTH, HEIGHT, " solar system ", nullptr, nullptr);
        if (!window) {
            cerr << "no window for you" << endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        // point every future drawing command at this window
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);   // one step per monitor refresh instead of thousands per second
        glewInit();

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);
    }

    // this runs once per frame
    void run() {
        // wipe the screen to almost-black
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // put (0, 0) in the middle instead of the corner
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-WIDTH / 2.0, WIDTH / 2.0, -HEIGHT / 2.0, HEIGHT / 2.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
};


//open gl cant create circles, just poins lines and triangles, no curves
void drawCircle(float cx, float cy, float r){

    const int segments = 100;
    const float pi = 3.14159265f;

    glBegin(GL_TRIANGLE_FAN);   

    glVertex2f(cx,cy);

    for(int i = 0; i <= segments; i++){
        float angle = 2.0f * pi * i / segments;

        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);

        glVertex2f(x,y);
    }

    glEnd();


}

//cos gives you the 'x' for an angle, whatever angle we want, sin gives us the y. Both come out between -1 and 1, so that's a circle of radius 1. Multiply them by any radius to make it that size


struct planet{
    float x, y;         //pos
    float vx, vy;       //speed
    float r;            //radius/size
    float m;            //mass
    float cr,cg,cb;     //colors, go from 0->1 not 0 -> 255
    
};

struct gravity{
    
};


std::vector<planet> planets = {
   //     x     y     vx     vy   r          m      cr    cg    cb
    {    0,    0,  0.00,  0.00, 30,      4000, 1.00, 0.78, 0.25 }, // sun
    {   52,   19, -2.25,  6.16,  3,  0.000066, 0.62, 0.60, 0.58 }, // mercury
    {  -27,   75, -5.15, -1.85,  5,   0.00098, 0.95, 0.85, 0.60 }, // venus
    {  -99,  -36,  1.63, -4.48,  5,    0.0012, 0.35, 0.62, 1.00 }, // earth
    {   44, -120,  4.07,  1.49,  4,   0.00013, 0.90, 0.38, 0.24 }, // mars
    {   88,  152, -3.20,  1.85, 11,     0.382, 0.80, 0.58, 0.40 }, // jupiter
    { -186,  107, -1.67, -2.90,  9,     0.114, 0.91, 0.79, 0.52 }, // saturn
    { -125, -217,  2.68, -1.54,  7,    0.0175, 0.60, 0.85, 0.90 }, // uranus
    {  242, -140,  1.47,  2.54,  7,    0.0206, 0.28, 0.40, 0.92 }, // neptune
};



int main() {
    Engine engine;

    float px = 0, py = 0;
    for (auto& p : planets) { px += p.m * p.vx; py += p.m * p.vy; }
    planets[0].vx -= px / planets[0].m;   // planets[0] is the sun
    planets[0].vy -= py / planets[0].m;

    const float dt = 0.99f; 

    // keep running until someone closes the window
    while (!glfwWindowShouldClose(engine.window)) {
        engine.run();

        for (auto& p : planets) {
            float ax =0;
            float ay=0;

            for (auto& other :planets){
                if(&other == &p){continue;}


                float dx = other.x - p.x;
                float dy = other.y - p.y;

                float r = sqrt(dx * dx + dy * dy);
                float r2 = r*r + 25.0f;
            
                float G = 0.6f;
                float F = ((G * p.m * other.m)/ r2);

                float a = F/ p.m;

                ax += a * (dx / r);
                ay += a * (dy / r);

            }

            p.vx += ax * dt;
            p.vy += ay * dt;
        }                                   // first loop ends here

        for (auto& p : planets) {           // second loop: beside the first, not inside it
            p.x += p.vx*dt;
            p.y += p.vy*dt;

            glColor3f(p.cr, p.cg, p.cb);
            drawCircle(p.x, p.y, p.r);
        }

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
