#ifndef VORONOIDIAGRAM_H
#define VORONOIDIAGRAM_H

#include <QLineF>
#include "cgaltypedef.h"

//A class to recover Voronoi diagram from stream.
//Rays, lines and segments are cropped to a rectangle
//so that only segments are stored
struct Cropped_voronoi_from_delaunay
{
  std::list<Epick::Segment_2> m_cropped_vd;
  Epick::Iso_rectangle_2 m_bbox;

  Cropped_voronoi_from_delaunay(const Epick::Iso_rectangle_2& bbox):m_bbox(bbox){}
  template <class RSL>
  void crop_and_extract_segment(const RSL& rsl)
  {
    CGAL::Object obj = CGAL::intersection(rsl,m_bbox);
    const Epick::Segment_2* s=CGAL::object_cast<Epick::Segment_2>(&obj);
    if (s) m_cropped_vd.push_back(*s);
  }

  void operator<<(const Epick::Ray_2& ray)
  { crop_and_extract_segment(ray); }

  void operator<<(const Epick::Line_2& line)
  { crop_and_extract_segment(line); }

  void operator<<(const Epick::Segment_2& seg)
  { crop_and_extract_segment(seg); }
};

class Triangulation;
class VoronoiDiagram
{
public:
    VoronoiDiagram();

    VD GetVoronoiDiagram() const;
    std::vector<QLineF> GetCroppedVoronoi() const;

    void Construct(Triangulation *dt);
    void CropBoundary(double *bbox);

private:
    VD vd;
    std::vector<QLineF> cropped_vd;
};

#endif // VORONOIDIAGRAM_H
