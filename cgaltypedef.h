#ifndef CGALTYPEDEF_H
#define CGALTYPEDEF_H

/* Arrangement */
#include <CGAL/Arrangement_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_walk_along_line_point_location.h>
#include <CGAL/Arr_point_location_result.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel Epeck;
typedef CGAL::Arr_segment_traits_2<Epeck> Traits;
typedef CGAL::Arrangement_2<Traits> Arrangement;
typedef CGAL::Arr_walk_along_line_point_location<Arrangement> Location;
typedef CGAL::Arr_point_location_result<Arrangement> Result;
typedef Result::Type Result_Type;
typedef Arrangement::Vertex_const_handle Vertex_const_handle;
typedef Arrangement::Halfedge_const_handle Halfedge_const_handle;
typedef Arrangement::Face_const_handle Face_const_handle;


/* Triangulation */
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Constrained_triangulation_face_base_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_face_base_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_vertex_base_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/lloyd_optimize_mesh_2.h>

class VertInfo;
class FaceInfo;

typedef CGAL::Exact_predicates_inexact_constructions_kernel Epick;
typedef CGAL::Triangulation_vertex_base_with_info_2<VertInfo, Epick> TVBI;
typedef CGAL::Delaunay_mesh_vertex_base_2<Epick, TVBI> DMVB;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo, Epick> TFBI;
typedef CGAL::Constrained_triangulation_face_base_2<Epick, TFBI> CTFB;
typedef CGAL::Constrained_Delaunay_triangulation_face_base_2<Epick, CTFB> CDTFB;
typedef CGAL::Delaunay_mesh_face_base_2<Epick, CDTFB> DMFB;

/* Constrained Delaunay Triangulation */
typedef CGAL::Exact_intersections_tag EItag;
typedef CGAL::Triangulation_data_structure_2<DMVB, DMFB> TDS;
typedef CGAL::Constrained_Delaunay_triangulation_2<Epick, TDS, EItag> CDT;
typedef CGAL::Constrained_triangulation_plus_2<CDT> CDTP;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDTP> Criteria;
typedef CGAL::Delaunay_mesher_2<CDTP, Criteria> Mesher;
typedef CGAL::Mesh_optimization_return_code Opt_code;

/* Constrained Triangulation */
typedef CGAL::No_intersection_tag NItag;
typedef CGAL::Triangulation_data_structure_2<TVBI, CTFB> CTDS;
typedef CGAL::Constrained_triangulation_2<Epick, CTDS, NItag> CT;

/* Voronoi Diagram */
#include "CGAL/Delaunay_triangulation_2.h"
#include "CGAL/Voronoi_diagram_2.h"
#include "CGAL/Delaunay_triangulation_adaptation_traits_2.h"
#include "CGAL/Delaunay_triangulation_adaptation_policies_2.h"

typedef CGAL::Delaunay_triangulation_2<Epick>                                             DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                              AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT>              AP;
typedef CGAL::Voronoi_diagram_2<DT, AT, AP>                                               VD;

#endif // CGALTYPEDEF_H
