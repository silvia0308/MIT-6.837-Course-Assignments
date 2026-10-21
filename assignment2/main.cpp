#include <assert.h>
#include <math.h>
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
#include "light.h"
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
    char *normal_file = NULL;
    bool shade_back = false;

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
        } else if (!strcmp(argv[i], "-normals")) {
            i++;
            assert(i < argc);
            normal_file = argv[i];
        } else if (!strcmp(argv[i], "-shade_back")) {
            shade_back = true;
        } else {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    assert(input_file != NULL);
    assert(output_file != NULL);
    assert(width > 0 && height > 0);
    assert(depth_file == NULL || depth_min < depth_max);

    const std::string input_dir_prefix = "../../inputs/";
    const std::string output_dir_prefix = "../../outputs/";

    std::string input_path = input_dir_prefix + input_file;
    std::string output_path = output_dir_prefix + output_file;
    std::string depth_path;
    std::string normal_path;
    if (depth_file != NULL) {
        depth_path = output_dir_prefix + depth_file;
    }
    if (normal_file != NULL) {
        normal_path = output_dir_prefix + normal_file;
    }

    SceneParser scene(input_path.c_str());
    Camera *camera = scene.getCamera();
    Group *group = scene.getGroup();

    Image color_image(width, height);
    Image *depth_image = NULL;
    if (depth_file != NULL) {
        depth_image = new Image(width, height);
    }
    Image *normal_image = NULL;
    if (normal_file != NULL) {
        normal_image = new Image(width, height);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Vec2f point(float(x) / width, float(y) / height);
            Ray ray = camera->generateRay(point);
            Hit hit(FLT_MAX, NULL, Vec3f(0.0f, 0.0f, 0.0f));

            bool intersected = group->intersect(ray, hit, camera->getTMin());
            if (intersected) {
                Vec3f normal = hit.getNormal();
                if (shade_back && ray.getDirection().Dot3(normal) > 0.0f) {
                    normal.Negate();
                }

                Vec3f diffuse_color = hit.getMaterial()->getDiffuseColor();
                Vec3f pixel_color = scene.getAmbientLight() * diffuse_color;

                for (int i = 0; i < scene.getNumLights(); i++) {
                    Vec3f light_direction;
                    Vec3f light_color;
                    scene.getLight(i)->getIllumination(hit.getIntersectionPoint(),
                                                       light_direction,
                                                       light_color);

                    float diffuse = light_direction.Dot3(normal);
                    if (diffuse > 0.0f) {
                        pixel_color += diffuse * light_color * diffuse_color;
                    }
                }

                pixel_color.Clamp();
                color_image.SetPixel(x, y, pixel_color);
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

            if (normal_image != NULL) {
                Vec3f normal_color(0.0f, 0.0f, 0.0f);
                if (intersected) {
                    Vec3f normal = hit.getNormal();
                    normal_color.Set(std::fabs(normal.x()),
                                     std::fabs(normal.y()),
                                     std::fabs(normal.z()));
                }
                normal_image->SetPixel(x, y, normal_color);
            }
        }
    }

    color_image.SaveTGA(output_path.c_str());
    if (depth_image != NULL) {
        depth_image->SaveTGA(depth_path.c_str());
        delete depth_image;
    }
    if (normal_image != NULL) {
        normal_image->SaveTGA(normal_path.c_str());
        delete normal_image;
    }

    return 0;
}
