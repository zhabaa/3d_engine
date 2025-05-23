#include <camera.hpp>
#include <matrix.hpp>
#include <cmath>

namespace mt {
    using math::Mat33d;
    using math::Vec3d;

    Camera::Camera(int width, int height, Intrinsic intrinsic, Point position, Angles angles)
        : m_width(width), 
          m_height(height),
          m_intrinsic(intrinsic),
          m_position(position),
          m_angles(angles)
    {
        m_picture = new Pixel[m_width * m_height];
        Clear();
    }

    Camera::~Camera() {
        delete[] m_picture;
    }

    Pixel* Camera::Picture() { 
        return m_picture; 
    }

    void Camera::Fill(Pixel pixel) {
        for (int i = 0; i < m_height; i++)
            for (int j = 0; j < m_width; j++)
                m_picture[i * m_width + j] = pixel;
    }

    void Camera::Clear() {
        Fill({0, 0, 0, 0});
    }

    void Camera::ProjectPoint(Point p, Pixel c)
    {
        double X = p.x;
        double Y = -p.z;  // Y становится -Z
        double Z = p.y;   // Z становится Y

        double a = m_angles.roll;
        double b = m_angles.pitch;
        
        double cosa = cos(a);
        double cosb = cos(b);
        double sina = sin(a);
        double sinb = sin(b);

        Mat33d R({{{cosb,       0,     -sinb},
                  {-sina*sinb,  cosa,  -sina*cosb},
                  {cosa*sinb,   sina,   cosa*cosb}}});

        Vec3d P({{{X - m_position.x},
                  {Y - m_position.y},
                  {Z - m_position.z}}});

        Vec3d P_res = R * P;

        double x = P_res.get(0, 0);
        double y = P_res.get(1, 0);
        double z = P_res.get(2, 0);

        if (z <= 0) return;

        double u = x / z;
        double v = y / z;

        u = u * m_intrinsic.fu + m_intrinsic.du;
        v = v * m_intrinsic.fv + m_intrinsic.dv;

        if (u >= 0 && u < m_width && v >= 0 && v < m_height) {
            m_picture[(int)v * m_width + (int)u] = c;
        }
    }

    void Camera::dX(double d) {
        m_position.x += d * cos(-m_angles.pitch);
        m_position.z += d * sin(-m_angles.pitch);
    }

    void Camera::dZ(double d) {
        m_position.x += d * sin(m_angles.pitch);
        m_position.y += d * sin(m_angles.roll);
        m_position.z += d * cos(m_angles.pitch);
    }

    void Camera::dRoll(double droll) {
        m_angles.roll += droll;
    }

    void Camera::dPitch(double dpitch) {
        m_angles.pitch += dpitch;
    }
}