// ======================================================================
// Olio: Simple renderer
//
// Author: Stephanie Jung, 2025
// ======================================================================

//! \file       surface_list.cc
//! \brief      Surface List
//! \author     Stephanie Jung, 2025

#include "core/geometry/surface_list.h"
#include <spdlog/spdlog.h>
#include "core/ray.h"

namespace olio {
namespace core {


SurfaceList::SurfaceList(const std::vector<Surface::Ptr> &surfaces, const std::string &name) :
  Surface{},
  surfaces_{surfaces}
{
  name_ = name.size() ? name : "SurfaceList";
}

bool SurfaceList::Hit(const Ray &ray, Real tmin, Real tmax, HitRecord &hit_record){
  bool hit_something = false;
  HitRecord current_hit_record;
  for (auto& surface : surfaces_){
    if (surface->Hit(ray, tmin, tmax, current_hit_record)){
      hit_record = current_hit_record;
      tmax = current_hit_record.GetRayT();
      hit_something = true;
    }
  }
  return hit_something;
}


}  // namespace core
}  // namespace olio