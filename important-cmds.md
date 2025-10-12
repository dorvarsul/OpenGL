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
