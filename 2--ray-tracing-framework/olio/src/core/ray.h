// ======================================================================
// Olio: Simple renderer
// Copyright (C) 2022 by Hadi Fadaifard
//
// Author: Hadi Fadaifard, 2022
// ======================================================================

//! \file       ray.h
//! \brief      Ray class
//! \author     Hadi Fadaifard, 2022

#pragma once

#include <memory>
#include <string>
#include "core/types.h"

namespace olio {
namespace core {

class Surface;

//! \class Ray
//! \brief Ray class used during path tracing
class Ray {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  //! \brief Constructor
  //! \param[in] origin Ray origin
  //! \param[in] dir Ray direction
  Ray(const Vec3r &origin, const Vec3r &dir);

  //! \brief Default constructor
  Ray() = default;

  //! \brief Set ray origin
  //! \param[in] origin Ray origin
  inline void SetOrigin(const Vec3r &origin);

  //! \brief Set ray direction
  //! \param[in] dir Ray direction
  inline void SetDirection(const Vec3r &dir);

  //! \brief Get ray origin
  //! \return Ray origin
  inline Vec3r GetOrigin() const;

  //! \brief Get ray direction
  //! \return Ray direction
  inline Vec3r GetDirection() const;

  //! \brief Evaluate ray at fractional distance t
  //! \param[in] t Fractional distance t to evaluate ray at
  //! \return Point along ray at t
  inline Vec3r At(Real t) const;
protected:
  Vec3r origin_{0, 0, 0};  //!< Ray origin
  Vec3r dir_{0, 0, 0};     //!< Ray direction
};


//! \class HitRecord
//! \brief HitRecord class is used during path tracing to keep track
//! of information about the point on a surface that was hit by a ray.
//! \details Information such as hit poisition, surface normal at
//! that position, whether the normal was facing towards or away
//! from the ray, the pointer to the surface that was hit, etc.
class HitRecord {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  //! \brief Constructor
  //! \param[in] ray Ray that intersected the surface and generated
  //!            the hit record
  //! \param[in] ray_t Ray's hit t
  //! \param[in] point Hit point on the surface
  //! \param[in] point Surface normal at hit point (should always be
  //!            facing towards fron ray)
  HitRecord(const Ray &ray, Real ray_t, const Vec3r &point,
	    const Vec3r &face_normal);

  //! \brief Default constructor
  HitRecord() = default;

  //! \brief Set ray's t (fractional distance) at hit time
  //! \param[in] ray_t Ray's t
  inline void SetRayT(Real ray_t);

  //! \brief Set hit point position
  //! \param[in] point Hit position
  inline void SetPoint(const Vec3r &point);

  //! \brief Set surface normal at hit position
  //! \details The function should store the face_normal in its
  //! 'normal_' data member. However, it should also adjust the
  //! orientation of the stored normal to guarantee that the normal is
  //! pointing towards the ray (the dot product is negative). If the
  //! original normal was facing towards the ray, 'front_face_' should
  //! be set to true. If the original normal was point away (the dot
  //! product is positive), 'front_face_' should be set to false.
  //! \param[in] ray Ray that hit the surface
  //! \param[in] face_normal Surface normal at hit position
  inline void SetNormal(const Ray &ray, const Vec3r &face_normal);

  //! \brief Set surface normal at hit position.
  //! \details See the details in the other (overloaded) SetNormals.
  //! \param[in] face_normal Surface normal at hit point
  //! \param[in] front_face whether the hit point was front or back facing
  inline void SetNormal(const Vec3r &face_normal, bool front_face);

  //! \brief Set surface that was hit
  //! \param[in] surface Pointer to surface that was hit
  inline void SetSurface(std::shared_ptr<Surface> surface);

  //! \brief Get ray's fractional distance
  //! \return Ray's fractional distance
  inline Real GetRayT() const;

  //! \brief Get hit position
  //! \return Hit position
  inline Vec3r GetPoint() const;

  //! \brief Get surface normal at hit point
  //! \return surface normal
  inline Vec3r GetNormal() const;

  //! \brief Return whether the hit point was front or back facing
  //! \return Whether the hit point was front or back facing
  inline bool IsFrontFace() const;

  //! \brief Get hit surface
  //! \return Hit surface
  inline std::shared_ptr<Surface> GetSurface() const;
protected:
  Real ray_t_{0}; //!< fractional distance along ray (t) that intersects surface
  Vec3r point_{0, 0, 0};   //!< hit point
  Vec3r normal_{0, 0, 0};  //!< surface normal at hit point
  bool front_face_{true};  //!< whether hit point was front or back facing
  std::shared_ptr<Surface> surface_;  //!< pointer to the hit surface
};


// ======================================================================
// *** Homework: implement Ray's inline functions here (non-inline ***
// functions/constructors should be implemented in the .cc file)
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

//! \brief Set ray origin
//! \param[in] origin Ray origin
inline void Ray::SetOrigin(const Vec3r &origin){
    origin_ = origin;
}

//! \brief Set ray direction
//! \param[in] dir Ray direction
inline void Ray::SetDirection(const Vec3r &dir){
    dir_ = dir;
}

//! \brief Get ray origin
//! \return Ray origin
inline Vec3r Ray::GetOrigin() const{
    return origin_;
}

//! \brief Get ray direction
//! \return Ray direction
inline Vec3r Ray::GetDirection() const{
    return dir_;
}

//! \brief Evaluate ray at fractional distance t
//! \param[in] t Fractional distance t to evaluate ray at
//! \return Point along ray at t
inline Vec3r Ray::At(Real t) const{
    return origin_ + (t*dir_);
}

// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

// ======================================================================
// *** Homework: implement HitRecord's inline functions here (non-inline ***
// functions/constructors should be implemented in the .cc file)
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

//! \brief Set ray's t (fractional distance) at hit time
//! \param[in] ray_t Ray's t
inline void HitRecord::SetRayT(Real ray_t){
    ray_t_ = ray_t;
}

//! \brief Set hit point position
//! \param[in] point Hit position
inline void HitRecord::SetPoint(const Vec3r &point){
    point_ = point;
}

//! \brief Set surface normal at hit position
//! \details The function should store the face_normal in its
//! 'normal_' data member. However, it should also adjust the
//! orientation of the stored normal to guarantee that the normal is
//! pointing towards the ray (the dot product is negative). 
//! If the original normal was facing towards the ray, 'front_face_' should
//! be set to true. If the original normal was point away (the dot
//! product is positive), 'front_face_' should be set to false.
//! \param[in] ray Ray that hit the surface
//! \param[in] face_normal Surface normal at hit position
inline void HitRecord::SetNormal(const Ray &ray, const Vec3r &face_normal){
    if (face_normal.dot(ray.GetDirection()) > 0){
        front_face_ = false;    // DP pos -> ray hits back face of the plane, normal facing away from ray
        //if ray is orthogonal to normal/parallel to plane, it will never hit
    }
    else{
        front_face_ = true; //if grazes sphere (DP==0), this counts as a front face hit
    }
    normal_ = front_face_ ? face_normal : -face_normal;
    // We want the ray to hit the front face, aka normal vector faces ray and DP negative
    // So multiply normal by -1 if the normal vector faces away from the ray
}

//! \brief Set surface normal at hit position.
//! \details See the details in the other (overloaded) SetNormals.
//! \param[in] face_normal Surface normal at hit point
//! \param[in] front_face whether the hit point was front or back facing
inline void HitRecord::SetNormal(const Vec3r &face_normal, bool front_face){
    front_face_ = front_face;
    normal_ = front_face_ ? face_normal : -face_normal;
    // Again, we want the ray to hit the front face, aka normal vector faces ray and DP negative
    // So multiply normal by -1 if the normal vector faces away from the ray
}

//! \brief Set surface that was hit
//! \param[in] surface Pointer to surface that was hit
inline void HitRecord::SetSurface(std::shared_ptr<Surface> surface){
    surface_ = surface;
}

//! \brief Get ray's fractional distance
//! \return Ray's fractional distance
inline Real HitRecord::GetRayT() const{
    return ray_t_;
}

//! \brief Get hit position
//! \return Hit position
inline Vec3r HitRecord::GetPoint() const{
    return point_;
}

//! \brief Get surface normal at hit point
//! \return surface normal
inline Vec3r HitRecord::GetNormal() const{
    return normal_;
}

//! \brief Return whether the hit point was front or back facing
//! \return Whether the hit point was front or back facing
inline bool HitRecord::IsFrontFace() const{
    return front_face_;
}

//! \brief Get hit surface
//! \return Hit surface
inline std::shared_ptr<Surface> HitRecord::GetSurface() const{
    return surface_;
}

// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

}  // namespace core
}  // namespace olio
