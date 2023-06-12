/* Assignment 3:PARTICLE FOUNTAIN
AUTHOR : SAMUEL ONABOLU
STUDENT NUMBER : 6783369
*/
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <stdlib.h>

#include <stdio.h>

#include <windows.h>

#include <malloc.h>

#include <freeglut.h>

#include <FreeImage.h>

#include <set>

#include <cstdlib>

#include <iostream>

#include <ctime>

#include <vector>

#include <algorithm>
/*
Extras I implemented:
1:color/size
2:spark
3:spray mode
4: collision
5: textures
*/

GLuint texture;
GLenum shade;
int windowHeight = 1000;
int windowWidth = 1000;
const float N = 3.0; // control max range for speed value when speed key is invoked
int num_of_particles = 1; // variable to control how many particles we want to generate depending on the firing mode
using namespace std;
int firingMode = 1; // variable to control which firing mode to use
bool isFiring = false; // variable to control if firing key is pressed
bool isFReleased = true; // variable to control if firing key is releaed
int maxParticles = 100; // Total amound of particles that can be rendered at once
int activeParticles = 0; // Active particles in the scene
bool rotateOn = false; // variable to check if rotate is on
bool speedOn = false; // variable to check if random speed is on
bool randomOn = false; // variable to check if randomness feature is on
bool sparkOn = false; // variable to check if spark on collision is on
bool sprayOn = false; // variable to check if spark on collision is on
bool shadeToggle = false; // variable to check which shading mode to use, defualt is flat shading
bool culling = false; // variable to check if backface culling is active, defualt is inactive
int renderMode = 4;  // variable to check which render mode we should render the particles. Defualt is polygon mode.
bool frictionOn = false; // variable to check if friction mode is on.
const float MAX_SPEED = 5.0f;// max speed a particle can have
const float FRICTION_GROUND = 0.60f;// particles will lose a percentage of their speed when colliding with the ground
const float FRICTION_COLLISION = 0.99f;  // particles will lose a percentage of their speed when colliding with other particles
const float SPEED_THRESHOLD = 0.04f; /* variable to track if particle when particle should be removed if their speed is less
than this value */
const float DEATH_THRESHOLD = -10.0f; // variable to track when particle should be removed after falling off the edge
const float GRAVITY = 0.3f; // gravity value
/*Struct to hold our particles. A particle holds a position, velocity, an angle as well as angle incremets,
 a rgb value, and a size*/
struct Particle {
    float x1, y1, z1;
    float x2, y2, z2;
    float x3, y3, z3;
    float vx, vy, vz;
    float Ax, Ay, Az;
    float dax, day, daz;
    float r, g, b;
    float size;
};
/*Struct for when I generate sparks. Holds a position, a rgb value for the color, and the lifetime of the
spark*/
struct Spark {
    float x, y, z;
    float r, g, b;
    float duration;
};
/*Struct for handling the rotation*/
struct Global {
    float angle[3];
    int axis;
} global;

enum { X, Y, Z };

vector<Spark> sparks;
vector<Particle> particles;
float triangleSize = 1.0f;
float fountainX = 0.0f;
float fountainY = 10.0f;
float fountainZ = 0.0f;
float randomX;
float randomY;
float randomZ;
void setupCamera();

/*This method is used to generate my particles in the scene.
It handles the initilization of the values and stores the information in the particles vector.*/
void generateParticles(int n) {
    srand((unsigned)time(NULL));
    Particle particle;
    for (int i = 0; i < n; i++) {
        //initialize the position with a random float value from -10 to 10
        randomX = 20.0f * (rand() / (float)RAND_MAX - 0.5f);
        randomY = 20.0f * (rand() / (float)RAND_MAX - 0.5f);
        randomZ = 20.0f * (rand() / (float)RAND_MAX - 0.5f);
         

        //random rotation from 0-360.
        particle.Ax = rand() % 361;
        particle.Ay = rand() % 361;
        particle.Az = rand() % 361;

        // increment the angle from the range -1 to 1
        particle.dax = (rand() % 101 - 50) / 50.0f;
        particle.day = (rand() % 101 - 50) / 50.0f;
        particle.daz = (rand() % 101 - 50) / 50.0f;

        /*Initialize the particle position somewhere on top of the fountain hence the use of fountain x, y, and z variables.
         Also randomize them a bit with the use of  random x, y, and z*/

        particle.x1 = fountainX + triangleSize + randomX;
        particle.y1 = fountainY + randomY + 20.0f;;
        particle.z1 = fountainZ + randomZ;

        particle.x2 = fountainX + randomX;
        particle.y2 = fountainY + triangleSize + randomY + 20.0f;;
        particle.z2 = fountainZ + randomZ;

        particle.x3 = fountainX + randomX;
        particle.y3 = fountainY + randomY + 20.0f;;
        particle.z3 = fountainZ + triangleSize + randomZ;

         
        particle.vx = 2.0f * ((float)rand() / RAND_MAX - 0.5f);//randomize my x velocity from -1 to 1.
        particle.vy = -2.0f * (float)rand() / RAND_MAX; //randomize my y velocity from -2 to 0.
        particle.vz = 2.0f * ((float)rand() / RAND_MAX - 0.5f); //randomize my z velocity from -1 to 1.

        //If High spray mode is on increae the randomess of the velocity
        if (sprayOn) {

            particle.vx = 5.0f * ((float)rand() / RAND_MAX - 0.5f);
            particle.vy = -5.0f * (float)rand() / RAND_MAX;
            particle.vz = 5.0f * ((float)rand() / RAND_MAX - 0.5f);
        }
        //Give the particles a rnadom color for when the user press the randomness key.
        particle.r = (float)rand() / RAND_MAX;
        particle.g = (float)rand() / RAND_MAX;
        particle.b = (float)rand() / RAND_MAX;

        //Initilize size with a random float value from 0-3.
        particle.size = fmod(rand(), 4.0);

        //Give the particles a rnadom size for when the user press the randomness key.
        if (randomOn) {
            particle.x1 = fountainX + particle.size + randomX;
            particle.y1 = fountainY + randomY + 20.0f;;
            particle.z1 = fountainZ + randomZ;

            particle.x2 = fountainX + randomX;
            particle.y2 = fountainY + particle.size + randomY + 20.0f;;
            particle.z2 = fountainZ + randomZ;

            particle.x3 = fountainX + randomX;
            particle.y3 = fountainY + randomY + 20.0f;;
            particle.z3 = fountainZ + particle.size + randomZ;
        }

        //Push the particle to the particles vector.
        particles.push_back(particle);
    }
}
/*This method simply draws the fountain as a triangle. I use glTexCoord2f to give it a texture.*/
void drawFountain() {
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.75f, -0.75f);
    glTexCoord2f(0.5f, 1.0f); glVertex2f(0.0f, 0.75f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.75f, -0.75f);

    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.75f, -0.75f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.75f, -0.75f);
    glTexCoord2f(0.5f, 1.0f); glVertex2f(0.0f, 0.75f);
}

/*This method calculates the distance from the center point of a triangle to the center point of another triangle.
In a later method I compare the distance to some value*/
float calculateDistance(Particle& p1, Particle& p2) {
    float x0 = (p1.x1 + p1.x2 + p1.x3) / 3;
    float y0 = (p1.y1 + p1.y2 + p1.y3) / 3;
    float z0 = (p1.z1 + p1.z2 + p1.z3) / 3;

    float x1 = (p2.x1 + p2.x2 + p2.x3) / 3;
    float y1 = (p2.y1 + p2.y2 + p2.y3) / 3;
    float z1 = (p2.z1 + p2.z2 + p2.z3) / 3;
    float distance = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2) + pow(z1 - z0, 2));


    return distance;

}
/*This method returns true if the distance between the 2 particles are less than some threshold meaning they collide,
false otherwise*/
bool collides(Particle& p1, Particle& p2) {
    return (calculateDistance(p1, p2)) < 5;
}
/*This methods generates a spark if the 2 particles collide using the collide method earleir. If true,
I push the position of the spark, as well as a random color for the spark into the spark vector*/
bool generateSpark(Particle& p1, Particle& p2) {
    if (collides(p1, p2)) {
        Spark spark;
        // Divide by 6 to get the average of the 2 particles coordiantes
        spark.x = (p1.x1 + p1.x2 + p1.x3 + p2.x1 + p2.x2 + p2.x3) / 6;
        spark.y = (p1.y1 + p1.y2 + p1.y3 + p2.y1 + p2.y2 + p2.y3) / 6;
        spark.z = (p1.z1 + p1.z2 + p1.z3 + p2.z1 + p2.z2 + p2.z3) / 6;

        spark.r = (float)rand() / RAND_MAX;
        spark.g = (float)rand() / RAND_MAX;
        spark.b = (float)rand() / RAND_MAX;
        
        spark.duration = 0.0f;
        sparks.push_back(spark);
        return true;
    }
    return false;
}
/* This method takes care of the updating of the particles. It handles the gravity factor 
as well as updating the particles position in terms of its velocity.*/
void display() {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     
    int width, height, channels;
    unsigned char* img = stbi_load("tex.jpg", &width, &height, &channels, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
         
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height,
            GL_RGB, GL_UNSIGNED_BYTE, img);
    
    stbi_image_free(img);

     
    srand((unsigned)time(NULL));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear both color and depth buffer

    // Render a flat polygon with 4 vertices
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    if (shadeToggle)glShadeModel(GL_SMOOTH);
    else glShadeModel(GL_FLAT);

    if (culling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else glDisable(GL_CULL_FACE);

    glBegin(GL_QUADS);
    // counter clockwise for the front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, 0.0f, -20.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0f, 0.0f, 20.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(20.0f, 0.0f, 20.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(20.0f, 0.0f, -20.0f);

    // clockwise for the back face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, 0.0f, -20.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(20.0f, 0.0f, -20.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(20.0f, 0.0f, 20.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0f, 0.0f, 20.0f);
    glEnd();
    glPopMatrix();

    // Render a triangle as fountain
    glColor3f(0.4f, 0.2f, 0.0f);
    glPushMatrix();
    glTranslatef(fountainX, 40.0f, fountainZ); // Add this line to translate the fountain to the center
    glScalef(10.0f, 10.0f, 10.0f);
    glBegin(GL_TRIANGLES);
    drawFountain();
    glEnd();
    glPopMatrix();

    /*These if statments tell the program which firing mode to use. Firing mode 1 is our default. Firing mode 2 is our
    manual mode. Firing mode 3 is a single shot mode. isFiring is a boolean variable to check if F key is pressed or not.
    Not applicable to our defualt firing mode 1.*/
    if (firingMode == 3 && isFiring) {
        generateParticles(1);
        isFiring = false;    
    }
    else if (firingMode == 2 && isFiring) {
        generateParticles(num_of_particles);
        isFiring = false;
    }
    else if (firingMode == 1) {

        if (activeParticles < maxParticles) {
            generateParticles(num_of_particles);
            activeParticles += num_of_particles;

        }

    }

    // Check for collisions between particles
    for (int i = 0; i < particles.size(); ++i) {
        for (int j = i + 1; j < particles.size(); ++j) {
            if (collides(particles[i], particles[j])) {
     
                // Calculate the new velocities after collision
                float tempVx = particles[i].vx;
                float tempVy = particles[i].vy;
                float tempVz = particles[i].vz;

                particles[i].vx = particles[j].vx * FRICTION_COLLISION;
                particles[i].vy = particles[j].vy * FRICTION_COLLISION;
                particles[i].vz = particles[j].vz * FRICTION_COLLISION;
                //swap the velocities to apply a colliding effect
                particles[j].vx = tempVx * FRICTION_COLLISION;
                particles[j].vy = tempVy * FRICTION_COLLISION;
                particles[j].vz = tempVz * FRICTION_COLLISION;
            }
        }
    }

    //check for spark
    if (sparkOn) {
        for (int i = 0; i < particles.size(); ++i) {
            for (int j = i + 1; j < particles.size(); ++j) {
                if (generateSpark(particles[i], particles[j])) {
                }
            }
        }
    }
    

    for (int i = 0; i < particles.size(); i++) {

        particles[i].vy -= GRAVITY;
        // Calculate new position and velocity
        particles[i].x1 += particles[i].vx;
        particles[i].y1 += particles[i].vy;
        particles[i].z1 += particles[i].vz;

        particles[i].x2 += particles[i].vx;
        particles[i].y2 += particles[i].vy;
        particles[i].z2 += particles[i].vz;

        particles[i].x3 += particles[i].vx;
        particles[i].y3 += particles[i].vy;
        particles[i].z3 += particles[i].vz;
        // Check for collision with the ground

        if ((particles[i].y1 <= 0.0f && particles[i].x1 >= -20.0f && particles[i].x1 <= 20.0f && particles[i].z1 >= -20.0f && particles[i].z1 <= 20.0f) ||
            (particles[i].y2 <= 0.0f && particles[i].x2 >= -20.0f && particles[i].x2 <= 20.0f && particles[i].z2 >= -20.0f && particles[i].z2 <= 20.0f) ||
            (particles[i].y3 <= 0.0f && particles[i].x3 >= -20.0f && particles[i].x3 <= 20.0f && particles[i].z3 >= -20.0f && particles[i].z3 <= 20.0f)) {
            if (frictionOn) {
                particles[i].vx *= FRICTION_GROUND;
                particles[i].vy *= FRICTION_GROUND;
                particles[i].vz *= FRICTION_GROUND;
            }
            particles[i].vy = -particles[i].vy;


        }
        // Reset particle if it falls below the death threshold
        if (particles[i].y1 < DEATH_THRESHOLD || particles[i].y2 < DEATH_THRESHOLD || particles[i].y3 < DEATH_THRESHOLD) {

            if (firingMode == 3) {
                particles.erase(particles.begin() + i);
                i--; // Decrease the index after we delete the particle
                activeParticles--;
            }
            else if (firingMode == 2) {
                particles.erase(particles.begin() + i);
                i--; // Decrease the index after we delete the particle
                activeParticles--;
            }
            else if (firingMode == 1) {
                particles.erase(particles.begin() + i);
                i--; // Decrease the index after we delete the particle
                activeParticles--;
            }
        }

    }

    //Handles the interactive rotation along the x, y, and z axis.
    glRotatef(global.angle[X], 1.0, 0.0, 0.0);
    glRotatef(global.angle[Y], 0.0, 1.0, 0.0);
    glRotatef(global.angle[Z], 0.0, 0.0, 1.0);

    //set the color to brown for my triangles
    glColor3f(0.4f, 0.2f, 0.0f);
    /*This for loop is where i render my particles. I also check if my boolean variables rotateOn, speedOn, shadeToggle,
    and culling is active, if so I need to make the necessary changes to my particles.*/
    for (int i = 0; i < particles.size(); i++) {
        glPushMatrix();
        if (rotateOn) {
            glRotatef(particles[i].Ax, 1.0f, 0.0f, 0.0f);
            glRotatef(particles[i].Ay, 0.0f, 1.0f, 0.0f);
            glRotatef(particles[i].Az, 0.0f, 0.0f, 1.0f);
        }
        //Random speed functionality
        if (speedOn) {
          
            float speed = fmod(rand(), N)+1;
            /*  I use the min function here to ensure speed does not go past a certain value. */
            particles[i].vx = min(particles[i].vx * speed, MAX_SPEED);
            particles[i].vy = min(particles[i].vy * speed, MAX_SPEED);
            particles[i].vz = min(particles[i].vz * speed, MAX_SPEED);
            
        }
        if (shadeToggle)glShadeModel(GL_SMOOTH);
        else glShadeModel(GL_FLAT);

        if (culling) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        else glDisable(GL_CULL_FACE);
         
        if (renderMode == GL_TRIANGLES) {
            glBegin(GL_POLYGON);
        }
        else if (renderMode == GL_LINES) {
            glBegin(GL_LINES);
        }
        else if (renderMode == GL_POINTS) {
            glPointSize(2.0f);
            glBegin(GL_POINTS);
           
        }

        //Render triangles in counter clockwise order
        if (randomOn) glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(particles[i].x1, particles[i].y1, particles[i].z1);
        if (randomOn) glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(particles[i].x2, particles[i].y2, particles[i].z2);
        if (randomOn) glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(particles[i].x3, particles[i].y3, particles[i].z3);

        // Render triangles in clockwise order so that triangles can be visible when backface culling is active
        if (randomOn) glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(particles[i].x3, particles[i].y3, particles[i].z3);
        if (randomOn) glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(particles[i].x2, particles[i].y2, particles[i].z2);
        if (randomOn) glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(particles[i].x1, particles[i].y1, particles[i].z1);

        //If the velocity is less than the speed threshold delete the particle
        if (abs(particles[i].vx) < SPEED_THRESHOLD && abs(particles[i].vy) < SPEED_THRESHOLD && abs(particles[i].vz) < SPEED_THRESHOLD) {

            particles.erase(particles.begin() + i);
            i--; // Decrease the index as the current particle has been removed
            activeParticles--;
        }

        glEnd();
        glPopMatrix();
    }
    //If the spark key is active render the sparks.
    if (sparkOn) {

        glPointSize(1.0f); // set the size of the points

        if (shadeToggle)glShadeModel(GL_SMOOTH);
        else glShadeModel(GL_FLAT);

        if (culling) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        else glDisable(GL_CULL_FACE);
        glBegin(GL_POINTS);
        for (int i = 0; i < sparks.size(); i++) {
            glColor3f(sparks[i].r, sparks[i].g, sparks[i].b);
            glVertex3f(sparks[i].x, sparks[i].y, sparks[i].z);
        }
        glEnd();
        // remove sparks after duration has been reached
        for (int i = 0; i < sparks.size(); ++i) {
            sparks[i].duration += 0.001f;

            if (sparks[i].duration >= 0.002f) {
                sparks.erase(sparks.begin() + i);
                i--; // Decrease the index once spark has been deleted
            }
        }
    }

    glutSwapBuffers();
    glutPostRedisplay();
    //Sleep(60);

}
/* Mouse function to handle the rotation view along x, y and z axis*/
void mouse(int btn, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (btn == GLUT_LEFT_BUTTON) {
            global.angle[global.axis] = global.angle[global.axis] + 0.2;
        }
        else if (btn == GLUT_RIGHT_BUTTON) {
            global.angle[global.axis] = global.angle[global.axis] - 0.2;
        }
        else {
            global.angle[X] = 0.0;
            global.angle[Y] = 0.0;
            global.angle[Z] = 0.0;
            glPopMatrix();
            glPushMatrix();
        }
    }
}
/*Keyboard function to handle all the modes.*/
void keyboard(unsigned char key, int x, int y) {

    switch (key) {
    case 'S':
    case 's':
        speedOn = !speedOn;
        if (speedOn) {
            cout << "Speed is turned on " << endl;
        }
        else {
            cout << "Speed is turned off " << endl;
        }
        break;

    case 'D':
    case 'd':
        rotateOn = !rotateOn;
        if (rotateOn) {
            cout << "Rotation is turned on " << endl;
        }
        else {
            cout << "Rotation is turned off " << endl;
        }
        break;

    case 'G':
    case 'g':
        frictionOn = !frictionOn;
        if (frictionOn) {
            cout << "Friction is turned on " << endl;
        }
        else {
            cout << "Friction is turned off " << endl;
        }
        break;
    case 'C':
    case 'c':
        randomOn = !randomOn;
        if (randomOn) {
            cout << "Randomness is turned on " << endl;
        }
        else {
            cout << "Randomness is turned off " << endl;
        }
        break;

    case 'T':
    case 't':
        sparkOn = !sparkOn;
        if (sparkOn) {
            cout << "Spark is turned on " << endl;
        }
        else {
            cout << "Spark is turned off " << endl;
        }
        break;
    case 'K':
    case 'k':
        shadeToggle = !shadeToggle;
        if (shadeToggle) {
            cout << "Smooth shading is turned on " << endl;
        }
        else {
            cout << "Flat shading is turned on" << endl;
        }
        break;

    case 'L':
    case 'l':
        culling = !culling;
        if (culling) {
            cout << "Back face culling is turned on " << endl;
        }
        else {
            cout << "Back face is turned off" << endl;
        }
        break;

    case 'q':
    case 'Q':
        sprayOn = !sprayOn;
        if (sprayOn) {
            cout << "High spray is turned on " << endl;
        }
        else {
            cout << "High spray is turned off " << endl;
        }
        break;

    case 'R':
    case 'r':
        isFiring = false;
         isFReleased = true; 
         rotateOn = false; 
          speedOn = false;
           randomOn = false;
         sparkOn = false;
        sprayOn = false;
          shadeToggle = false;
         culling = false;
         renderMode = 4;
         frictionOn = false;
        particles.clear();
        firingMode = 1;
        activeParticles = 0;
         
        cout << "Reseting to defualth settings " << endl;
        break;

    case '1':
        cout << "Firing mode set to 1 " << endl;
        activeParticles = 0;
        firingMode = 1;
        
        break;
    case '2':
        cout << "Firing mode set to 2 " << endl;
        activeParticles = 0;
        firingMode = 2;
        num_of_particles = 4;
        
        break;
    case '3':
        activeParticles = 0;
        cout << "Setting Firing mode to 3" << endl;
        firingMode = 3;
        num_of_particles = 1;
        break;
    case '4':
        cout << "Entering polygon Mode!" << endl;
        renderMode = GL_TRIANGLES;
        break;
    case '5':
        cout << "Entering wireframe Mode!" << endl;
        renderMode = GL_LINES;
        break;
    case '6':
        cout << "Entering points Mode!" << endl;
        renderMode = GL_POINTS;
        break;

    case 'x':
    case 'X':
        cout << "Rotating along X axis" << endl;
        global.axis = X;
        break;

    case 'y':
    case 'Y':
        cout << "Rotating along Y axis" << endl;
        global.axis = Y;
        break;

    case 'z':
    case 'Z':
        cout << "Rotating along Z axis" << endl;
        global.axis = Z;
        break;
    case 'F':
    case 'f':
        if ((firingMode == 3 && isFReleased) && activeParticles < maxParticles) {
            isFiring = true; // Start firing when 'F' or 'f' key is pressed
            isFReleased = false;
            activeParticles++;
        }

        else if (firingMode == 2) {
            if (activeParticles < maxParticles) {
                isFiring = true;
                activeParticles += num_of_particles;

            }
        }

        break;
    }

} 

/*This method is used to detect when firing mode 2, if the F key is released or not. We need to check this because 
we want to be able to hold down the F key in mode 2.*/
void keyboardUp(unsigned char key, int x, int y) {
    if (firingMode == 2 && (key == 'F' || key == 'f')) {
        isFiring = false;
    }
    if (key == 'F' || key == 'f') {
        isFiring = false;;
        isFReleased = true;
    }
}
/* Sets the camera up using glulook at and gluPerspective*/
void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
    gluLookAt(0.0, 80.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int main(int argc, char** argv) {
    cout << "1 : Constant stream\n"
        << "2 : Manual firing\n"
        << "3 : Single shot mode\n"
        << "4 : Polygon Mode\n"
        << "5 : Wireframe Mode\n"
        << "6 : Points Mode\n"
        << "F : Fire\n"
        << "S : Random speed\n"
        << "D : Random spin\n"
        << "C : Random color and size (bonus)\n"
        << "G : Friction mode\n"
        << "T : Spark on collision (bonus)\n"
        << "Q : High spray mode (bonus)\n"
        << "L : Backface culling Mode\n"
        << "K : smooth (Gouraud) Mode\n"
        << "X : Rotate along X axis\n"
        << "Y : Rotate along Y axis\n"
        << "Z : Rotate along Z axis\n"
        <<"Left mouse : rotate left faster\n"
        << "Left mouse : rotate right faster\n"
        << "R : Reset\n";


    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("3D PARTICLE FOUNTAIN!");
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glEnable(GL_DEPTH_TEST);
    setupCamera();
    glutMainLoop();
    return 0;
}