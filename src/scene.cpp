#include <scene.hpp>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <random>
#include <iostream>
#include <algorithm>
#include <functional>
#include <array>

// #ifndef M_PI
#define M_PI 3.14159265358979323846
// #endif

namespace mt
{
    Point Scene::rotatePoint(const Point& p, double rotX, double rotY, double rotZ) {
        Point result = p;
        
        if (rotX != 0.0) {
            double cosX = cos(rotX);
            double sinX = sin(rotX);
            double y = result.y * cosX - result.z * sinX;
            double z = result.y * sinX + result.z * cosX;
            result.y = y;
            result.z = z;
        }
        
        if (rotY != 0.0) {
            double cosY = cos(rotY);
            double sinY = sin(rotY);
            double x = result.x * cosY + result.z * sinY;
            double z = -result.x * sinY + result.z * cosY;
            result.x = x;
            result.z = z;
        }
        
        if (rotZ != 0.0) {
            double cosZ = cos(rotZ);
            double sinZ = sin(rotZ);
            double x = result.x * cosZ - result.y * sinZ;
            double y = result.x * sinZ + result.y * cosZ;
            result.x = x;
            result.y = y;
        }
        
        return result;
    }

    bool Scene::IsPointVisible(const Point& p) const {
        if (p.z < m_frustum.nearPlane || p.z > m_frustum.farPlane)
            return false;

        float aspect = static_cast<float>(m_width) / m_height;
        float tanFOV = tan(m_frustum.fov * 0.5f * M_PI / 180.0f);
        float xLimit = tanFOV * p.z * aspect;
        float yLimit = tanFOV * p.z;
        
        return (fabs(p.x) <= xLimit) && (fabs(p.y) <= yLimit);
    }

    Scene::Scene(int width, int height) 
        : m_width(width), 
          m_height(height),
          m_window(std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "3D Engine")),
          m_texture(std::make_unique<sf::Texture>()),
          m_sprite(std::make_unique<sf::Sprite>())
    {
        if (!m_texture->create(m_width, m_height)) {
            throw std::runtime_error("Failed to create texture");
        }

        m_sprite->setTexture(*m_texture);

        const Intrinsic intrinsic = {960.0, 540.0, 960.0, 540.0};
        const Point position = {0.0, 0.0, -5.0};
        const Angles angles = {0.0, 0.0, 0.0};

        m_camera = std::make_unique<Camera>(m_width, m_height, intrinsic, position, angles);

        generateSpheres(1.0, {0.0, 2.0, 0.0}, {255, 255, 255, 255});
        generateSpheres(1.0, {0.0, -2.0, 0.0}, {255, 255, 255, 255});
        generateSurface({0,0,0}, 10.0, {255,255,255,255}, M_PI/2, 0, 0);
        generateCube(1.0, {1.0, 1.0, 1.0}, {255, 255, 255, 255});
    }

    void Scene::SetViewFrustum(const ViewFrustum& frustum) {
        m_frustum = frustum;
    }

    void Scene::generateSpheres(double radius, const Point& center, const Pixel& color) {
        Object sphere;
        sphere.color = color;

        const double step = 0.02;
        for (double fi = 0; fi < 2*M_PI; fi += step) {
            for (double teta = 0; teta < M_PI; teta += step) {
                Point p;
                p.x = radius * sin(teta) * cos(fi);
                p.y = radius * sin(teta) * sin(fi);
                p.z = radius * cos(teta);
                
                sphere.points.push_back({p.x + center.x, p.y + center.y, p.z + center.z});
            }
        }
        objects.push_back(sphere);
    }
    
    void Scene::generateCube(double side, const Point& center, const Pixel& color) {
        Object cube;
        cube.color = color;

        const double step = 0.02;
        double halfSide = side / 2.0;

        for (int face = 0; face < 6; face++) {
            for (double u = -halfSide; u <= halfSide; u += step) {
                for (double v = -halfSide; v <= halfSide; v += step) {
                    Point p;

                    switch (face) {
                        case 0:
                            p.x = u;
                            p.y = v;
                            p.z = halfSide;
                            break;
                        case 1:
                            p.x = u;
                            p.y = v;
                            p.z = -halfSide;
                            break;
                        case 2:
                            p.x = halfSide;
                            p.y = u;
                            p.z = v;
                            break;
                        case 3:
                            p.x = -halfSide;
                            p.y = u;
                            p.z = v;
                            break;
                        case 4:
                            p.x = u;
                            p.y = halfSide;
                            p.z = v;
                            break;
                        case 5:
                            p.x = u;
                            p.y = -halfSide;
                            p.z = v;
                            break;
                    }

                    cube.points.push_back({p.x + center.x, p.y + center.y, p.z + center.z});
                }
            }
        }
        objects.push_back(cube);
    }
    
    void Scene::generateSurface(const Point& center, double size, const Pixel& color, double rotX, double rotY, double rotZ) {
        Object surface;
        surface.color = color;

        const double step = 0.1;

        const double cosX = cos(rotX), sinX = sin(rotX);
        const double cosY = cos(rotY), sinY = sin(rotY);
        const double cosZ = cos(rotZ), sinZ = sin(rotZ);
        
        const double m00 = cosY * cosZ;
        const double m01 = sinX * sinY * cosZ - cosX * sinZ;
        const double m02 = cosX * sinY * cosZ + sinX * sinZ;
        
        const double m10 = cosY * sinZ;
        const double m11 = sinX * sinY * sinZ + cosX * cosZ;
        const double m12 = cosX * sinY * sinZ - sinX * cosZ;
        
        const double m20 = -sinY;
        const double m21 = sinX * cosY;
        const double m22 = cosX * cosY;

        for (double x = -size/2; x < size/2; x += step) {
            for (double z = -size/2; z < size/2; z += step) {

                Point p;
                p.x = m00 * x + m01 * 0.0 + m02 * z;
                p.y = m10 * x + m11 * 0.0 + m12 * z;
                p.z = m20 * x + m21 * 0.0 + m22 * z;

                p.x += center.x;
                p.y += center.y;
                p.z += center.z;
                
                surface.points.push_back(p);
            }
        }
        objects.push_back(surface);
    }

    void Scene::handleInput() {
        const float moveSpeed = 0.05f;
        const float rotateSpeed = 0.01f;
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) m_camera->dZ(moveSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) m_camera->dZ(-moveSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) m_camera->dX(-moveSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) m_camera->dX(moveSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) m_camera->dPitch(-rotateSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) m_camera->dPitch(rotateSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) m_camera->dRoll(-rotateSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) m_camera->dRoll(rotateSpeed);
    }

    void Scene::LifeCycle() {
        sf::Clock clock;
        
        while (m_window->isOpen()) {
            float deltaTime = clock.restart().asSeconds();
            
            sf::Event event;
            while (m_window->pollEvent(event)) {
                if (event.type == sf::Event::Closed || 
                   (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                    m_window->close();
                }
            }

            handleInput();
            
            m_camera->Clear();
            
            for (const auto& object : objects) {
                for (const auto& point : object.points) {
                    m_camera->ProjectPoint(point, object.color);
                }
            }
            
            m_texture->update(reinterpret_cast<const uint8_t*>(m_camera->Picture()));
            m_window->clear();
            m_window->draw(*m_sprite);
            m_window->display();
        }
    }
}