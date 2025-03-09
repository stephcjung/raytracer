#include "raytra_parser.h"
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>
#include "core/geometry/surface.h"
#include "core/geometry/sphere.h"
#include "core/camera/camera.h"
#include "core/geometry/triangle.h"
#include "core/material/phong_material.h"
#include "core/light/light.h"

#include <iostream>

namespace olio {
namespace core {

using namespace std;
using boost::algorithm::trim;
namespace fs = boost::filesystem;

bool RaytraParser::ParseFile(const std::string &filename, SurfaceList::Ptr &scene, std::vector<Light::Ptr> &lights, Camera::Ptr &camera, Vec2i &image_size)
{
  std::vector<Surface::Ptr> surfaces;
  
  // get absoulte file path
  fs::path filepath(filename);
  if (!filepath.is_absolute())
    filepath = boost::filesystem::absolute(filepath,
      boost::filesystem::current_path());
  fs::path path_prefix(filepath.parent_path());

  // check if file exists
  if (!fs::exists(filepath)) {
    spdlog::error("RaytraParser::ParseFile: file {} does not exist", filename);
    return false;
  }

  // open file
  ifstream in(filename);
  if (!in) {
    spdlog::error("RaytraParser::ParseFile: could not open file {} "
                  "for reading", filename);
    return false;
  }
  int camera_count = 0;
  int surface_count = 0;
  int ambient_light_count = 0;
  int point_light_count = 0;
  PhongMaterial::Ptr current_material = nullptr;  // To store the current material

  // parse file
  for (string line; getline(in, line);) {
    // skip comments and empty lines
    trim(line);
    if (line.size() && line.back() == '\n')
      line.pop_back();
    if (!line.size() || line[0] == '/')
      continue;

    // parse file
    char cmd {'\0'};
    istringstream iss(line);
    iss >> cmd;
    switch(cmd) {
    case '/': continue;

    case 'l':
    {
        char light_type;
        iss >> light_type;

        if (light_type == 'a'){
            //ambient light source
            Real r, g, b;
            iss >> r >> g >> b;
            auto ambient_light = AmbientLight::Create(Vec3r{r, g, b});
            lights.push_back(ambient_light);
            ++ambient_light_count;
        }
        else if (light_type == 'p'){
            //point light source
            Real x, y, z, r, g, b;
            iss >> x >> y >> z >> r >> g >> b;
            auto point_light = PointLight::Create(Vec3r{x, y, z}, Vec3r{r, g, b});
            lights.push_back(point_light);
            ++point_light_count;
        }
        else{
            spdlog::error("Parse error: scene file requires light sources to be either ambient or point (directional not implemented)");
            return false;
        }
        break;
    }
    case 'm':
    {
        //Assign Phong materials to correct surfaces
        //m dr dg db sr sg sb r
        Real dr, dg, db, sr, sg, sb, shininess, ar, ag, ab, mr, mg, mb;
        iss >> dr >> dg >> db >> sr >> sg >> sb >> shininess >> mr >> mg >> mb;
        ar = std::max(dr, 0.01);
        ag = std::max(dg, 0.01);
        ab = std::max(db, 0.01);
        current_material = std::make_shared<PhongMaterial>(
            Vec3r{ar, ag, ab}, 
            Vec3r{dr, dg, db}, 
            Vec3r{sr, sg, sb}, 
            shininess);
        break;

    }

    case 's':
      {
        // sphere
        Real x, y, z, r;
        iss >> x >> y >> z >> r;
        auto sphere_scene = Sphere::Create(Vec3r{x, y, z}, r);
        surfaces.push_back(sphere_scene);
        ++surface_count;
        if (!current_material){
            spdlog::error("Parse error: scene file requires current material for sphere surface ");
            return false;
        }
        sphere_scene->SetMaterial(current_material);
        break;
      }
    case 'c':
      {
        // camera
        Real x, y, z, vx, vy, vz, focal_length, viewport_width, viewport_height,
          pixels_width, pixels_height;
        iss >> x >> y >> z >> vx >> vy >> vz >> focal_length >> viewport_width
            >> viewport_height >> pixels_width >> pixels_height;
        Vec3r eye{x, y, z};
        Vec3r view_vec{vx, vy, vz};
        view_vec.normalize();
        Vec3r target = eye + view_vec;
        Vec3r up_vec{0, 1, 0};
        if (view_vec.isApprox(up_vec))
          up_vec = Vec3r{0, 0, 1};
        Real fovy = 2 * atan2(viewport_height * 0.5f, focal_length) * kRADtoDEG;

        // check viewport/image aspect ratios
        Real viewport_aspect = viewport_width / viewport_height;
        if (!isfinite(viewport_aspect) || viewport_aspect <= 0) {
          spdlog::error("Camera has bad viewport_aspect ratio: {}",
                        viewport_aspect);
          return false;
        }
        if (viewport_aspect > 20000) {
          spdlog::warn("Camera has very large viewport_aspect ratio: {}",
                       viewport_aspect);
        }
        Real image_aspect = pixels_width / pixels_height;
        if (fabs(viewport_aspect - image_aspect) > kEpsilon) {
          spdlog::warn("Camera viewport has a different aspect ratio than "
                       "output image (viewport_aspect: {} vs "
                       "image_aspect: {})", viewport_aspect, image_aspect);
          spdlog::warn("Output image width will be adjusted to match the "
                       "viewport aspect ratio");
        }

        // create camera
        camera = Camera::Create(eye, target, up_vec, fovy,viewport_aspect);
        image_size = Vec2i{static_cast<int>(pixels_width),
                           static_cast<int>(pixels_height)};
        ++camera_count;
        break;
      }
    case 't':
      {
        // triangle
        Real ax, ay, az, bx, by, bz, cx, cy, cz;
        iss >> ax >> ay >> az >> bx >> by >> bz >> cx >> cy >> cz;
        vector<Vec3r> points;
        points.reserve(3);
        points.push_back(Vec3r{ax, ay, az});
        points.push_back(Vec3r{bx, by, bz});
        points.push_back(Vec3r{cx, cy, cz});
        auto triangle_scene = Triangle::Create(points);
        surfaces.push_back(triangle_scene);
        ++surface_count;
        if (!current_material){
            spdlog::error("Parse error: scene file requires current material for triangle surface ");
            return false;
        }
        triangle_scene->SetMaterial(current_material);
        break;
      }
    default:
      continue;
    }
  }

  // close input file
  in.close();

  if (camera_count != 1) {
    spdlog::error("Parse error: scene file should contain only one camera");
    return false;
  }

  if (ambient_light_count > 1) {
    spdlog::error("Parse error: scene file currently can only contain one "
                  "ambient light");
    return false;
  }
  scene = std::make_shared<SurfaceList>(surfaces);
  return true;
}

}  // namespace core
}  // namespace raytra
