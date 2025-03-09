// ======================================================================
// Olio: Simple renderer
//
// Author: Stephanie Jung, 2025
// ======================================================================

//! \file       surface_list.h
//! \brief      Surface List class
//! \author     Stephanie Jung, 2025

#pragma once

#include <memory>
#include <string>

#include "core/geometry/surface.h"

namespace olio {
namespace core {

//! \class Sphere
//! \brief Sphere class
class SurfaceList : public Surface {
public:
  OLIO_NODE(SurfaceList)

  //! \brief Constructor
  //! \param[in] surfaces Reference to list of surfaces
  //! \param[in] name Node name
  SurfaceList(const std::vector<Surface::Ptr> &surfaces, const std::string &name=std::string());

  //! \brief Check if ray intersects with surface. This function should iterate through all the surfaces in surfaces_ and find out if the input ray hits any of them. If it does, the hit_record by the function should correspond to the surface that’s closest: the surface whose hit record has the smallest fractional ray distance (ray_t)
  //! \details If the ray intersections the surface, the function
  //!          should fill in the 'hit_record' (i.e., information
  //!          about the hit point, normal, etc.)
  //! \param[in] ray Ray to check intersection against
  //! \param[in] tmin Minimum value for acceptable t (ray fractional distance)
  //! \param[in] tmax Maximum value for acceptable t (ray fractional distance)
  //! \param[out] hit_record Resulting hit record if ray intersected with surface
  //! \return True if ray intersected with surface
  bool Hit(const Ray &ray, Real tmin, Real tmax,
	   HitRecord &hit_record) override;

protected:
  std::vector<Surface::Ptr> surfaces_;  //!< list of surfaces
private:
};

}  // namespace core
}  // namespace olio
