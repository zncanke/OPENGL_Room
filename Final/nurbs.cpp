#include <cstdlib>
#include "nurbs.h"

void Nurbs::init() {
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++) {
            ctlpoints[i][j][0]=0.95f*(i-1.5f);
            ctlpoints[i][j][1]=0.95f*(j-1.5f);
            ctlpoints[i][j][2]=((rand()%1000)/1000.0f-0.5f)*2.0f;
        }

    theNurb=gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 10.0);
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    
    glPointSize(5.0);
    glLineWidth(2.0);
}

void Nurbs::drawNurbs() {
    GLfloat tknots[4]={0.0, 0.0, 1.0, 1.0};
    GLfloat knots[8]={0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    glPushMatrix();
    glRotatef(0,1.0,1.0,0.0);
    gluBeginSurface(theNurb);
//    gluNurbsSurface(theNurb, 4, tknots, 4, tknots, 2*2, 2, &tcoords[0][0][0], 2, 2, GL_MAP2_TEXTURE_COORD_2);
    gluNurbsSurface(theNurb, 8, knots, 8, knots, 4*3, 3, &ctlpoints[0][0][0], 4, 4, GL_MAP2_VERTEX_3);

    gluEndSurface(theNurb);
    
    glPopMatrix();
}