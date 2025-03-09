// ======================================================================
// Olio: Simple renderer
//
// ======================================================================

//! \file       main.cc
//! \brief      main tests file

#include <iostream>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "core/types.h"
#include "core/ray.h"
#include "core/geometry/surface.h"
#include "core/geometry/sphere.h"
#include "core/geometry/triangle.h"
#include "core/camera/camera.h"

using namespace std;
using namespace olio;
using namespace olio::core;

TEST_CASE("DoNothing") {
}

// ==============================================================================
// *** Test cases 1: Ray and HitRecord classes ***
// ==============================================================================
TEST_CASE("TestRay", "[Ray]") {
  SECTION("Constructors") {
    Ray r1;
    Ray r2{Vec3r{0, 0, 0}, Vec3r{1, 1, 1}};
    
    // Test default constructor
    REQUIRE(r1.GetOrigin() == Vec3r{0, 0, 0});
    REQUIRE(r1.GetDirection() == Vec3r{0, 0, 0});
    
    // Test parameterized constructor
    REQUIRE(r2.GetOrigin() == Vec3r{0, 0, 0});
    REQUIRE(r2.GetDirection() == Vec3r{1, 1, 1});
  }

  SECTION("SetOrigin and SetDirection") {
    Ray r1;
    r1.SetOrigin(Vec3r{1, 2, 3});
    r1.SetDirection(Vec3r{4, 5, 6});
    
    REQUIRE(r1.GetOrigin() == Vec3r{1, 2, 3});
    REQUIRE(r1.GetDirection() == Vec3r{4, 5, 6});
  }

  SECTION("At()") {
    Ray r1{Vec3r{1, 2, 3}, Vec3r{4, 5, 6}};
    Vec3r point = r1.At(2.0);
    REQUIRE(point == Vec3r{9, 12, 15}); // At(t) = origin + t * direction
  }
}

TEST_CASE("TestHitRecord", "[HitRecord]") {
  SECTION("Constructors") {
    Ray r1{Vec3r{1, 2, 3}, Vec3r{0, 0, 1}};
    Vec3r hit_point{1, 2, 4};
    Vec3r normal{0, 0, -1};
    HitRecord hit_record{r1, 1.0, hit_point, normal};
    
    // Test constructor
    REQUIRE(hit_record.GetRayT() == 1.0);
    REQUIRE(hit_record.GetPoint() == Vec3r{1, 2, 4});
    REQUIRE(hit_record.GetNormal() == Vec3r{0, 0, -1});
    REQUIRE(hit_record.IsFrontFace() == true);
  }

  SECTION("SetRayT and SetPoint") {
    Ray r1{Vec3r{1, 1, 1}, Vec3r{0, 0, 1}};
    HitRecord hit_record;
    hit_record.SetRayT(2.0);
    hit_record.SetPoint(Vec3r{1, 1, 3});
    
    REQUIRE(hit_record.GetRayT() == 2.0);
    REQUIRE(hit_record.GetPoint() == Vec3r{1, 1, 3});
  }

  SECTION("SetNormal (Ray, Face Normal)") {
    Ray r1{Vec3r{1, 1, 1}, Vec3r{0, 0, 1}};
    HitRecord hit_record;
    Vec3r face_normal{0, 0, -1};
    
    hit_record.SetNormal(r1, face_normal);
    
    // Test normal facing towards the ray (dot product negative)
    REQUIRE(hit_record.GetNormal() == Vec3r{0, 0, -1});
    REQUIRE(hit_record.IsFrontFace() == true);
  }

  SECTION("SetNormal (Vec3r, bool)") {
    HitRecord hit_record;
    Vec3r face_normal{0, 0, 1};
    
    // Test with front_face set to false
    hit_record.SetNormal(face_normal, false);
    REQUIRE(hit_record.GetNormal() == Vec3r{0, 0, -1});
    REQUIRE(hit_record.IsFrontFace() == false);

    // Test with front_face set to true
    hit_record.SetNormal(face_normal, true);
    REQUIRE(hit_record.GetNormal() == Vec3r{0, 0, 1});
    REQUIRE(hit_record.IsFrontFace() == true);
  }

  SECTION("SetSurface") {
    std::shared_ptr<Surface> surface = std::make_shared<Surface>();  // Assuming Surface is a valid class
    HitRecord hit_record;
    hit_record.SetSurface(surface);
    
    REQUIRE(hit_record.GetSurface() == surface);
  }
}

// ==============================================================================
// *** Test cases 2: Sphere class ***
// ==============================================================================
TEST_CASE("TestSphere", "[Sphere]") {
  //test with a ray intersecting our sphere
  SECTION("Sphere Intersection") {
    Sphere::Ptr sphere = Sphere::Create(Vec3r{1, -4, 0}, 2);
    Ray ray{Vec3r{1,0,0}, Vec3r{0,-1,0}};
    HitRecord hit_record;
    bool result = sphere->Hit(ray, 0, kInfinity, hit_record);
    REQUIRE(result); //ray should hit the surface
    REQUIRE_THAT(hit_record.GetRayT(), Catch::WithinAbs(2, kEpsilon)); //t-val should be approximately 2, with error bound of kEpsilon
    REQUIRE(hit_record.GetPoint().isApprox(Vec3r{1,-2,0}, kEpsilon));
    //std::cout << "Hit normal 1: " << hit_record.GetNormal() << std::endl;
    REQUIRE(hit_record.GetNormal().isApprox(Vec3r{0,1,0}, kEpsilon));
    REQUIRE(hit_record.IsFrontFace());
    REQUIRE(hit_record.GetSurface() == sphere); //Surface of hit record should be our sphere
  }

  //test with a ray not intersecting the sphere
  SECTION("Sphere Non-Intersection"){
    Sphere::Ptr sphere = Sphere::Create(Vec3r{1, -4, 0}, 2);
    Ray ray{Vec3r{1,0,0}, Vec3r{0,1,0}};
    HitRecord hit_record;
    bool result = sphere->Hit(ray, 0, kInfinity, hit_record);
    REQUIRE_FALSE(result); //ray should not hit the surface
  }

  //test with a ray that grazes the sphere (intersects 1 time)
  SECTION("Sphere Grazing Intersection") {
    Sphere::Ptr sphere = Sphere::Create(Vec3r{1, -4, 0}, 2);
    Ray ray{Vec3r{3,0,0}, Vec3r{0,-1,0}};
    HitRecord hit_record;
    bool result = sphere->Hit(ray, 0, kInfinity, hit_record);
    REQUIRE(result); //ray should hit the surface
    REQUIRE_THAT(hit_record.GetRayT(), Catch::WithinAbs(4, kEpsilon)); //t-val should be approximately 2, with error bound of kEpsilon
    REQUIRE(hit_record.GetPoint().isApprox(Vec3r{3,-4,0}, kEpsilon));
    //std::cout << "Hit normal 2: " << hit_record.GetNormal() << std::endl;
    REQUIRE(hit_record.GetNormal().isApprox(Vec3r{1,0,0}, kEpsilon));
    REQUIRE(hit_record.IsFrontFace());
    REQUIRE(hit_record.GetSurface() == sphere); //Surface of hit record should be our sphere
  }

  //test with a ray not intersecting the sphere (too short)
  SECTION("Sphere Non-Intersection (too short)"){
    Sphere::Ptr sphere = Sphere::Create(Vec3r{1, -4, 0}, 2);
    Ray ray{Vec3r{1,0,0}, Vec3r{0,-1,0}};
    HitRecord hit_record;
    bool result = sphere->Hit(ray, 0, 1, hit_record);
    REQUIRE_FALSE(result); //ray should not hit the surface
  }

  // Test for ray shooting from inside the sphere
  SECTION("Ray Shooting from Inside Sphere") {
    // Create the sphere with center (1, -4, 0) and radius 2
    Sphere::Ptr sphere = Sphere::Create(Vec3r{1, -4, 0}, 2);
    Ray ray{Vec3r{1, -4, 0}, Vec3r{0, 1, 0}};
    HitRecord hit_record;
    bool result = sphere->Hit(ray, 0, kInfinity, hit_record);
    
    REQUIRE(result);  // There should be an intersection (ray is inside the sphere)
    REQUIRE_THAT(hit_record.GetRayT(), Catch::WithinAbs(2, kEpsilon));
    REQUIRE(hit_record.GetPoint().isApprox(Vec3r{1, -2, 0}, kEpsilon));  // Point where the ray enters the sphere
    REQUIRE(hit_record.GetNormal().isApprox(Vec3r{0, -1, 0}, kEpsilon));  // NOTE, Normal will always be facing the sphere
    REQUIRE_FALSE(hit_record.IsFrontFace());  // The front face is false (we are entering the sphere)
  }
}


// ==============================================================================
// *** Test cases 3: Triangle class ***
// ==============================================================================
TEST_CASE("TestTriangle", "[Triangle]") {
  
  // Test with a ray intersecting the triangle
  SECTION("Triangle Intersection") {
    // Create a triangle with three points
    Triangle::Ptr triangle = Triangle::Create(Vec3r{0, 0, 0}, Vec3r{2, 0, 0}, Vec3r{1, 2, 0});
    // Define a ray that intersects the triangle
    Ray ray{Vec3r{1, 1, -1}, Vec3r{0, 0, 1}};
    HitRecord hit_record;
    bool result = triangle->Hit(ray, 0, kInfinity, hit_record);
    
    REQUIRE(result);  // Ray should hit the triangle
    REQUIRE_THAT(hit_record.GetRayT(), Catch::WithinAbs(1.0, kEpsilon));  // t-value should be approximately 1
    REQUIRE(hit_record.GetPoint().isApprox(Vec3r{1, 1, 0}, kEpsilon));  // Point should be (1, 1, 0)
    //std::cout << "Normal vec 1: " << hit_record.GetNormal().transpose() << std::endl;
    REQUIRE(hit_record.GetNormal().isApprox(Vec3r{0, 0, -1}, kEpsilon));  
    // NOTE: Normal should be (0, 0, 1) but will be recorded as (0,0,-1) so that it is facing the ray
    REQUIRE_FALSE(hit_record.IsFrontFace());  // Should not be the front face
    REQUIRE(hit_record.GetSurface() == triangle);  // The surface should be the triangle
  }

  // Test with a ray not intersecting the triangle
  SECTION("Triangle Non-Intersection") {
    // Create a triangle with three points
    Triangle::Ptr triangle = Triangle::Create(Vec3r{0, 0, 0}, Vec3r{2, 0, 0}, Vec3r{1, 2, 0});
    // Define a ray that does not intersect the triangle
    Ray ray{Vec3r{0, -1, 1}, Vec3r{0, 0, -1}};
    HitRecord hit_record;
    bool result = triangle->Hit(ray, 0, kInfinity, hit_record);
    
    REQUIRE_FALSE(result);  // Ray should not hit the triangle
  }

  // Test with a ray grazing the triangle at a vertex
  SECTION("Triangle Grazing Intersection") {
    // Create a triangle with three points
    Triangle::Ptr triangle = Triangle::Create(Vec3r{0, 0, 0}, Vec3r{2, 0, 0}, Vec3r{1, 2, 0});
    // Define a ray that grazes the triangle
    Ray ray{Vec3r{2, 0, -1}, Vec3r{0, 0, 1}};
    HitRecord hit_record;
    bool result = triangle->Hit(ray, 0, kInfinity, hit_record);
    
    REQUIRE(result);  // Ray should hit the triangle
    REQUIRE_THAT(hit_record.GetRayT(), Catch::WithinAbs(1.0, kEpsilon));  
    REQUIRE(hit_record.GetPoint().isApprox(Vec3r{2, 0, 0}, kEpsilon));  
    //std::cout << "Normal vec 2: " << hit_record.GetNormal().transpose() << std::endl;
    REQUIRE(hit_record.GetNormal().isApprox(Vec3r{0, 0, -1}, kEpsilon));  // NOTE: Normal should be (0, 0, 1) but will be recorded as (0,0,-1) so that it is facing the ray
    REQUIRE_FALSE(hit_record.IsFrontFace());  // Should be the front face
    REQUIRE(hit_record.GetSurface() == triangle);  // The surface should be the triangle
  }

  // Test with a ray that is too short to intersect the triangle
  SECTION("Triangle Non-Intersection (Too Short)") {
    // Create a triangle with three points
    Triangle::Ptr triangle = Triangle::Create(Vec3r{0, 0, 0}, Vec3r{2, 0, 0}, Vec3r{1, 2, 0});
    // Define a ray that is too short to intersect the triangle
    Ray ray{Vec3r{1, 1, 2}, Vec3r{0, 0, -1}};
    HitRecord hit_record;
    bool result = triangle->Hit(ray, 0, 1, hit_record);
    
    REQUIRE_FALSE(result);  // Ray should not hit the triangle
  }

  // Test with a ray that is going the opposite direction (triangle behind ray)
  SECTION("Triangle Non-Intersection (Behind camera)") {
    Triangle::Ptr triangle = Triangle::Create(Vec3r{10, 10, 10}, Vec3r{15, 20, 10}, Vec3r{20, 10, 10});
    Ray ray{Vec3r{15, 15, 0}, Vec3r{0, 0, -1}};
    HitRecord hit_record;
    bool result = triangle->Hit(ray, 0, kInfinity, hit_record);
    
    REQUIRE_FALSE(result);  // Ray should not hit the triangle
  }

  // Test with a ray intersecting with a triangle with large coordinates
  SECTION("Triangle Intersection with Large Coordinates") {
    // Create a triangle with large coordinates
    Triangle::Ptr triangle = Triangle::Create(Vec3r{10, 10, 10}, Vec3r{15, 20, 10}, Vec3r{20, 10, 10});
    // Define a ray that intersects the large triangle
    Ray ray{Vec3r{15, 15, 0}, Vec3r{0, 0, 1}};
    HitRecord hit_record;
    bool result = triangle->Hit(ray, 0, kInfinity, hit_record);
    
    REQUIRE(result);  // Ray should hit the triangle
    REQUIRE_THAT(hit_record.GetRayT(), Catch::WithinAbs(10.0, kEpsilon));  // t-value should be approximately 1
    REQUIRE(hit_record.GetPoint().isApprox(Vec3r{15, 15, 10}, kEpsilon));  // Point should be (15, 15, 10)
   //std::cout << "Normal vec 3: " << hit_record.GetNormal().transpose() << std::endl;
    REQUIRE(hit_record.GetNormal().isApprox(Vec3r{0, 0, -1}, kEpsilon));  // Normal will be (0, 0, -1), facing the ray
    REQUIRE(hit_record.IsFrontFace());  // Should be the front face
    REQUIRE(hit_record.GetSurface() == triangle);  // The surface should be the triangle
  }

}

// ==============================================================================
// *** Test cases for Camera class ***
// ==============================================================================
TEST_CASE("TestCamera", "[Camera]") {

  // Test Camera Constructor and LookAt Method
  SECTION("Camera LookAt") {
    // Create a camera with a LookAt setup
    Vec3r eye{0, 0, 5};
    Vec3r target{0, 0, 0};
    Vec3r up{0, 1, 0};
    Real fovy = 45.0;
    Real aspect = 1.0;

    Camera camera(eye, target, up, fovy, aspect);

    // Test if the camera's eye, target, and up vectors are set correctly
    REQUIRE(camera.GetEye().isApprox(eye, kEpsilon));
    REQUIRE(camera.GetTarget().isApprox(target, kEpsilon));
    REQUIRE(camera.GetUpVector().isApprox(up, kEpsilon));

    // Check if camera_xform_ correctly encodes the view matrix (rotation and translation)
    Mat4r xform = camera.GetCameraXform();
    Vec3r u = xform.block<3, 1>(0, 0);
    Vec3r v = xform.block<3, 1>(0, 1);
    Vec3r w = xform.block<3, 1>(0, 2);
    Vec3r eye_xform = xform.block<3, 1>(0, 3);

    // Check if camera's rotation (u, v, w) and translation are correct
    REQUIRE(u.isApprox(Vec3r{1, 0, 0}, kEpsilon));
    REQUIRE(v.isApprox(Vec3r{0, 1, 0}, kEpsilon));
    REQUIRE(w.isApprox(Vec3r{0, 0, 1}, kEpsilon));
    REQUIRE(eye_xform.isApprox(eye, kEpsilon));
  }

  // Test Field of View (fovy) and Aspect Ratio
  SECTION("Field of View and Aspect Ratio") {
    // Create camera with fovy and aspect ratio
    Camera camera(Vec3r{0, 0, 5}, Vec3r{0, 0, 0}, Vec3r{0, 1, 0}, 30.0, 1.5);
    camera.SetFovy(60.0);
    camera.SetAspect(1.77);

    // Test the vertical field of view and aspect ratio
    REQUIRE_THAT(camera.GetFovy(), Catch::WithinAbs(60.0, kEpsilon));
    REQUIRE_THAT(camera.GetAspectRatio(), Catch::WithinAbs(1.77, kEpsilon));
  }

  // Test Camera Transformation (with negative fovy)
  SECTION("Negative Fovy Handling") {
    // Create a camera with a negative fovy (which is unconventional but should be handled)
    Camera camera(Vec3r{0, 0, 5}, Vec3r{0, 0, 0}, Vec3r{0, 1, 0}, -60.0, 1.77);

    // Test the negative fovy value doesn't break calculations
    REQUIRE_THAT(camera.GetFovy(), Catch::WithinAbs(-60.0, kEpsilon));
    camera.UpdateViewport();

  }

  // Test Camera with Large Coordinates
  SECTION("Camera with Large Coordinates") {
    // Create a camera with large coordinates
    Camera camera(Vec3r{1000, 1000, 1000}, Vec3r{1000, 1000, 0}, Vec3r{0, 1, 0}, 45.0, 1.0);

    // Test if the camera's eye position and target are set correctly
    REQUIRE(camera.GetEye().isApprox(Vec3r{1000, 1000, 1000}, kEpsilon));
    REQUIRE(camera.GetTarget().isApprox(Vec3r{1000, 1000, 0}, kEpsilon));

    // Check the camera's view matrix for large coordinates
    Mat4r xform = camera.GetCameraXform();
    Vec3r u = xform.block<3, 1>(0, 0);
    Vec3r v = xform.block<3, 1>(0, 1);
    Vec3r w = xform.block<3, 1>(0, 2);
    Vec3r eye_xform = xform.block<3, 1>(0, 3);

    // Verify that the camera transformation matrix is correctly computed for large coordinates
    REQUIRE(eye_xform.isApprox(Vec3r{1000, 1000, 1000}, kEpsilon));
    REQUIRE(u.isApprox(Vec3r{1, 0, 0}, kEpsilon));
    REQUIRE(v.isApprox(Vec3r{0, 1, 0}, kEpsilon));
    REQUIRE(w.isApprox(Vec3r{0, 0, 1}, kEpsilon));
  }

  // Test GetRay (Ray generation through viewport)
  //Ray generated should go straight through center
  SECTION("Viewport Coordinates through Camera Ray Generation") {
    // Create a camera with a specific look at setup
    Camera camera(Vec3r{0, 0, 5}, Vec3r{0, 0, 0}, Vec3r{0, 1, 0}, 60.0, 1.77);

    // Define the coordinates s and t for a point in the viewport
    Real s = 0.5;  // middle of horizontal axis
    Real t = 0.5;  // middle of vertical axis

    // Get the ray through the point (s, t)
    Ray ray = camera.GetRay(s, t);

    // Check if the ray starts from the camera position (eye) and has the correct direction
    REQUIRE(ray.GetOrigin().isApprox(camera.GetEye(), kEpsilon));
    REQUIRE(ray.GetDirection().isApprox(Vec3r{0, 0, -1}));

    // Check if the ray passes through the expected point on the near plane
    Vec3r expected_point = camera.GetEye() + ray.GetDirection();
    REQUIRE((ray.GetOrigin()+1*ray.GetDirection()).isApprox(expected_point, kEpsilon)); //t=1 
  }

}

TEST_CASE("Test GetRay", "[Camera]") {

  // Define a camera for testing
  Camera camera(Vec3r{0, 0, 0}, Vec3r{0, 0, -1}, Vec3r{0, 1, 0}, 90.0, 1.0);

  // Define the expected values for the ray origin and direction
  Vec3r expected_origin = camera.GetEye();  // Should be the camera's eye position
  
  // Test for different (s, t) values
  SECTION("Ray through center of the viewport (s=0.5, t=0.5)") {
    Ray ray = camera.GetRay(0.5, 0.5);  // Get ray passing through the center of the viewport

    // Check that the ray origin is correct
    REQUIRE(ray.GetOrigin().isApprox(expected_origin, kEpsilon));

    // Check that the ray direction is correctly calculated for the center of the viewport
    //Vec3r center_point = camera.GetLowerLeftCorner() + 0.5 * camera.GetHorizontal() + 0.5 * camera.GetVertical();
    //REQUIRE(ray.GetDirection().isApprox(center_point - expected_origin, kEpsilon));
  }

  SECTION("Ray through bottom-left corner of the viewport (s=0, t=0)") {
    Ray ray = camera.GetRay(0.0, 0.0);  // Get ray passing through the bottom-left corner of the viewport

    // Check that the ray origin is correct
    REQUIRE(ray.GetOrigin().isApprox(expected_origin, kEpsilon));

    // Check that the ray direction is correctly calculated for the bottom-left corner of the viewport
    //Vec3r bottom_left_point = camera.GetLowerLeftCorner();  // (s=0, t=0) should correspond to the lower-left corner
    //REQUIRE(ray.GetDirection().isApprox(bottom_left_point - expected_origin, kEpsilon));
  }

  SECTION("Ray through top-right corner of the viewport (s=1, t=1)") {
    Ray ray = camera.GetRay(1.0, 1.0);  // Get ray passing through the top-right corner of the viewport

    // Check that the ray origin is correct
    REQUIRE(ray.GetOrigin().isApprox(expected_origin, kEpsilon));

    // Check that the ray direction is correctly calculated for the bottom-right corner of the viewport
    //Vec3r top_right_point = camera.GetLowerLeftCorner() + camera.GetHorizontal() + camera.GetVertical();
    //REQUIRE(ray.GetDirection().isApprox(top_right_point - expected_origin, kEpsilon));
  }
}

