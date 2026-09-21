#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "material.h"
#include "ray.h"
#include "hit.h"
#include "camera.h"
#include "group.h"
#include "scene_parser.h"

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    int width = 100;
    int height = 100;
    char *output_file = NULL;
    float depth_min = 0.0f;
    float depth_max = 1.0f;
    char *depth_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-input")) {
            i++;
            assert(i < argc);
            input_file = argv[i];
        } else if (!strcmp(argv[i], "-size")) {
            i++;
            assert(i < argc);
            width = atoi(argv[i]);
            i++;
            assert(i < argc);
            height = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-output")) {
            i++;
            assert(i < argc);
            output_file = argv[i];
        } else if (!strcmp(argv[i], "-depth")) {
            i++;
            assert(i < argc);
            depth_min = (float)atof(argv[i]);
            i++;
            assert(i < argc);
            depth_max = (float)atof(argv[i]);
            i++;
            assert(i < argc);
            depth_file = argv[i];
        } else {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    assert(input_file != NULL);
    assert(output_file != NULL);
    assert(width > 0 && height > 0);
    assert(depth_file == NULL || depth_min < depth_max);

    const std::string input_prefix =
        "C:\\Users\\silvi\\Desktop\\assignments\\assignment1\\inputs\\";
    const std::string output_prefix =
        "C:\\Users\\silvi\\Desktop\\assignments\\assignment1\\outputs\\";

    std::string input_path = input_prefix + input_file;
    std::string output_path = output_prefix + output_file;
    std::string depth_path;
    if (depth_file != NULL) {
        depth_path = output_prefix + depth_file;
    }

    SceneParser scene(input_path.c_str());
    Camera *camera = scene.getCamera();
    Group *group = scene.getGroup();

    Image color_image(width, height);
    Image *depth_image = NULL;
    if (depth_file != NULL) {
        depth_image = new Image(width, height);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Vec2f point(float(x) / width, float(y) / height);
            Ray ray = camera->generateRay(point);
            Hit hit(FLT_MAX, NULL);

            bool intersected = group->intersect(ray, hit, camera->getTMin());
            if (intersected) {
                color_image.SetPixel(x, y, hit.getMaterial()->getDiffuseColor());
            } else {
                color_image.SetPixel(x, y, scene.getBackgroundColor());
            }

            if (depth_image != NULL) {
                float depth = 0.0f;
                if (intersected) {
                    depth = 1.0f - (hit.getT() - depth_min) / (depth_max - depth_min);
                    if (depth < 0.0f) depth = 0.0f;
                    if (depth > 1.0f) depth = 1.0f;
                }
                depth_image->SetPixel(x, y, Vec3f(depth, depth, depth));
            }
        }
    }

    color_image.SaveTGA(output_path.c_str());
    if (depth_image != NULL) {
        depth_image->SaveTGA(depth_path.c_str());
        delete depth_image;
    }

    return 0;
}
