#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>

// Window dimensions
const int WIDTH = 800;
const int HEIGHT = 600;

// Animation variables
float ferrisWheelAngle = 0.0f;

// Day/Night cycle variables
float timeOfDay = 0.0f;
bool isTransitioning = true;
float transitionSpeed = 0.002f;

// Sun and Moon positions
float sunAngle = 0.0f;
float moonAngle = 180.0f;

// Carousel variables
float carouselAngle = 0.0f;
float horseUpDown[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float horsePhase[6] = {0.0f, 60.0f, 120.0f, 180.0f, 240.0f, 300.0f};

// Swing ride variables
float swingAngle = 0.0f;
float swingDirection = 1.0f;
const float MAX_SWING_ANGLE = 45.0f;

// Food cart seller animation
float sellerArmAngle = 0.0f;
float sellerArmDirection = 1.0f;

// Entrance sign animation
float signBlinkTimer = 0.0f;
bool signLightsOn = true;

// People/Crowd structures
struct Person {
    float x, y;
    float speed;
    float direction; // 1 = right, -1 = left
    float scale;
    float r, g, b; // Clothing color
    bool isWalking;
};

const int NUM_PEOPLE = 8;
Person people[NUM_PEOPLE];

// Fireworks structures
struct Particle {
    float x, y;
    float vx, vy;
    float life;
    float r, g, b;
    bool active;
};

struct Firework {
    float x, y;
    float targetY;
    float vy;
    bool exploded;
    bool active;
    Particle particles[50];
    float r, g, b;
};

const int MAX_FIREWORKS = 10;
Firework fireworks[MAX_FIREWORKS];
float fireworkTimer = 0.0f;
float nextFireworkTime = 100.0f;

// Stars structure
struct Star {
    float x, y;
    float brightness;
    float twinkleSpeed;
};
const int NUM_STARS = 100;
Star stars[NUM_STARS];

// Initialize OpenGL
void init() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-400, 400, -300, 300);
    
    srand(time(NULL));
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].x = (rand() % 800) - 400.0f;
        stars[i].y = (rand() % 400) - 100.0f + 100;
        stars[i].brightness = 0.5f + (rand() % 100) / 200.0f;
        stars[i].twinkleSpeed = 0.01f + (rand() % 100) / 5000.0f;
    }
    
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        fireworks[i].active = false;
        fireworks[i].exploded = false;
        for (int j = 0; j < 50; j++) {
            fireworks[i].particles[j].active = false;
        }
    }
    
    // Initialize people
    for (int i = 0; i < NUM_PEOPLE; i++) {
        people[i].x = (rand() % 700) - 350.0f;
        people[i].y = -245.0f; // Higher up so more visible
        people[i].speed = 0.5f + (rand() % 30) / 100.0f; // Faster
        people[i].direction = (rand() % 2 == 0) ? 1.0f : -1.0f;
        people[i].scale = 1.2f + (rand() % 60) / 100.0f; // Bigger scale
        people[i].isWalking = true;
        
        // Random clothing colors
        int colorChoice = rand() % 6;
        switch(colorChoice) {
            case 0: people[i].r = 0.9f; people[i].g = 0.2f; people[i].b = 0.2f; break; // Red
            case 1: people[i].r = 0.2f; people[i].g = 0.3f; people[i].b = 0.9f; break; // Blue
            case 2: people[i].r = 0.2f; people[i].g = 0.8f; people[i].b = 0.3f; break; // Green
            case 3: people[i].r = 0.9f; people[i].g = 0.7f; people[i].b = 0.2f; break; // Yellow
            case 4: people[i].r = 0.7f; people[i].g = 0.2f; people[i].b = 0.8f; break; // Purple
            case 5: people[i].r = 0.9f; people[i].g = 0.5f; people[i].b = 0.2f; break; // Orange
        }
    }
}

// Draw a circle
void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

// Draw filled circle
void drawFilledCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

// Get sky color
void getSkyColor(float& r, float& g, float& b) {
    r = 0.53f * (1.0f - timeOfDay) + 0.05f * timeOfDay;
    g = 0.81f * (1.0f - timeOfDay) + 0.05f * timeOfDay;
    b = 0.92f * (1.0f - timeOfDay) + 0.15f * timeOfDay;
}

// Draw sun
void drawSun() {
    float sunX = 350 * cos(sunAngle * 3.1415926f / 180.0f);
    float sunY = 350 * sin(sunAngle * 3.1415926f / 180.0f);
    
    if (sunY > -50 && timeOfDay < 0.7f) {
        glColor4f(1.0f, 0.9f, 0.3f, 0.3f * (1.0f - timeOfDay));
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawFilledCircle(sunX, sunY, 45, 30);
        
        glColor3f(1.0f, 0.95f, 0.2f);
        drawFilledCircle(sunX, sunY, 30, 30);
        glDisable(GL_BLEND);
    }
}

// Draw moon
void drawMoon() {
    float moonX = 350 * cos(moonAngle * 3.1415926f / 180.0f);
    float moonY = 350 * sin(moonAngle * 3.1415926f / 180.0f);
    
    if (moonY > -50 && timeOfDay > 0.3f) {
        glColor4f(0.9f, 0.9f, 1.0f, 0.2f * timeOfDay);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawFilledCircle(moonX, moonY, 35, 30);
        
        glColor3f(0.95f, 0.95f, 1.0f);
        drawFilledCircle(moonX, moonY, 25, 30);
        
        glColor3f(0.85f, 0.85f, 0.9f);
        drawFilledCircle(moonX - 8, moonY + 5, 6, 20);
        drawFilledCircle(moonX + 5, moonY - 8, 5, 20);
        drawFilledCircle(moonX + 8, moonY + 8, 4, 20);
        glDisable(GL_BLEND);
    }
}

// Draw stars
void drawStars() {
    if (timeOfDay > 0.2f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPointSize(2.0f);
        
        glBegin(GL_POINTS);
        for (int i = 0; i < NUM_STARS; i++) {
            float alpha = stars[i].brightness * (timeOfDay - 0.2f) / 0.8f;
            glColor4f(1.0f, 1.0f, 1.0f, alpha);
            glVertex2f(stars[i].x, stars[i].y);
        }
        glEnd();
        
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i < NUM_STARS / 5; i++) {
            float alpha = stars[i * 5].brightness * (timeOfDay - 0.2f) / 0.8f;
            glColor4f(1.0f, 1.0f, 0.9f, alpha);
            glVertex2f(stars[i * 5].x, stars[i * 5].y);
        }
        glEnd();
        
        glDisable(GL_BLEND);
    }
}

// Launch firework
void launchFirework() {
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        if (!fireworks[i].active) {
            fireworks[i].active = true;
            fireworks[i].exploded = false;
            fireworks[i].x = (rand() % 400) - 200.0f;
            fireworks[i].y = -250.0f;
            fireworks[i].targetY = 150.0f + (rand() % 80); // Higher explosion height!
            fireworks[i].vy = 13.0f + (rand() % 4); // Faster initial velocity
            
            // Better color variety - bright, vibrant colors
            int colorType = rand() % 7;
            switch(colorType) {
                case 0: // Red
                    fireworks[i].r = 1.0f; fireworks[i].g = 0.2f; fireworks[i].b = 0.2f;
                    break;
                case 1: // Blue
                    fireworks[i].r = 0.2f; fireworks[i].g = 0.5f; fireworks[i].b = 1.0f;
                    break;
                case 2: // Green
                    fireworks[i].r = 0.2f; fireworks[i].g = 1.0f; fireworks[i].b = 0.3f;
                    break;
                case 3: // Purple
                    fireworks[i].r = 0.9f; fireworks[i].g = 0.2f; fireworks[i].b = 1.0f;
                    break;
                case 4: // Orange
                    fireworks[i].r = 1.0f; fireworks[i].g = 0.6f; fireworks[i].b = 0.1f;
                    break;
                case 5: // Cyan
                    fireworks[i].r = 0.2f; fireworks[i].g = 1.0f; fireworks[i].b = 1.0f;
                    break;
                case 6: // White/Gold
                    fireworks[i].r = 1.0f; fireworks[i].g = 1.0f; fireworks[i].b = 0.8f;
                    break;
            }
            
            for (int j = 0; j < 50; j++) {
                fireworks[i].particles[j].active = false;
            }
            return;
        }
    }
    fireworks[0].active = false;
}

// Update fireworks
void updateFireworks() {
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        if (!fireworks[i].active) continue;
        
        if (!fireworks[i].exploded) {
            fireworks[i].y += fireworks[i].vy;
            fireworks[i].vy -= 0.2f;
            
            if (fireworks[i].y >= fireworks[i].targetY || (fireworks[i].vy < 0 && fireworks[i].y > 50)) {
                fireworks[i].exploded = true;
                
                for (int j = 0; j < 50; j++) {
                    fireworks[i].particles[j].active = true;
                    fireworks[i].particles[j].x = fireworks[i].x;
                    fireworks[i].particles[j].y = fireworks[i].y;
                    fireworks[i].particles[j].life = 1.0f;
                    
                    float angle = (rand() % 360) * 3.1415926f / 180.0f;
                    float speed = 4.0f + (rand() % 50) / 10.0f; // Faster, more spread
                    fireworks[i].particles[j].vx = speed * cos(angle);
                    fireworks[i].particles[j].vy = speed * sin(angle);
                    
                    // Color variation - some particles slightly different for depth
                    float colorVar = (rand() % 40 - 20) / 100.0f;
                    fireworks[i].particles[j].r = fireworks[i].r + colorVar;
                    fireworks[i].particles[j].g = fireworks[i].g + colorVar;
                    fireworks[i].particles[j].b = fireworks[i].b + colorVar;
                    
                    if (fireworks[i].particles[j].r > 1.0f) fireworks[i].particles[j].r = 1.0f;
                    if (fireworks[i].particles[j].g > 1.0f) fireworks[i].particles[j].g = 1.0f;
                    if (fireworks[i].particles[j].b > 1.0f) fireworks[i].particles[j].b = 1.0f;
                    if (fireworks[i].particles[j].r < 0.3f) fireworks[i].particles[j].r = 0.3f;
                    if (fireworks[i].particles[j].g < 0.3f) fireworks[i].particles[j].g = 0.3f;
                    if (fireworks[i].particles[j].b < 0.3f) fireworks[i].particles[j].b = 0.3f;
                }
            }
            
            // Only deactivate if it falls back down after launch
            if (fireworks[i].vy < 0 && fireworks[i].y < -280) {
                fireworks[i].active = false;
            }
        } else {
            bool anyActive = false;
            
            for (int j = 0; j < 50; j++) {
                if (fireworks[i].particles[j].active) {
                    fireworks[i].particles[j].x += fireworks[i].particles[j].vx;
                    fireworks[i].particles[j].y += fireworks[i].particles[j].vy;
                    fireworks[i].particles[j].vy -= 0.15f; // Stronger gravity
                    fireworks[i].particles[j].vx *= 0.98f; // More air resistance
                    fireworks[i].particles[j].vy *= 0.98f;
                    fireworks[i].particles[j].life -= 0.010f; // Slower fade for longer visibility
                    
                    if (fireworks[i].particles[j].life <= 0.0f) {
                        fireworks[i].particles[j].active = false;
                    } else {
                        anyActive = true;
                    }
                }
            }
            
            if (!anyActive) {
                fireworks[i].active = false;
            }
        }
    }
}

// Draw fireworks
void drawFireworks() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for glow!
    
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        if (!fireworks[i].active) continue;
        
        if (!fireworks[i].exploded) {
            // Bright rocket core
            glPointSize(12.0f);
            glBegin(GL_POINTS);
            glColor4f(fireworks[i].r * 1.2f, fireworks[i].g * 1.2f, fireworks[i].b * 1.2f, 1.0f);
            glVertex2f(fireworks[i].x, fireworks[i].y);
            glEnd();
            
            // Large glow
            glPointSize(30.0f);
            glBegin(GL_POINTS);
            glColor4f(fireworks[i].r, fireworks[i].g, fireworks[i].b, 0.6f);
            glVertex2f(fireworks[i].x, fireworks[i].y);
            glEnd();
            
            // Sparkling trail
            glPointSize(6.0f);
            glBegin(GL_POINTS);
            for (int k = 1; k < 15; k++) {
                float alpha = 0.9f - k * 0.06f;
                float sparkle = (rand() % 100) / 100.0f * 0.3f + 0.7f;
                glColor4f(fireworks[i].r * sparkle, fireworks[i].g * sparkle, fireworks[i].b * sparkle, alpha);
                glVertex2f(fireworks[i].x + (rand() % 6 - 3), fireworks[i].y - k * 6);
            }
            glEnd();
        } else {
            // Main explosion particles - BRIGHT
            glPointSize(8.0f);
            glBegin(GL_POINTS);
            for (int j = 0; j < 50; j++) {
                if (fireworks[i].particles[j].active) {
                    float alpha = fireworks[i].particles[j].life * 1.2f;
                    if (alpha > 1.0f) alpha = 1.0f;
                    glColor4f(
                        fireworks[i].particles[j].r * 1.3f,
                        fireworks[i].particles[j].g * 1.3f,
                        fireworks[i].particles[j].b * 1.3f,
                        alpha
                    );
                    glVertex2f(fireworks[i].particles[j].x, fireworks[i].particles[j].y);
                }
            }
            glEnd();
            
            // Large glow halos
            glPointSize(20.0f);
            glBegin(GL_POINTS);
            for (int j = 0; j < 50; j++) {
                if (fireworks[i].particles[j].active) {
                    float alpha = fireworks[i].particles[j].life * 0.5f;
                    glColor4f(
                        fireworks[i].particles[j].r,
                        fireworks[i].particles[j].g,
                        fireworks[i].particles[j].b,
                        alpha
                    );
                    glVertex2f(fireworks[i].particles[j].x, fireworks[i].particles[j].y);
                }
            }
            glEnd();
            
            // Outer soft glow
            glPointSize(35.0f);
            glBegin(GL_POINTS);
            for (int j = 0; j < 50; j++) {
                if (fireworks[i].particles[j].active && fireworks[i].particles[j].life > 0.7f) {
                    float alpha = fireworks[i].particles[j].life * 0.25f;
                    glColor4f(
                        fireworks[i].particles[j].r * 0.8f,
                        fireworks[i].particles[j].g * 0.8f,
                        fireworks[i].particles[j].b * 0.8f,
                        alpha
                    );
                    glVertex2f(fireworks[i].particles[j].x, fireworks[i].particles[j].y);
                }
            }
            glEnd();
        }
    }
    
    glDisable(GL_BLEND);
}

// Draw Ferris Wheel
void drawFerrisWheel() {
    glPushMatrix();
    glTranslatef(0, 50, 0);
    glRotatef(ferrisWheelAngle, 0, 0, 1);
    
    float wheelR = 0.8f * (1.0f - timeOfDay * 0.3f);
    float wheelG = 0.3f * (1.0f - timeOfDay * 0.3f);
    float wheelB = 0.3f + timeOfDay * 0.3f;
    glColor3f(wheelR, wheelG, wheelB);
    glLineWidth(3.0f);
    drawCircle(0, 0, 150, 50);
    drawCircle(0, 0, 20, 20);
    
    glBegin(GL_LINES);
    for (int i = 0; i < 8; i++) {
        float angle = (360.0f / 8.0f) * i * 3.1415926f / 180.0f;
        glVertex2f(0, 0);
        glVertex2f(150 * cos(angle), 150 * sin(angle));
    }
    glEnd();
    
    if (timeOfDay > 0.3f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (int i = 0; i < 24; i++) {
            float angle = (360.0f / 24.0f) * i * 3.1415926f / 180.0f;
            float x = 150 * cos(angle);
            float y = 150 * sin(angle);
            
            if (i % 3 == 0) {
                glColor4f(1.0f, 1.0f, 0.3f, timeOfDay);
            } else if (i % 3 == 1) {
                glColor4f(1.0f, 0.3f, 0.3f, timeOfDay);
            } else {
                glColor4f(0.3f, 0.3f, 1.0f, timeOfDay);
            }
            
            drawFilledCircle(x, y, 8, 20);
            glColor3f(1.0f, 1.0f, 1.0f);
            drawFilledCircle(x, y, 3, 20);
        }
        glDisable(GL_BLEND);
    }
    
    for (int i = 0; i < 8; i++) {
        float angle = (360.0f / 8.0f) * i * 3.1415926f / 180.0f;
        float x = 150 * cos(angle);
        float y = 150 * sin(angle);
        
        glPushMatrix();
        glTranslatef(x, y, 0);
        glRotatef(-ferrisWheelAngle, 0, 0, 1);
        
        float cabinBrightness = 0.9f * (1.0f - timeOfDay * 0.5f);
        glColor3f(cabinBrightness, cabinBrightness, 0.2f);
        glBegin(GL_POLYGON);
        glVertex2f(-10, -10);
        glVertex2f(10, -10);
        glVertex2f(10, 10);
        glVertex2f(-10, 10);
        glEnd();
        
        if (timeOfDay > 0.3f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 0.9f, 0.5f, timeOfDay * 0.8f);
            drawFilledCircle(-5, 0, 3, 20);
            drawFilledCircle(5, 0, 3, 20);
            glDisable(GL_BLEND);
        }
        
        glPopMatrix();
    }
    
    glPopMatrix();
}

// Draw support
void drawSupport() {
    float brightness = 1.0f - timeOfDay * 0.6f;
    glColor3f(0.4f * brightness, 0.2f * brightness, 0.1f * brightness);
    glLineWidth(5.0f);
    
    glBegin(GL_LINES);
    glVertex2f(-30, -100);
    glVertex2f(-10, 50);
    glVertex2f(30, -100);
    glVertex2f(10, 50);
    glEnd();
}

// Draw horse
void drawHorse(float upDownOffset) {
    float brightness = 1.0f - timeOfDay * 0.5f;
    
    glPushMatrix();
    glTranslatef(0, upDownOffset, 0);
    
    glColor3f(0.9f * brightness, 0.7f * brightness, 0.5f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-8, -5);
    glVertex2f(8, -5);
    glVertex2f(8, 5);
    glVertex2f(-8, 5);
    glEnd();
    
    glBegin(GL_POLYGON);
    glVertex2f(8, 0);
    glVertex2f(12, 0);
    glVertex2f(12, 8);
    glVertex2f(8, 6);
    glEnd();
    
    glColor3f(0.8f * brightness, 0.6f * brightness, 0.4f * brightness);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-5, -5);
    glVertex2f(-5, -10);
    glVertex2f(5, -5);
    glVertex2f(5, -10);
    glEnd();
    
    glColor3f(0.8f * brightness, 0.2f * brightness, 0.2f * brightness);
    drawFilledCircle(0, 5, 4, 20);
    
    glPopMatrix();
}

// Draw Carousel
void drawCarousel() {
    glPushMatrix();
    glTranslatef(-200, -50, 0);
    
    float brightness = 1.0f - timeOfDay * 0.5f;
    
    glColor3f(0.7f * brightness, 0.3f * brightness, 0.3f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-60, -15);
    glVertex2f(60, -15);
    glVertex2f(50, -5);
    glVertex2f(-50, -5);
    glEnd();
    
    glColor3f(0.9f * brightness, 0.9f * brightness, 0.3f * brightness);
    glLineWidth(8.0f);
    glBegin(GL_LINES);
    glVertex2f(0, -5);
    glVertex2f(0, 50);
    glEnd();
    
    glColor3f(0.9f * brightness, 0.4f * brightness, 0.4f * brightness);
    glBegin(GL_TRIANGLES);
    glVertex2f(-55, 50);
    glVertex2f(55, 50);
    glVertex2f(0, 70);
    glEnd();
    
    glColor3f(1.0f * brightness, 1.0f * brightness, 1.0f * brightness);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i = -40; i <= 40; i += 20) {
        glVertex2f(i, 50);
        glVertex2f(i * 0.7f, 65);
    }
    glEnd();
    
    glPushMatrix();
    glRotatef(carouselAngle, 0, 0, 1);
    
    for (int i = 0; i < 6; i++) {
        float angle = (360.0f / 6.0f) * i;
        glPushMatrix();
        glRotatef(angle, 0, 0, 1);
        glTranslatef(35, 0, 0);
        glRotatef(-carouselAngle - angle, 0, 0, 1);
        drawHorse(horseUpDown[i]);
        
        glColor3f(0.8f * brightness, 0.8f * brightness, 0.2f * brightness);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(0, 10 + horseUpDown[i]);
        glVertex2f(0, 40);
        glEnd();
        
        glPopMatrix();
    }
    
    glPopMatrix();
    
    if (timeOfDay > 0.3f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (int i = 0; i < 12; i++) {
            float angle = (360.0f / 12.0f) * i * 3.1415926f / 180.0f;
            float x = 50 * cos(angle);
            float y = 50 + 10 * sin(angle);
            
            glColor4f(1.0f, 0.9f, 0.3f, timeOfDay * 0.8f);
            drawFilledCircle(x, y, 4, 20);
        }
        glDisable(GL_BLEND);
    }
    
    glPopMatrix();
}

// Draw Swing Ride
void drawSwingRide() {
    glPushMatrix();
    glTranslatef(200, -20, 0);
    
    float brightness = 1.0f - timeOfDay * 0.5f;
    
    glColor3f(0.5f * brightness, 0.5f * brightness, 0.5f * brightness);
    glLineWidth(6.0f);
    glBegin(GL_LINES);
    glVertex2f(0, -60);
    glVertex2f(0, 40);
    glEnd();
    
    glColor3f(0.6f * brightness, 0.3f * brightness, 0.3f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-25, 35);
    glVertex2f(25, 35);
    glVertex2f(25, 45);
    glVertex2f(-25, 45);
    glEnd();
    
    glColor3f(0.8f * brightness, 0.4f * brightness, 0.4f * brightness);
    glBegin(GL_TRIANGLES);
    glVertex2f(-30, 45);
    glVertex2f(30, 45);
    glVertex2f(0, 65);
    glEnd();
    
    glPushMatrix();
    glRotatef(swingAngle, 0, 0, 1);
    
    for (int i = 0; i < 4; i++) {
        float angle = (360.0f / 4.0f) * i;
        glPushMatrix();
        glRotatef(angle, 0, 0, 1);
        
        glColor3f(0.7f * brightness, 0.7f * brightness, 0.7f * brightness);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(0, 40);
        glVertex2f(40, 10);
        glEnd();
        
        glPushMatrix();
        glTranslatef(40, 10, 0);
        glRotatef(-swingAngle - angle, 0, 0, 1);
        
        glColor3f(0.9f * brightness, 0.8f * brightness, 0.3f * brightness);
        glBegin(GL_POLYGON);
        glVertex2f(-6, -3);
        glVertex2f(6, -3);
        glVertex2f(6, 3);
        glVertex2f(-6, 3);
        glEnd();
        
        glColor3f(0.3f * brightness, 0.3f * brightness, 0.8f * brightness);
        drawFilledCircle(0, 8, 4, 20);
        
        glPopMatrix();
        glPopMatrix();
    }
    
    glPopMatrix();
    
    if (timeOfDay > 0.3f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.5f, 0.2f, timeOfDay * 0.8f);
        drawFilledCircle(0, 50, 6, 20);
        glDisable(GL_BLEND);
    }
    
    glPopMatrix();
}

// Draw Food Cart
void drawFoodCart() {
    glPushMatrix();
    glTranslatef(-280, -80, 0);
    
    float brightness = 1.0f - timeOfDay * 0.5f;
    
    glColor3f(0.8f * brightness, 0.5f * brightness, 0.3f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-25, -10);
    glVertex2f(25, -10);
    glVertex2f(25, 15);
    glVertex2f(-25, 15);
    glEnd();
    
    glColor3f(0.9f * brightness, 0.2f * brightness, 0.2f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-30, 15);
    glVertex2f(30, 15);
    glVertex2f(25, 25);
    glVertex2f(-25, 25);
    glEnd();
    
    glColor3f(0.3f * brightness, 0.3f * brightness, 0.3f * brightness);
    drawCircle(-15, -10, 5, 20);
    drawCircle(15, -10, 5, 20);
    
    glColor3f(1.0f * brightness, 0.9f * brightness, 0.3f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-20, 5);
    glVertex2f(20, 5);
    glVertex2f(20, 12);
    glVertex2f(-20, 12);
    glEnd();
    
    glColor3f(0.9f * brightness, 0.7f * brightness, 0.2f * brightness);
    drawFilledCircle(-10, 0, 3, 20);
    drawFilledCircle(0, 0, 3, 20);
    drawFilledCircle(10, 0, 3, 20);
    
    glPushMatrix();
    glTranslatef(5, 20, 0);
    
    glColor3f(0.9f * brightness, 0.7f * brightness, 0.6f * brightness);
    drawFilledCircle(0, 15, 5, 20);
    
    glColor3f(0.4f * brightness, 0.6f * brightness, 0.8f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-4, 5);
    glVertex2f(4, 5);
    glVertex2f(4, 15);
    glVertex2f(-4, 15);
    glEnd();
    
    glColor3f(0.9f * brightness, 0.7f * brightness, 0.6f * brightness);
    glLineWidth(3.0f);
    
    glBegin(GL_LINES);
    glVertex2f(-4, 12);
    glVertex2f(-10, 8);
    glEnd();
    
    glPushMatrix();
    glTranslatef(4, 12, 0);
    glRotatef(sellerArmAngle, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(8, -4);
    glEnd();
    glPopMatrix();
    
    glColor3f(0.9f * brightness, 0.3f * brightness, 0.3f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-6, 20);
    glVertex2f(6, 20);
    glVertex2f(5, 23);
    glVertex2f(-5, 23);
    glEnd();
    
    glPopMatrix();
    
    if (timeOfDay > 0.3f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.8f, 0.3f, timeOfDay * 0.9f);
        drawFilledCircle(0, 20, 8, 20);
        glDisable(GL_BLEND);
    }
    
    glPopMatrix();
}

// Draw Entrance Gate
void drawEntranceGate() {
    glPushMatrix();
    glTranslatef(0, -200, 0);
    
    float brightness = 1.0f - timeOfDay * 0.6f;
    
    // Left pillar
    glColor3f(0.7f * brightness, 0.3f * brightness, 0.2f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-120, -100);
    glVertex2f(-100, -100);
    glVertex2f(-100, 0);
    glVertex2f(-120, 0);
    glEnd();
    
    // Right pillar
    glBegin(GL_POLYGON);
    glVertex2f(100, -100);
    glVertex2f(120, -100);
    glVertex2f(120, 0);
    glVertex2f(100, 0);
    glEnd();
    
    // Arch top
    glColor3f(0.9f * brightness, 0.7f * brightness, 0.4f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-120, 0);
    glVertex2f(-120, 20);
    glVertex2f(120, 20);
    glVertex2f(120, 0);
    glEnd();
    
    // Arch curve (decorative)
    glLineWidth(3.0f);
    glColor3f(0.8f * brightness, 0.5f * brightness, 0.3f * brightness);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float angle = 3.14159f * i / 20.0f;
        float x = -100 + 200 * (i / 20.0f);
        float y = 20 + 30 * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(1.0f);
    
    glPopMatrix();
}

// Draw Entrance Sign
void drawEntranceSign() {
    glPushMatrix();
    glTranslatef(0, -150, 0);
    
    float brightness = 1.0f - timeOfDay * 0.6f;
    
    // Sign board - bigger and more visible
    glColor3f(0.15f * brightness, 0.35f * brightness, 0.65f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-140, 0);
    glVertex2f(140, 0);
    glVertex2f(140, 50);
    glVertex2f(-140, 50);
    glEnd();
    
    // Border
    glColor3f(0.95f * brightness, 0.85f * brightness, 0.35f * brightness);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-140, 0);
    glVertex2f(140, 0);
    glVertex2f(140, 50);
    glVertex2f(-140, 50);
    glEnd();
    glLineWidth(1.0f);
    
    // Blinking lights around sign (only at night)
    if (timeOfDay > 0.3f && signLightsOn) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        
        // Colorful lights
        float colors[6][3] = {
            {1.0f, 0.2f, 0.2f}, // Red
            {0.2f, 1.0f, 0.2f}, // Green
            {0.2f, 0.3f, 1.0f}, // Blue
            {1.0f, 1.0f, 0.2f}, // Yellow
            {1.0f, 0.2f, 1.0f}, // Magenta
            {0.2f, 1.0f, 1.0f}  // Cyan
        };
        
        for (int i = 0; i < 14; i++) {
            float x = -130 + i * 20;
            int colorIdx = i % 6;
            glColor4f(colors[colorIdx][0], colors[colorIdx][1], colors[colorIdx][2], timeOfDay * 0.9f);
            drawFilledCircle(x, 55, 5, 12);
            
            // Glow
            glColor4f(colors[colorIdx][0], colors[colorIdx][1], colors[colorIdx][2], timeOfDay * 0.4f);
            drawFilledCircle(x, 55, 8, 12);
        }
        
        // Bottom lights
        for (int i = 0; i < 14; i++) {
            float x = -130 + i * 20;
            int colorIdx = (i + 3) % 6;
            glColor4f(colors[colorIdx][0], colors[colorIdx][1], colors[colorIdx][2], timeOfDay * 0.9f);
            drawFilledCircle(x, -5, 5, 12);
            
            glColor4f(colors[colorIdx][0], colors[colorIdx][1], colors[colorIdx][2], timeOfDay * 0.4f);
            drawFilledCircle(x, -5, 8, 12);
        }
        
        glDisable(GL_BLEND);
    }
    
    // "VILLAGE FAIR" text - simplified blocks for better visibility
    glColor3f(1.0f * brightness, 0.95f * brightness, 0.3f * brightness);
    
    float letterHeight = 22.0f;
    float letterY = 16.0f;
    
    // V - using triangles
    glBegin(GL_TRIANGLES);
    glVertex2f(-115, letterY + letterHeight); glVertex2f(-108, letterY); glVertex2f(-105, letterY);
    glVertex2f(-105, letterY); glVertex2f(-98, letterY + letterHeight); glVertex2f(-101, letterY + letterHeight);
    glVertex2f(-115, letterY + letterHeight); glVertex2f(-105, letterY); glVertex2f(-101, letterY + letterHeight);
    glEnd();
    
    // I
    glBegin(GL_POLYGON);
    glVertex2f(-92, letterY); glVertex2f(-87, letterY); 
    glVertex2f(-87, letterY + letterHeight); glVertex2f(-92, letterY + letterHeight);
    glEnd();
    
    // L
    glBegin(GL_POLYGON);
    glVertex2f(-81, letterY); glVertex2f(-76, letterY); 
    glVertex2f(-76, letterY + letterHeight); glVertex2f(-81, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-81, letterY); glVertex2f(-68, letterY); glVertex2f(-68, letterY + 5); glVertex2f(-81, letterY + 5);
    glEnd();
    
    // L
    glBegin(GL_POLYGON);
    glVertex2f(-62, letterY); glVertex2f(-57, letterY); 
    glVertex2f(-57, letterY + letterHeight); glVertex2f(-62, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-62, letterY); glVertex2f(-49, letterY); glVertex2f(-49, letterY + 5); glVertex2f(-62, letterY + 5);
    glEnd();
    
    // A (VILLAGE)
    glBegin(GL_POLYGON);
    glVertex2f(-43, letterY); glVertex2f(-38, letterY); 
    glVertex2f(-38, letterY + letterHeight); glVertex2f(-43, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-30, letterY); glVertex2f(-25, letterY); 
    glVertex2f(-25, letterY + letterHeight); glVertex2f(-30, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-43, letterY + letterHeight - 5); glVertex2f(-25, letterY + letterHeight - 5); 
    glVertex2f(-25, letterY + letterHeight); glVertex2f(-43, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-38, letterY + 10); glVertex2f(-30, letterY + 10); 
    glVertex2f(-30, letterY + 15); glVertex2f(-38, letterY + 15);
    glEnd();
    
    // G
    glBegin(GL_POLYGON);
    glVertex2f(-17, letterY); glVertex2f(-12, letterY); 
    glVertex2f(-12, letterY + letterHeight); glVertex2f(-17, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-17, letterY + letterHeight - 5); glVertex2f(-2, letterY + letterHeight - 5); 
    glVertex2f(-2, letterY + letterHeight); glVertex2f(-17, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-17, letterY); glVertex2f(-2, letterY); glVertex2f(-2, letterY + 5); glVertex2f(-17, letterY + 5);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-2, letterY); glVertex2f(1, letterY); glVertex2f(1, letterY + 13); glVertex2f(-2, letterY + 13);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(-8, letterY + 10); glVertex2f(1, letterY + 10); glVertex2f(1, letterY + 13); glVertex2f(-8, letterY + 13);
    glEnd();
    
    // E
    glBegin(GL_POLYGON);
    glVertex2f(8, letterY); glVertex2f(13, letterY); 
    glVertex2f(13, letterY + letterHeight); glVertex2f(8, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(8, letterY + letterHeight - 5); glVertex2f(22, letterY + letterHeight - 5); 
    glVertex2f(22, letterY + letterHeight); glVertex2f(8, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(8, letterY + 9); glVertex2f(20, letterY + 9); 
    glVertex2f(20, letterY + 13); glVertex2f(8, letterY + 13);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(8, letterY); glVertex2f(22, letterY); glVertex2f(22, letterY + 5); glVertex2f(8, letterY + 5);
    glEnd();
    
    // Space between words
    
    // F
    glBegin(GL_POLYGON);
    glVertex2f(36, letterY); glVertex2f(41, letterY); 
    glVertex2f(41, letterY + letterHeight); glVertex2f(36, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(36, letterY + letterHeight - 5); glVertex2f(50, letterY + letterHeight - 5); 
    glVertex2f(50, letterY + letterHeight); glVertex2f(36, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(36, letterY + 9); glVertex2f(48, letterY + 9); 
    glVertex2f(48, letterY + 13); glVertex2f(36, letterY + 13);
    glEnd();
    
    // A (FAIR)
    glBegin(GL_POLYGON);
    glVertex2f(56, letterY); glVertex2f(61, letterY); 
    glVertex2f(61, letterY + letterHeight); glVertex2f(56, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(69, letterY); glVertex2f(74, letterY); 
    glVertex2f(74, letterY + letterHeight); glVertex2f(69, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(56, letterY + letterHeight - 5); glVertex2f(74, letterY + letterHeight - 5); 
    glVertex2f(74, letterY + letterHeight); glVertex2f(56, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(61, letterY + 10); glVertex2f(69, letterY + 10); 
    glVertex2f(69, letterY + 15); glVertex2f(61, letterY + 15);
    glEnd();
    
    // I
    glBegin(GL_POLYGON);
    glVertex2f(80, letterY); glVertex2f(85, letterY); 
    glVertex2f(85, letterY + letterHeight); glVertex2f(80, letterY + letterHeight);
    glEnd();
    
    // R - improved with clearer diagonal
    glBegin(GL_POLYGON);
    glVertex2f(91, letterY); glVertex2f(96, letterY); 
    glVertex2f(96, letterY + letterHeight); glVertex2f(91, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(91, letterY + letterHeight - 5); glVertex2f(107, letterY + letterHeight - 5); 
    glVertex2f(107, letterY + letterHeight); glVertex2f(91, letterY + letterHeight);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(91, letterY + 9); glVertex2f(107, letterY + 9); 
    glVertex2f(107, letterY + 13); glVertex2f(91, letterY + 13);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(103, letterY + 9); glVertex2f(107, letterY + 9); 
    glVertex2f(107, letterY + letterHeight - 5); glVertex2f(103, letterY + letterHeight - 5);
    glEnd();
    // Diagonal leg of R - thick and clear
    glLineWidth(5.0f);
    glBegin(GL_LINES);
    glVertex2f(96, letterY + 9);
    glVertex2f(108, letterY);
    glEnd();
    glLineWidth(1.0f);
    
    glPopMatrix();
}

// Draw a person (stick figure style)
void drawPerson(float x, float y, float scale, float r, float g, float b, float legPhase) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);
    
    float brightness = 1.0f - timeOfDay * 0.6f;
    
    // Head
    glColor3f(0.9f * brightness, 0.7f * brightness, 0.6f * brightness);
    drawFilledCircle(0, 20, 4, 12);
    
    // Body (colored shirt)
    glColor3f(r * brightness, g * brightness, b * brightness);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(0, 16);
    glVertex2f(0, 5);
    glEnd();
    
    // Arms
    glBegin(GL_LINES);
    glVertex2f(0, 14);
    glVertex2f(-5, 10);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0, 14);
    glVertex2f(5, 10);
    glEnd();
    
    // Legs (animated walking)
    glBegin(GL_LINES);
    glVertex2f(0, 5);
    glVertex2f(-3 * cos(legPhase), -5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0, 5);
    glVertex2f(3 * cos(legPhase), -5);
    glEnd();
    
    glLineWidth(1.0f);
    glPopMatrix();
}

// Draw all people
void drawPeople() {
    for (int i = 0; i < NUM_PEOPLE; i++) {
        if (people[i].isWalking) {
            float legPhase = people[i].x * 0.1f; // Walking animation based on position
            drawPerson(people[i].x, people[i].y, people[i].scale, 
                      people[i].r, people[i].g, people[i].b, legPhase);
        }
    }
}

// Draw ground
void drawGround() {
    float grassR = 0.2f * (1.0f - timeOfDay * 0.7f);
    float grassG = 0.6f * (1.0f - timeOfDay * 0.7f);
    float grassB = 0.2f * (1.0f - timeOfDay * 0.5f);
    glColor3f(grassR, grassG, grassB);
    glBegin(GL_POLYGON);
    glVertex2f(-400, -300);
    glVertex2f(400, -300);
    glVertex2f(400, -100);
    glVertex2f(-400, -100);
    glEnd();
}

// Draw stalls
void drawStalls() {
    float brightness = 1.0f - timeOfDay * 0.6f;
    
    glColor3f(0.8f * brightness, 0.6f * brightness, 0.4f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(-350, -100);
    glVertex2f(-250, -100);
    glVertex2f(-250, -20);
    glVertex2f(-350, -20);
    glEnd();
    
    if (timeOfDay > 0.3f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.8f, 0.3f, timeOfDay * 0.7f);
        drawFilledCircle(-320, -40, 8, 20);
        drawFilledCircle(-280, -40, 8, 20);
        glDisable(GL_BLEND);
    }
    
    glColor3f(0.6f * brightness, 0.2f * brightness, 0.2f * brightness);
    glBegin(GL_TRIANGLES);
    glVertex2f(-360, -20);
    glVertex2f(-240, -20);
    glVertex2f(-300, 20);
    glEnd();
    
    glColor3f(0.7f * brightness, 0.5f * brightness, 0.6f * brightness);
    glBegin(GL_POLYGON);
    glVertex2f(250, -100);
    glVertex2f(350, -100);
    glVertex2f(350, -20);
    glVertex2f(250, -20);
    glEnd();
    
    if (timeOfDay > 0.3f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.8f, 0.3f, timeOfDay * 0.7f);
        drawFilledCircle(280, -40, 8, 20);
        drawFilledCircle(320, -40, 8, 20);
        glDisable(GL_BLEND);
    }
    
    glColor3f(0.5f * brightness, 0.3f * brightness, 0.1f * brightness);
    glBegin(GL_TRIANGLES);
    glVertex2f(240, -20);
    glVertex2f(360, -20);
    glVertex2f(300, 20);
    glEnd();
}

// Display
void display() {
    float skyR, skyG, skyB;
    getSkyColor(skyR, skyG, skyB);
    glClearColor(skyR, skyG, skyB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawStars();
    drawSun();
    drawMoon();
    drawFireworks();
    drawGround();
    drawEntranceGate();      // NEW: Entrance gate
    drawEntranceSign();       // NEW: Sign with blinking lights
    drawPeople();             // NEW: Animated crowd
    drawStalls();
    drawFoodCart();
    drawCarousel();
    drawSwingRide();
    drawSupport();
    drawFerrisWheel();
    
    glutSwapBuffers();
}

// Timer
void timer(int value) {
    ferrisWheelAngle += 0.5f;
    if (ferrisWheelAngle > 360.0f) ferrisWheelAngle -= 360.0f;
    
    carouselAngle += 0.8f;
    if (carouselAngle >= 360.0f) carouselAngle -= 360.0f;
    
    for (int i = 0; i < 6; i++) {
        horseUpDown[i] = 8.0f * sin((carouselAngle + horsePhase[i]) * 3.1415926f / 180.0f);
    }
    
    swingAngle += swingDirection * 1.2f;
    if (swingAngle >= MAX_SWING_ANGLE || swingAngle <= -MAX_SWING_ANGLE) {
        swingDirection = -swingDirection;
    }
    
    sellerArmAngle += sellerArmDirection * 2.0f;
    if (sellerArmAngle >= 45.0f || sellerArmAngle <= -10.0f) {
        sellerArmDirection = -sellerArmDirection;
    }
    
    fireworkTimer++;
    float fireworkFrequency = (timeOfDay > 0.5f) ? 80.0f : 200.0f;
    
    if (fireworkTimer >= nextFireworkTime) {
        if (timeOfDay > 0.4f || (rand() % 100 < 20)) {
            launchFirework();
        }
        fireworkTimer = 0.0f;
        nextFireworkTime = fireworkFrequency + (rand() % 100);
    }
    
    updateFireworks();
    
    // Update sign blinking
    signBlinkTimer += 1.0f;
    if (signBlinkTimer >= 30.0f) {
        signLightsOn = !signLightsOn;
        signBlinkTimer = 0.0f;
    }
    
    // Update people walking
    for (int i = 0; i < NUM_PEOPLE; i++) {
        if (people[i].isWalking) {
            people[i].x += people[i].direction * people[i].speed;
            
            // Wrap around screen
            if (people[i].x > 400.0f) {
                people[i].x = -400.0f;
            } else if (people[i].x < -400.0f) {
                people[i].x = 400.0f;
            }
        }
    }
    
    if (isTransitioning) {
        timeOfDay += transitionSpeed;
        if (timeOfDay >= 1.0f) {
            timeOfDay = 1.0f;
            transitionSpeed = -transitionSpeed;
        } else if (timeOfDay <= 0.0f) {
            timeOfDay = 0.0f;
            transitionSpeed = -transitionSpeed;
        }
    }
    
    sunAngle += 0.1f;
    if (sunAngle >= 360.0f) sunAngle -= 360.0f;
    
    moonAngle = sunAngle + 180.0f;
    if (moonAngle >= 360.0f) moonAngle -= 360.0f;
    
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].brightness += stars[i].twinkleSpeed;
        if (stars[i].brightness > 1.0f || stars[i].brightness < 0.5f) {
            stars[i].twinkleSpeed = -stars[i].twinkleSpeed;
        }
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// Keyboard
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27:
            exit(0);
            break;
        case 'n':
        case 'N':
            isTransitioning = !isTransitioning;
            break;
        case 'd':
        case 'D':
            timeOfDay = 0.0f;
            isTransitioning = false;
            break;
        case 't':
        case 'T':
            timeOfDay = 1.0f;
            isTransitioning = false;
            break;
        case ' ':
            isTransitioning = true;
            break;
        case 'f':
        case 'F':
            launchFirework();
            printf("🎆 Firework launched!\n");
            break;
    }
    glutPostRedisplay();
}

// Main
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Village Fair - Ferris Wheel");
    
    init();
    
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}
