// ======================================================================
// Olio: Simple renderer
//
// ======================================================================

//! \file       sphere.cc
//! \brief      Sphere class

#include "core/geometry/sphere.h"
#include <spdlog/spdlog.h>
#include "core/ray.h"

namespace olio {
namespace core {

// ======================================================================
// *** Homework: implement all of Sphere class' member functions here
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

//! \brief Constructor
//! \param[in] name Node name
Sphere::Sphere(const std::string &name)
    : Surface(name) {
        name_ = name.empty() ? "Sphere" : name;
    }

  //! \brief Constructor
  //! \param[in] center Sphere position
  //! \param[in] radius Sphere radius
  //! \param[in] name Node name
Sphere::Sphere(const Vec3r &center, Real radius, const std::string &name)
    : Surface(name) 
    {
        name_ = name.empty() ? "Sphere" : name;
        center_ = center;
        radius_ = radius;
    }

  //! \brief Check if ray intersects with surface
  //! \details If the ray intersections the surface, the function
  //!          should fill in the 'hit_record' (i.e., information
  //!          about the hit point, normal, etc.)
  //! \param[in] ray Ray to check intersection against
  //! \param[in] tmin Minimum value for acceptable t (ray fractional distance)
  //! \param[in] tmax Maximum value for acceptable t (ray fractional distance)
  //! \param[out] hit_record Resulting hit record if ray intersected with surface
  //! \return True if ray intersected with surface
bool Sphere::Hit(const Ray &ray, Real tmin, Real tmax, HitRecord &hit_record) {
    Vec3r q = ray.GetOrigin() - center_;
    Real b2 = pow((ray.GetDirection()).dot(q), 2);
    Real d_r_dp = (ray.GetDirection()).dot(ray.GetDirection());
    Real discriminant = b2 - (d_r_dp * ((q.dot(q))-(radius_*radius_)));
    if (discriminant < 0){
        return false; // no intersections
    }
    Real neg_b = -(ray.GetDirection()).dot(q);
    Real sqrt_discriminant = sqrt(discriminant);
    
    //t1 < t2
    Real t1 = (neg_b - sqrt_discriminant)/d_r_dp; 
    Real t2 = (neg_b + sqrt_discriminant)/d_r_dp;

    //if both are negative, then the ray is shooting away from the sphere
    if (t2<tmin){
        return false;
    }
    //t1<t2 and we want smaller value, so first check if t1 is in valid range
    else if (t1>tmin && t1<tmax){
        hit_record.SetRayT(t1);
    }
    //if t1 not in valid range, then check if t2 is
    else if (t2>tmin && t2<tmax){
        hit_record.SetRayT(t2);
    }
    //neither are in valid range
    else{
        return false;
    }

    //Finish filling out the hit record
    hit_record.SetPoint(ray.GetOrigin() + (hit_record.GetRayT()*ray.GetDirection())); //point that ray hit on surface
    Vec3r norm_vec = (hit_record.GetPoint() - center_)/radius_;
    hit_record.SetNormal(ray, norm_vec); // Normal vector = (p-O)/r
    hit_record.SetSurface(GetPtr());
    return true;
}

//! \brief Set sphere position
//! \param[in] center Sphere center/position
void Sphere::SetCenter(const Vec3r &center){
    center_ = center;
}

//! \brief Set sphere radius
//! \param[in] radius Sphere radius
void Sphere::SetRadius(Real radius){
    radius_ = radius;
}

//! \brief Get sphere position
//! \return Sphere position
Vec3r Sphere::GetCenter() const{
    return center_;
}

//! \brief Get sphere radius
//! \return Sphere radius
Real Sphere::GetRadius() const{
    return radius_;
}

// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

}  // namespace core
}  // namespace olio
