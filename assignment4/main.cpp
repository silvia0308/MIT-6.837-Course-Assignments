#include "image.h"
#include "ray.h"
#include "hit.h"
#include "camera.h"
#include "scene_parser.h"
#include "ray_tracer.h"
#include "glCanvas.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

static SceneParser* g_scene = nullptr;
static RayTracer* g_tracer = nullptr;

int width = 100;
int height = 100;

float depth_min = 0.0f;
float depth_max = 1.0f;
const char* depth_file = nullptr;
const char* normal_file = nullptr;

bool shade_back = false;

// sphere.cpp 使用这些全局变量
int theta_steps = 20;
int phi_steps = 10;
bool gouraud = false;

std::string output_path;
std::string depth_path;
std::string normal_path;

void renderScene()
{
    if (output_path.empty() && depth_path.empty() && normal_path.empty()) {
        fprintf(stderr, "No output file specified; use -output to save a render.\n");
        return;
    }

    Camera* camera = g_scene->getCamera();

    Image color_image(width, height);
    Image* depth_image =
        depth_file != nullptr ? new Image(width, height) : nullptr;
    Image* normal_image =
        normal_file != nullptr ? new Image(width, height) : nullptr;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vec2f point(
                static_cast<float>(x) / width,
                static_cast<float>(y) / height);

            Ray ray = camera->generateRay(point);
            Hit hit(FLT_MAX, nullptr, Vec3f(0.0f, 0.0f, 0.0f));

            Vec3f pixel_color = g_tracer->traceRay(
                ray,
                camera->getTMin(),
                0,      // 当前递归深度
                1.0f,   // 主射线的权重
                1.0f,   // 空气的折射率
                hit);

            pixel_color.Clamp();
            color_image.SetPixel(x, y, pixel_color);

            const bool intersected = hit.getMaterial() != nullptr;

            if (depth_image != nullptr) {
                float depth = 0.0f;

                if (intersected && depth_max != depth_min) {
                    depth = 1.0f -
                        (hit.getT() - depth_min) /
                        (depth_max - depth_min);

                    if (depth < 0.0f) depth = 0.0f;
                    if (depth > 1.0f) depth = 1.0f;
                }

                depth_image->SetPixel(
                    x, y, Vec3f(depth, depth, depth));
            }

            if (normal_image != nullptr) {
                Vec3f normal_color(0.0f, 0.0f, 0.0f);

                if (intersected) {
                    Vec3f normal = hit.getNormal();
                    normal_color.Set(
                        static_cast<float>(std::fabs(normal.x())),
                        static_cast<float>(std::fabs(normal.y())),
                        static_cast<float>(std::fabs(normal.z())));
                }

                normal_image->SetPixel(x, y, normal_color);
            }
        }
    }

    if (!output_path.empty()) {
        color_image.SaveTGA(output_path.c_str());
    }

    if (depth_image != nullptr) {
        depth_image->SaveTGA(depth_path.c_str());
        delete depth_image;
    }

    if (normal_image != nullptr) {
        normal_image->SaveTGA(normal_path.c_str());
        delete normal_image;
    }
}

void traceRayFunction(float x, float y)
{
    Camera* camera = g_scene->getCamera();
    Ray ray = camera->generateRay(Vec2f(x, y));
    Hit hit(FLT_MAX, nullptr, Vec3f(0.0f, 0.0f, 0.0f));

    g_tracer->traceRay(
        ray,
        camera->getTMin(),
        0,
        1.0f,
        1.0f,
        hit);
}

int main(int argc, char* argv[])
{
    bool gui = false;
    bool shadows = false;
    int max_bounces = 0;
    float cutoff_weight = 0.0f;

    const char* input_file = nullptr;
    const char* output_file = nullptr;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-input")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-input requires a filename\n");
                return 1;
            }
            input_file = argv[++i];
        }
        else if (!strcmp(argv[i], "-size")) {
            if (i + 2 >= argc) {
                fprintf(stderr, "-size requires width and height\n");
                return 1;
            }
            width = atoi(argv[++i]);
            height = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-output")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-output requires a filename\n");
                return 1;
            }
            output_file = argv[++i];
        }
        else if (!strcmp(argv[i], "-depth")) {
            if (i + 3 >= argc) {
                fprintf(stderr,
                    "-depth requires min, max, and filename\n");
                return 1;
            }
            depth_min = static_cast<float>(atof(argv[++i]));
            depth_max = static_cast<float>(atof(argv[++i]));
            depth_file = argv[++i];
        }
        else if (!strcmp(argv[i], "-normals")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-normals requires a filename\n");
                return 1;
            }
            normal_file = argv[++i];
        }
        else if (!strcmp(argv[i], "-shade_back")) {
            shade_back = true;
        }
        else if (!strcmp(argv[i], "-tessellation")) {
            if (i + 2 >= argc) {
                fprintf(stderr,
                    "-tessellation requires two numbers\n");
                return 1;
            }
            theta_steps = atoi(argv[++i]);
            phi_steps = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-gouraud")) {
            gouraud = true;
        }
        else if (!strcmp(argv[i], "-gui")) {
            gui = true;
        }
        else if (!strcmp(argv[i], "-shadows")) {
            shadows = true;
        }
        else if (!strcmp(argv[i], "-bounces")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-bounces requires a number\n");
                return 1;
            }
            max_bounces = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-weight")) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-weight requires a number\n");
                return 1;
            }
            cutoff_weight =
                static_cast<float>(atof(argv[++i]));
        }
        else {
            fprintf(stderr,
                "Unknown command line argument: %s\n", argv[i]);
            return 1;
        }
    }

    if (input_file == nullptr || (!gui && output_file == nullptr)) {
        fprintf(stderr,
            "Usage: -input scene.txt [-gui] [-output result.tga] "
            "[-size width height] [-shadows] "
            "[-bounces n] [-weight w]\n"
            "-output is required unless -gui is specified.\n");
        return 1;
    }

    if (width <= 0 || height <= 0 ||
        max_bounces < 0 || cutoff_weight < 0.0f) {
        fprintf(stderr, "Invalid size, bounces, or weight\n");
        return 1;
    }

    const std::string input_path =
        std::string("../../inputs/") + input_file;
    if (output_file != nullptr) {
        output_path =
            std::string("../../outputs/") + output_file;
    }

    if (depth_file != nullptr) {
        depth_path =
            std::string("../../outputs/") + depth_file;
    }
    if (normal_file != nullptr) {
        normal_path =
            std::string("../../outputs/") + normal_file;
    }

    SceneParser scene(input_path.c_str());
    RayTracer tracer(
        &scene, max_bounces, cutoff_weight, shadows, shade_back);

    g_scene = &scene;
    g_tracer = &tracer;

    if (gui) {
        glutInit(&argc, argv);
        GLCanvas canvas;
        canvas.initialize(&scene, renderScene, traceRayFunction);
    }
    else {
        renderScene();
    }

    return 0;
}
