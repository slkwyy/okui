#include "gtest/gtest.h"

#include "bittorrent/ui/Rectangle.h"

using namespace bittorrent;
using namespace bittorrent::ui;

TEST(Rectangle, construction) {
    Rectangle<int> r(1, 2, 3, 4);
    EXPECT_EQ(r.x, 1);
    EXPECT_EQ(r.y, 2);
    EXPECT_EQ(r.width, 3);
    EXPECT_EQ(r.height, 4);
}

TEST(Rectangle, defaultConstruction) {
    Rectangle<int> r;
    EXPECT_EQ(r.x, 0);
    EXPECT_EQ(r.y, 0);
    EXPECT_EQ(r.width, 0);
    EXPECT_EQ(r.height, 0);
}

TEST(Rectangle, origin) {
    Rectangle<int> r(1, 2, 3, 4);
    EXPECT_EQ(r.origin().x, 1);
    EXPECT_EQ(r.origin().y, 2);
}

TEST(Rectangle, size) {
    Rectangle<int> r(1, 2, 3, 4);
    EXPECT_EQ(r.size().x, 3);
    EXPECT_EQ(r.size().y, 4);
}

TEST(Rectangle, contains) {
    Rectangle<int> r(1, 2, 3, 4);

    EXPECT_FALSE(r.contains(5, 10));
    EXPECT_TRUE(r.contains(1, 2));
    EXPECT_FALSE(r.contains(1, 10));
    EXPECT_TRUE(r.contains(2, 3));
    EXPECT_FALSE(r.contains(4, 6));
    EXPECT_FALSE(r.contains(4, 3));
    EXPECT_FALSE(r.contains(2, 6));
}

TEST(Rectangle, intersection) {
    {
        Rectangle<int> a(1, 2, 3, 4);
        Rectangle<int> b(2, 1, 3, 4);
        
        EXPECT_TRUE(a.intersects(b));
    }

    {
        Rectangle<int> a(1, 2, 3, 4);
        Rectangle<int> b(0, 1, 3, 4);
        
        EXPECT_TRUE(a.intersects(b));
    }

    {
        Rectangle<int> a(1, 2, 3, 4);
        Rectangle<int> b(0, 0, 1, 2);
        
        EXPECT_FALSE(a.intersects(b));
        EXPECT_EQ(a.intersection(b), Rectangle<int>());
    }

    {
        Rectangle<int> a(-1, -2, 3, 4);
        Rectangle<int> b(0, 0, 1, 2);
        
        EXPECT_TRUE(a.intersects(b));
    }

    {
        Rectangle<int> a(-1, -2, 3, 4);
        Rectangle<int> b(10, 10, 1, 2);
        
        EXPECT_FALSE(a.intersects(b));
        EXPECT_EQ(a.intersection(b), Rectangle<int>());
    }

    {
        Rectangle<int> a(1, 2, 3, 4);
        Rectangle<int> b(2, 1, 3, 4);
        
        EXPECT_EQ(a.intersection(b), Rectangle<int>(2, 2, 2, 3));
    }

    {
        Rectangle<int> a(270, 760, 500, 0);
        Rectangle<int> b(270, 760, 2000, 600);
        
        EXPECT_FALSE(a.intersects(b));
        EXPECT_EQ(a.intersection(b), Rectangle<int>());
    }

    {
        Rectangle<int> a(770, 0, 1130, 500);
        Rectangle<int> b(270, 0, 2230, 1400);
        
        EXPECT_EQ(a.intersection(b), Rectangle<int>(770, 0, 1130, 500));
    }
}