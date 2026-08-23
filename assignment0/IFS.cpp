#include <stdio.h>
#include <assert.h>
#include "IFS.h"

IFS::IFS() {
    n = 0;
    transformations = nullptr;
    probabilities = nullptr;
}

IFS::~IFS() {
    delete[] transformations;
    delete[] probabilities;
}

void IFS::Read(const char* file_path) {
    FILE* input = fopen(file_path, "r");
    assert(input != NULL);   // 打不开文件直接报错

    // 1. 读 transform 个数
    fscanf(input, "%d", &n);

    // 2. 根据 n 分配数组空间
    transformations = new Matrix[n];
    probabilities = new float[n];

    // 3. 逐个读入概率和 3x3 矩阵
    for (int i = 0; i < n; i++)
    {
        fscanf(input, "%f", &probabilities[i]);
        transformations[i].Read3x3(input);
    }

    // 4. 关闭文件
    fclose(input);
}

void IFS::Render(Image* image, int n_points, int n_iterations) {
    int width = image->Width();
    int height = image->Height();

    // 白底
    image->SetAllPixels(Vec3f(1, 1, 1));

    // ---- 准备：把概率转成累积区间，用于加权随机选变换 ----
    float total = 0;
    for (int i = 0; i < n; i++)
        total += probabilities[i];

    float* cumulative = new float[n];
    float acc = 0;
    for (int i = 0; i < n; i++)
    {
        acc += probabilities[i];
        cumulative[i] = acc;
    }

    // ---- 伪代码：for "lots" of random points (x0, y0) ----
    for (int i = 0; i < n_points; i++)
    {
        // 单位正方形 [0,1) 内的随机起点
        Vec2f p((float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX);

        // ---- for k=0 to num_iters ----
        for (int k = 0; k < n_iterations; k++)
        {
            // ---- pick a random transform fi（按概率加权）----
            float r = (float)rand() / RAND_MAX * total;
            int j = 0;
            while (j < n - 1 && r > cumulative[j])
                j++;

            // ---- (xk+1, yk+1) = fi(xk, yk) ----
            transformations[j].Transform(p);
        }

        // ---- display a dot at (xk, yk) ----
        int x = (int)(p.x() * width);
        int y = (int)(p.y() * height);
        if (x >= 0 && x < width && y >= 0 && y < height)
            image->SetPixel(x, y, Vec3f(0, 0, 0));   // 黑点
    }

    delete[] cumulative;
}