#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "IFS.h"
#include "image.h"

int main(int argc, char* argv[])
{
    // ---- 命令行参数（默认值）----
    char* input_file = NULL;
    int   num_points = 10000;
    int   num_iters = 10;
    int   size = 100;
    char* output_file = NULL;

    // 输入目录前缀（inputs 子文件夹）
    const char* input_dir_prefix = "c:/Users/silvi/Desktop/assignments/assignment0/inputs/";
    // 输出目录前缀（outputs 子文件夹）
    const char* output_dir_prefix = "c:/Users/silvi/Desktop/assignments/assignment0/outputs/";

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-input")) { i++; assert(i < argc); input_file = argv[i]; }
        else if (!strcmp(argv[i], "-points")) { i++; assert(i < argc); num_points = atoi(argv[i]); }
        else if (!strcmp(argv[i], "-iters")) { i++; assert(i < argc); num_iters = atoi(argv[i]); }
        else if (!strcmp(argv[i], "-size")) { i++; assert(i < argc); size = atoi(argv[i]); }
        else if (!strcmp(argv[i], "-output")) { i++; assert(i < argc); output_file = argv[i]; }
        else
        {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    assert(input_file != NULL);
    assert(output_file != NULL);

    // ---- 分别拼接输入/输出目录前缀 + 文件名 ----
    char full_input[512], full_output[512];
    sprintf(full_input, "%s%s", input_dir_prefix, input_file);
    sprintf(full_output, "%s%s", output_dir_prefix, output_file);

    srand((unsigned)time(NULL));

    IFS ifs;
    ifs.Read(full_input);

    Image* image = new Image(size, size);
    ifs.Render(image, num_points, num_iters);

    image->SaveTGA(full_output);

    delete image;
    return 0;
}
