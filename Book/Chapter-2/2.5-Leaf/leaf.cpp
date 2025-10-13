#include <GL/glew.h>
#include <GL/freeglut.h>

int numSteps = 1;

float computeBezier(float p0, float p1, float p2, float t) {
  float u = 1 - t;
  return u * u * p0 + 2 * u * t * p1 + t * t * p2;
};

void drawBezierCurve(float p0[], float p1[], float p2[]) {
  glBegin(GL_LINE_STRIP);
  for (int i = 0; i <= numSteps; ++i) {
    float t = (float)i / numSteps;
    float x = computeBezier(p0[0], p1[0], p2[0], t);
    float y = computeBezier(p0[1], p1[1], p2[1], t);
    glVertex3f(x, y, 0.0f);
  }
  glEnd();
}

void drawScene(void) {
  float p1[] = {50.0, 10.0, 0.0};
  float p2[] = {40.0, 20.0, 0.0};
  float p3[] = {40.0, 30.0, 0.0};
  float p4[] = {28.0, 38.0, 0.0};
  float p5[] = {25.0, 50.0, 0.0};
  float p6[] = {20.0, 65.0, 0.0};
  float p7[] = {40.0, 90.0, 0.0};
  float p8[] = {60.0, 75.0, 0.0};
  float p9[] = {65.0, 60.0, 0.0};
  float p10[] = {70.0, 40.0, 0.0};
  float p11[] = {46.0, 40.0, 0.0};
  float p12[] = {52.0, 50.0, 0.0};
  float p13[] = {56.0, 60.0, 0.0};

  float x, y, t;
  glClear(GL_COLOR_BUFFER_BIT);
  glLineWidth(3);
  glColor3f(0.0, 1.0, 0.0);

  drawBezierCurve(p1, p2, p3);
  drawBezierCurve(p3, p4, p5);
  drawBezierCurve(p5, p6, p7);
  drawBezierCurve(p7, p8, p9);
  drawBezierCurve(p9, p10, p3);
  drawBezierCurve(p3, p11, p12);
  drawBezierCurve(p12, p13, p7);
  glFlush();
}

void setup(void) { glClearColor(1.0, 1.0, 1.0, 0.0); }

void resize(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyInput(unsigned char key, int x, int y) {
  switch (key) {
  case 27:
    exit(0);
    break;
  case '+':
    numSteps++;
    glutPostRedisplay();
    break;
  case '-':
    if (numSteps > 1)
      numSteps--;
    glutPostRedisplay();
    break;
  default:
    break;
  }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  glutInitContextVersion(4, 3);
  glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("leaf.cpp");
  glutDisplayFunc(drawScene);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyInput);

  glewExperimental = GL_TRUE;
  glewInit();

  setup();

  glutMainLoop();
}
