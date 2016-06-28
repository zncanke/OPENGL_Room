#ifndef __Nurbs__
#define __Nurbs__

#include <stdio.h>

class Nurbs {
private:
    GLUnurbsObj *theNurb;
    GLfloat ctlpoints[4][4][3];
    GLfloat tcoords[2][2][2]={0, 0, 0, 1, 1, 0, 1, 1};
public:
    void init();
    void drawNurbs();
};

#endif /* defined(__Nurbs__) */