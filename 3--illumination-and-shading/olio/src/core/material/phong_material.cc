// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2022 by Hadi Fadaifard
//
// Author: Hadi Fadaifard, 2022
// ======================================================================

//! \file       phong_material.cc
//! \brief      PhongMaterial class
//! \author     Hadi Fadaifard, 2022

#include "core/material/phong_material.h"
#include <spdlog/spdlog.h>
#include "core/light/light.h"

#include <iostream>

namespace olio {
namespace core {

using namespace std;

PhongMaterial::PhongMaterial(const std::string &name) :
  Material{}
{
  name_ = name.size() ? name : "PhongMaterial";
  SetDiffuse(Vec3r{0, 0, 0});
}


PhongMaterial::PhongMaterial(const Vec3r &ambient, const Vec3r &diffuse,
			     const Vec3r &specular, Real shininess,
			     const Vec3r &mirror, const std::string &name) :
  Material{},
  ambient_{ambient},
  specular_{specular},
  shininess_{shininess},
  mirror_{mirror}
{
  name_ = name.size() ? name : "PhongMaterial";
  SetDiffuse(diffuse);
}


void
PhongMaterial::SetDiffuse(const Vec3r &diffuse)
{
  diffuse_ = diffuse;
}


Vec3r
PhongMaterial::Evaluate(const HitRecord &hit_record, const Vec3r &light_vec,
			const Vec3r &view_vec) const
{
// ======================================================================
// *** Homework: Your code for PhongMaterial::Evaluate() goes here.
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****
  //Specular shading component
  Vec3r n = hit_record.GetNormal().normalized();
  Vec3r v = view_vec.normalized();
  Vec3r h = (v+light_vec).normalized(); //light_vec is vector from hit point to light source
  Vec3r ls = GetSpecular() * std::pow(std::max(0.0, n.dot(h)), GetShininess());
  //std::cout << "Specular values" << ls.transpose() <<std::endl;

  //Diffuse/Lambertian shading component
  Vec3r ld = GetDiffuse();
  //std::cout << "Diffuse values" << ld.transpose() <<std::endl;

  //Add specular and diffuse
  return ls+ld;

// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****
}

}  // namespace core
}  // namespace olio
