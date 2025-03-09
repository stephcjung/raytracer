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

#include <iostream>

namespace olio {
namespace core {

using namespace std;

Light::Light(const std::string &name) :
  Node{name}
{
  name_ = name.size() ? name : "Light";
}


Vec3r
Light::Illuminate(const HitRecord &/*hit_record*/, const Vec3r &/*view_vec*/) const
{
  return Vec3r{0, 0, 0};
}


// ======================================================================
// *** Homework: Implement PointLight constructors and member
// *** functions here
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

PointLight::PointLight(const Vec3r &position, const Vec3r &intensity, const std::string &name) :
  Light{name}, position_(position), intensity_(intensity)
{
  name_ = name.size() ? name : "PointLight";
}

void PointLight::SetPosition(const Vec3r &position){
    position_=position;
}

void PointLight::SetIntensity(const Vec3r &intensity){
    intensity_=intensity;
}

Vec3r PointLight::GetPosition() const{
    return position_;
}

Vec3r PointLight::GetIntensity() const{
    return intensity_;
}

Vec3r PointLight::Illuminate(const HitRecord &hit_record, const Vec3r &view_vec) const{
    //First, compute irradiance
    //Get hit point
    Vec3r x = hit_record.GetPoint(); //shading point, aka hit point on surface
    Real r = (GetPosition() - x).norm(); //gets magnitude/distance between hit point and light source
    Vec3r l = (GetPosition() - x)/r; //normalized direction vector from hit point to light source
    Vec3r n = hit_record.GetNormal();
    Vec3r E = (std::max(0.0, n.dot(l))*GetIntensity())/(r*r); //Computed irradiance
    //Now get the surface material
    std::shared_ptr<Material> material = hit_record.GetSurface()->GetMaterial();
    if (!material){
        return Vec3r(0,0,0);
    }
    // If the phong material exists (cast to PhongMaterial)
    PhongMaterial* phongMaterial = dynamic_cast<PhongMaterial*>(material.get());
    //std::cout << "Illuminate's phong material diffuse coefficients: " << phongMaterial->GetDiffuse().transpose() <<std::endl;
    //std::cout << "Illuminate's phong material specular coefficients: " << phongMaterial->GetSpecular().transpose() <<std::endl;
    if (phongMaterial == nullptr) {
        std::cout << "PointLight Illuminate Error - not a Phong material" <<std::endl;
        return Vec3r(0, 0, 0);  // If it's not a PhongMaterial, no shading
    }
    //Then evaluate total attenuation
    Vec3r k = phongMaterial->Evaluate(hit_record, l, view_vec.normalized());
    return k.array()*E.array();

}

// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

// ======================================================================
// *** Homework: Implement AmbientLight constructors and member
// *** functions here
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

AmbientLight::AmbientLight(const Vec3r &ambient, const std::string &name) :
  Light{name}, ambient_(ambient)
{
  name_ = name.size() ? name : "AmbientLight";
}

void AmbientLight::SetAmbient(const Vec3r &ambient){
    ambient_=ambient;
}

Vec3r AmbientLight::GetAmbient() const{
    return ambient_;
}

Vec3r AmbientLight::Illuminate(const HitRecord &hit_record, const Vec3r &view_vec) const{
    //Get the surface material
    std::shared_ptr<Material> material = hit_record.GetSurface()->GetMaterial();
    if (!material){
        std::cout << "Ambient Lighting Error - surface has no material" <<std::endl;
        return Vec3r(0,0,0);
    }
    //If the material exists, then evaluate
    // Cast to PhongMaterial (ensure that the material is of type PhongMaterial)
    PhongMaterial* phongMaterial = dynamic_cast<PhongMaterial*>(material.get());
    if (phongMaterial == nullptr) {
        std::cout << "Ambient Error - not a Phong material" <<std::endl;
        return Vec3r{0, 0, 0};  // If it's not a PhongMaterial, no ambient shading
    }
    //Use view_vec as direction of light source since it is ambient
    Vec3r k = phongMaterial->GetAmbient();
    //std::cout << "Ambient values" << k.transpose() <<std::endl;
    return k.array()*ambient_.array();

}
// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

}  // namespace core
}  // namespace olio
