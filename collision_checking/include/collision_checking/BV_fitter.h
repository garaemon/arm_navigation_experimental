
/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Jia Pan */

#ifndef COLLISION_CHECKING_BV_FITTER_H
#define COLLISION_CHECKING_BV_FITTER_H

#include "collision_checking/BVH_defs.h"
#include "collision_checking/primitive.h"
#include "collision_checking/vec_3f.h"
#include "collision_checking/obb.h"
#include "collision_checking/rss.h"
#include <iostream>

/** \brief Main namespace */
namespace collision_checking
{

/** \brief A class for fitting a bounding volume to a set of points  */
template<typename BV>
class BVFitter
{
public:

  /** \brief Compute a bounding volume that fits a set of n points. */
  static BV fit(Point* ps, int n)
  {
    BV bv;
    for(int i = 0; i < n; ++i)
    {
      Point p = ps[i];
      bv += Vec3f(p[0], p[1], p[2]);
    }

    return bv;
  }

  /** \brief Prepare the geometry primitive data for fitting */
  void set(Point* vertices_, Triangle* tri_indices_, BVHModelType type_)
  {
    vertices = vertices_;
    prev_vertices = NULL;
    tri_indices = tri_indices_;
    type = type_;
  }

  /** \brief Prepare the geometry primitive data for fitting */
  void set(Point* vertices_, Point* prev_vertices_, Triangle* tri_indices_, BVHModelType type_)
  {
    vertices = vertices_;
    prev_vertices = prev_vertices_;
    tri_indices = tri_indices_;
    type = type_;
  }

  /** \brief Compute a bounding volume that fits a set of primitives (points or triangles).
   * The primitive data was set by set function and primitive_indices is the primitive index relative to the data
   */
  BV fit(unsigned int* primitive_indices, int num_primitives)
  {
    BV bv;

    if(type == BVH_MODEL_TRIANGLES)             /* The primitive is triangle */
    {
      for(int i = 0; i < num_primitives; ++i)
      {
        Triangle t = tri_indices[primitive_indices[i]];
        Point v = vertices[t[0]];
        bv += Vec3f(v[0], v[1], v[2]);
        v = vertices[t[1]];
        bv += Vec3f(v[0], v[1], v[2]);
        v = vertices[t[2]];
        bv += Vec3f(v[0], v[1], v[2]);

        if(prev_vertices) /* When fitting both current and previous frame */
        {
          v = prev_vertices[t[0]];
          bv += Vec3f(v[0], v[1], v[2]);
          v = prev_vertices[t[1]];
          bv += Vec3f(v[0], v[1], v[2]);
          v = prev_vertices[t[2]];
          bv += Vec3f(v[0], v[1], v[2]);
        }
      }
    }
    else if(type == BVH_MODEL_POINTCLOUD)       /* The primitive is point */
    {
      for(int i = 0; i < num_primitives; ++i)
      {
        Point v = vertices[primitive_indices[i]];
        bv += Vec3f(v[0], v[1], v[2]);

        if(prev_vertices) /* When fitting both current and previous frame */
        {
          v = prev_vertices[primitive_indices[i]];
          bv += Vec3f(v[0], v[1], v[2]);
        }
      }
    }

    return bv;
  }

  /** \brief Clear the geometry primitive data */
  void clear()
  {
    vertices = NULL;
    prev_vertices = NULL;
    tri_indices = NULL;
    type = BVH_MODEL_UNKNOWN;
  }

private:

  Point* vertices;
  Point* prev_vertices;
  Triangle* tri_indices;
  BVHModelType type;
};


/** \brief Specification of BVFitter for OBB bounding volume */
template<>
class BVFitter<OBB>
{
public:

  /** \brief Compute a bounding volume that fits a set of n points. */
  static OBB fit(Point* ps, int n);

  /** \brief Prepare the geometry primitive data for fitting */
  void set(Point* vertices_, Triangle* tri_indices_, BVHModelType type_)
  {
    vertices = vertices_;
    prev_vertices = NULL;
    tri_indices = tri_indices_;
    type = type_;
  }

  /** \brief Prepare the geometry primitive data for fitting */
  void set(Point* vertices_, Point* prev_vertices_, Triangle* tri_indices_, BVHModelType type_)
  {
    vertices = vertices_;
    prev_vertices = prev_vertices_;
    tri_indices = tri_indices_;
    type = type_;
  }

  /** \brief Compute a bounding volume that fits a set of primitives (points or triangles).
   * The primitive data was set by set function and primitive_indices is the primitive index relative to the data.
   */
  OBB fit(unsigned int* primitive_indices, int num_primitives);

  /** \brief Clear the geometry primitive data */
  void clear()
  {
    vertices = NULL;
    prev_vertices = NULL;
    tri_indices = NULL;
    type = BVH_MODEL_UNKNOWN;
  }

private:

  Point* vertices;
  Point* prev_vertices;
  Triangle* tri_indices;
  BVHModelType type;

  /** \brief Fit OBB for one point */
  static inline OBB fit1(Point* ps);

  /** \brief Fit OBB for two points */
  static inline OBB fit2(Point* ps);

  /** \brief Fit OBB for three point (one triangle) */
  static inline OBB fit3(Point* ps);

  /** \brief Fit OBB for six point (two triangles) */
  static inline OBB fit6(Point* ps);

  /** \brief Fit OBB for n points */
  static OBB fitn(Point* ps, int n);
};


/** \brief Specification of BVFitter for RSS bounding volume */
template<>
class BVFitter<RSS>
{
public:

  /** \brief Compute a bounding volume that fits a set of n points. */
  static RSS fit(Point* ps, int n);

  /** \brief Prepare the geometry primitive data for fitting */
  void set(Point* vertices_, Triangle* tri_indices_, BVHModelType type_)
  {
    vertices = vertices_;
    prev_vertices = NULL;
    tri_indices = tri_indices_;
    type = type_;
  }

  /** \brief Prepare the geometry primitive data for fitting */
  void set(Point* vertices_, Point* prev_vertices_, Triangle* tri_indices_, BVHModelType type_)
  {
    vertices = vertices_;
    prev_vertices = prev_vertices_;
    tri_indices = tri_indices_;
    type = type_;
  }

  /** \brief Compute a bounding volume that fits a set of primitives (points or triangles).
   * The primitive data was set by set function and primitive_indices is the primitive index relative to the data.
   */
  RSS fit(unsigned int* primitive_indices, int num_primitives);

  /** \brief Clear the geometry primitive data */
  void clear()
  {
    vertices = NULL;
    prev_vertices = NULL;
    tri_indices = NULL;
    type = BVH_MODEL_UNKNOWN;
  }

private:

  Point* vertices;
  Point* prev_vertices;
  Triangle* tri_indices;
  BVHModelType type;

  /** \brief Fit RSS for one point */
  static RSS fit1(Point* ps);

  static RSS fit2(Point* ps);

  static RSS fit3(Point* ps);

  /** \brief Fit RSS for n points */
  static RSS fitn(Point* ps, int n);

};

}

#endif