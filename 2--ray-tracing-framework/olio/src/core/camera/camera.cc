// ======================================================================
// Olio: Simple renderer
//
// ======================================================================

//! \file       camera.cc
//! \brief      Camera class

#include "core/camera/camera.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <cmath>

namespace olio {
namespace core {

using namespace std;

// ======================================================================
// *** Homework: implement Camera class' member functions here
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

//! \brief Constructor
//! \param[in] name Node name
Camera::Camera(const std::string &name)
    : Node(name) {
        name_ = name.empty() ? "Camera" : name;
    }

//! \brief Constructor
//! \details Set up the camera using the input (LookAt) parameters.
//! \param[in] eye Eye (center of projection) position
//! \param[in] target Target/reference position for camera to look at
//! \param[in] up The up vector
//! \param[in] fovy y field of view in degrees
//! \param[in] aspect viewport aspect ratio
//! \param[in] name Node name
Camera::Camera(const Vec3r &eye, const Vec3r &target, const Vec3r &up,
	Real fovy, Real aspect, const std::string &name)
    : Node(name), eye_(eye), target_(target), up_(up), fovy_(fovy), aspect_(aspect) 
    {
        LookAt(eye, target, up, true);
    }

//! \brief Set up camera using the provided LookAt parameters
//! \param[in] eye Eye (center of projection) position
//! \param[in] target Target/reference position for camera to look at
//! \param[in] up The up vector
//! \param[in] update_viewport Whether to also compute/update the
//!            viewport parameters
void Camera::LookAt(const Vec3r &eye, const Vec3r &target, const Vec3r &up, bool update_viewport) {
    eye_ = eye;
    target_ = target;
    up_ = up;

    Vec3r w = (eye - target).normalized();
    Vec3r u = up.cross(w).normalized();
    Vec3r v = w.cross(u);

    Mat3r rotation_matrix;
    rotation_matrix.col(0) = u;
    rotation_matrix.col(1) = v;
    rotation_matrix.col(2) = w;
    camera_xform_.block<3, 3>(0, 0) = rotation_matrix; //Rotation
    camera_xform_.block<3, 1>(0, 3) << eye_(0), eye_(1), eye_(2); // Translation

    if (update_viewport){
        UpdateViewport();
    }
}

//! \brief Set vertical field of view
//! \param[in] fovy Vertical field of view in degrees
//! \param[in] update_viewport Whether to update viewport parameters
void Camera::SetFovy(Real fovy, bool update_viewport){
    fovy_ = fovy;
    if (update_viewport){
        UpdateViewport();
    }
}

//! \brief Set viewport aspect ratio
//! \param[in] aspect viewport aspect ratio
//! \param[in] update_viewport Whether to update viewport parameters
void Camera::SetAspect(Real aspect, bool update_viewport){
    aspect_ = aspect;
    if (update_viewport){
        UpdateViewport();
    }
  }

//! \brief Get eye (cop) position
//! \return Eye position
Vec3r Camera::GetEye() const {
    return eye_;
  }

//! \brief Get target/reference position
//! \return Target position
Vec3r Camera::GetTarget() const {
    return target_;
  }

//! \brief Get up vector
//! \return Up vector
Vec3r Camera::GetUpVector() const {
    return up_;
  }

//! \brief Get camera xform, which describes the eye position and
//!        the u, v, w axes
//! \return 4x4 camera matrix
Mat4r Camera::GetCameraXform() const {
    return camera_xform_;
}

//! \brief Get the vertical field of view
//! \return Vertical field of view in degrees
Real Camera::GetFovy() const {
    return fovy_;
}

//! \brief Get the viewport aspect ratio
//! \return Viewport aspect ratio
Real Camera::GetAspectRatio() const {
    return aspect_;
}


//! \brief Update viewport parameters using current values of
//!        camera_xform_, fovy_, and aspect_
//! \details Parameters that will be updated by the function:
//!    * horizontal_: viewport's horizontal axis
//!    * vertical_: viewport's vertical axis
//!    * lower_left_corner_: lower left corner of the viewport
void Camera::UpdateViewport(){
    Real H = 2*tan(fovy_*M_PI/360);
    Real W = aspect_*H;
    Vec3r u = camera_xform_.block<3, 1>(0, 0);
    Vec3r v = camera_xform_.block<3, 1>(0, 1);
    Vec3r w = camera_xform_.block<3, 1>(0, 2);
    Vec3r e = camera_xform_.block<3, 1>(0, 3);

    horizontal_ = u*W;
    vertical_ = v*H;
    lower_left_corner_ = e - w - 0.5*(horizontal_ + vertical_);
}




/***********My extra functions for testing
Vec3r Camera::GetLowerLeftCorner() const {
    return lower_left_corner_;
}

Vec3r Camera::GetHorizontal() const {
    return horizontal_;
}

Vec3r Camera::GetVertical() const {
    return vertical_;
}
*/

// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

}  // namespace core
}  // namespace olio
