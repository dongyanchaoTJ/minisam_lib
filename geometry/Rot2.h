#ifndef  ROT2_H
#define  ROT2_H

/**
 * @file Rot2.h
 * @brief 2D rotation
 * @date
 * @author
 */

#pragma once
#include "../base/Matrix.h"
/**
 * Rotation matrix
 * NOTE: the angle theta is in radians unless explicitly stated
 * @addtogroup geometry
 * \nosubgrouping
 */
class Rot2
{

    /** we store cos(theta) and sin(theta) */
    double c_, s_;

    /** normalize to make sure cos and sin form unit vector */
    Rot2& normalize();

    /** private constructor from cos/sin */
    inline Rot2(double c, double s) : c_(c), s_(s) {}

public:

    /// @name Constructors and named constructors
    /// @{

    /** default constructor, zero rotation */
    Rot2() : c_(1.0), s_(0.0) {}

    /// Constructor from angle in radians == exponential map at identity
    Rot2(double theta) : c_(cos(theta)), s_(sin(theta)) {}

    /// Named constructor from angle in radians
    static Rot2 fromAngle(double theta)
    {
        return Rot2(theta);
    }

    /// Named constructor from angle in degrees
    static Rot2 fromDegrees(double theta)
    {
        const double degree = M_PI / 180;
        return fromAngle(theta * degree);
    }

    /// Named constructor from cos(theta),sin(theta) pair, will *not* normalize!
    static Rot2 fromCosSin(double c, double s);

    /**
     * Named constructor with derivative
     * Calculate relative bearing to a landmark in local coordinate frame
     * @param d 2D location of landmark
     * @param H optional reference for Jacobian
     * @return 2D rotation \f$ \in SO(2) \f$
     */
    //static Rot2 relativeBearing(const Point2& d, OptionalJacobian<1,2> H =
    //    boost::none);
    //  static Rot2 relativeBearing(const Eigen::Vector2d& d);
    static Rot2 relativeBearing(const Eigen::Vector2d& d,Eigen::MatrixXd* H=NULL);
    /** Named constructor that behaves as atan2, i.e., y,x order (!) and normalizes */
    static Rot2 atan2(double y, double x);

    /// @}
    /// @name Testable
    /// @{

    /** print */
    // void print(const std::string& s = "theta") const;

    /** equals with an tolerance */
    // bool equals(const Rot2& R, double tol = 1e-9) const;

    /// @}
    /// @name Group
    /// @{

    /** identity */
    inline static Rot2 identity()
    {
        return Rot2();
    }

    /** The inverse rotation - negative angle */
    Rot2 inverse() const
    {
        return Rot2(c_, -s_);
    }

    /** Compose - make a new rotation by adding angles */
    Rot2 operator*(const Rot2& R) const
    {
        return fromCosSin(c_ * R.c_ - s_ * R.s_, s_ * R.c_ + c_ * R.s_);
    }

    /// @}
    /// @name Lie Group
    /// @{

    /// Exponential map at identity - create a rotation from canonical coordinates
    //static Rot2 Expmap(const Vector1& v, ChartJacobian H = boost::none);
    static Rot2 Expmap(const Eigen::VectorXd& v);
    static Rot2 Expmap(const Eigen::VectorXd& v,Eigen::MatrixXd* H);
    /// Log map at identity - return the canonical coordinates of this rotation
    //static Vector1 Logmap(const Rot2& r, ChartJacobian H = boost::none);
    static double Logmap(const Rot2& r);
    static double Logmap(const Rot2& r, Eigen::MatrixXd* H);
    /** Calculate Adjoint map */
    //Matrix1 AdjointMap() const { return I_1x1; }
    Eigen::MatrixXd AdjointMap() const
    {
        return Eigen::MatrixXd::Identity(1,1);
    }

    /// Left-trivialized derivative of the exponential map
    // static Matrix ExpmapDerivative(const Vector& /*v*/) {
    //   return I_1x1;
    // }

    static Eigen::MatrixXd ExpmapDerivative()
    {
        return Eigen::MatrixXd::Identity(1,1);
    }

    /// Left-trivialized derivative inverse of the exponential map
    //static Matrix LogmapDerivative(const Vector& /*v*/) {
    // //  return I_1x1;
    // }
    static Eigen::MatrixXd LogmapDerivative()
    {
        return Eigen::MatrixXd::Identity(1,1);
    }

    // Chart at origin simply uses exponential map and its inverse
    struct ChartAtOrigin
    {
        // static Rot2 Retract(const Vector1& v, ChartJacobian H = boost::none) {
        //  return Expmap(v, H);
        // }
        static Rot2 Retract(const Eigen::VectorXd& v)
        {
            return Expmap(v);
        }
        static Rot2 Retract(const Eigen::VectorXd& v,Eigen::MatrixXd* H)
        {
            return Expmap(v,H);
        }
        //static Vector1 Local(const Rot2& r, ChartJacobian H = boost::none) {
        //  return Logmap(r, H);
        // }
        static double Local(const Rot2& r)
        {
            return Logmap(r);
        }
        static double Local(const Rot2& r,Eigen::MatrixXd* H)
        {
            return Logmap(r,H);
        }
    };

    //using LieGroup<Rot2, 1>::inverse; // version with derivative
    double local(const Rot2&r);
    /// @}
    /// @name Group Action on Point2
    /// @{

    /**
     * rotate point from rotated coordinate frame to world \f$ p^w = R_c^w p^c \f$
     */
    //Point2 rotate(const Point2& p, OptionalJacobian<2, 1> H1 = boost::none,
    //    OptionalJacobian<2, 2> H2 = boost::none) const;
    Eigen::Vector2d rotate(const Eigen::Vector2d& p) const;
    Eigen::Vector2d rotate(const Eigen::Vector2d& p,Eigen::MatrixXd* H1,
                           Eigen::Matrix2d* H2) const;
    /** syntactic sugar for rotate */
    // inline Point2 operator*(const Point2& p) const {
    //   return rotate(p);
    // }
    inline Eigen::Vector2d operator*(const Eigen::Vector2d& p) const
    {
        return rotate(p);
    }

    /**
     * rotate point from world to rotated frame \f$ p^c = (R_c^w)^T p^w \f$
     */
    //  Point2 unrotate(const Point2& p, OptionalJacobian<2, 1> H1 = boost::none,
    //      OptionalJacobian<2, 2> H2 = boost::none) const;

    Eigen::Vector2d unrotate(const Eigen::Vector2d& p) const;
    Eigen::Vector2d unrotate(const Eigen::Vector2d& p,
                             Eigen::MatrixXd* H1,
                             Eigen::Matrix2d* H2) const;
    /// @}
    /// @name Standard Interface
    /// @{

    /// Creates a unit vector as a Point2
    //inline Point2 unit() const {
    //   return Point2(c_, s_);
    // }
    inline Eigen::Vector2d unit() const
    {
        Eigen::Vector2d p2;
        p2<<c_,s_;
        return p2;
    }

    /** return angle (RADIANS) */
    double theta() const
    {
        return ::atan2(s_, c_);
    }

    /** return angle (DEGREES) */
    double degrees() const
    {
        const double degree = M_PI / 180;
        return theta() / degree;
    }

    /** return cos */
    inline double c() const
    {
        return c_;
    }

    /** return sin */
    inline double s() const
    {
        return s_;
    }

    /** return 2*2 rotation matrix */
    //Matrix2 matrix() const;
    Eigen::Matrix2d matrix() const;
    /** return 2*2 transpose (inverse) rotation matrix   */
    //Matrix2 transpose() const;
    Eigen::Matrix2d transpose() const;

    friend std::ostream &operator<<(std::ostream &os, const Rot2& p);

};

#endif // ROT2_H
