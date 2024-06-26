#include "Geometry/MTDGeometryBuilder/interface/RectangularMTDTopology.h"

//--------------------------------------------------------------------
// PixelTopology interface.
// Transform LocalPoint in cm to measurement in pitch units.
std::pair<float, float> RectangularMTDTopology::pixel(const LocalPoint& p) const {
  // check limits
  float py = p.y();
  float px = p.x();

  // In Y
  float newybin = (py - m_yoffset) / m_pitchy;
  // In X
  float newxbin = (px - m_xoffset) / m_pitchx;

  return std::pair<float, float>(newxbin, newybin);
}

//provide pixel indices based on local module position (with protection for border positions)
std::pair<int, int> RectangularMTDTopology::pixelIndex(const LocalPoint& p) const {
  const auto& thepixel = pixel(p);
  bool fail(false);

  static constexpr float tol = 5e-5f;  // 0.5 micron tolerance

  int row = static_cast<int>(thepixel.first);
  if (row >= m_nrows) {
    fail = (p.x() - m_pitchx * m_nrows > tol);
    if (!fail) {
      row--;
    }
  }
  int col = static_cast<int>(thepixel.second);
  if (col >= m_ncols) {
    fail = (p.y() - m_pitchy * m_ncols > tol);
    if (!fail) {
      col--;
    }
  }
  if (fail) {
    throw cms::Exception("RectangularMTDTopology")
        << "Incorrect pixel id! r/c " << std::fixed << std::setw(2) << row << " / " << std::fixed << std::setw(2) << col
        << " with max " << std::fixed << std::setw(2) << m_nrows << " / " << std::fixed << std::setw(2) << m_ncols
        << " for LocalPoint " << std::fixed << std::setw(8) << std::setprecision(4) << p.x() << " , " << std::fixed
        << std::setw(8) << std::setprecision(4) << p.y() << " pixel " << std::fixed << std::setw(8)
        << std::setprecision(4) << thepixel.first << " , " << std::fixed << std::setw(8) << std::setprecision(4)
        << thepixel.second << " deltas " << p.x() - m_pitchx * m_nrows << " , " << p.y() - m_pitchy * m_ncols;
  }

  return std::pair<int, int>(row, col);
}

//The following lines check whether the point is actually out of the active pixel area.
bool RectangularMTDTopology::isInPixel(const LocalPoint& p) const {
  bool isInside = true;
  const auto& thepixel = pixel(p);
  const int ixbin = static_cast<int>(thepixel.first);
  const int iybin = static_cast<int>(thepixel.second);
  const float fractionX = thepixel.first - ixbin;
  const float fractionY = thepixel.second - iybin;
  if ((fractionX > 1.0 - m_GAPxInterpadFrac || fractionX < m_GAPxInterpadFrac) ||
      (ixbin == 0 && fractionX < m_GAPxBorderFrac) || (ixbin == m_nrows - 1 && fractionX > 1.0 - m_GAPxBorderFrac)) {
    isInside = false;
  }
  if ((fractionY > 1.0 - m_GAPyInterpadFrac || fractionY < m_GAPyInterpadFrac) ||
      (iybin == 0 && fractionY < m_GAPyBorderFrac) || (iybin == m_ncols - 1 && fractionY > 1.0 - m_GAPyBorderFrac)) {
    isInside = false;
  }
  return isInside;
}

//----------------------------------------------------------------------
// Topology interface, go from Measurement to Local corrdinates
// pixel coordinates (mp) -> cm (LocalPoint)
LocalPoint RectangularMTDTopology::localPosition(const MeasurementPoint& mp) const {
  float mpy = mp.y();  // measurements
  float mpx = mp.x();

  float lpY = localY(mpy);
  float lpX = localX(mpx);

  // Return it as a LocalPoint
  return LocalPoint(lpX, lpY);
}

//--------------------------------------------------------------------
//
// measuremet to local transformation for X coordinate
float RectangularMTDTopology::localX(const float mpx) const {
  // The final position in local coordinates
  float lpX = (mpx + 0.5f) * m_pitchx + m_xoffset;

  return lpX;
}

float RectangularMTDTopology::localY(const float mpy) const {
  // The final position in local coordinates
  float lpY = (mpy + 0.5f) * m_pitchy + m_yoffset;

  return lpY;
}

///////////////////////////////////////////////////////////////////
// Get hit errors in LocalPoint coordinates (cm)
LocalError RectangularMTDTopology::localError(const MeasurementPoint& mp, const MeasurementError& me) const {
  return LocalError(me.uu() * float(m_pitchx * m_pitchx), 0, me.vv() * float(m_pitchy * m_pitchy));
}

/////////////////////////////////////////////////////////////////////
// Get errors in pixel pitch units.
MeasurementError RectangularMTDTopology::measurementError(const LocalPoint& lp, const LocalError& le) const {
  return MeasurementError(le.xx() / float(m_pitchx * m_pitchx), 0, le.yy() / float(m_pitchy * m_pitchy));
}
