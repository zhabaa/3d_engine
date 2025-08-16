#pragma once

#include <SFML/Graphics.hpp>
#include <camera.hpp>
#include <vector>
#include <memory>


namespace mt {
    struct CustomColor {
        std::vector<int> color;
    };

    struct ViewFrustum {
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        float fov = 90.0f;
    };

    struct Object {
        std::vector<Point> points;
        Pixel color;
    };

    class Scene {
    public:
        Scene(int width, int height);

        ~Scene() = default;

        void LifeCycle();

    private:
        int m_width;
        int m_height;
        int m_size = 0;

        ViewFrustum m_frustum;

        std::unique_ptr<Camera> m_camera;
        std::unique_ptr<sf::RenderWindow> m_window;
        std::unique_ptr<sf::Texture> m_texture;
        std::unique_ptr<sf::Sprite> m_sprite;

        std::vector<Object> objects;

        void SetViewFrustum(const ViewFrustum &frustum);

        bool IsPointVisible(const Point &p) const;
        bool IsPointVisible(const Point& p);

        Point rotatePoint(const Point& p, double rotX, double rotY, double rotZ);

        void generateSurface(const Point &center, double size, const Pixel &color, double rotX = 0.0, double rotY = 0.0, double rotZ = 0.0);
        void generateSpheres(double radius, const Point &center, const Pixel &color);
        void generateCube(double side, const Point &center, const Pixel &color);
        void generateHeart(double scale, const Point& center, const Pixel& color);
        void generateBeautifulHeart(double size, const Point& center, const Pixel& color, int detail = 50);    
        void generateHeart2(double size, const Point& center, const Pixel& color, double precision = 0.05, double threshold = 0.01);

        void handleInput();
    };
}
