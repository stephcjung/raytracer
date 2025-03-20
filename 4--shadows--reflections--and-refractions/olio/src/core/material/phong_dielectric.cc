// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2022 by Hadi Fadaifard
//
// Author: Stephanie Jung, 2025
// ======================================================================

//! \file       phong_material.cc
//! \brief      PhongMaterial class
//! \author     Stephanie Jung, 2025

#include "core/material/phong_material.h"
#include <spdlog/spdlog.h>
#include "core/light/light.h"
#include "core/material/phong_dielectric.h"
#include <cmath>

#include <iostream>

namespace olio {
namespace core {

using namespace std;

PhongDielectric::PhongDielectric(Real ior, const Vec3r &attenuation, 
        const std::string &name) :
PhongMaterial{}
{
    ior_ = ior;
    attenuation_ = attenuation;
    SetDiffuse(attenuation);
    name_ = name.size() ? name : "PhongDielectric";
}


//how much of the light is reflected, between [0,1]
Real PhongDielectric::SchlicksReflectance(Real cos_theta, Real ior_in, Real ior_out){
    /*cos_theta = (cos_theta < 0.0f) ? 0.0f : (cos_theta > 1.0f) ? 1.0f : cos_theta;
    Real ratio = ior_in/ior_out;
    Real r0 = (1.0f - ratio)/(1.0+ratio);
    Real r0_sq = r0*r0;*/
    Real R0 = ((ior_out - ior_in)/(ior_out+ior_in))*((ior_out - ior_in)/(ior_out+ior_in));
    Real schlick = R0 + (1-R0)*pow(1-cos_theta, 5);
    //Real schlick = r0_sq + (1.0f-r0_sq)* static_cast<Real>(pow(1-cos_theta,5));
    return schlick;
}

Vec3r PhongDielectric::Scatter( const HitRecord &hit_record, 
                                const Ray &ray_in, 
                                std::shared_ptr<Ray> &reflect_ray, 
                                std::shared_ptr<Ray> &refract_ray, 
                                Real &schlick_reflectance) const{
    //std::cout << "ior: " << ior_ << std::endl;
    //std::cout << "attenuation: " << attenuation_.transpose() << std::endl;
    
    Real n1 = 1.0; //index of refraction of air
    Real n2 = ior_;
    
    if (!hit_record.IsFrontFace()){
        std::swap(n1, n2); //if exiting surface, swap
    }

    //Check for total internal reflection
    Vec3r d = ray_in.GetDirection().normalized();
    Vec3r N = hit_record.GetNormal().normalized();
    Real d_dot_N = N.dot(d);
    Real cos_theta_i = std::min(N.dot(-d), 1.0); //theta angle of incidence
    Real sin_theta_i = std::sqrt(1.0 - (cos_theta_i*cos_theta_i));
        
    //Refract the ray
    //d is incident ray/ray in
    //N is normal
    //n1 is n 
    //n2 is n_t 
    Real ratio = n1/n2;
    Real sin_phi = ratio*sin_theta_i;
    if (sin_phi > 1){
        //This means there is total internal reflection
        refract_ray = nullptr;
        schlick_reflectance = 1.0; //everything is reflected
    }
    else{
        //Real d_dot_N = d.dot(N);
        Real temp = 1 - ratio*ratio * (1-d_dot_N*d_dot_N);
        Vec3r t = ratio * (d - d_dot_N * N) - std::sqrt(temp)*N;

        refract_ray = std::make_shared<Ray>(hit_record.GetPoint(), t);
        schlick_reflectance = SchlicksReflectance(cos_theta_i, n1, n2); //how much of the light is reflected
    }
    reflect_ray = std::make_shared<Ray>(hit_record.GetPoint(), (d - 2 * d_dot_N * N));
    return GetDiffuse();
}


}  // namespace core
}  // namespace olio
