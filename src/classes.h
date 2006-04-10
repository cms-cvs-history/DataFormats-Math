#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/Math/interface/Vector.h"
#include "DataFormats/Math/interface/Error.h"

namespace {
  namespace {
    math::XYZVector v1;
    math::XYZVectorD vd1; 
    math::XYZVectorF vf1; 
    math::RhoEtaPhiVector v2;
    math::RhoEtaPhiVectorD vd2;
    math::RhoEtaPhiVectorF vf2;
    math::RThetaPhiVector v3;
    math::RThetaPhiVectorD vd3;
    math::RThetaPhiVectorF vf3;
    math::XYZPoint p1;
    math::XYZPointD pd1;
    math::XYZPointF pf1;
    math::PtEtaPhiELorentzVector l1;
    math::PtEtaPhiELorentzVectorD ld1;
    math::PtEtaPhiELorentzVectorF lf1;
    math::XYZTLorentzVector l2;
    math::XYZTLorentzVectorD ld2;
    math::XYZTLorentzVectorF lf2;

    math::Vector<1>::type vv1;
    math::Vector<2>::type vv2;
    math::Vector<3>::type vv3;
    math::Vector<4>::type vv4;
    math::Vector<5>::type vv5;
    math::Vector<6>::type vv6;
    math::VectorD<1>::type vvd1;
    math::VectorD<2>::type vvd2;
    math::VectorD<3>::type vvd3;
    math::VectorD<4>::type vvd4;
    math::VectorD<5>::type vvd5;
    math::VectorD<6>::type vvd6;
    math::VectorF<1>::type vvf1;
    math::VectorF<2>::type vvf2;
    math::VectorF<3>::type vvf3;
    math::VectorF<4>::type vvf4;
    math::VectorF<5>::type vvf5;
    math::VectorF<6>::type vvf6;

    math::Error<1>::type e1;
    math::Error<2>::type e2;
    math::Error<3>::type e3;
    math::Error<4>::type e4;
    math::Error<5>::type e5;
    math::Error<6>::type e6;
    math::ErrorD<1>::type ed1;
    math::ErrorD<2>::type ed2;
    math::ErrorD<3>::type ed3;
    math::ErrorD<4>::type ed4;
    math::ErrorD<5>::type ed5;
    math::ErrorD<6>::type ed6;
    math::ErrorF<1>::type ef1;
    math::ErrorF<2>::type ef2;
    math::ErrorF<3>::type ef3;
    math::ErrorF<4>::type ef4;
    math::ErrorF<5>::type ef5;
    math::ErrorF<6>::type ef6;
  }
}
