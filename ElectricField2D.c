#include <stdio.h>
#include <math.h>
#include <string.h>

/* Program that calculates electric field gradients or electric field vector fields by reading
data from a file and creating as many vector fields or gradients as there are charged particles and finally
adds them together to form a single vector field or gradient, that is then written to a file.
Version: 1.1
Date: 22/08/2021
*/

struct electricFieldPoint {
    double xPos, yPos, eX, eY, e;
};

struct chargedParticle {
    double xPos, yPos, charge;
};

const double M_0 = 4*M_PI*pow(10, -7);
const double EPSILON_0 = 8.8541878128*pow(10, -12);

struct electricFieldPoint calculateElectricFieldPoint(struct electricFieldPoint point, struct chargedParticle particle, double relativePermittivity, double ignorePrecision) {
    double r_x, r_y, r;
    r_x = fabs(point.xPos - particle.xPos);
    r_y = fabs(point.yPos - particle.yPos);
    r = sqrt(r_x*r_x+r_y*r_y);
    if(r<=ignorePrecision) {
        point.e = 0;
        point.eX = 0;
        point.eY = 0;
        return point;
    }
    point.e = particle.charge/(4*M_PI*EPSILON_0*relativePermittivity*r*r);
    double alpha = atanf(r_y/r_x);
    point.eX = fabs(point.e*cos(alpha));
    point.eY = fabs(point.e*sin(alpha));
    if(point.xPos<particle.xPos&&particle.charge>0) {
        point.eX = -1*fabs(point.eX);
    }
    else if(point.xPos>particle.xPos&&particle.charge<0) {
        point.eX = -1*fabs(point.eX);
    }
    if(point.yPos<particle.yPos&&particle.charge>0) {
        point.eY = -1*fabs(point.eY);
    }
    else if(point.yPos>particle.yPos&&particle.charge<0) {
        point.eY = -1*fabs(point.eY);
    }
    return point;
};

struct electricFieldPoint addElectricFields(struct electricFieldPoint point1, struct electricFieldPoint point2) {
    struct electricFieldPoint resultPoint;
    resultPoint.xPos = point1.xPos;
    resultPoint.yPos = point1.yPos;
    resultPoint.e = point1.e+point2.e;
    resultPoint.eX = point1.eX+point2.eX;
    resultPoint.eY = point1.eY+point2.eY;
    return resultPoint;
};

int main(int argc, char** argv) {
    int size, n;
    double step, relativePermittivity, ignorePrecision;
    if(!strcmp("-h", argv[1])) {
        printf("You can view help at the GitHub page of this program:\n");
        printf("https://github.com/TheNumber5/electric-field-2d\n");
        printf("Version: v1.1\nDate: 22/08/2021");
        return 0;
    }
    FILE *inputFile, *outputFile;
    if(!(inputFile = fopen(argv[2], "rb"))) {
       perror(argv[2]);
       return 0;
    }
    inputFile = fopen(argv[2], "rb");
    outputFile = fopen(argv[3], "wb");
    fscanf(inputFile, "%i %lf %lf %lf %i", &size, &step, &relativePermittivity, &ignorePrecision, &n);
    double currentPositionX = -1*size*step, currentPositionY = size*step;
    struct electricFieldPoint electricField1[size*2+1][size*2+1], electricField2[size*2+1][size*2+1];
    struct electricFieldPoint auxField[size*2+1][size*2+1];
    struct chargedParticle particles[n];
    for(int i=0; i<n; i++) {
        fscanf(inputFile, "%lf %lf %lf", &particles[i].xPos, &particles[i].yPos, &particles[i].charge);
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            auxField[j][i].xPos = electricField2[j][i].xPos = electricField1[j][i].xPos = currentPositionX;
            auxField[j][i].yPos = electricField2[j][i].yPos = electricField1[j][i].yPos = currentPositionY;
            currentPositionX += step;
        }
        currentPositionX = -1*size*step;
        currentPositionY -= step;
    }
    if(n==1) {
        for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            auxField[j][i] = calculateElectricFieldPoint(auxField[j][i], particles[0], relativePermittivity, ignorePrecision);
        }
    }
    }
    else {
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            electricField1[j][i] = calculateElectricFieldPoint(electricField1[j][i], particles[0], relativePermittivity, ignorePrecision);
            electricField2[j][i] = calculateElectricFieldPoint(electricField2[j][i], particles[1], relativePermittivity, ignorePrecision);
        }
    }
    for(int q=2; q<n+1; q++) {
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            auxField[j][i] = addElectricFields(electricField1[j][i], electricField2[j][i]);
        }
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            electricField1[j][i].e = auxField[j][i].e;
            electricField1[j][i].eX = auxField[j][i].eX;
            electricField1[j][i].eY = auxField[j][i].eY;
        }
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            electricField2[j][i] = calculateElectricFieldPoint(electricField2[j][i], particles[q], relativePermittivity, ignorePrecision);
        }
    }
    }
    }
    if(!strcmp(argv[1], "-g")) {
    for(int i=0; i<size*2+1; i++) {
    for(int j=0; j<size*2+1; j++) {
        fprintf(outputFile, "%0.2lf %0.2lf %0.12lf\n", auxField[j][i].xPos, auxField[j][i].yPos, auxField[j][i].e);
    }
    }
    printf("Electric field gradient written to %s successfully.", argv[3]);
    }
    else if(!strcmp(argv[1], "-vf")) {
    for(int i=0; i<size*2+1; i++) {
    for(int j=0; j<size*2+1; j++) {
        fprintf(outputFile, "%0.2lf %0.2lf %0.12lf %0.12lf\n", auxField[j][i].xPos, auxField[j][i].yPos, auxField[j][i].eX, auxField[j][i].eY);
    }
    }
    printf("Electric vector field written to %s successfully.", argv[3]);
    }
    else {
        printf("Unknown command.");
    }
    fclose(inputFile);
    fclose(outputFile);
return 0;
}
