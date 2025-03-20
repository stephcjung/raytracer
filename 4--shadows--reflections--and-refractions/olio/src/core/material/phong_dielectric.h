// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2022 by Hadi Fadaifard
//
// Author: Stephanie Jung, 2025
// ======================================================================

//! \file       phong_material.h
//! \brief      PhongMaterial class
//! \author     Stephanie Jung, 2025

#pragma once

#include <memory>
#include <string>
#include "core/types.h"
#include "core/node.h"
#include "core/material/material.h"
#include "core/ray.h"
#include "core/material/phong_material.h"

namespace olio {
namespace core {

class Light;
class AmbientLight;
class PointLight;

//! \class PhongDielectric
//! \brief PhongDielectric class
class PhongDielectric : public PhongMaterial {
public:
    OLIO_NODE(PhongDielectric)

    //! Inheriting constructors from PhongMaterial
    using PhongMaterial::PhongMaterial;  

      //! \brief Constructor
    //! \param[in] ior Index of refraction
    //! \param[in] attenuation Absorption of light by the dielectric material
    //! \param[in] name Node name
    PhongDielectric(Real ior, const Vec3r &attenuation=Vec3r{1, 1, 1}, 
        const std::string &name=std::string());


    //! \brief Scatter incoming ray ray_in
    //! \details The function will generate a reflection and a refraction
    //!    ray. The refraction ray will be null (not generated) if there is total
    //!    internal reflection.
    //! \param[in] hit_record Hit record at hit point
    //! \param[in] ray_in Incoming ray that hit the point
    //! \param[out] reflect_ray Reflected ray
    //! \param[out] refract_ray Refracted ray (null if not refracted because of total internal reflection)
    //! \param[out] schlick_reflectance Schlick's reflectance
    //! \return Attenuation factor (how much color of reflected/refracted
    //! rays should be attenuated)
    Vec3r Scatter(
        const HitRecord &hit_record, 
        const Ray &ray_in, 
        std::shared_ptr<Ray> &reflect_ray, 
        std::shared_ptr<Ray> &refract_ray, 
        Real &schlick_reflectance) const;

    static Real SchlicksReflectance(Real cos_theta, Real ior_in, Real ior_out);


protected:
    Real ior_{1};
    Vec3r attenuation_{1,1,1};
};

}  // namespace core
}  // namespace olio
