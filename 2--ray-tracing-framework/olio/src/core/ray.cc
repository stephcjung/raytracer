// ======================================================================
// Olio: Simple renderer
//
// ======================================================================

//! \file       ray.cc
//! \brief      Ray class

#include "core/ray.h"
#include <spdlog/spdlog.h>

namespace olio {
namespace core {

using namespace std;

// ======================================================================
// *** Homework: implement Ray class' parameterized constructor here
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

Ray::Ray(const Vec3r &origin, const Vec3r &dir) :
    origin_(origin), dir_(dir) {}

// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****


// ======================================================================
// *** Homework: implement HitRecord class' parameterized constructor here
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

HitRecord::HitRecord(const Ray &ray, Real ray_t, const Vec3r &point, const Vec3r &face_normal) :
        ray_t_(ray_t), point_(point), surface_(nullptr)
        {
            SetNormal(ray, face_normal); //initializes normal_ and front_face_
        }


// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

}  // namespace core
}  // namespace olio
