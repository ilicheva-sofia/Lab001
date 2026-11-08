#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "SvgChart.hpp"

namespace {
    std::string read_file(const std::string& path) {
        std::ifstream in(path);
        std::stringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }
}  // namespace

TEST(SvgChartTest, WritesValidSvgWithMultipleSeries) {
    const std::string path = "/tmp/lab001_svg_test.svg";
    Infrastructure::Series s1{"Line A", "#e41a1c", {1.0, 2.0, 3.0, 2.0}};
    Infrastructure::Series s2{"Line B", "#377eb8", {4.0, 3.0, 2.0, 1.0}};

    Infrastructure::SvgChart::write(path, "Test Chart", {s1, s2});

    std::string content = read_file(path);
    EXPECT_FALSE(content.empty());
    EXPECT_NE(content.find("<?xml"), std::string::npos);
    EXPECT_NE(content.find("<svg"), std::string::npos);
    EXPECT_NE(content.find("</svg>"), std::string::npos);
    // Обе серии присутствуют
    EXPECT_NE(content.find("#e41a1c"), std::string::npos);
    EXPECT_NE(content.find("#377eb8"), std::string::npos);
    // Заголовок
    EXPECT_NE(content.find("Test Chart"), std::string::npos);
    // Легенда
    EXPECT_NE(content.find("Line A"), std::string::npos);
    EXPECT_NE(content.find("Line B"), std::string::npos);
    // Полимаршруты (2 серии = 2 polyline)
    EXPECT_NE(content.find("polyline"), std::string::npos);

    std::remove(path.c_str());
}

TEST(SvgChartTest, EscapesXmlSpecialCharacters) {
    const std::string path = "/tmp/lab001_svg_escape.svg";
    Infrastructure::Series s{"A<B>&\"'", "#000000", {1.0, 2.0}};
    Infrastructure::SvgChart::write(path, "T&<>", {s});

    std::string content = read_file(path);
    // Экранированные символы в заголовке
    EXPECT_NE(content.find("T&amp;&lt;&gt;"), std::string::npos);
    // Экранированные символы в легенде
    EXPECT_NE(content.find("A&lt;B&gt;&amp;&quot;&apos;"), std::string::npos);

    std::remove(path.c_str());
}

TEST(SvgChartTest, HandlesSinglePointAndFlatSeries) {
    const std::string path = "/tmp/lab001_svg_flat.svg";
    Infrastructure::Series s{"Flat", "#000000", {5.0, 5.0, 5.0}};
    Infrastructure::SvgChart::write(path, "Flat", {s});

    std::string content = read_file(path);
    EXPECT_FALSE(content.empty());
    EXPECT_NE(content.find("</svg>"), std::string::npos);

    std::remove(path.c_str());
}

TEST(SvgChartTest, MinMaxHelpers) {
    Infrastructure::Series s1{"A", "#000", {10.0, 20.0, 30.0}};
    Infrastructure::Series s2{"B", "#111", {5.0, 25.0, 15.0}};
    std::vector<Infrastructure::Series> all{s1, s2};

    EXPECT_DOUBLE_EQ(Infrastructure::SvgChart::min_of(all), 5.0);
    EXPECT_DOUBLE_EQ(Infrastructure::SvgChart::max_of(all), 30.0);
}

TEST(SvgChartTest, YAxisFloorIsZeroEvenWhenDataIsPositive) {
    // Все значения > 0 — нижняя граница всё равно 0 (баланс не уходит в минус)
    const std::string path = "/tmp/lab001_svg_floor0.svg";
    Infrastructure::Series s{"Bal", "#000", {100.0, 200.0, 150.0}};
    Infrastructure::SvgChart::write(path, "Floor", {s});

    std::string content = read_file(path);
    EXPECT_FALSE(content.empty());
    // Нижняя метка Y-оси должна быть "0" (не отрицательное число)
    EXPECT_NE(content.find(">0</text>"), std::string::npos);
    // Никакой отрицательной метки (">-")
    EXPECT_EQ(content.find(">-"), std::string::npos);

    std::remove(path.c_str());
}

TEST(SvgChartTest, EmptySeriesListWritesNothing) {
    const std::string path = "/tmp/lab001_svg_empty.svg";
    Infrastructure::SvgChart::write(path, "Empty", {});
    // Файл не создаётся (early return)
    std::ifstream in(path);
    EXPECT_FALSE(in.good());
    std::remove(path.c_str());
}
