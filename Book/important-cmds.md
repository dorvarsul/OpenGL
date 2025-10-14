# Chapter 2
-glOrtho(left, right, top, bottom, near, far); : Defines the virtual viewerbox border properties
-glBegin() / glEnd();
-glVertex3f(x, y, z);
-glutInitWindowSize(w, h); : Sets the size of the OpenGL window to width w and height h measued in pixels.
-glutInitWindowPosition(x, y); : Specify the loation of the upper left orner of hte OpenGL window on the computer screen.
-glColor3f(r, g, b); : Color applied to objects being drawn
-glClearColor(r,g,b, alpha); : Specifies the background color (clearing color)
-glClear(GL COLOR BUFFER BIT); : Clears the window the specified background color

### OpenGL Geometric Primitives
**glBegin Parameters:**
-GL_POINTS: draws a point at each vertex
-GL_LINES: draws a disconnected sequence of straight line segments between the vertices, taken two at a time.
-GL_LINE_STRIP: draws the connected sequence of segments
-GL_LINE_LOOP: same as line strip but drawn to complete a loop
-GL_TRIANGLES: draws a sequence of triangles using the vertices three at a time. (if n is not a multiple of 3, last one or two vertices are ignored)
-GL_TRIANGLE_STRIP: draws a sequence of triangles as a sliding window picking the vertices of triangles in a strip
-GL_TRIANGLE_FAN: draws a sequence of triangles around the first vertex as follows: v0v1v2, v0v2v3, v0vn-2vn-1

**standalone call**
-glRectf(x1,y1,x2,y2): draws a rectangle with a list of vertices: [(x1,y1,0), (x2,y1,0), (x1,y2,0), (x2,y2,0)]

**glPolygonMode(face, mode):**
-face: GL_FRONT / GL_BACK / GL_FRONT_AND_BACK
-mode: GL_FILL / GL_LINE / GL_POINT

**Syntax of hidden surface removal:**
-glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) causes the depth buffer to be cleared
-glEnable(GL_DEPTH_TEST) turns hidden surface removal on. glDisable(...) turns it off.
-glutInitDisplayMode(GLUT SINGLE | GLUT RGB | GLUT DEPTH) causes the depth buffer to be initialized
