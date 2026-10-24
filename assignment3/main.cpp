#include "image.h"
#include "material.h"
#include "ray.h"
#include "hit.h"
#include "camera.h"
#include "group.h"
#include "light.h"
#include "scene_parser.h"
#include "glCanvas.h"
#include <GL/glut.h>

static SceneParser* g_scene = nullptr;
int width = 100;
int height = 100;
float depth_min = 0.0f;
float depth_max = 1.0f;
char* depth_file = NULL;
char* normal_file = NULL;
bool shade_back = false;
std::string output_path;
std::string depth_path;
std::string normal_path;
int theta_steps = 20;
int phi_steps = 10;
bool gouraud = false;

void renderScene()
{
    SceneParser& scene = *g_scene;

    Camera* camera = scene.getCamera();
    Group* group = scene.getGroup();

    Image color_image(width, height);
    Image* depth_image = NULL;
    if (depth_file != NULL) {
        depth_image = new Image(width, height);
    }
    Image* normal_image = NULL;
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
                Hit shadingHit = hit;
                if (shade_back && ray.getDirection().Dot3(normal) > 0.0f) {
                    normal.Negate();
                    shadingHit.set(hit.getT(), hit.getMaterial(), normal, ray);
                }

                Vec3f pixel_color =
                    scene.getAmbientLight() * hit.getMaterial()->getDiffuseColor();

                for (int i = 0; i < scene.getNumLights(); i++) {
                    Vec3f light_direction;
                    Vec3f light_color;
                    float distanceToLight;

                    scene.getLight(i)->getIllumination(
                        hit.getIntersectionPoint(),
                        light_direction,
                        light_color,
                        distanceToLight);

                    pixel_color += hit.getMaterial()->Shade(
                        ray, shadingHit, light_direction, light_color);
                }

                pixel_color.Clamp();
                color_image.SetPixel(x, y, pixel_color);
            }
            else {
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
}

int main(int argc, char* argv[])
{
    bool gui = false;
    char* input_file = NULL;
    char* output_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-input")) {
            i++;
            input_file = argv[i];
        }
        else if (!strcmp(argv[i], "-size")) {
            i++;
            width = atoi(argv[i]);
            i++;
            height = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-output")) {
            i++;
            output_file = argv[i];
        }
        else if (!strcmp(argv[i], "-depth")) {
            i++;
            depth_min = (float)atof(argv[i]);
            i++;
            depth_max = (float)atof(argv[i]);
            i++;
            depth_file = argv[i];
        }
        else if (!strcmp(argv[i], "-normals")) {
            i++;
            normal_file = argv[i];
        }
        else if (!strcmp(argv[i], "-shade_back")) {
            shade_back = true;
        }
        else if (!strcmp(argv[i], "-tessellation")) {
            theta_steps = atoi(argv[++i]);
            phi_steps = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-gouraud")) {
            gouraud = true;
        }
        else if (!strcmp(argv[i], "-gui")) {
            gui = true;
        }
        else {
            printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
        }
    }

    const std::string input_dir_prefix = "../../inputs/";
    const std::string output_dir_prefix = "../../outputs/";

    std::string input_path = input_dir_prefix + input_file;
    output_path = output_dir_prefix + output_file;
    if (depth_file != NULL) {
        depth_path = output_dir_prefix + depth_file;
    }
    if (normal_file != NULL) {
        normal_path = output_dir_prefix + normal_file;
    }

    SceneParser scene(input_path.c_str());
    g_scene = &scene;

    if (gui) {
        glutInit(&argc, argv);
        GLCanvas canvas;
        canvas.initialize(&scene, renderScene);
        // initialize 进入事件循环，正常使用时不会执行到下面
    }
    else {
        renderScene();
    }

    return 0;
}