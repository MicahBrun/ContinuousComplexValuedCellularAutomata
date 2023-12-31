#include "../include/Grid.hpp"
#include "../include/ComplexMatToColor.hpp"

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
#include <opencv2/core.hpp>

#include <cmath>
#include <vector>
#include <iostream>

bool Grid::load(sf::Vector2f tileSize, unsigned int width, unsigned int height, float saturation)
{
    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(width * height * 4);
    m_saturation = saturation;

    // populate the vertex array, with one quad per tile
    for (unsigned int j = 0; j < height; ++j)
    for (unsigned int i = 0; i < width; ++i)
    {
        // get a pointer to the current tile's quad
        sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

        // define its 4 corners
        quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
        quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
        quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
        quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
    }

    m_width = width;
    m_height = height;

    return true;
}

void Grid::setValues(const cv::Mat& values)
{
    auto rgb {Color::complexMatToRgb(values, m_saturation)};
    cv::Mat channels[3];
    cv::split(rgb, channels);

    // populate the vertex array, with one quad per tile
    for (unsigned int j = 0; j < m_height; ++j)
    for (unsigned int i = 0; i < m_width; ++i)
    {
        int x = i + j * m_width;
        sf::Vertex* quad = &m_vertices[x * 4];

        for (int corner = 0; corner < 4; ++corner)
        {
            sf::Vertex& vertex = quad[corner];
            vertex.color.r = channels[0].at<float>(i, j) * 256.0;
            vertex.color.g = channels[1].at<float>(i, j) * 256.0;
            vertex.color.b = channels[2].at<float>(i, j) * 256.0;
        }
    }
}

void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // draw the vertex array
    target.draw(m_vertices, states);
}