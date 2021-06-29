#include <stdio.h>
#include <math.h>
#include <string.h>

/* Program that calculates electric field gradients or electric field vector fields by reading
data from a file and creating as many vector fields or gradients as there are charged particles and finally
adds them together to form a single vector field or gradient, that is then written to a file.
Version: 1.0b
Date: 29/06/2021
*/

struct electric_field_point {
    double x_pos, y_pos, e_x, e_y, e;
};

struct charged_particle {
    double x_pos, y_pos, charge;
};

const double M_0 = 4*M_PI*pow(10, -7);
const double EPSILON_0 = 8.8541878128*pow(10, -12);

struct electric_field_point calculate_electric_field_point(struct electric_field_point point, struct charged_particle particle, double relative_permittivity, double ignore_precision) {
    double r_x, r_y, r;
    r_x = fabs(point.x_pos - particle.x_pos);
    r_y = fabs(point.y_pos - particle.y_pos);
    r = sqrt(r_x*r_x+r_y*r_y);
    if(r<=ignore_precision) {
        point.e = 0;
        point.e_x = 0;
        point.e_y = 0;
        return point;
    }
    point.e = particle.charge/(4*M_PI*EPSILON_0*relative_permittivity*r*r);
    double alpha = atanf(r_y/r_x);
    point.e_x = fabs(point.e*cos(alpha));
    point.e_y = fabs(point.e*sin(alpha));
    if(point.x_pos<particle.x_pos&&particle.charge>0) {
        point.e_x = -1*fabs(point.e_x);
    }
    else if(point.x_pos>particle.x_pos&&particle.charge<0) {
        point.e_x = -1*fabs(point.e_x);
    }
    if(point.y_pos<particle.y_pos&&particle.charge>0) {
        point.e_y = -1*fabs(point.e_y);
    }
    else if(point.y_pos>particle.y_pos&&particle.charge<0) {
        point.e_y = -1*fabs(point.e_y);
    }
    return point;
};

struct electric_field_point add_electric_fields(struct electric_field_point point_1, struct electric_field_point point_2) {
    struct electric_field_point result_point;
    result_point.x_pos = point_1.x_pos;
    result_point.y_pos = point_1.y_pos;
    result_point.e = point_1.e+point_2.e;
    result_point.e_x = point_1.e_x+point_2.e_x;
    result_point.e_y = point_1.e_y+point_2.e_y;
    return result_point;
};

int main(int argc, char** argv) {
    int size, n;
    double step, relative_permittivity, ignore_precision;
    FILE *in_file, *out_file;
    if(!(in_file = fopen(argv[1], "rb"))) {
       perror(argv[1]);
       return 0;
    }
    in_file = fopen(argv[1], "rb");
    out_file = fopen(argv[2], "wb");
    fscanf(in_file, "%i %lf %lf %lf %i", &size, &step, &relative_permittivity, &ignore_precision, &n);
    double current_position_x = -1*size*step, current_position_y = size*step;
    struct electric_field_point electric_field_1[size*2+1][size*2+1], electric_field_2[size*2+1][size*2+1];
    struct electric_field_point aux_field[size*2+1][size*2+1];
    struct charged_particle particles[n];
    for(int i=0; i<n; i++) {
        fscanf(in_file, "%lf %lf %lf", &particles[i].x_pos, &particles[i].y_pos, &particles[i].charge);
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            aux_field[j][i].x_pos = electric_field_2[j][i].x_pos = electric_field_1[j][i].x_pos = current_position_x;
            aux_field[j][i].y_pos = electric_field_2[j][i].y_pos = electric_field_1[j][i].y_pos = current_position_y;
            current_position_x += step;
        }
        current_position_x = -1*size*step;
        current_position_y -= step;
    }
    if(n==1) {
        for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            aux_field[j][i] = calculate_electric_field_point(aux_field[j][i], particles[0], relative_permittivity, ignore_precision);
        }
    }
    }
    else {
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            electric_field_1[j][i] = calculate_electric_field_point(electric_field_1[j][i], particles[0], relative_permittivity, ignore_precision);
            electric_field_2[j][i] = calculate_electric_field_point(electric_field_2[j][i], particles[1], relative_permittivity, ignore_precision);
        }
    }
    for(int q=2; q<n+1; q++) {
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            aux_field[j][i] = add_electric_fields(electric_field_1[j][i], electric_field_2[j][i]);
        }
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            electric_field_1[j][i].e = aux_field[j][i].e;
            electric_field_1[j][i].e_x = aux_field[j][i].e_x;
            electric_field_1[j][i].e_y = aux_field[j][i].e_y;
        }
    }
    for(int i=0; i<size*2+1; i++) {
        for(int j=0; j<size*2+1; j++) {
            electric_field_2[j][i] = calculate_electric_field_point(electric_field_2[j][i], particles[q], relative_permittivity, ignore_precision);
        }
    }
    }
    }
    if(!strcmp(argv[3], "-g")) {
    for(int i=0; i<size*2+1; i++) {
    for(int j=0; j<size*2+1; j++) {
        fprintf(out_file, "%0.2lf %0.2lf %0.12lf\n", aux_field[j][i].x_pos, aux_field[j][i].y_pos, aux_field[j][i].e);
    }
    }
    printf("Electric field gradient written to %s successfully.", argv[2]);
    }
    else if(!strcmp(argv[3], "-vf")) {
    for(int i=0; i<size*2+1; i++) {
    for(int j=0; j<size*2+1; j++) {
        fprintf(out_file, "%0.2lf %0.2lf %0.12lf %0.12lf\n", aux_field[j][i].x_pos, aux_field[j][i].y_pos, aux_field[j][i].e_x, aux_field[j][i].e_y);
    }
    }
    printf("Electric vector field written to %s successfully.", argv[2]);
    }
    else {
        printf("Unknown command.");
    }
    fclose(in_file);
    fclose(out_file);
return 0;
}
