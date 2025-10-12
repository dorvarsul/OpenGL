#include <GL/glew.h>
#include <GL/freeglut.h>

// Drawing routine.
void drawScene(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(1.0, 0.0, 0.0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_TRIANGLE_STRIP);
  glVertex3f(80.0, 80.0, 0.0);
  glVertex3f(60.0, 70.0, 0.0);
  glVertex3f(20.0, 80.0, 0.0);
  glVertex3f(30.0, 70.0, 0.0);
  glVertex3f(20.0, 20.0, 0.0);
  glVertex3f(30.0, 30.0, 0.0);
  glVertex3f(80.0, 20.0, 0.0);
  glVertex3f(60.0, 30.0, 0.0);
  glEnd();

  glFlush();
}

// Initialization routine.
void setup(void) { glClearColor(1.0, 1.0, 1.0, 0.0); }

// OpenGL window reshape routine.
void resize(int w, int h) {
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Set up virtual viewerbox where the programmer can draw scenes
  // --glOrtho(Left, Right, Bottom, Top, Near, Far)
  glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y) {
  switch (key) {
  case 27:
    exit(0);
    break;
  default:
    break;
  }
}

// Main routine.
int main(int argc, char **argv) {
  glutInit(&argc, argv);

  glutInitContextVersion(4, 3);
  glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);

  glutCreateWindow("square.cpp");

  glutDisplayFunc(drawScene);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyInput);

  glewExperimental = GL_TRUE;
  glewInit();

  setup();

  glutMainLoop();
}
