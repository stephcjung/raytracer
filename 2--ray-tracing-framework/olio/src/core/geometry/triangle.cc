// ======================================================================
// Olio: Simple renderer
//
// ======================================================================

//! \file       triangle.cc
//! \brief      Triangle class

#include <iostream>

#include "core/geometry/triangle.h"
#include <spdlog/spdlog.h>
#include "core/ray.h"

namespace olio {
namespace core {

// ======================================================================
// *** Homework: implement all of Triangle class' member functions here
// ======================================================================
// ***** START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

//! \brief Constructor
//! \param[in] name Node name
Triangle::Triangle(const std::string &name)
    : Surface(name) {
        name_ = name.empty() ? "Triangle" : name;
    }

  //! \brief Constructor
  //! \details The constructor should also compute the face/triangle
  //! normal by calling ComputeNormal(). Points should be ordered
  //! counterclockwise wrt triangle normal.
  //! \param[in] point0 first triangle point
  //! \param[in] point1 second triangle point
  //! \param[in] point2 third triangle point
  //! \param[in] name Node name
Triangle::Triangle(const Vec3r &point0, const Vec3r &point1, const Vec3r &point2, const std::string &name) 
    : Surface(name), point0_(point0), point1_(point1), point2_(point2) {
        name_ = name.empty() ? "Triangle" : name;
        ComputeNormal();
    }

void Triangle::ComputeNormal(){
    //! \brief Compute/update triangle normal
    Vec3r AB = point1_ - point0_;
    Vec3r AC = point2_ - point0_;
    normal_ = AB.cross(AC).normalized();
}


  //! \brief Static function to compute ray-triangle intersection
  //!        between input ray and triangle defined by the three input
  //!        points.
  //! \param[in] p0 first triangle point
  //! \param[in] p1 second triangle point
  //! \param[in] p2 third triangle point
  //! \param[in] ray Input ray
  //! \param[in] tmin Minimum acceptable value for ray_t
  //! \param[in] tmax Maximum acceptable value for ray_t
  //! \param[out] ray_t In case of intersection, value of t for hit point p:
  //!             p = ray_origin + t * ray_dir
  //! \param[out] uv UV coordinates of the hit point inside the
  //!             triangle. Can be used to compute the barycentric
  //!             coordinates as: (1 - uv[0] - uv[1], uv[0], uv[1])
  //! \return True on success
bool Triangle::RayTriangleHit(const Vec3r &p0, const Vec3r &p1, const Vec3r &p2,
	const Ray &ray, Real tmin, Real tmax, Real &ray_t, Vec2r &uv){
    
    Vec3r u = p1-p0;
    Real u_x = u[0];
    Real u_y = u[1];
    Real u_z = u[2];
    //std::cout << "u: " << u.transpose() << std::endl;

    Vec3r v = p2-p0;
    Real v_x = v[0];
    Real v_y = v[1];
    Real v_z = v[2];
    //std::cout << "v: " << v.transpose() << std::endl;

    Vec3r d = ray.GetDirection();
    Real d_x = d[0];
    Real d_y = d[1];
    Real d_z = d[2];
    //std::cout << "ray origin: " << ray.GetOrigin().transpose() << std::endl;
    //std::cout << "ray dir: " << d.transpose() << std::endl;

    //q = p_r - a
    Vec3r q = ray.GetOrigin() - p0;
    Real q_x = q[0];
    Real q_y = q[1];
    Real q_z = q[2];
    //std::cout << "q=p_r - a: " << q.transpose() << std::endl;

    Real ei_hf = d_y*v_z - v_y*d_z;
    Real gf_di = v_x*d_z - d_x*v_z;
    Real dh_eg = v_y*d_x - v_x*d_y;
    Real ak_jb = u_x*q_y - q_x*u_y;
    Real jc_al = q_x*u_z - u_x*q_z;
    Real bl_kc = u_y*q_z - q_y*u_z;

    Real M = u_x*ei_hf + u_y*gf_di + u_z*dh_eg;
    
    Real t = -(v_z*ak_jb + v_y*jc_al + v_x*bl_kc)/M;
    //std::cout << "t: " << t << std::endl;
    if (t<tmin || t>tmax){
        return false;
    }
    Real gamma = (-d_z*ak_jb - d_y*jc_al - d_x*bl_kc)/M;
    //std::cout << "gamma: " << gamma << std::endl;
    if (gamma<0 || gamma>1){
        return false;
    }
    Real beta = (q_x*ei_hf + q_y*gf_di + q_z*dh_eg)/M;
    //std::cout << "beta: " << beta << std::endl;
    if (beta<0 || beta>(1-gamma)){
        return false;
    }
    //There was a valid hit
    ray_t = t;
    uv[0] = beta;
    uv[1] = gamma;
    return true;

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
bool Triangle::Hit(const Ray &ray, Real tmin, Real tmax, HitRecord &hit_record) {
    Vec3r point0, point1, point2;
    GetPoints(point0, point1, point2);
    Real t;
    Vec2r uv;
    if (RayTriangleHit(point0, point1, point2, ray, tmin, tmax, t, uv)){
        //std::cout << "Triangle hit true!" << std::endl;

        hit_record.SetRayT(t);
        hit_record.SetPoint((1 - uv[0] - uv[1])*point0 + uv[0]*point1 + uv[1]*point2);
        //std::cout << "GetNormal(): " << GetNormal() << std::endl;
        hit_record.SetNormal(ray, GetNormal());
        //std::cout << "hit_record.GetNormal(): " << hit_record.GetNormal() << std::endl;
        hit_record.SetSurface(GetPtr());
        return true;
    }
    //std::cout << "Triangle hit false!" << std::endl;
    return false;

}

  //! \brief Set triangle points
  //! \details The function should also compute/update the triangle
  //!          normal by calling 'ComputeNormal()'
  //! \param[in] point0 first triangle point
  //! \param[in] point1 second triangle point
  //! \param[in] point2 third triangle point
void Triangle::SetPoints(const Vec3r &point0, const Vec3r &point1, const Vec3r &point2){
    point0_ = point0;
    point1_ = point1;
    point2_ = point2;
    ComputeNormal();
}

  //! \brief Get triangle points
  //! \param[out] point0 first triangle point
  //! \param[out] point1 second triangle point
  //! \param[out] point2 third triangle point
void Triangle::GetPoints(Vec3r &point0, Vec3r &point1, Vec3r &point2) const{
    point0 = point0_;
    point1 = point1_;
    point2 = point2_;
}

  //! \brief Get triangle normal
  //! \return Triangle normal
Vec3r Triangle::GetNormal() const{
    return normal_;
}

// ***** END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE) *****

}  // namespace core
}  // namespace olio
