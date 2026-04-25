#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

enum class BlockType { CUBE, PILLAR, ROTATING, OBSTACLE };

struct Node {
    std::string id;
    glm::vec3 offset;
    std::vector<std::string> connections;
};

struct Platform {
    BlockType type = BlockType::CUBE;
    glm::vec3 pos;
    glm::vec3 color;
    std::vector<Node> nodes;
    bool isDecorative = false;
    glm::vec3 scale = glm::vec3(1, 1, 1);
    bool isDamaging = false;
};

struct Level {
    std::string id;
    std::string name;
    std::string startNodeId;
    std::string goalNodeId;
    std::vector<Platform> blocks;
    glm::vec3 pivot;
    float initYaw, initPitch, initRadius;
    std::string hint;
};

namespace C {
    static const glm::vec3 iceBlue   (0.659f,0.847f,0.918f);
    static const glm::vec3 pinkOrch  (1.000f,0.800f,0.976f);
    static const glm::vec3 mintGreen (0.482f,0.929f,0.624f);
    static const glm::vec3 golden    (0.925f,0.800f,0.408f);
    static const glm::vec3 periwink  (0.439f,0.631f,1.000f);
    static const glm::vec3 coral     (1.000f,0.498f,0.314f);
    static const glm::vec3 amber     (1.000f,0.647f,0.008f);
    static const glm::vec3 sunYellow (1.000f,0.827f,0.165f);
    static const glm::vec3 silverGray(0.875f,0.894f,0.918f);
    static const glm::vec3 orchPink  (1.000f,0.624f,0.953f);
    static const glm::vec3 warmYellow(0.996f,0.792f,0.341f);
    static const glm::vec3 redCoral  (1.000f,0.420f,0.420f);
    static const glm::vec3 teal      (0.333f,0.937f,0.769f);
    static const glm::vec3 cyanLight (0.506f,0.925f,0.925f);
    static const glm::vec3 skyBlue   (0.455f,0.725f,1.000f);
    static const glm::vec3 lavender  (0.635f,0.608f,0.996f);
    static const glm::vec3 cream     (1.000f,0.918f,0.655f);
    static const glm::vec3 salmon    (1.000f,0.463f,0.459f);
    static const glm::vec3 peach     (0.980f,0.694f,0.627f);
    static const glm::vec3 crimson   (0.839f,0.188f,0.192f);
    static const glm::vec3 honey     (0.992f,0.796f,0.431f);
    static const glm::vec3 hotPink   (0.992f,0.475f,0.659f);
    static const glm::vec3 deepPink  (0.910f,0.263f,0.576f);
    static const glm::vec3 aqua      (0.000f,0.808f,0.788f);
    static const glm::vec3 indigo    (0.424f,0.361f,0.906f);
    static const glm::vec3 emerald   (0.000f,0.722f,0.580f);
    static const glm::vec3 magenta   (0.910f,0.263f,0.576f);
    static const glm::vec3 rose      (0.914f,0.118f,0.388f);
    static const glm::vec3 purple    (0.612f,0.153f,0.690f);
    static const glm::vec3 cyan      (0.000f,0.737f,0.831f);
    static const glm::vec3 orange    (1.000f,0.596f,0.000f);
    static const glm::vec3 limeGreen (0.298f,0.686f,0.314f);
    static const glm::vec3 brown     (0.475f,0.333f,0.282f);
    static const glm::vec3 grass     (0.545f,0.765f,0.290f);
    static const glm::vec3 sunflower (1.000f,0.757f,0.027f);
    static const glm::vec3 tomato    (0.957f,0.263f,0.212f);
    static const glm::vec3 slateBlue (0.376f,0.490f,0.545f);
    static const glm::vec3 orangeRed (1.000f,0.341f,0.133f);
    static const glm::vec3 stone     (0.620f,0.620f,0.620f);
    static const glm::vec3 violet    (0.404f,0.227f,0.718f);
    static const glm::vec3 royal     (0.247f,0.318f,0.710f);
    static const glm::vec3 sienna    (0.475f,0.333f,0.282f);
    // Minecraft-inspired additions
    static const glm::vec3 lava      (0.85f, 0.22f, 0.02f);
    static const glm::vec3 startBlue (0.10f, 0.72f, 1.00f);
}

inline std::vector<Level> buildLevels() {
    std::vector<Level> L;

    // Level 1
    L.push_back({
        "level-1", "The Beginning", "b1-n1", "b3-n1",
        {
            {BlockType::CUBE, {0,0,0}, C::iceBlue,  {{"b1-n1", {0,0.5f,0}, {"b2-n1"}}}},
            {BlockType::CUBE, {1,0,0}, C::iceBlue,  {{"b2-n1", {0,0.5f,0}, {"b1-n1", "b3-n1"}}}},
            {BlockType::CUBE, {2,0,0}, C::pinkOrch, {{"b3-n1", {0,0.5f,0}, {"b2-n1"}}}},
            {BlockType::OBSTACLE, {1,0,-0.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {1.0f, 0.5f, 0.0f}, -135.f, 38.f, 10.f, "Move right to reach the goal."
    });

    // Level 2
    L.push_back({
        "level-2", "The Illusion", "start", "goal",
        {
            {BlockType::CUBE, {0,0,0}, C::iceBlue,  {{"start", {0,0.5f,0}, {"p1"}}}},
            {BlockType::CUBE, {1,0,0}, C::iceBlue,  {{"p1", {0,0.5f,0}, {"start"}}}},
            {BlockType::CUBE, {1,2,-2}, C::pinkOrch, {{"p2", {0,0.5f,0}, {"goal"}}}},
            {BlockType::CUBE, {2,2,-2}, C::pinkOrch, {{"goal", {0,0.5f,0}, {"p2"}}}},
            {BlockType::OBSTACLE, {0.5f,0,-0.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {1.5f,2,-1.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {1.0f, 1.0f, -1.0f}, -135.f, 38.f, 12.f, "Align platforms to cross the gap."
    });

    // Level 3
    L.push_back({
        "level-3", "The Spiral", "s1", "g1",
        {
            {BlockType::CUBE, {0,0,0}, C::iceBlue, {{"s1", {0,0.5f,0}, {"s2"}}}},
            {BlockType::CUBE, {1,0,0}, C::iceBlue, {{"s2", {0,0.5f,0}, {"s1", "s3"}}}},
            {BlockType::CUBE, {1,0,1}, C::iceBlue, {{"s3", {0,0.5f,0}, {"s2", "s4"}}}},
            {BlockType::CUBE, {0,0,1}, C::iceBlue, {{"s4", {0,0.5f,0}, {"s3", "s5"}}}},
            {BlockType::CUBE, {0,1,1}, C::iceBlue, {{"s5", {0,0.5f,0}, {"s4", "s6"}}}},
            {BlockType::CUBE, {0,2,1}, C::iceBlue, {{"s6", {0,0.5f,0}, {"s5", "g1"}}}},
            {BlockType::CUBE, {0,2,0}, C::pinkOrch, {{"g1", {0,0.5f,0}, {"s6"}}}},
            {BlockType::OBSTACLE, {0.5f,0,0.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {0.5f, 1.0f, 0.5f}, -125.f, 40.f, 12.f, "Follow the spiral staircase."
    });

    // Level 4
    L.push_back({
        "level-4", "The Bridge of Faith", "s1", "g1",
        {
            {BlockType::CUBE, {0,0,0}, C::iceBlue, {{"s1", {0,0.5f,0}, {"p1"}}}},
            {BlockType::CUBE, {1,0,0}, C::iceBlue, {{"p1", {0,0.5f,0}, {"s1", "p2"}}}},
            {BlockType::CUBE, {2,0,0}, C::iceBlue, {{"p2", {0,0.5f,0}, {"p1"}}}},
            {BlockType::CUBE, {2,4,-4}, C::pinkOrch, {{"p3", {0,0.5f,0}, {"p4"}}}},
            {BlockType::CUBE, {3,4,-4}, C::pinkOrch, {{"p4", {0,0.5f,0}, {"p3", "g1"}}}},
            {BlockType::CUBE, {4,4,-4}, C::pinkOrch, {{"g1", {0,0.5f,0}, {"p4"}}}},
            {BlockType::PILLAR, {2,1,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, {2,2,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, {2,3,0}, C::silverGray, {}, true},
            {BlockType::OBSTACLE, {1,0,-0.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {3,4,-3.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {2.0f, 2.0f, -2.0f}, -130.f, 36.f, 16.f, "A leap of faith requires alignment."
    });

    // Level 5
    L.push_back({
        "level-5", "The Grand Palace", "s1", "goal",
        {
            {BlockType::CUBE, {0,0,0}, C::mintGreen, {{"s1", {0,0.5f,0}, {"p1", "p2"}}}},
            {BlockType::CUBE, {1,0,0}, C::mintGreen, {{"p1", {0,0.5f,0}, {"s1", "p3"}}}},
            {BlockType::CUBE, {0,0,1}, C::mintGreen, {{"p2", {0,0.5f,0}, {"s1", "p4"}}}},
            {BlockType::CUBE, {1,0,1}, C::mintGreen, {{"p3", {0,0.5f,0}, {"p1", "p4"}}}},
            {BlockType::CUBE, {0,0,2}, C::mintGreen, {{"p4", {0,0.5f,0}, {"p2", "p3", "stairs1"}}}},
            {BlockType::CUBE, {0,1,3}, C::golden,    {{"stairs1", {0,0.5f,0}, {"p4", "stairs2"}}}},
            {BlockType::CUBE, {0,2,4}, C::golden,    {{"stairs2", {0,0.5f,0}, {"stairs1", "upper1"}}}},
            {BlockType::CUBE, {0,3,5}, C::periwink,  {{"upper1", {0,0.5f,0}, {"stairs2", "upper2"}}}},
            {BlockType::CUBE, {1,3,5}, C::periwink,  {{"upper2", {0,0.5f,0}, {"upper1", "upper3"}}}},
            {BlockType::CUBE, {2,3,5}, C::periwink,  {{"upper3", {0,0.5f,0}, {"upper2", "upper4"}}}},
            {BlockType::CUBE, {3,3,5}, C::periwink,  {{"upper4", {0,0.5f,0}, {"upper3", "bridge1"}}}},
            {BlockType::CUBE, {5,3,5}, C::coral,     {{"bridge1", {0,0.5f,0}, {"upper4", "bridge2"}}}},
            {BlockType::CUBE, {5,3,4}, C::coral,     {{"bridge2", {0,0.5f,0}, {"bridge1", "bridge3"}}}},
            {BlockType::CUBE, {5,3,3}, C::coral,     {{"bridge3", {0,0.5f,0}, {"bridge2"}}}},
            {BlockType::CUBE, {5,6,0}, C::amber,     {{"goal_p1", {0,0.5f,0}, {"goal"}}}},
            {BlockType::CUBE, {6,6,0}, C::sunYellow, {{"goal", {0,0.5f,0}, {"goal_p1"}}}},
            {BlockType::PILLAR, {5,0,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, {5,1,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, {5,2,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, {5,3,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, {5,4,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, {5,5,0}, C::silverGray, {}, true},
            {BlockType::CUBE, {2,0,0}, C::mintGreen, {}, true},
            {BlockType::CUBE, {-1,0,1}, C::mintGreen, {}, true},
            {BlockType::CUBE, {3,3,4}, C::periwink, {}, true},
            {BlockType::OBSTACLE, {0,1,2.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {4,3,4.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {3.0f, 3.0f, 2.5f}, -140.f, 38.f, 22.f, "Climb the palace and bridge the gap."
    });

    // Level 6
    L.push_back({
        "level-6", "The Clockwork", "s1", "goal",
        {
            {BlockType::CUBE, {0,0,0}, C::iceBlue, {{"s1", {0,0.5f,0}, {"s2"}}}},
            {BlockType::CUBE, {1,0,0}, C::iceBlue, {{"s2", {0,0.5f,0}, {"s1", "r_arm1", "r_arm2"}}}},
            {BlockType::ROTATING, {3,0,0}, C::orchPink, 
                {{"r_center", {0,0.5f,0}, {"r_arm1", "r_arm2"}},
                 {"r_arm1", {-1,0.5f,0}, {"r_center", "s2", "g1"}},
                 {"r_arm2", {1,0.5f,0}, {"r_center", "s2", "g1"}}},
                false, {3,1,1}
            },
            {BlockType::CUBE, {3,0,2}, C::warmYellow, {{"g1", {0,0.5f,0}, {"goal", "r_arm1", "r_arm2"}}}},
            {BlockType::CUBE, {3,0,3}, C::warmYellow, {{"goal", {0,0.5f,0}, {"g1"}}}},
            {BlockType::CUBE, {3,2,-3}, C::redCoral, {{"p1", {0,0.5f,0}, {"p2"}}}, true},
            {BlockType::CUBE, {3,2,-2}, C::redCoral, {{"p2", {0,0.5f,0}, {"p1"}}}, true},
            {BlockType::PILLAR, {3,-1,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, {3,-2,0}, C::silverGray, {}, true},
            {BlockType::OBSTACLE, {2,0,-0.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {2.0f, 1.0f, -0.5f}, -130.f, 38.f, 14.f, "Sync with the clockwork rotation."
    });

    // Level 7
    L.push_back({
        "level-7", "The Labyrinth", "s1", "goal",
        {
            {BlockType::CUBE, {0,0,0}, C::teal, {{"s1", {0,0.5f,0}, {"s2"}}}},
            {BlockType::CUBE, {1,0,0}, C::teal, {{"s2", {0,0.5f,0}, {"s1", "r1_arm1"}}}},
            {BlockType::ROTATING, {3,0,0}, C::cyanLight,
                {{"r1_center", {0,0.5f,0}, {"r1_arm1", "r1_arm2"}},
                 {"r1_arm1", {-1,0.5f,0}, {"r1_center", "s2", "m1"}},
                 {"r1_arm2", {1,0.5f,0}, {"r1_center", "s2", "m1"}}},
                false, {3,1,1}
            },
            {BlockType::CUBE, {3,0,-2}, C::skyBlue, {{"m1", {0,0.5f,0}, {"r1_arm1", "r1_arm2", "m2"}}}},
            {BlockType::CUBE, {3,0,-3}, C::skyBlue, {{"m2", {0,0.5f,0}, {"m1", "r2_arm1"}}}},
            {BlockType::ROTATING, {3,2,-5}, C::lavender,
                {{"r2_center", {0,0.5f,0}, {"r2_arm1", "r2_arm2"}},
                 {"r2_arm1", {0,0.5f,1}, {"r2_center", "m2"}},
                 {"r2_arm2", {0,0.5f,-1}, {"r2_center", "g1"}}},
                false, {1,1,3}
            },
            {BlockType::CUBE, {3,2,-7}, C::cream, {{"g1", {0,0.5f,0}, {"goal", "r2_arm2"}}}},
            {BlockType::CUBE, {3,2,-8}, C::cream, {{"goal", {0,0.5f,0}, {"g1"}}}},
            {BlockType::CUBE, {5,0,0}, C::salmon, {{"d1", {0,0.5f,0}, {"d2"}}}, true},
            {BlockType::CUBE, {6,0,0}, C::salmon, {{"d2", {0,0.5f,0}, {"d1"}}}, true},
            {BlockType::OBSTACLE, {2,0,-0.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {3,2,-5.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {3.0f, 1.0f, -4.0f}, -125.f, 37.f, 18.f, "Two stages of illusion await."
    });

    // Level 8
    L.push_back({
        "level-8", "Ascension", "s1", "goal",
        {
            {BlockType::CUBE, { 0,0,0}, C::peach, {{"s1", {0,0.5f,0}, {"s2"}}}},
            {BlockType::CUBE, { 1,0,0}, C::peach, {{"s2", {0,0.5f,0}, {"s1", "stair1"}}}},
            {BlockType::CUBE, { 2,1,0}, C::salmon, {{"stair1", {0,0.5f,0}, {"s2", "stair2"}}}},
            {BlockType::CUBE, { 3,2,0}, C::salmon, {{"stair2", {0,0.5f,0}, {"stair1", "plat1"}}}},
            {BlockType::CUBE, { 4,2,0}, C::hotPink, {{"plat1", {0,0.5f,0}, {"stair2", "r1_arm1"}}}},
            {BlockType::ROTATING, {4,2,2}, C::deepPink,
                {{"r1_center", {0,0.5f,0}, {"r1_arm1", "r1_arm2"}},
                 {"r1_arm1", {0,0.5f,-1}, {"r1_center", "plat1"}},
                 {"r1_arm2", {0,0.5f,1}, {"r1_center", "plat2"}}},
                false, {1,1,3}
            },
            {BlockType::CUBE, { 4,2,4}, C::hotPink, {{"plat2", {0,0.5f,0}, {"r1_arm2", "stair3"}}}},
            {BlockType::CUBE, { 2,4,4}, C::crimson, {{"stair3", {0,0.5f,0}, {"plat2", "stair4"}}}},
            {BlockType::CUBE, { 1,5,4}, C::crimson, {{"stair4", {0,0.5f,0}, {"stair3", "g1"}}}},
            {BlockType::CUBE, { 0,5,4}, C::honey, {{"g1", {0,0.5f,0}, {"stair4", "goal"}}}},
            {BlockType::CUBE, {-1,5,4}, C::honey, {{"goal", {0,0.5f,0}, {"g1"}}}},
            {BlockType::PILLAR, { 4,0,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, { 4,1,0}, C::silverGray, {}, true},
            {BlockType::PILLAR, { 4,0,4}, C::silverGray, {}, true},
            {BlockType::PILLAR, { 4,1,4}, C::silverGray, {}, true},
            {BlockType::PILLAR, { 0,0,4}, C::silverGray, {}, true},
            {BlockType::PILLAR, { 0,1,4}, C::silverGray, {}, true},
            {BlockType::PILLAR, { 0,2,4}, C::silverGray, {}, true},
            {BlockType::PILLAR, { 0,3,4}, C::silverGray, {}, true},
            {BlockType::PILLAR, { 0,4,4}, C::silverGray, {}, true},
            {BlockType::OBSTACLE, {2,2,0.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {1,5,3.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {2.0f, 2.5f, 2.0f}, -135.f, 38.f, 18.f, "Ascend to reach the golden goal."
    });

    // Level 9
    L.push_back({
        "level-9", "The Impossible Machine", "s1", "goal",
        {
            {BlockType::CUBE, {0,0, 0}, C::aqua, {{"s1", {0,0.5f,0}, {"s2"}}}},
            {BlockType::CUBE, {1,0, 0}, C::aqua, {{"s2", {0,0.5f,0}, {"s1", "r1_arm1"}}}},
            {BlockType::ROTATING, {3,0,0}, C::honey,
                {{"r1_center", {0,0.5f,0}, {"r1_arm1", "r1_arm2"}},
                 {"r1_arm1", {-1,0.5f,0}, {"r1_center", "s2"}},
                 {"r1_arm2", {1,0.5f,0}, {"r1_center", "m1"}}},
                false, {3,1,1}
            },
            {BlockType::CUBE, {5,0,0}, C::salmon, {{"m1", {0,0.5f,0}, {"r1_arm2", "m2"}}}},
            {BlockType::CUBE, {6,0,0}, C::salmon, {{"m2", {0,0.5f,0}, {"m1", "r2_arm1"}}}},
            {BlockType::ROTATING, {6,2,-2}, C::indigo,
                {{"r2_center", {0,0.5f,0}, {"r2_arm1", "r2_arm2"}},
                 {"r2_arm1", {0,0.5f,1}, {"r2_center", "m2"}},
                 {"r2_arm2", {0,0.5f,-1}, {"r2_center", "plat1"}}},
                false, {1,1,3}
            },
            {BlockType::CUBE, {6,2,-4}, C::emerald, {{"plat1", {0,0.5f,0}, {"r2_arm2", "plat2"}}}},
            {BlockType::CUBE, {5,2,-4}, C::emerald, {{"plat2", {0,0.5f,0}, {"plat1", "r3_arm1"}}}},
            {BlockType::ROTATING, {3,2,-4}, C::magenta,
                {{"r3_center", {0,0.5f,0}, {"r3_arm1", "r3_arm2"}},
                 {"r3_arm1", {1,0.5f,0}, {"r3_center", "plat2"}},
                 {"r3_arm2", {-1,0.5f,0}, {"r3_center", "g1"}}},
                false, {3,1,1}
            },
            {BlockType::CUBE, {1,2,-4}, C::cream, {{"g1", {0,0.5f,0}, {"goal", "r3_arm2"}}}},
            {BlockType::CUBE, {0,2,-4}, C::cream, {{"goal", {0,0.5f,0}, {"g1"}}}},
            {BlockType::PILLAR, {6,0,-2}, C::silverGray, {}, true},
            {BlockType::PILLAR, {6,1,-2}, C::silverGray, {}, true},
            {BlockType::PILLAR, {6,0,-4}, C::silverGray, {}, true},
            {BlockType::PILLAR, {6,1,-4}, C::silverGray, {}, true},
            {BlockType::PILLAR, {5,0,-4}, C::silverGray, {}, true},
            {BlockType::PILLAR, {5,1,-4}, C::silverGray, {}, true},
            {BlockType::PILLAR, {1,0,-4}, C::silverGray, {}, true},
            {BlockType::PILLAR, {1,1,-4}, C::silverGray, {}, true},
            {BlockType::PILLAR, {0,0,-4}, C::silverGray, {}, true},
            {BlockType::PILLAR, {0,1,-4}, C::silverGray, {}, true},
            {BlockType::OBSTACLE, {4,0,-0.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {5,2,-3.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {3.0f, 1.0f, -2.0f}, -130.f, 37.f, 20.f, "Chain illusions to solve the machine."
    });

    // Level 10
    L.push_back({
        "level-10", "The Crystal Maze", "s1", "goal",
        {
            {BlockType::CUBE, {0,0, 0}, C::rose, {{"s1", {0,0.5f,0}, {"s2"}}}},
            {BlockType::CUBE, {1,0, 0}, C::rose, {{"s2", {0,0.5f,0}, {"s1", "p1"}}}},
            {BlockType::CUBE, {2,0, 0}, C::purple, {{"p1", {0,0.5f,0}, {"s2", "p2"}}}},
            {BlockType::CUBE, {3,0, 0}, C::purple, {{"p2", {0,0.5f,0}, {"p1", "p3"}}}},
            {BlockType::CUBE, {4,0, 0}, C::purple, {{"p3", {0,0.5f,0}, {"p2", "p4"}}}},
            {BlockType::CUBE, {4,3,-3}, C::cyan, {{"p4", {0,0.5f,0}, {"p3", "p5"}}}},
            {BlockType::CUBE, {5,3,-3}, C::cyan, {{"p5", {0,0.5f,0}, {"p4", "p6"}}}},
            {BlockType::CUBE, {6,3,-3}, C::cyan, {{"p6", {0,0.5f,0}, {"p5", "p7"}}}},
            {BlockType::CUBE, {6,3,-1}, C::orange, {{"p7", {0,0.5f,0}, {"p6", "p8"}}}},
            {BlockType::CUBE, {6,3, 0}, C::orange, {{"p8", {0,0.5f,0}, {"p7", "p9"}}}},
            {BlockType::CUBE, {6,3, 1}, C::orange, {{"p9", {0,0.5f,0}, {"p8", "goal"}}}},
            {BlockType::CUBE, {6,3, 3}, C::limeGreen, {{"goal", {0,0.5f,0}, {"p9"}}}},
            {BlockType::PILLAR, {4,0,0}, C::rose, {}, true},
            {BlockType::PILLAR, {4,1,0}, C::rose, {}, true},
            {BlockType::PILLAR, {4,2,0}, C::rose, {}, true},
            {BlockType::PILLAR, {6,0,-3}, C::cyan, {}, true},
            {BlockType::PILLAR, {6,1,-3}, C::cyan, {}, true},
            {BlockType::PILLAR, {6,2,-3}, C::cyan, {}, true},
            {BlockType::OBSTACLE, {3,0,-0.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {5,3,-2.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {3.5f, 1.5f, -0.5f}, -130.f, 37.f, 18.f, "Navigate the floating crystal path."
    });

    // Level 11
    L.push_back({
        "level-11", "The Floating Gardens", "s1", "goal",
        {
            {BlockType::CUBE, {0,0, 0}, C::brown, {{"s1", {0,0.5f,0}, {"s2"}}}},
            {BlockType::CUBE, {1,0, 0}, C::brown, {{"s2", {0,0.5f,0}, {"s1", "s3"}}}},
            {BlockType::CUBE, {2,0, 0}, C::brown, {{"s3", {0,0.5f,0}, {"s2", "p1"}}}},
            {BlockType::CUBE, {2,2,-2}, C::grass, {{"p1", {0,0.5f,0}, {"s3", "p2"}}}},
            {BlockType::CUBE, {3,2,-2}, C::grass, {{"p2", {0,0.5f,0}, {"p1", "p3"}}}},
            {BlockType::CUBE, {4,2,-2}, C::grass, {{"p3", {0,0.5f,0}, {"p2", "p4"}}}},
            {BlockType::CUBE, {4,4,-4}, C::sunflower, {{"p4", {0,0.5f,0}, {"p3", "p5"}}}},
            {BlockType::CUBE, {5,4,-4}, C::sunflower, {{"p5", {0,0.5f,0}, {"p4", "p6"}}}},
            {BlockType::CUBE, {6,4,-4}, C::sunflower, {{"p6", {0,0.5f,0}, {"p5", "goal"}}}},
            {BlockType::CUBE, {6,4,-2}, C::tomato, {{"goal", {0,0.5f,0}, {"p6"}}}},
            {BlockType::PILLAR, {2,0,-2}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {2,1,-2}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {4,0,-2}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {4,1,-2}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {4,2,-2}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {4,3,-2}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {6,0,-4}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {6,1,-4}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {6,2,-4}, C::slateBlue, {}, true},
            {BlockType::PILLAR, {6,3,-4}, C::slateBlue, {}, true},
            {BlockType::OBSTACLE, {3,2,-1.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {5,4,-3.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {3.0f, 2.0f, -2.0f}, -130.f, 37.f, 18.f, "Leap across garden terraces."
    });

    // Level 12
    L.push_back({
        "level-12", "The Ancient Temple", "s1", "goal",
        {
            {BlockType::CUBE, {0,0, 0}, C::orangeRed, {{"s1", {0,0.5f,0}, {"s2"}}}},
            {BlockType::CUBE, {1,0, 0}, C::orangeRed, {{"s2", {0,0.5f,0}, {"s1", "s3"}}}},
            {BlockType::CUBE, {2,0, 0}, C::orangeRed, {{"s3", {0,0.5f,0}, {"s2", "p1"}}}},
            {BlockType::CUBE, {3,0, 0}, C::stone, {{"p1", {0,0.5f,0}, {"s3", "p2"}}}},
            {BlockType::CUBE, {4,0, 0}, C::stone, {{"p2", {0,0.5f,0}, {"p1", "p3"}}}},
            {BlockType::CUBE, {5,0, 0}, C::stone, {{"p3", {0,0.5f,0}, {"p2", "p4"}}}},
            {BlockType::CUBE, {5,2,-2}, C::violet, {{"p4", {0,0.5f,0}, {"p3", "p5"}}}},
            {BlockType::CUBE, {6,2,-2}, C::violet, {{"p5", {0,0.5f,0}, {"p4", "p6"}}}},
            {BlockType::CUBE, {7,2,-2}, C::violet, {{"p6", {0,0.5f,0}, {"p5", "p7"}}}},
            {BlockType::CUBE, {8,2,-2}, C::royal, {{"p7", {0,0.5f,0}, {"p6", "goal"}}}},
            {BlockType::CUBE, {9,2,-2}, C::royal, {{"goal", {0,0.5f,0}, {"p7"}}}},
            {BlockType::PILLAR, {3,0,-1}, C::sienna, {}, true},
            {BlockType::PILLAR, {3,1,-1}, C::sienna, {}, true},
            {BlockType::PILLAR, {5,0,-1}, C::sienna, {}, true},
            {BlockType::PILLAR, {5,1,-1}, C::sienna, {}, true},
            {BlockType::PILLAR, {7,0,-2}, C::sienna, {}, true},
            {BlockType::PILLAR, {7,1,-2}, C::sienna, {}, true},
            {BlockType::PILLAR, {9,0,-2}, C::sienna, {}, true},
            {BlockType::PILLAR, {9,1,-2}, C::sienna, {}, true},
            {BlockType::CUBE, {1,0,-1}, C::orangeRed, {}, true},
            {BlockType::CUBE, {1,0, 1}, C::orangeRed, {}, true},
            {BlockType::CUBE, {8,2,-3}, C::royal, {}, true},
            {BlockType::CUBE, {8,2,-1}, C::royal, {}, true},
            {BlockType::OBSTACLE, {4,0,-0.5f}, C::lava, {}, true, {1,1,1}, true},
            {BlockType::OBSTACLE, {7,2,-1.5f}, C::lava, {}, true, {1,1,1}, true}
        },
        {4.5f, 1.0f, -1.0f}, -130.f, 35.f, 22.f, "Find the hidden altar in the temple."
    });

    return L;
}
