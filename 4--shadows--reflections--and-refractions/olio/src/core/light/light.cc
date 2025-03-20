// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2022 by Hadi Fadaifard
//
// Author: Hadi Fadaifard, 2022
// ======================================================================

//! \file       light.cc
//! \brief      Node class
//! \author     Hadi Fadaifard, 2022

#include "core/light/light.h"
#include "core/geometry/surface.h"
#include "core/ray.h"
#include "core/material/phong_material.h"

namespace olio {
namespace core {

using namespace std;

Light::Light(const std::string &name) :
  Node{name}
{
  name_ = name.size() ? name : "Light";
}


Vec3r
Light::Illuminate(const HitRecord &hit_record, const Vec3r &view_vec, Surface::Ptr scene) const
{
  return Vec3r{0, 0, 0};
}


AmbientLight::AmbientLight(const std::string &name) :
  Light{name}
{
  name_ = name.size() ? name : "AmbientLight";
}


AmbientLight::AmbientLight(const Vec3r &ambient, const std::string &name) :
  Light{name},
  ambient_{ambient}
{
  name_ = name.size() ? name : "AmbientLight";
}


Vec3r
AmbientLight::Illuminate(const HitRecord &hit_record, const Vec3r &view_vec, Surface::Ptr scene) const
{
  // only process phong materials
  auto surface = hit_record.GetSurface();
  if (!surface)
    return Vec3r{0, 0, 0};
  auto phong_material = dynamic_pointer_cast<PhongMaterial>(surface->
                                                            GetMaterial());
  if (!phong_material)
    return Vec3r{0, 0, 0};
  return ambient_.cwiseProduct(phong_material->GetAmbient());
}


PointLight::PointLight(const std::string &name) :
  Light{name}
{
  name_ = name.size() ? name : "PointLight";
}


PointLight::PointLight(const Vec3r &position, const Vec3r &intensity,
                       const std::string &name) :
  Light{name},
  position_{position},
  intensity_{intensity}
{
  name_ = name.size() ? name : "PointLight";
}

/*
Vec3r
PointLight::Illuminate_orig(const HitRecord &hit_record, const Vec3r &view_vec) const
{
  // evaluate hit points material
  Vec3r black{0, 0, 0};

  // only process phong materials
  auto surface = hit_record.GetSurface();
  if (!surface)
    return black;
  auto phong_material = dynamic_pointer_cast<PhongMaterial>(surface->
                                                            GetMaterial());
  if (!phong_material)
    return black;

  // compute irradiance at hit point
  const Vec3r &hit_position = hit_record.GetPoint();
  const Vec3r &normal = hit_record.GetNormal();
  Vec3r light_vec = position_ - hit_position;
  auto distance2 = light_vec.squaredNorm();
  light_vec.normalize();
  auto denominator = std::max(kEpsilon2, distance2);
  Vec3r irradiance = intensity_ * fmax(0.0f, normal.dot(light_vec))/denominator;

  // compute how much the material absorts light
  const Vec3r &attenuation = phong_material->Evaluate(hit_record, light_vec,
                                                      view_vec);
  return irradiance.cwiseProduct(attenuation);
}
*/

Vec3r 
PointLight::Illuminate(const HitRecord &hit_record, const Vec3r &view_vec, Surface::Ptr scene) const{

    Vec3r light_dir = position_ - hit_record.GetPoint(); // Vector from hit point to light
    Ray shadow_ray(hit_record.GetPoint(), light_dir); // Shadow ray from hit point towards light source

    // Normalize the direction of the light vector
    light_dir.normalize();

    // evaluate hit points material
    Vec3r black{0, 0, 0};

    HitRecord temp_hit_record;
    if (scene->Hit(shadow_ray, kEpsilon, light_dir.norm(), temp_hit_record)){
        return black;
    }
    // only process phong materials
    auto surface = hit_record.GetSurface();
    if (!surface)
        return black;
    auto phong_material = dynamic_pointer_cast<PhongMaterial>(surface->
                                                                    GetMaterial());
    if (!phong_material)
        return black;

    // compute irradiance at hit point
    const Vec3r &hit_position = hit_record.GetPoint();
    const Vec3r &normal = hit_record.GetNormal();
    Vec3r light_vec = position_ - hit_position;
    auto distance2 = light_vec.squaredNorm();
    light_vec.normalize();
    auto denominator = std::max(kEpsilon2, distance2);
    Vec3r irradiance = intensity_ * fmax(0.0f, normal.dot(light_vec))/denominator;

    // compute how much the material absorts light
    const Vec3r &attenuation = phong_material->Evaluate(hit_record, light_vec,
                                                            view_vec);
    return irradiance.cwiseProduct(attenuation);
    
}

}  // namespace core
}  // namespace olio
