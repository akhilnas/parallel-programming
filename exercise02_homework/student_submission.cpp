#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <complex>
#include <pthread.h>

#include "mandelbrot_set.h"

#define CHANNELS 3
// Returns the one dimensional index into our pseudo 3D array
#define OFFSET(y, x, c) (y * arg->x_resolution * CHANNELS + x * CHANNELS + c)

/*
 * TODO@Students: This is your kernel. Take a look at any dependencies and decide how to parallelize it.
 */

struct pthread_args
{
 int x_resolution; 
 int y_resolution; 
 int max_iter;	                
 double view_x0; 
 double view_x1; 
 double view_y0; 
 double view_y1;
 double x_stepsize; 
 double y_stepsize;
 int palette_shift; 
 void* image; 
 double power;
 bool no_output;

 int start_point;
 int end_point;

 int pointsInSetCount;

 }; 

void* kernel(void* args) {

    struct pthread_args *arg = (struct pthread_args*) args;

 	// Type Casting
 	unsigned char (*image_local)[arg->x_resolution][3] = ((unsigned char (*)[arg->x_resolution][3]) arg->image); 	

    using namespace std::complex_literals;

    double y;
    double x;

    std::complex<double> Z;
    std::complex<double> C;

    int k;

    int pointsInSetCount = 0;

    // For each pixel in the image
    for (int i = arg->start_point; i < arg->end_point; i++) {
        for (int j = 0; j < arg->x_resolution; j++) {
            y = arg->view_y1 - i * arg->y_stepsize;
            x = arg->view_x0 + j * arg->x_stepsize;

            Z = 0.0 + 0.0i;
            C = x + y * 1.0i;

            k = 0;

            // Apply the Mandelbrot calculation until the absolute value >= 2 (meaning the calculation will diverge to
            // infinity) or the maximum number of iterations was reached.
            do {
                Z = std::pow(Z, arg->power) + C;
                k++;
            } while (std::abs(Z) < 2 && k < arg->max_iter);

            // If the maximum number of iterations was reached then this point is in the Mandelbrot set and we color it
            // black. Else, it is outside and we color it with a color that corresponds to how many iterations there
            // were before we confirmed the divergence.
            if (k == arg->max_iter) {
                pointsInSetCount++;
            }

            if(!arg->no_output) {
                if (k == arg->max_iter) {
                    memcpy(image_local[OFFSET(i, j, 0)], black, 3);
                } else {
                    int index = (k + arg->palette_shift) % (sizeof(colors) / sizeof(colors[0]));
                    memcpy(image_local[OFFSET(i, j, 0)], colors[index], 3);
                }
            }
        }
    }

    arg->pointsInSetCount = pointsInSetCount;
    return NULL;
}


int main(int argc, char **argv) {
    struct timespec begin, end;
    /*
     * TODO@Students: Decide how many threads will get you an appropriate speedup.
     */
    
    double power = 1.0;
    int max_iter = 50; // sizeof(colors);
    int x_resolution = 256;
    int y_resolution = 256;
    int palette_shift = 0;
    double view_x0 = -2;
    double view_x1 = +2;
    double view_y0 = -2;
    double view_y1 = +2;
    char file_name[256] = "mandelbrot.ppm";
    int no_output = 0;

    double x_stepsize;
    double y_stepsize;

    // This option parsing is not very interesting.
    int c;
    while ((c = getopt(argc, argv, "t:p:i:r:v:n:f:")) != -1) {
        switch (c) {
            case 'p':
                if (sscanf(optarg, "%d", &palette_shift) != 1)
                    goto error;
                break;

            case 'i':
                if (sscanf(optarg, "%d", &max_iter) != 1)
                    goto error;
                break;

            case 'r':
                if (sscanf(optarg, "%dx%d", &x_resolution, &y_resolution) != 2)
                    goto error;
                break;
            case 'n':
                if (sscanf(optarg, "%d", &no_output) != 1)
                    goto error;
                break;
            case 'f':
                strncpy(file_name, optarg, sizeof(file_name) - 1);
                break;

            case '?':
            error:
                fprintf(stderr,
                        "Usage:\n"
                        "-i \t maximum number of iterations per pixel\n"
                        "-r \t image resolution to be computed\n"
                        "-p \t shift palette to change colors\n"
                        "-f \t output file name\n"
                        "-n \t no output(default: 0)\n"
                        "\n"
                        "Example:\n"
                        "%s -t 1 -r 720x480 -i 5000 -f mandelbrot.ppm\n",
                        argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    FILE *file = nullptr;
    if (!no_output) {
        if ((file = fopen(file_name, "w")) == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }

    // This is a pseudo 3d (1d storage) array that can be accessed using the OFFSET Macro.
    auto *image = static_cast<unsigned char *>(malloc(x_resolution * y_resolution * sizeof(unsigned char[3])));


    // Calculate the step size.
    x_stepsize = (view_x1 - view_x0) / (1.0 * x_resolution);
    y_stepsize = (view_y1 - view_y0) / (1.0 * y_resolution);

    if(x_stepsize <= 0 || y_stepsize <= 0) {
        printf("Please specify a valid range.\n");
        exit(-1);
    }

    // Don't modify anything that prints to stdout.
    // The output on standard out needs to be the same as in the sequential implementation.
    fprintf(stderr,
            "Following settings are used for computation:\n"
            "Max. iterations: %d\n"
            "Resolution: %dx%d\n"
            "View frame: [%lf,%lf]x[%lf,%lf]\n"
            "Stepsize x = %lf y = %lf\n", max_iter, x_resolution,
            y_resolution, view_x0, view_x1, view_y0, view_y1, x_stepsize,
            y_stepsize);

    if (!no_output)
        fprintf(stderr, "Output file: %s\n", file_name);
    else
        fprintf(stderr, "No output will be written\n");

    getProblemFromInput(&power);
    printf("Check");

    /// Perosonal Code

    int num_threads = 8;
    int numSamplesInSet = 0;

    printf("Check");
    pthread_t *threads = (pthread_t*) malloc (num_threads *sizeof(pthread_t));
    printf("Check");

	// Creating Local copy of Variables for each of the threads
	struct pthread_args* args = (struct pthread_args*) malloc (num_threads*sizeof (struct pthread_args));

	for (int i = 0; i < num_threads; ++i) {
 		// Allocating Local Variables for each of the threads		
 		args[i].y_resolution = y_resolution;
 		args[i].max_iter = max_iter;
 		args[i].x_stepsize = x_stepsize;
 		args[i].y_stepsize = y_stepsize;
 		args[i].palette_shift = palette_shift;
 		args[i].x_resolution = x_resolution;
 		args[i].view_x0 = view_x0;
 		args[i].view_y0 = view_y0;
 		args[i].view_y1 = view_y1;
 		args[i].view_x1 = view_x1;
 		args[i].image   = image; 	
        args[i].power = power;
        args[i].no_output = no_output;
        args[i].pointsInSetCount = numSamplesInSet;

        // Temporary Variable fwf
 		int y_res = y_resolution - (y_resolution%num_threads);
 		// Start Point & End-Point of Calculation for Each Thread
 		args[i].start_point =  (i * ((y_res/num_threads)));
 		args[i].end_point   =  args[i].start_point + y_res/num_threads;
 		if (i == num_threads-1){
 			args[i].end_point   =  args[i].start_point + y_res/num_threads + (y_resolution%num_threads);
 		}
 		printf("Check");
 		// Pass the args as argument to the threads
        clock_gettime(CLOCK_MONOTONIC, &begin);
 		pthread_create(&threads[i], NULL, kernel, args+i); // passing args[i] to threads[i]
        clock_gettime(CLOCK_MONOTONIC, &end);

        numSamplesInSet += args[i].pointsInSetCount;


 	}
    for (int i = 0; i < num_threads; ++i) {
 		pthread_join(threads[i], NULL);
 	}

    free(threads);	
    free(args);
    ///


    
    
    outputSolution(numSamplesInSet);

    if (!no_output) {
        if (fprintf(file, "P6\n%d %d\n%d\n", x_resolution, y_resolution, 255) < 0) {
            std::perror("fprintf");
            exit(EXIT_FAILURE);
        }
        size_t bytes_written = fwrite(image, 1,
                                      x_resolution * y_resolution * sizeof(unsigned char[3]), file);
        if (bytes_written < x_resolution * y_resolution * sizeof(char[3])) {
            std::perror("fwrite");
            exit(EXIT_FAILURE);
        }
//        for (int i = 0; i < y_resolution; ++i) {
//            for (int j = 0; j < x_resolution; ++j) {
//                fprintf(file, "%i %i %i ", image[OFFSET(i, j, 0)], image[OFFSET(i, j, 1)], image[OFFSET(i, j, 2)]);
//            }
//            fprintf(file, "\n");
//        }
        fclose(file);
    }

    // This is just here for convenience, it isn't actually used to measure your runtime
    fprintf(stderr, "\n\nTime: %.5f seconds\n", ((double) end.tv_sec + 1.0e-9 * end.tv_nsec) -
                                                ((double) begin.tv_sec + 1.0e-9 * begin.tv_nsec));

    free(image);
    return 0;
}
