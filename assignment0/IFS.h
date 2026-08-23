#ifndef _IFS_H_
#define _IFS_H_

#include "matrix.h"
#include "image.h"

class IFS
{
private:
    int n;
    Matrix* transformations;
    float* probabilities;

public:
    IFS();
    ~IFS();
    void Read(const char* file_path);
    void Render(Image* image, int n_points, int n_iterations);
};

#endif